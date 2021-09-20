/****************************************
  Blink

  DARC-HamGroup LoRaWAN

  eine blinkende LED als Hello-World-Programm  

  20.09.2021  Jürgen, DL8MA

  http://www.p37.de/LoRaWAN
  
*/

#define LED_PIN   25                          // I/O-Pin über für die eingebaute LED

#define ON        500                         //  Einschaltzeit in ms 
#define OUT       500                         //  Ausschaltzeit in ms

void setup() {
  pinMode(LED_PIN , OUTPUT);                  // Port-Pin als Ausgabe schalten
}

void loop() {
  digitalWrite(LED_PIN , HIGH);               // LED einschalten
  delay( ON );                       
  digitalWrite(LED_PIN , LOW);                // LED ausschalten
  delay( OUT );                       
}
