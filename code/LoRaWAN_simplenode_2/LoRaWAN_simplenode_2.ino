/****************************************
  DARC-HamGroup LoRaWAN

  TTGO-Node einfach 1

  einfacher Node 2 - 8bit-Zähler

  Based on the LoRaWAN example from Thomas Telkamp and Matthijs 

  13.11.2021  Felix, D06FP; Jürgen, DL8MA

  http://www.p37.de/LoRaWAN
*/
#include <lmic.h>                                                                                             // LMIC library from the libary-manager
#include <hal/hal.h>                                                                                          // includet in the lmic library
#include <SPI.h>

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8]={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8]={ ENTERHEREDEVEUI };
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
// The key shown here is the semtech default key.
static const u1_t PROGMEM APPKEY[16] = { ENTERHEREAPPKEY };
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

static uint8_t mydata[] = " ";                                      // Fixtext der ausgesendet wird
static osjob_t sendjob;

int i = 0;
int zaehler = 0;

const unsigned TX_INTERVAL = 50;                                            // Sendeintervall in Sekunden     50 entspricht ca. 60 Sekunden Sendezyklus

// Pin mapping
const lmic_pinmap lmic_pins = {                                             // Pin mapping zum LoRaWAN-TRX-Modul
    .nss = 18,
    .rxtx = LMIC_UNUSED_PIN,                                                // gilt für TTGO LORA32  T3_V1.6   20210104 & 20180606
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
            break;
        case EV_JOINED:                                    
            Serial.println(F("EV_JOINED"));

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
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
            break;
        case EV_TXCOMPLETE:
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Received ack"));
            if (LMIC.dataLen) {
              Serial.println(F("Received "));
              Serial.println(LMIC.dataLen);
              Serial.println(F(" bytes of payload"));
            }
            for( i = 0; i < 2; i++ ) {                                                                // nach Aussendung kurz blinken
              digitalWrite( LED_BUILTIN, HIGH );  
              delay( 1000 );
              digitalWrite( LED_BUILTIN, LOW );  
              delay( 1000 );
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

    zaehler++;                                                                                    // 8bit-Zähler
    mydata[ 0 ] = zaehler;                                                                        // Zahl in Sendearray ablegen
      
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

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW );
  
    Serial.begin(115200);
    Serial.println(F("Starting"));

    os_init();
    LMIC_reset();

    do_send(&sendjob);
}

void loop() {
    os_runloop_once();
}


/******************************************************
 * Payload-Formatter (Java-Script)

function Decoder(bytes, port) {
  
  var decode = {};

  decode.zaehler = bytes[ 0 ];

  decode.bytes = bytes;

  return decode;
}
*/