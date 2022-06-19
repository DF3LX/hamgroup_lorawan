/*
   DARC-HamGroup LoRaWAN
   Mit einem ESP32 Spannung messen.
   Die Spannung wird auf einem 0,96 OLED-Display und Seriell ausgegeben.

   Genutzt wurde ein ESP32 DEV KIT mit 30 Pins, Arduino Ide 1.8.19 und die ESP32-Core 2.0.2

   Bernd DG8BR I05 21.04.2022
*/

#include "SSD1306Wire.h"                 // ThingPulse OLED SSD1306 - Library        https://github.com/ThingPulse/esp8266-oled-ssd1306


uint32_t alte_zeit = 0;
const uint32_t wartezeit = 500;
bool ledState = LOW;

#define LED_BUILTIN 2                    // ist an GPIO2 gebunden
#define messpin 34                       // ADC1 Kanal6

SSD1306Wire display(0x3c, SDA, SCL);

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  display.init();                        // Konfiguration Display
  display.flipScreenVertically();
  display.setFont( ArialMT_Plain_24 );
  display.clear();
  display.display();
}

void loop()
{
  String volt;
  uint16_t messwert = 0;

  if (millis() - alte_zeit > wartezeit)
  {
    alte_zeit = millis();              // aktuelle Zeit abspeichern
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);

    messwert = analogRead(messpin);
    volt = String(messwert * (3.30f / 4095));  // Formel: Messwert * (Messbereich / Auflösung)

    Serial.print("Messwert = ");
    Serial.print(messwert);
    Serial.print("    Volt = ");
    Serial.println(volt);
    zeige_display(volt);
  }
}

void zeige_display(String volt)
{
  display.clear();
  display.drawString(0, 30, volt );
  display.drawString(60, 30, "V");
  display.display();
}
