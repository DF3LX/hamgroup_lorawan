/****************************************
  DARC-HamGroup LoRaWAN

  Temperatur-Node DS18B20

  Based on the LoRaWAN example from Thomas Telkamp and Matthijs

  16.01.2022  Jürgen, DL8MA

  http://www.p37.de/LoRaWAN

  passender Payload-Formatter befindet sich ganz unten in einem Kommentarfeld

*/
#include <lmic.h>                                   // LMIC library from the libary-manager
#include <hal/hal.h>                                // includet in the lmic library
#include <SPI.h>
#include "SSD1306Wire.h"                            // ThingPulse OLED SSD1306 - Library  https://github.com/ThingPulse/esp8266-oled-ssd1306
#include <OneWire.h>
#include <DallasTemperature.h>


// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8]={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8]={ = eigene DevEUI eintragen = };
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
// The key shown here is the semtech default key.
static const u1_t PROGMEM APPKEY[16] = { = eigener APPKEY eintragen = };
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}


SSD1306Wire display(0x3c, SDA, SCL);                    // Display-Anschlüsse sind schon über die Board-Auswahl definiert

const int oneWireBus = 13;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

unsigned TX_INTERVAL = 60;                              // Sendeintervall in Sekunden

double temperatur = 999;
double ringPuffer[] = { 9999, 9999, 9999 };             // Puffer für gleitenden Mittelwertbildung
int ringPufferZeiger = 0;

static uint8_t mydata[] = "  ";                         // Feld für Hex-Werte die ausgesendet werden
static osjob_t sendjob;

int i = 0;
long sendeZeit = 0;                                     // für Anzeige der nächsten Aussendung / Count-Down-Zähler im Display
long displayPause = 0;                                  // Refresh-Rate für das OLED-Display
long zeitspanne = 0;                                    // für die Anzeige der Statusmeldungen auf dem Display
String status = "";

int txAktiv = 0;

// Pin mapping
const lmic_pinmap lmic_pins = {                          // Pin mapping zum LoRaWAN-TRX-Modul
    .nss = 18,
    .rxtx = LMIC_UNUSED_PIN,                             // gilt für TTGO LORA32 T3_V1.6.1 20210104 und T3_V1.6 20180606
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

  int i;
  double summe = 0;

  for( i = 0; i < 10; i++ ) {                                                                     // 10 Messungen
    sensors.requestTemperatures();
    temperatur = sensors.getTempCByIndex(0);
    summe = summe + temperatur;                                                                   // aufsummieren
  }
  temperatur = summe / i;                                                                         // Mittelwert bilden

/* optional */
  ringPuffer[ 2 ] = ringPuffer[ 1 ];                                                              // neue Messung in Ringpuffer schreiben
  ringPuffer[ 1 ] = ringPuffer[ 0 ];
  ringPuffer[ 0 ] = temperatur;
  if( ringPuffer[ 1 ] < 9999 and ringPuffer[ 2 ] < 9999 ) {
    temperatur =  0.25 * ringPuffer[ 0 ] + 0.5 * ringPuffer[ 1 ] + 0.25 * ringPuffer[ 2 ];        // gleitender Mittelwert aus den letzten
  }                                                                                               // bilden
/* optional */

  Serial.println( temperatur );

  int temp = 1000 + round( temperatur * 10 );
  mydata[0] = temp >> 8;
  mydata[1] = temp & 0xFF;

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

    TX_INTERVAL -= 12;                                                                            // Korrektur des Sendeintervals ( einige Sekunden für Sendevorgang abziehen)

    sensors.begin();                                                                              // Temperatursensor initialisieren
    sensors.requestTemperatures();
    temperatur = sensors.getTempCByIndex(0);

    Serial.begin(115200);
    Serial.println(F("Starting"));
    display.init();                                                                                 // Konfiguration Display
    display.flipScreenVertically();
    display.setFont( ArialMT_Plain_24 );
    display.clear();
    display.display();

    os_init();
    LMIC_reset();

    do_send(&sendjob);                                                                              // Hintergrund-Job für die LoRaWAN-Aussendungen starten
}

void loop() {
    os_runloop_once();

    String str = "";

    if( ( displayPause + 1000 ) < millis()  ) {                                                     // Auffrischung des Displays nur alle Sekunde
      display.clear();
      String tempString = String( temperatur, 2 ) + " ºC";
      display.drawString( 15, 22, tempString );
      display.display();

      int statusDisplayZeit = 60;
      if( TX_INTERVAL < 60 )
        statusDisplayZeit = TX_INTERVAL / 2;
      if( zeitspanne < ( statusDisplayZeit ) )
        status = "";
      displayPause = millis();
    }
}


/***********************************************
 * Payload-Formatter für TTN

------------------------------------------------
function Decoder(bytes, port) {

    var decode = {};

    decode.temperatur = ( ((bytes[0] << 8) | bytes[1]) / 10 ) - 100;
    decode.temperatur = round( decode.temperatur );

    decode.bytes = bytes;

    return decode;
  }

  function round(num) {
    var m = Number((Math.abs(num) * 100).toPrecision(15));
    return Math.round(m) / 100 * Math.sign(num);
}
------------------------------------------------
*/