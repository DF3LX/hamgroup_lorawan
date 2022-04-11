/*
 * BME680 with BOSCH BSEC library
 */
#include "bsec.h"
#include <Wire.h>

/*
 * OLED library
 */
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

/*
 * Prototypes 
 */
void CheckIaqSensorStatus(void);
void ErrLeds(void);
void PrintValues(void);
void DisplayValues(void);

/*
 * BME680 Instance
 */
Bsec IaqSensor;

String Output;
String IAQStatus;
const char* IAQsts;

const long interval = 1000; /* ms */
unsigned long previousMillis = 0;

/*
 * Entry point
 */
void setup(void){

  Serial.begin(115200);
  delay(100);
  Wire.begin();
 
  Serial.println(F("Starting..."));
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  Serial.println(F("BME680"));
 
  display.display();
  delay(100);
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setRotation(0);

  IaqSensor.begin(BME680_I2C_ADDR_SECONDARY, Wire);
  Output = "\nBSEC library version " + String(IaqSensor.version.major) + "." + String(IaqSensor.version.minor) + "." + String(IaqSensor.version.major_bugfix) + "." + String(IaqSensor.version.minor_bugfix);
  Serial.println(Output);
  CheckIaqSensorStatus();

  bsec_virtual_sensor_t sensorList[10] = {
    BSEC_OUTPUT_RAW_TEMPERATURE,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_RAW_HUMIDITY,
    BSEC_OUTPUT_RAW_GAS,
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_STATIC_IAQ,
    BSEC_OUTPUT_CO2_EQUIVALENT,
    BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };

  IaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
  CheckIaqSensorStatus();
}

/*
 * endles loop
 */
void loop(void){
  
  unsigned long currentMillis = millis();
  
  /*
   * Check for new data
   */
  if (IaqSensor.run()) {

    if (currentMillis - previousMillis >= interval) { 
      previousMillis = currentMillis; 
      
      int16_t temperature = IaqSensor.temperature * 100;
      int16_t humidity = IaqSensor.humidity * 100;
      int32_t ia = IaqSensor.staticIaq * 100;
      int16_t pressure = round(IaqSensor.pressure / 10);
      int16_t co2 = round(IaqSensor.co2Equivalent);
      int16_t voc = IaqSensor.breathVocEquivalent*100;
      
      EvaluateIAQ();
      
      DisplayValues();
      PrintValues();
    }
  } else {
    CheckIaqSensorStatus();
  }
}

/*
 * Evaluate sensor status
 */
void CheckIaqSensorStatus(void){
  
  if (IaqSensor.status != BSEC_OK) {
    if (IaqSensor.status < BSEC_OK) {
      Output = "BSEC error code : " + String(IaqSensor.status);
      Serial.println(Output);
      for (;;)
        ErrLeds(); /* Halt in case of failure */
    } else {
      Output = "BSEC warning code : " + String(IaqSensor.status);
      Serial.println(Output);
    }
  }

  if (IaqSensor.bme680Status != BME680_OK) {
    if (IaqSensor.bme680Status < BME680_OK) {
      Output = "BME680 error code : " + String(IaqSensor.bme680Status);
      Serial.println(Output);
      for (;;)
        ErrLeds(); /* Halt in case of failure */
    } else {
      Output = "BME680 warning code : " + String(IaqSensor.bme680Status);
      Serial.println(Output);
    }
  }
}

/*
 * Translation value to text according to the table 4 BME680_ED-EN.pdf
 */
void EvaluateIAQ(void){
  if ((IaqSensor.staticIaq > 0)  && (IaqSensor.staticIaq  <= 50)) {
    IAQStatus = "IAQ: Good";
  }
  if ((IaqSensor.staticIaq > 51)  && (IaqSensor.staticIaq  <= 100)) {
    IAQStatus = "IAQ: Average";
  }
  if ((IaqSensor.staticIaq > 101)  && (IaqSensor.staticIaq  <= 150)) {
    IAQStatus = "IAQ: Little Bad";
  }
  if ((IaqSensor.staticIaq > 151)  && (IaqSensor.staticIaq  <= 200)) {
    IAQStatus = "IAQ: Bad";
  }
  if ((IaqSensor.staticIaq > 201)  && (IaqSensor.staticIaq  <= 300)) {
    IAQStatus = "IAQ: Worse";
  }
  if ((IaqSensor.staticIaq > 301)  && (IaqSensor.staticIaq  <= 500)) {
    IAQStatus = "IAQ: Very Bad";
  }
  if ((IaqSensor.staticIaq > 500)){
    IAQStatus = "IAQ: Very Very Bad";
  }  
}

/*
 * Output to serial interface
 */
void PrintValues(void){
  Serial.print("Temperature = "); 
  Serial.print(IaqSensor.temperature); 
  Serial.println(" *C");

  Serial.print("Pressure = "); 
  Serial.print(IaqSensor.pressure / 100.0); 
  Serial.println(" hPa");

  Serial.print("Humidity = "); 
  Serial.print(IaqSensor.humidity); 
  Serial.println(" %");

  Serial.print("IAQ = "); 
  Serial.print(IaqSensor.staticIaq); 
  Serial.println(" PPM");

  Serial.print("CO2 equiv = "); 
  Serial.print(IaqSensor.co2Equivalent); 
  Serial.println(" PPM");

  Serial.print("Breath VOC = "); 
  Serial.print(IaqSensor.breathVocEquivalent); 
  Serial.println(" PPM");

  Serial.print(IAQStatus);
  
  Serial.println();
}

/*
 * Output to OLED display
 */
void DisplayValues(void){

  display.setCursor(0,0);
  display.clearDisplay();
  
  display.print("Temperature: "); 
  display.print(IaqSensor.temperature); 
  display.println(" *C");
 
  display.print("Pressure: "); 
  display.print(IaqSensor.pressure / 100); 
  display.println(" hPa");
 
  display.print("Humidity: "); 
  display.print(IaqSensor.humidity); 
  display.println(" %");
 
  display.print("IAQ: "); 
  display.print(IaqSensor.staticIaq); 
  display.println(" PPM"); 

  display.print("CO2eq: "); 
  display.print(IaqSensor.co2Equivalent); 
  display.println(" PPM");
 
  display.print("VOC: "); 
  display.print(IaqSensor.breathVocEquivalent); 
  display.println(" PPM");

  display.print(IAQStatus);
  
  display.display();
}

/*
 * LED blink
 */
void ErrLeds(void){
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}
