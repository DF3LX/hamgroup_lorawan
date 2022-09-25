/*
   LoRa-Receiver
   Ein einfacher Empfänger für LoRa.
   Hier wird nur auf einem Kanal gehört.
   Es werden die Default-Daten der Library genutzt.

   Arduino IDE 1.8.19
   ESP32-Core 2.0.5

   DARC-HamGroup LoRaWAN
   Bernd DG8BR
   15.09.2022
*/

//Libraries für LoRa
#include <SPI.h>
#include <LoRa.h>                 // https://github.com/sandeepmistry/arduino-LoRa

//Libraries für OLED Display
#include <Wire.h>
#include "SSD1306Wire.h"          // ThingPulse OLED SSD1306 - Library        https://github.com/ThingPulse/esp8266-oled-ssd1306

// Diese GPIO's werden für das LoRa-Modul genutzt
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26
// Frequenz
#define BAND 868100000

SSD1306Wire display(0x3c, SDA, SCL);

void setup()
{
  Serial.begin(115200);
  Serial.println();

  if (!display.init())                  // Konfiguration Display
  {
    Serial.println("Displayfehler!");
    while (1);
  }
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.drawString(0, 0, "LORA RECEIVER");
  display.display();
  delay(5000);
  Serial.println("LoRa Receiver Test");

  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND))
  {
    Serial.println("LoRa-Modul Fehler!");
    while (1);
  }
  Serial.println("LoRa Initialisierung OK!");

  display.drawString(0, 10, "LoRa Initialisierung OK!");
  display.display();
}

void loop()
{
  char buffer[40];                      // Hilfsbuffer für drawStringf
  String LoRaData;

  uint16_t packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    LoRaData = LoRa.readString();
    Serial.print(LoRaData);
    int8_t rssi = LoRa.packetRssi();
    Serial.print("\nEmpfangsstaerke RSSi:  ");
    Serial.println(rssi);

    display.clear();
    display.drawString(0, 0, "LORA RECEIVER");
    display.drawString(0, 20, "empfangenes Paket:");
    display.drawString(0, 30, LoRaData);
    display.drawStringf(0, 40, buffer, "RSSI: %i", rssi);
    display.display();
  }
}
