/*
 * Ein simples Programm um die Funktion des GPS-Empfängers zu testen.
 * Es werden im Sekundentakt 6 Zeilen ausgegeben.
 * Wer es genauer wissen will, sollte sich das NMEA-Protokoll ansehen.
 * 
 * Genutzt wird ein LILYGO T3_V1.6.1 und die Arduino ESP32-Core 2.0.3. 
 * Geschrieben mit der Arduino Ide 1.8.19
 * 
 * Achtung!! Das Board ist unter "Werkzeuge" nun anders einzustellen.
 * 
 * Es wird für Softserial eine neine Bibliothek benötigt.Sie ist über den Bibliothekverwalter zu installieren.
 * Sucht nach EspSoftwareSerial von Dirk Kaar.
 * 
 * Für die HamGroup LoRaWAN von DG8BR
 * 12.05.2022
 */

#include <SoftwareSerial.h>                                 // https://github.com/plerup/espsoftwareserial/

#define RXPin 34      // Gpio34
#define TXPin 35      // GPIO35 
#define GPSBaud 9600


// Die Verbindung zum GPS-RX
SoftwareSerial sserial(RXPin, TXPin);

void setup()
{
  Serial.begin(115200);
  sserial.begin(GPSBaud);
}

void loop()
{
  while (sserial.available() > 0)
  {
    // Daten lesen von GPS
    byte gpsData = sserial.read();
    Serial.write(gpsData);
  }
}
