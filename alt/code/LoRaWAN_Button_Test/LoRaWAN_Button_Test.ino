/****************************************
  DARC-HamGroup LoRaWAN

  Taster-Test-Programm  
  
  18.12.2021  Jürgen, DL8MA

  http://www.p37.de/LoRaWAN
  
*/
#include <SPI.h>
#include "SSD1306Wire.h"                            // ThingPulse OLED SSD1306 - Library  https://github.com/ThingPulse/esp8266-oled-ssd1306

SSD1306Wire display(0x3c, SDA, SCL);                // Display-Anschlüsse sind schon über die Board-Auswahl definiert

const int buttonPin = 36;                           // Eingang für Button GPIO36
const int ledPin =  LED_BUILTIN;                    // Port der eingebauten grünen LED
int buttonState = 0;                                // aktueller Status des Buttons
String text = "LoRaWAN HamGroup";


void setup() {
    pinMode( buttonPin, INPUT );                    // Pin für Taster
    pinMode( LED_BUILTIN, OUTPUT );                 // eingebaute LED (grün) für Status-Blinken
    digitalWrite(LED_BUILTIN, LOW );                // beim Start ausschalten
  
    Serial.begin(115200);

    display.init();                                 // Konfiguration Display
    display.flipScreenVertically();
    display.setFont( ArialMT_Plain_10 );
    display.clear();
    display.display(); 
}

void loop() {
  buttonState = digitalRead(buttonPin);             // Taster einlesen

  Serial.println( buttonState );                    // auf serieller Schnittstelle ausgeben ( 0 oder 1 )
  display.clear();                                  // Ausgabe auf OLED-Display
  display.drawString( 0, 0, text );
  display.drawString( 0, 20, "Taster: " );    

  if (buttonState == HIGH) {                        // gedrückt
    digitalWrite(ledPin, HIGH);
    display.drawString( 40, 20, "gedrückt" );    
    display.display(); 
  } else {                                          // offen
    digitalWrite(ledPin, LOW);
    display.drawString( 40, 20, "offen" );    
    display.display(); 
  }
}
