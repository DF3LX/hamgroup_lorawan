/*
   LoRa-Receiver
   Ein Empfänger für LoRa, der nur auf einen Sensor hört.
   Die empfangenen Daten werden für das Display aufbereitet
   und kann auch weiter verarbeitet werden.

   Hier wird nur auf einem Kanal gehört.
   Es werden die Default-Daten der Library genutzt.

   Arduino IDE 1.8.19
   ESP32-Core 2.0.5

   DARC-HamGroup LoRaWAN
   Bernd DG8BR
   22.09.2022
*/

/* Libraries für LoRa */
#include <SPI.h>
#include <LoRa.h>                // https://github.com/sandeepmistry/arduino-LoRa

/* Libraries für OLED Display */
#include <Wire.h>
#include "SSD1306Wire.h"         // ThingPulse OLED SSD1306 - Library        https://github.com/ThingPulse/esp8266-oled-ssd1306

/* Diese GPIO's werden für das LoRa-Modul genutzt */
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26
/* Frequenz */
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
  display.drawString(0, 0, "LORA RECEIVER DHT");

  Serial.println("LoRa Receiver DHT22");

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
  delay(5000);
}

/*
   Die gültigen Daten stehen, als Rohdaten in 'loradaten' oder
   selektiert in 'daten'. Beide Variablen muß!!!! man wegsichern,
   wenn man sie noch woanders verarbeiten will..
*/
void loop()
{
  uint8_t i;
  int8_t rssi;
  char buffer[40];                      // Hilfsbuffer für drawStringf
  char loradaten[200];
  char *trenner;
  char daten[4][6];  // 4 Datensätze a max. 5 Zeichen + '\0'

  uint16_t packetSize = LoRa.parsePacket();

  if (packetSize)
  {
    i = 0;
    loradaten[0] = '\0';

    while ( LoRa.available() )
    {
      loradaten[i++] = (char)LoRa.read();
      if (i >= 199) // Leider empfange ich auch Daten, die verschlüsselt und extrem lang sind
        break;
    }
    loradaten[i] = '\0';
    rssi = LoRa.packetRssi();
    /*
       Hier lass ich nur die ID 001 zu. Alle anderen Pakete ignoriere ich
    */
    if ((loradaten[0] == 0x30) && (loradaten[1] == 0x30) && (loradaten[2] == 0x31))
    {
      i = 0;
      trenner = strtok(loradaten, ";");
      while (trenner != NULL)
      {
        Serial.println(trenner);
        strcpy(daten[i], trenner);
        i++;
        trenner = strtok(NULL, ";");
      }
      display.clear();
      display.drawString(0, 0, "LoRa Receiver DHT");
      display.drawStringf(0, 10, buffer, "ID: %s", daten[0]);
      display.drawStringf(0, 20, buffer, "Zähler: %s", daten[1]);
      display.drawStringf(0, 30, buffer, "Temperatur: %s", daten[2]);
      display.drawStringf(0, 40, buffer, "Feuchtigkeit: %s", daten[3]);
      display.drawStringf(0, 50, buffer, "RSSI: %i", rssi);
      display.display();
    }
    else
      loradaten[0] = '\0';
  }
}
