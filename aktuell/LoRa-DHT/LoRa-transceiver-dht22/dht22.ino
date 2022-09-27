/*
   Hier wird der DHT-Baustein eingerichtet.
   Die Daten abgefragt und in einen Sendestring verpackt.
   Und via LoRa versendet.
*/
#include "DHT.h"

#define DHTPIN 15
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void init_dht(void)
{
  dht.begin();
  return;
}

void read_dht(void)
{
  float h, t;

  h = dht.readHumidity();
  t = dht.readTemperature();
  if (isnan(h) || isnan(t))
  {
#ifdef DEBUG
    Serial.println("Failed to read from DHT sensor!");
#endif
    fehler_led(1000, 4);
    return;
  }
#ifdef DEBUG
  Serial.print("Temperatur: ");
  Serial.print(t);
  Serial.print(" °C ");
  Serial.print("Feuchtigkeit: ");
  Serial.print(h);
  Serial.println(" %H");
#endif
  /*  Der sendestring setzt sich folgendermassen zusammen.
      Kennung des Senders, Paketnummer, Temperatur und Feuchtihkeit.
      Alles Werte werden durch ein ';' getrennt.
  */
  snprintf(sendestring, 24, "001;%d;%.2f;%.2f;", zaehler, t, h);
  return;
}

void sendedaten(void)
{
#ifdef DEBUG
  display.clear();
  display.drawString(0, 0, "LoRa Transceiver DHT");
  display.drawString(0, 20, "sende Paket:");
  display.drawString(0, 30,  sendestring);
  display.display();

  Serial.println(sendestring);
#endif
  LoRa.beginPacket();
  LoRa.print(sendestring);
  LoRa.endPacket(0);

  zaehler++;
  return;
}
