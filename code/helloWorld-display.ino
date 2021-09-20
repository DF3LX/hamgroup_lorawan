/****************************************
  Blink

  DARC-HamGroup LoRaWAN

  Hello World auf dem OLED-Display  

  20.09.2021  Jürgen, DL8MA

  http://www.p37.de/LoRaWAN
  
*/

#include "SSD1306Wire.h"                            // ThingPulse OLED SSD1306 - Library  https://github.com/ThingPulse/esp8266-oled-ssd1306

SSD1306Wire display(0x3c, SDA, SCL);                // Display-Anschlüsse sind schon über die Board-Auswahl definiert

void setup() { 
  display.init();                                   // Konfiguration Display
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
}

void drawFontFaceDemo() {                           // Textausgabe
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Online-Workshop");
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, "DARC-HamGroup");
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 26, "LoRaWAN");
}

void loop() {
  display.clear();

  drawFontFaceDemo();

  display.setFont(ArialMT_Plain_10);                // Zeitzähler
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 54, String(millis()));
  // write the buffer to the display
  display.display();
  
  delay(100);
}
