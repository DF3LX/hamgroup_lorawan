/****************************************
  DARC-HamGroup LoRaWAN

  Temp-Sensor-Test-Programm  
  
  23.12.2021  Jürgen, DL8MA

  http://www.p37.de/LoRaWAN 
*/
#include "SSD1306Wire.h"                                      // ThingPulse OLED SSD1306 - Library        https://github.com/ThingPulse/esp8266-oled-ssd1306
#include <OneWire.h>                                          // 1Wire-Library für die Bus-Kommunikation  https://www.pjrc.com/teensy/td_libs_OneWire.html
#include <DallasTemperature.h>                                // Library für die Dallas DS-Sensoren       https://github.com/milesburton/Arduino-Temperature-Control-Library

long displayPause = 0;                                        // für die Zeitsteuerung des Displays ( "Sekundentakt" )
double temperatur = 999;                                      // gemessener Temperaturwert

SSD1306Wire display(0x3c, SDA, SCL);                          // Display-Anschlüsse sind schon über die Board-Auswahl definiert

const int oneWireBus = 13;                                    // GPIO-Pin für 1Wire-Bus     

OneWire oneWire(oneWireBus);                                  // 1Wire-Bus initialsieren
DallasTemperature sensors(&oneWire);                          // für den Zugriff auf den bzw. die Temperatur-Sensoren

void setup() {
  Serial.begin(115200);
  sensors.begin();                                            // Temperatur-Sensoren intialsieren

  display.init();                                             // Konfiguration Display
  display.flipScreenVertically();
  display.setFont( ArialMT_Plain_24 );                        // für die älteren Teilnehmer eine etwas grössere Schrift ;-)
  display.clear();
  display.display();
}

void loop() {
  if( ( displayPause + 1000 ) < millis()  ) {                 // Auffrischung des Displays nur alle Sekunde
    sensors.requestTemperatures();                            // Temperaturmessung anstossen und Bytefolge aus dem Sensor auslesen
    temperatur = sensors.getTempCByIndex(0);                  // Temperaturwert in ºC aus der Bytefolge des Sensors extrahieren
  
    Serial.print(temperatur);                                 // Temperaturwert über die serielle Schnittstelle ausgeben
    Serial.println("ºC");
    
    display.clear();                                          // Temperaturwert auf dem Display ausgeben
    String tempString = String( temperatur, 2 ) + " ºC";
    display.drawString( 15, 22, tempString );      
    display.display();      
    displayPause = millis();
  }
}
