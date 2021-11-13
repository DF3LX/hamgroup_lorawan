/****************************************
  Blink

  DARC-HamGroup LoRaWAN

  Hello World - Ausgabe über serielle Schnittstelle und blinkende LED
  
  20.09.2021  Jürgen, DL8MA

  http://www.p37.de/LoRaWAN
  
*/

#define PAUSE   1000

#define LED_PIN   25                          // I/O-Pin über für die eingebaute LED


int   zaehler = 0;
int   led = 0;                                // LED-Steuerung

void setup() {

  pinMode(LED_PIN , OUTPUT);                  // Port-Pin als Ausgabe schalten  

  Serial.begin(9600);                         // serieller Monitor auf 9600 Baud einstellen!
  while (!Serial) {
    ; 
  }

  delay( 100 );
  Serial.println("");
  Serial.println("DARC-HamGroup LoRaWAN");
  Serial.println("");
  delay( 100 );
}


void loop() {
  digitalWrite(LED_PIN , led);               // LED umschalten
  led = !led;
  
  Serial.print( zaehler );
  Serial.println( ". Hello World" );

  zaehler++;

  delay( PAUSE );
}
