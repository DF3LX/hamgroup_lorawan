/*
 * BME680 with BOSCH BSEC library and TTN
 */
#include "bsec.h"
#include <Wire.h>

/*
 * TTN LMIC includes
 */
#include <lmic.h>
#include <hal/hal.h>

/*
 * LoRaWAN
 */
#include "ttn.h"
uint8_t MyData[12];

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
void SendToTTN(void);
void PackData(void);

/*
 * BME680 Instance
 */
Bsec IaqSensor;

String Output;
String IAQStatus;
const char* IAQsts;

const long Interval = 1000; /* ms */
unsigned long PreviousMillis = 0;

const long TTNInterval = 60000; /* ms */
unsigned long PreviousTTNMillis = 0;
      
/*
 * Entry point
 */
void setup(void){

  Serial.begin(115200);
  delay(100);
  Wire.begin();
 
  Serial.println(F("Starting..."));
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  /* 
   *  LMIC init
   */
  SPI.begin(5, 19, 27, 18);
  
  /*
   * LMIC OS init
   */
  os_init();
  
  /* 
   *  Reset the MAC state. Session and pending data transfers will be discarded.
   */
  LMIC_reset();
  
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
 * Pin mapping
 * TTGO LORA32 T3_V1.6.1 20210104 und T3_V1.6 20180606
 */
const lmic_pinmap lmic_pins = {                          
    .nss = 18,
    .rxtx = LMIC_UNUSED_PIN,             
    .rst = 23,
    .dio = {26, 33, 32},
};

/*
 * endles loop
 */
void loop(void){

  /*
  * LMIC runloop
  */
  os_runloop_once();
  
  unsigned long currentmillis = millis();
  
  /*
   * Check for new data
   */
  if (IaqSensor.run()) {

    if (currentmillis - PreviousMillis >= Interval) { 
      PreviousMillis = currentmillis; 
      
      EvaluateIAQ();
      
      DisplayValues();
      PrintValues();
    }

    if (currentmillis - PreviousTTNMillis >= TTNInterval) { 
      PreviousTTNMillis = currentmillis;       
      SendToTTN();
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

/*********************************
 * Event-Handler
 */
void onEvent (ev_t ev) {
  Serial.print(os_getTime());
  Serial.print(": ");
  switch (ev) {
    case EV_SCAN_TIMEOUT:
      Serial.println(F("EV_SCAN_TIMEOUT"));
      break;
    case EV_BEACON_FOUND:
      Serial.println(F("EV_BEACON_FOUND"));
      break;
    case EV_BEACON_MISSED:
      Serial.println(F("EV_BEACON_MISSED"));
      break;
    case EV_BEACON_TRACKED:
      Serial.println(F("EV_BEACON_TRACKED"));
      break;
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
    break;
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      // Disable link check validation (automatically enabled
      // during join, but not supported by TTN at this time).
      LMIC_setLinkCheckMode(0);
      break;
    case EV_RFU1:
      Serial.println(F("EV_RFU1"));
      break;
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      break;
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      if (LMIC.txrxFlags & TXRX_ACK)
        Serial.println(F("Received ack"));
      if (LMIC.dataLen) {
        Serial.println(F("Received "));
        Serial.println(LMIC.dataLen);
        Serial.println(F(" bytes of payload"));
      }
      break;
    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      break;
    case EV_LINK_DEAD:
      Serial.println(F("EV_LINK_DEAD"));
      break;
    case EV_LINK_ALIVE:
      Serial.println(F("EV_LINK_ALIVE"));
      break;
    case EV_TXSTART:
      Serial.println(F("EV_TXSTART"));
      break;
    default:
      Serial.println(F("Unknown event"));
      break;
  }
}

void SendToTTN(void){ 
  PackData();
      
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    // Prepare upstream data transmission at the next possible time.
    LMIC_setTxData2(1, MyData, sizeof(MyData), 0);
    Serial.println(F("Packet queued"));
  }
}  

/*
 * Pack data for TTN
 */
void PackData(void){
      
  uint16_t temperature = (IaqSensor.temperature + 100) * 100;
  uint16_t humidity = IaqSensor.humidity * 100;
  uint32_t ia = IaqSensor.staticIaq;
  uint16_t pressure = round(IaqSensor.pressure / 10);
  uint16_t co2 = round(IaqSensor.co2Equivalent);
  uint16_t voc = IaqSensor.breathVocEquivalent * 100;
  
  MyData[0] = temperature & 0x00FF;
  MyData[1] = (temperature & 0xFF00) >> 8;
  MyData[2] = humidity & 0x00FF;
  MyData[3] = (humidity & 0xFF00) >> 8;
  MyData[4] = pressure & 0x00FF;
  MyData[5] = (pressure & 0xFF00) >> 8;
  
  if(IaqSensor.iaqAccuracy > 0){
    MyData[6] = ia & 0x00FF;
    MyData[7] = (ia & 0xFF00) >> 8;
    MyData[8] = co2 & 0x00FF;
    MyData[9] = (co2 & 0xFF00) >> 8;
    MyData[10] = voc & 0x00FF;
    MyData[11] = (voc & 0xFF00) >> 8;  
  }
  else
  {
    MyData[6] = 0;
    MyData[7] = 0;
    MyData[8] = 0;
    MyData[9] = 0;
    MyData[10] = 0;
    MyData[11] = 0;  
  }
}
