/****************************************
  DARC-HamGroup LoRaWAN

  Button-Node-1

  ein Taster bzw. Schalter löst bei Betätigung eine LoRaWAN-Aussendung aus
  nach der letzten Aussendung wird nach einer einstellbaren Zeit zyklisch eine Statusmeldung ausgesendet
  
  Based on the LoRaWAN example from Thomas Telkamp and Matthijs 

  20.12.2021  Jürgen, DL8MA

  http://www.p37.de/LoRaWAN

  passender Payload-Formatter befindet sich ganz unten in einem Kommentarfeld
  
*/
#include <lmic.h>                                   // LMIC library from the libary-manager
#include <hal/hal.h>                                // includet in the lmic library
#include <SPI.h>
#include "SSD1306Wire.h"                            // ThingPulse OLED SSD1306 - Library  https://github.com/ThingPulse/esp8266-oled-ssd1306

SSD1306Wire display(0x3c, SDA, SCL);                // Display-Anschlüsse sind schon über die Board-Auswahl definiert


// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8]={ APPEUI };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8]={ DEVEUI };
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
// The key shown here is the semtech default key.
static const u1_t PROGMEM APPKEY[16] = { APPKEY };
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}


unsigned TX_INTERVAL = 1800;                                               // Sendeintervall in Sekunden / z.B. 3600 Sekunden ( = 1h ) Sendezyklus

static uint8_t mydata[] = " ";                                             // Feld für Hex-Werte die ausgesendet werden
static osjob_t sendjob;

int i = 0;
long sendeZeit = 0;                                                         // für Anzeige der nächsten Aussendung / Count-Down-Zähler im Display
long displayPause = 0;                                                      // Refresh-Rate für das OLED-Display 
long zeitspanne = 0;                                                        // für die Anzeige der Statusmeldungen auf dem Display
int anzahl = 0;                                                             // Anzahl der Zeichen für Displayausgabe
String status = "";

const int buttonPin = 36;                                                   // Eingang für Button GPIO36
int buttonState = 0;
int txAktiv = 0;

// Pin mapping
const lmic_pinmap lmic_pins = {                                             // Pin mapping zum LoRaWAN-TRX-Modul
    .nss = 18,
    .rxtx = LMIC_UNUSED_PIN,                                                // gilt für TTGO LORA32 T3_V1.6.1 20210104 und T3_V1.6 20180606
    .rst = 23,
    .dio = {26, 33, 32},
};



/*********************************
 * Event-Handler
 */
void onEvent (ev_t ev) {
    
//    Serial.print(os_getTime());
//    Serial.print(": ");
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println(F("EV_SCAN_TIMEOUT"));            
            break;
        case EV_BEACON_FOUND:
            Serial.println(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            Serial.println(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            Serial.println(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            Serial.println(F("EV_JOINING"));
            status = "Join läuft";
            break;
        case EV_JOINED:                                    
            Serial.println(F("EV_JOINED"));
            status = "Join erfolgreich";
            // Disable link check validation (automatically enabled
            // during join, but not supported by TTN at this time).
            LMIC_setLinkCheckMode(0);
            for( i = 0; i < 20; i++ ) {                                                                 // nach erfolgreichem Join wird 20x kurz geblinkt
              digitalWrite( LED_BUILTIN, HIGH );  
              delay( 100 );
              digitalWrite( LED_BUILTIN, LOW );  
              delay( 100 );
            }                          
            break;
        case EV_RFU1:
            Serial.println(F("EV_RFU1"));
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            status = "Join fehlgeschlagen";
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            status = "Re-Join fehlgeschlagen";
            break;
            break;
        case EV_TXCOMPLETE:
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            txAktiv = 0;                                                                              // Freigabe für die nächste Aussendung
            status = "TX ok";
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Received ack"));
            if (LMIC.dataLen) {
              Serial.println(F("Received "));
              Serial.println(LMIC.dataLen);
              Serial.println(F(" bytes of payload"));
            }
            sendeZeit = millis();            
            for( i = 0; i < 1; i++ ) {                                                                // nach Aussendung kurz blinken
              digitalWrite( LED_BUILTIN, HIGH );  
              delay( 100 );
              digitalWrite( LED_BUILTIN, LOW );  
              delay( 100 );
            }           
            // Schedule next transmission           
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);         
            break;
        case EV_LOST_TSYNC:
            Serial.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            Serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            Serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Serial.println(F("EV_LINK_ALIVE"));
            break;
         default:         
            Serial.println(F("Unknown event"));
            break;
    }
}

/*****************************************
 * LoRaWAN-Datenpaket aussenden
 */
void do_send(osjob_t* j){
         
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
        // Prepare upstream data transmission at the next possible time.
                        
        digitalWrite( LED_BUILTIN, HIGH );                               
        LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 0);
        Serial.println(F("Packet queued"));
        digitalWrite( LED_BUILTIN, LOW );
    }
    // Next TX is scheduled after TX_COMPLETE event.
}


