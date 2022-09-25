/*
  Hier wird das Display initiert.
  Und die gewünschten Rohdaten des GPS-Bausteines in Strings gewandelt.
  Diese Zeichenketten werden dann, immer, auf dem Display und, nach Wunsch, auf der seriellen Schnittstelle(USB) ausgegeben.
*/

#include "SSD1306Wire.h"                 // ThingPulse OLED SSD1306 - Library        https://github.com/ThingPulse/esp8266-oled-ssd1306

/* globale Variablen */
char laenge[11];                         // Längengrad
char breite[11];                         // Breitengrad
char hoehe[8];                           // Höhe über NN
char satanzahl[5];                       // empfangende Satelliten
char datum[11];                          // Datum
char uhrzeit[9];                         // Uhrzeit in UTC
char meine_hoehe[8];                     // Hoehe ueber Meeresspiegel

SSD1306Wire display(0x3c, SDA, SCL);

TinyGPSCustom ns(gps, "GPGGA", 3);       // Benutzerdefiniertes Feld für die Nord/Sued Anzeige
TinyGPSCustom ew(gps, "GPGGA", 5);       // Benutzerdefiniertes Feld für die Ost/West Anzeige

uint8_t initDisplay(void)
{
  if (display.init() == false)           // Konfiguration Display
    return (false);
  display.flipScreenVertically();
  display.setFont( ArialMT_Plain_16 );
  display.setBrightness(100);
  display.clear();
  display.drawString( 30, 0, "HamGroup");
  display.drawString( 30, 20, "LoRaWAN");
  display.drawString( 30, 40, "GPS-RX");
  display.display();
  delay(2000);
  display.setFont( ArialMT_Plain_10 );
  return (true);
}

void Ausgabe(void)
{
  char buffer[30];                      // Hilfsbuffer für drawStringf

  if (gps.date.isUpdated())             // Daten wurden erneuert.
  {
    snprintf(datum, 11, "%02d.%02d.%d", gps.date.day(), gps.date.month(), gps.date.year());
    snprintf(uhrzeit, 9, "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
    display.clear();
    display.drawStringf(0, 0, buffer, "%s   %s UTC", datum, uhrzeit);

#ifdef DEBUG
    Serial.printf("%s  ", datum);
    Serial.printf("%s  ", uhrzeit);
#endif

    if (gps.location.isUpdated() )
    {
      snprintf(laenge, 11, "%.6f%s", gps.location.lat(), ns.value());
      snprintf(breite, 11, "%.6f%s", gps.location.lng(), ew.value());
      snprintf(hoehe, 8, "%.2f", gps.altitude.meters());
      snprintf(satanzahl, 5, "%i", gps.satellites.value());

      display.drawStringf(0, 12, buffer, "Laenge          %s", laenge);
      display.drawStringf(0, 24, buffer, "Breite              %s", breite);
      display.drawStringf(0, 36, buffer, "Hoehe                 %s", hoehe);
      display.drawStringf(0, 48, buffer, "Anzahl  %s", satanzahl);
      display.display();

#ifdef DEBUG
      Serial.printf("Längengrad: %s  ", laenge);
      Serial.printf("Breitengrad: %s  ", breite);
      Serial.printf("Höhe: %s m", hoehe);
      Serial.printf("  Satelitten: %s\n", satanzahl);
#endif
    }
    else
    {

#ifdef DEBUG
      Serial.println("Keine Daten");
#endif

      display.drawString(30, 25, "Keine Daten");
      display.display();
    }
  }
}
