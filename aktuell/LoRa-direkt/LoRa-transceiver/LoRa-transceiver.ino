/*
   LoRa-Transceiver
   Ein einfacher Sender um Daten via LoRa zu verschicken.
   Die Frequnz wird auf einem Kanal festgelegt.
   Es werden die Default-Werte der Library genommen.

   Arduino IDE 1.8.19
   ESP32-Core 2.0.5

   DARC-HamGroup LoRaWAN
   Bernd DG8BR
   15.09.2022
*/

/* Libraries für LoRa */
#include <SPI.h>
#include <LoRa.h>               // https://github.com/sandeepmistry/arduino-LoRa

/* Libraries für OLED Display */
#include <Wire.h>
#include "SSD1306Wire.h"        // ThingPulse OLED SSD1306 - Library        https://github.com/ThingPulse/esp8266-oled-ssd1306

/* Diese GPIO's werden für das LoRa-Modul genutzt. */
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26
/* Frequenz */
#define BAND 868100000

/* Globale Variablen */
volatile uint8_t zaehler = 0;
volatile unsigned long previousMillis = 0;
const unsigned long interval = 6000;
char teststring[15] = "DG8BR Test\0";

/* Definitionen */
void sendedaten(void);

SSD1306Wire display(0x3c, SDA, SCL);

void setup()
{
  Serial.begin(115200);
  Serial.println();

  if (!display.init())                    // Konfiguration Display
  {
    Serial.println("Displayfehler!");
    while (1);
  }
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.drawString(0, 0, "LORA TRANSCEIVER");
  Serial.println("LoRa Transceiver Test");

  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND))
  {
    Serial.println("Fehler LoRa-Modul");
    while (1);
  }
  LoRa.setPreambleLength(40);
  Serial.println("LoRa Initialisierung OK!");

  display.drawString(0, 10, "LoRa Initialisierung OK!");
  display.display();
  delay(5000);
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    sendedaten();
  }
  yield();
}
void sendedaten(void)
{
  char buffer[40];

  display.clear();
  display.drawString(0, 0, "LORA TRANSCEIVER");
  display.drawString(0, 20, "sende Paket:");
  display.drawStringf(0, 30, buffer, "%s %d", teststring, zaehler);
  display.display();

  Serial.println(buffer);
  
  LoRa.beginPacket();
  LoRa.print(buffer);
  LoRa.endPacket(0);

  zaehler++;
  return;
}
