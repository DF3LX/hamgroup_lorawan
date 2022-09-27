/*
   LoRa-Transceiver mit einem Sensor(DHT22).
   Ein formatierter Datensatz wird via LoRa verschickt.
   Da ein Sensor keine serielle Ausgabe hat, ist diese Ausgabe
   durch einen Schalter ausschaltbar. Eventuelle Fehler weden
   durch die eingebaute Led dargestellt.

   Die Frequnz wird auf einem Kanal festgelegt.
   Es werden die Default-Werte der Library genommen.

   Wenn man die serielle Ausgabe und die Displayausgabe haben
   will, dann muß man die '//' vor #define DEBUG entfernen.
   
   Arduino IDE 1.8.19
   ESP32-Core 2.0.5

   DARC-HamGroup LoRaWAN
   Bernd DG8BR
   26.09.2022
*/

//#define DEBUG   // Wenn man serielle Ausgabe und Display haben will. Kommentierung entfernen.

/* Libraries für LoRa */
#include <SPI.h>
#include <LoRa.h>               // https://github.com/sandeepmistry/arduino-LoRa

#ifdef DEBUG
/* Libraries für OLED Display */
#include <Wire.h>
#include "SSD1306Wire.h"        // ThingPulse OLED SSD1306 - Library        https://github.com/ThingPulse/esp8266-oled-ssd1306
#endif

/* Diese GPIO's werden für das LoRa-Modul genutzt. */
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26
/* Frequenz */
#define BAND 868100000

#define led 25

/* Globale Variablen */
volatile uint8_t zaehler = 1;
volatile uint8_t ledstatus = LOW;
volatile unsigned long previousMillis = 0L;
const unsigned long interval = 10000L;
char sendestring[25];

/* Definitionen */
void fehler_led(uint16_t, uint8_t);
void sendedaten(void);
void init_dht(void);
void read_dht(void);

#ifdef DEBUG
SSD1306Wire display(0x3c, SDA, SCL);
#endif

void setup()
{
  pinMode(led, OUTPUT);
  digitalWrite(led, ledstatus);

#ifdef DEBUG
  Serial.begin(115200);
  Serial.println();

  if (!display.init())                    // Konfiguration Display
  {
    Serial.println("Displayfehler!");
    fehler_led(100, 10);
    while (1);
  }

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.drawString(0, 0, "LoRa Transceiver DHT");
  display.display();
#endif

  init_dht();              // In dht.ino

#ifdef DEBUG
  Serial.println("LoRa Transceiver Test");
#endif

  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND))
  {
#ifdef DEBUG
    Serial.println("Fehler LoRa-Modul");
#endif
    fehler_led(100, 10);
    while (1);
  }
#ifdef DEBUG
  Serial.println("LoRa Initialisierung OK!");
  display.drawString(0, 10, "LoRa Initialisierung OK!");
  display.display();
  delay(5000);
#endif
  read_dht();                   // In dht.ino
  sendedaten();                 // In dht.ino
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    read_dht();                   // In dht.ino
    sendedaten();                 // In dht.ino
  }
  yield();
}

/*  Bei einem Sensor hat man keinen Nutzen von der seriellen Schnittstelle.
    Darum soltte ein Fehler optisch mit einer Léd signalisiert werden.
    Damit man die einzelnen Fehler unterscheiden kann, werden unterschiedliche Blinkfrequenzen
    und Dauer variert.
*/
void fehler_led(uint16_t periode, uint8_t dauer)
{
  uint8_t i;

  for (i = 0; i < dauer; i++)
  {
    ledstatus = !ledstatus;
    digitalWrite(led, ledstatus);
    delay(periode);
  }
  return;
}