void setup() {

    TX_INTERVAL -= 6;                                                                               // Korrektur des Sendeintervals ( einige Sekunden für Sendevorgang abziehen)

    pinMode(LED_BUILTIN, OUTPUT);                                                                   // eingebaute LED (grün) für Status-Blinken
    digitalWrite(LED_BUILTIN, LOW );
    pinMode( buttonPin, INPUT );                                                                    // Pin für Taster

    buttonState = digitalRead(buttonPin);
    mydata[ 0 ] = buttonState << 4;                                                                 // Datenfeld einrichten
    
    Serial.begin(115200);
    Serial.println(F("Starting"));

    display.init();                                                                                 // Konfiguration Display
    display.flipScreenVertically();
    display.setFont( ArialMT_Plain_16 );
    display.clear();
    display.display();
  
    os_init();
    LMIC_reset();

    do_send(&sendjob);                                                                              // Hintergrund-Job für die LoRaWAN-Aussendungen starten                                                                             
}

void loop() {
    String str = "";
    
    os_runloop_once();

    if( !txAktiv ) {                                                                                // Aussendungen wieder freigegeben?
      if( buttonState != digitalRead(buttonPin) ) {                                                 // Kontakt wurde geöffnet bzw. geschlossen
        buttonState = digitalRead(buttonPin);                                                       // Taster einlesen

        txAktiv = 1;                                                                                // neue LoRaWAN-Aussendung
        mydata[ 0 ] = buttonState << 4;                                                             // 1. Nibble des Bytes: Kontaktzustand       
        mydata[ 0 ] = mydata[ 0 ] | 0x01;                                                           // 2. Nibble des Bytes: Trigger-Info = 1 ( Kontakt geändert )
    
        LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 0);                                            // Daten für die Aussendung einstellen und Aussendung starten

        Serial.print( "Flanke " );
        Serial.println( buttonState );                                                              // für Anschauungszwecke über serielle Schnittstelle ausgeben
        Serial.print( "Payload: " );
        str = hexString( mydata, sizeof( mydata ) - 1 );     
        Serial.println( str );     
      }
    } else {
        mydata[ 0 ] = buttonState << 4;                                                             // 1. Nibble des Bytes:   Kontaktzustand
                                                                                                    // 2. Nibble des Bytes:   Trigger-Info = 0 ( kein Trigger )
    }
    if( ( displayPause + 1000 ) < millis()  ) {                                                     // Auffrischung des Displays nur alle Sekunde
      str = hexString( mydata, sizeof( mydata ) - 1 );     
      display.clear();
      display.drawString( 0, 0, str );                                                              // Byte-Array ausgeben
      zeitspanne = -1 * ( ( ( millis() - sendeZeit ) / 1000 ) - TX_INTERVAL - 9 );
      display.drawString( 0, 22, String( zeitspanne ) );                                            // Count-Down-Zähler
      if( zeitspanne > 60 )
        display.drawString( 55, 22, "( " + String( zeitspanne / 60 ) + " min )" );
      display.drawString( 0, 44, status );                                                          // Status-Meldung      
            
      display.drawString( 55, 0, "Taster: " );                                                      // Tasterstatus
      display.drawString( 113, 0, String( buttonState ) );      
      display.display();
      
      int statusDisplayZeit = 60;
      if( TX_INTERVAL < 60 )
        statusDisplayZeit = TX_INTERVAL / 2;
      if( zeitspanne < ( statusDisplayZeit ) )
        status = "";    
      displayPause = millis();
    }
}

/****************************************************
 * Array mit Hexzahlen in lesbaren String wandeln
 * 
 * 19.12.21 JM
 */
String hexString( uint8_t mydata[], int anzahl ) {     
      String str = "";
      for( i = 0; i < anzahl; i++ ) { 
        if(  mydata[ i ] < 16 ) 
          str = str + "0";
         str = str + String( mydata[ i ], HEX);
         str = str + " ";
      }
      str.trim();
      str.toUpperCase();
      return( str );
}


/***********************************************
 * Payload-Formatter für TTN

------------------------------------------------ 
function Decoder(bytes, port) {
  
  var decode = {};

  decode.button = ( bytes[ 0 ] >> 4 );        // Tasterstatus

  decode.buttonTrigger = bytes[ 0 ] & 0x0F;   // Trigger = Taster betätigt

  decode.bytes = bytes;                       // nur zur Anschauung

  return decode;
}
------------------------------------------------ 

*/


