/****************************************
  Blink

  DARC-HamGroup LoRaWAN

  Hello World - Ausgabe über serielle Schnittstelle
  
  20.09.2021  Jürgen, DL8MA

  http://www.p37.de/LoRaWAN
  
*/

#define PAUSE   1000

int   zaehler = 0;

void setup() {
  Serial.begin(9600);
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

  Serial.print( zaehler );
  Serial.println( ". Hello World" );

  zaehler++;

  delay( PAUSE );
}
