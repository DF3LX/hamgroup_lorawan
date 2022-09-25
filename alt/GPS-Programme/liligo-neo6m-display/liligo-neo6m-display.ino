/*
   Dieses Programm wertet die Daten eines GPS-Empfängers aus. Sie werden, entsprechend aufbereitet, auf einem Display
   und, wenn gewollt, seriell ausgegeben.
   Eingesetzt wird ein LILYGO V2.1 1.6.1 und ein U-BLOX NEO-6M.

   Programmiert wurde mit der Arduino Ide 1.8.19 und der ESP32-Core 2.0.3

   Für die HamGroup LoraWAN von DG8BR
   21.05.2022
*/


#include <SoftwareSerial.h>                   // https://github.com/plerup/espsoftwareserial/
#include <TinyGPS++.h>                        // https://github.com/mikalhart/TinyGPSPlus

#define RxPin 34                              // passt bei dieser Hardware
#define TxPin 12                              // sollte beim LILYGO T-BEAM auch so sein. 
#define GpsBaud 9600

#define DEBUG                               // Wenn KEINE serielle Ausgabe gewünscht, dann auskommentieren!!

/* Funktionsdeklaration */
uint8_t initDisplay(void);
void Ausgabe(void);

TinyGPSPlus gps;
SoftwareSerial sserial(RxPin, TxPin);

void setup()
{
  Serial.begin(115200);
  sserial.begin(GpsBaud);

  if (initDisplay() == false)
  {
    Serial.println();
    Serial.println(F("Displayfehler!!"));
    delay(5000);
    ESP.restart();    
  }
}

void loop()
{
  while (sserial.available() > 0)
  {
    if (gps.encode(sserial.read() ) )
    {
      Ausgabe();
    }
  }
}
