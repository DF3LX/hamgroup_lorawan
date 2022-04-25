/*
 * BME680 TTN Node on M5StackCore1 with COM.LoRaWAN module
 */

/*
 * Application and Version
 */
const char* Application  = "BME680_TTN_Node";
const char* Sw_Version = "1.00";
const char* Room_Screen  = "BME680 TTN Node";

/*
 * M5 STACK TFT Display
 * 
 */
#include <M5Stack.h>

/* 
 *  we use special fonts
 */
#include "Free_Fonts.h"

/* 
 *  backlight status
 */
bool Tft_Backlight = true; 

/*
 * TTN
 */
#include "ttn.h"
uint8_t MyData[12];

/*
 * BME680 with BOSCH BSEC library and TTN
 */
#include "bsec.h"
#include <Wire.h>
Bsec IaqSensor;

String Output;
String IAQStatus;
const char* IAQsts;

int M5Stack_Battery     = 0;

/*
 * Scheduler definitions
 */
const long Interval = 1000; /* ms */
unsigned long PreviousMillis = 0;

const long TTNInterval = 60000; /* ms */
unsigned long PreviousTTNMillis = 0;

const long TFTInterval = 60000; /* ms */
unsigned long PreviousTFTMillis = 0;

/*
 * send AT commands to the M5Stack COM.LoRaWAN Module 
 */
void ATCommand(char cmd[],char date[], uint32_t timeout = 50)
{
  char buf[256] = {0};
  if(date == NULL)
  {
    sprintf(buf,"AT+%s",cmd);
  }
  else 
  {
    sprintf(buf,"AT+%s=%s",cmd,date); 
  }
  Serial2.write(buf);
  delay(200);
  ReceiveAT(timeout);
}

/*
 * Receive AT messages from the M5Stack COM.LoRaWAN Module 
 */
bool ReceiveAT(uint32_t timeout)
{
  uint32_t nowtime = millis();
  while(millis() - nowtime < timeout){
    if (Serial2.available() !=0) {
      String str = Serial2.readString();
      if (str.indexOf("+OK") != -1 || str.indexOf("+ERROR") != -1) {
        Serial.println(str);
        return true;
      }else {
        Serial.println("[!] Syntax Error");
        break;
      }
    }
  }
  Serial.println("[!] Timeout");
  return false;
}

/*
 * build string out of payload data
 */
void ArrayToString(byte array[], unsigned int len, char buffer[])
{
  for (unsigned int i = 0; i < len; i++)
  {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len*2] = '\0';
}

/*
 * show tft display room screen
 */
void TftDisplayRoomScreen(String room_name, long int room_bg) 
{
  M5.Lcd.fillScreen(TFT_WHITE);
  M5.Lcd.fillRect(0,0,320,45,room_bg);
  
  M5.Lcd.setFreeFont(FMB18);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setCursor((320 - M5.Lcd.textWidth(room_name)) / 2, 32);
  M5.Lcd.print(room_name); 
  
  /* 
   * drawing verticle line 
   */
  M5.Lcd.drawFastVLine(150,50,190,TFT_DARKGREEN);
  
  /* 
   * drawing horizontal line 
   */
  M5.Lcd.drawFastHLine(0,100,320,TFT_DARKGREEN);

  /* 
   * drawing horizontal line 
   */
  M5.Lcd.drawFastHLine(0,155,320,TFT_DARKGREEN);
}


/*
 * show tft display status bar
 */
void TftDisplayStatusBar(String room_name, long int room_bg) 
{
  M5.Lcd.fillRect(0,0,320,45,room_bg);
  M5.Lcd.setFreeFont(FMB18);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setCursor((320 - M5.Lcd.textWidth(room_name)) / 2, 32);
  M5.Lcd.print(room_name); 
}

/*
 * show tft display sensor temperature
 */
void TftDisplayTemperature(float temperature) 
{
  M5.Lcd.setTextColor(TFT_BLACK);
  M5.Lcd.setFreeFont(FMB9);
  M5.Lcd.drawString("Temperature",15,45);
  M5.Lcd.setTextColor(TFT_BLACK);
  M5.Lcd.setFreeFont(FMB18);
  M5.Lcd.drawFloat(temperature,1,10,65);
  M5.Lcd.setFreeFont(FMB12);
  M5.Lcd.drawString("C",100,65);  
}

/*
 * show tft display update temperature
 */
void TftDisplayUpdateTemperature(float temperature) 
{
  M5.Lcd.fillRect(10,65,90,30,TFT_WHITE);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setFreeFont(FMB18);
  M5.Lcd.drawFloat(temperature,1,10,65);
}

/*
 * show tft display sensor pressure
 */
void TftDisplayPressure(int pressure) 
{
  M5.Lcd.setTextColor(TFT_BLACK);
  M5.Lcd.setFreeFont(FMB9);
  M5.Lcd.drawString("Pressure",190,45);
  M5.Lcd.setTextColor(TFT_BLACK);
  M5.Lcd.setFreeFont(FMB18);
  M5.Lcd.drawNumber(pressure,160,65);
  M5.Lcd.setFreeFont(FMB12);
  M5.Lcd.drawString("mBar",250,65);
}

/*
 * show tft display update pressure
 */
void TftDisplayUpdatePressure(int pressure) 
{
  M5.Lcd.fillRect(160,65,85,30,TFT_WHITE);
  M5.Lcd.setTextColor(TFT_BLACK);
  M5.Lcd.setFreeFont(FMB18);
  M5.Lcd.drawNumber(pressure,160,65);
}

/*
 * show tft display sensor humidity
 */
void TftDisplayHumidity(int humidity) 
{
  M5.Lcd.setTextColor(TFT_BLACK);
  M5.Lcd.setFreeFont(FMB9);
  M5.Lcd.drawString("Humidity",30,105);
  M5.Lcd.setTextColor(TFT_BLACK);
  M5.Lcd.setFreeFont(FMB18);
  M5.Lcd.drawNumber(humidity,50,125);
  M5.Lcd.setFreeFont(FMB12);
  M5.Lcd.drawString("%",105,125);
}

/*
 * show tft display update humidity  
 */
void TftDisplayUpdateHumidity(int humidity) 
{
  M5.Lcd.fillRect(50,125,50,30,TFT_WHITE);
  M5.Lcd.setTextColor(TFT_BLACK);
  M5.Lcd.setFreeFont(FMB18);
  M5.Lcd.drawNumber(humidity,50,125);
}

/*
 * show tft display sensor battery
 */
void TftDisplayBattery(int battery) 
{ 
  M5.Lcd.setTextColor(TFT_BLACK);
  M5.Lcd.setFreeFont(FMB9);
  M5.Lcd.drawString("Battery",200,105);
  M5.Lcd.setTextColor(TFT_BLACK);
  M5.Lcd.setFreeFont(FMB18);
  M5.Lcd.drawNumber(battery,200,125);
  M5.Lcd.setFreeFont(FMB12);
  M5.Lcd.drawString("%",280,125);
}

/*
 * show the battery level
 */
void TftDisplayUpdateBattery(int battery) 
{
  M5.Lcd.fillRect(200,125,70,30,TFT_WHITE);
  M5.Lcd.setTextColor(TFT_BLACK);
  M5.Lcd.setFreeFont(FMB18);
  M5.Lcd.drawNumber(battery,200,125);
}

/*
 * Display VOC and IAQ
 */
void TftDisplayVocIaq(int voc, String iaqstatus) 
{
  M5.Lcd.setTextColor(TFT_BLACK);
  M5.Lcd.setFreeFont(FMB9);
  M5.Lcd.drawString("VOC/IAQ",30,160);
  M5.Lcd.setTextColor(TFT_BLACK);
  M5.Lcd.setFreeFont(FMB18);
  M5.Lcd.drawFloat(voc,2,10,180);
  M5.Lcd.setFreeFont(FMB12);
  M5.Lcd.drawString("ppm",105,180);
  M5.Lcd.setFreeFont(FMB9);
  M5.Lcd.drawString(iaqstatus,10,210);
}

/*
 * Display voc (volatile organic compounds) and IAQ (indoor air quality
 */
void TftDisplayUpdateVocIaq(float voc, String iaqstatus) 
{
  M5.Lcd.fillRect(10,180,90,30,TFT_WHITE);
  M5.Lcd.setTextColor(TFT_BLACK);
  M5.Lcd.setFreeFont(FMB18);
  M5.Lcd.drawFloat(voc,2,10,180);

  M5.Lcd.fillRect(10,210,100,30,TFT_WHITE);
  M5.Lcd.setTextColor(TFT_BLACK);
  M5.Lcd.setFreeFont(FMB9);
  M5.Lcd.drawString(iaqstatus,10,210);
}

/*
 * Display the CO2 value in ppm
 */
void TftDisplayCo2(int co2) 
{
  M5.Lcd.setTextColor(TFT_BLACK);
  M5.Lcd.setFreeFont(FMB9);
  M5.Lcd.drawString("CO2",220,160);
  M5.Lcd.setTextColor(TFT_BLACK);
  M5.Lcd.setFreeFont(FMB18);
  M5.Lcd.drawNumber(co2,180,180);
  M5.Lcd.setFreeFont(FMB12);
  M5.Lcd.drawString("ppm",260,180);
}

/*
 * Display the CO2 value in ppm
 */
void TftDisplayUpdateCo2(int co2) 
{
  M5.Lcd.fillRect(180,180,80,30,TFT_WHITE);
  M5.Lcd.setTextColor(TFT_BLACK);
  M5.Lcd.setFreeFont(FMB18);
  M5.Lcd.drawNumber(co2,180,180);
}

/*
 * Calls the update functions for displaying the values
 */
void TftDisplayUpdateM5Stack(void) 
{
  /* 
   * display temperature sensor 
   */
  TftDisplayUpdateTemperature(IaqSensor.temperature);
  /* 
   * display pressure sensor 
   */
  TftDisplayUpdatePressure(round(IaqSensor.pressure / 100)); 
  /* 
   * display humidity sensor  
   */
  TftDisplayUpdateHumidity(IaqSensor.humidity); 
  /* 
   * display battery sensor 
   */
  TftDisplayUpdateBattery(M5Stack_Battery);
  /* 
   * display co2 sensor 
   */
  TftDisplayUpdateVocIaq(IaqSensor.breathVocEquivalent, IAQStatus);
  /* 
   * display co2 sensor  
   */
  TftDisplayUpdateCo2(IaqSensor.co2Equivalent);
}

/*
 * setup the display positions
 */
void TftDisplayM5Stack(void) 
{
  /* 
   * display room screen  
   */
  TftDisplayRoomScreen(Room_Screen,BLUE);   
  /* 
   * display temperature sensor  
   */
  TftDisplayTemperature(IaqSensor.temperature); 
  /* 
   * display pressure sensor  
   */
  TftDisplayPressure(round(IaqSensor.pressure) / 100);   
  /* 
   * display humidity sensor 
   */
  TftDisplayHumidity(IaqSensor.humidity); 
  /* 
   * display battery sensor  
   */
  TftDisplayBattery(M5Stack_Battery);
  /*
   * display VOC IAQ
   */
  TftDisplayVocIaq(IaqSensor.breathVocEquivalent, IAQStatus); 
  /* 
   * display Co2 sensor  
   */
  TftDisplayCo2(IaqSensor.co2Equivalent);   
}

/*
 * 
 */
void setup() 
{
  /* 
   * initialize the M5Stack object  
   */
  M5.begin();

  /*
    Power chip connected to gpio21, gpio22, I2C device
    Set battery charging voltage and current
    If used battery, please call this function in your project
  */
  M5.Power.begin();

  // open serial monitor
  // Serial.begin(115200);
 
  /* 
   * boot application 
   */
  delay(1000);
  Serial.println(F(" "));
  Serial.println(F(" "));
  Serial.println(F("Starting..."));
  Serial.print(F(Application)); 
  Serial.print(F(" Version ")); 
  Serial.println(F(Sw_Version));
  Serial.println(F(" ")); 

  M5.Lcd.fillScreen(TFT_BLUE);
  M5.Lcd.setFreeFont(FMB12);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setCursor((320 - M5.Lcd.textWidth(Room_Screen)) / 2, 100);
  M5.Lcd.print(Room_Screen);
  M5.Lcd.setCursor((320 - M5.Lcd.textWidth("Version x.xx")) / 2, 140);
  M5.Lcd.print("Version ");
  M5.Lcd.print(Sw_Version);
  delay(1000);
 
  /* 
   * Lcd display  
   */
  M5.Lcd.setBrightness(255); 

  /* 
   *  actual battery level
   */
  uint8_t bat = M5.Power.getBatteryLevel();
  Serial.print(F("[?] M5STACK BATT LEVEL  --> "));
  Serial.print(bat);
  Serial.println(F(" %"));
  M5Stack_Battery = bat;  

  /*
   * Init BME680 sensor
   */
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

  // now connect to the M%Stack COM.LoRaWAN module
  // TX 0/3/17
  // RX 5/15/16
  // Before you connect the module to your M5Stack device, make sure you set the TXD/RXD dip switches correctly. 
  // If you use the grey or basic and have nothing else connected, the default UART pin configuration 16/17 is fine. 
  // For the Fire you should use 13/5, as all other settings can interfere with internals of the Fire.
  //   
  // Serial2.begin(115200, SERIAL_8N1, 15, 13);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  
  /* 
   * first run - we have to get and programm some parameters  
   */
  ATCommand("LORAWAN", "?");
  delay(500);
  ATCommand("LORAWAN", "1");
  delay(500);

  /* 
   * your TTN access data 
   */
  ATCommand("DevEui", DevEui);
  delay(500);
  
  /* 
   * always the same for all devices 
   */
  ATCommand("AppEui", AppEui);
  delay(500);
  
  ATCommand("AppKey", AppKey);
  delay(500);

  Serial.println("Join=1");
  ATCommand("Join", "1");

  /* 
   * display sensor data 
   */
  TftDisplayM5Stack();   
}

/*
 * Cyclic loop 
 */
void loop()
{ 
   unsigned long currentmillis = millis();
    
  /* 
   * check if some one has pressed a button 
   */
  if (M5.BtnA.wasPressed()) 
  {
    PreviousTFTMillis = currentmillis;
    
    Serial.println("[x] Button A was pressed - Display ON/OFF");
    Tft_Backlight = !Tft_Backlight;   
        
    /* 
     * Turning off the LCD backlight 
     */
    if (Tft_Backlight == false) {  
      M5.Lcd.sleep();  
      M5.Lcd.setBrightness(0); 
    }
    
    /* 
     * Turning on the LCD backlight  
     */
    if (Tft_Backlight == true)  { 
      M5.Lcd.wakeup(); 
      M5.Lcd.setBrightness(255); 
    }
    delay(200);
  }

  /* 
   * check if some one has pressed a button 
   */
  if (M5.BtnB.wasPressed()) 
  {
    PreviousTFTMillis = currentmillis;
    Serial.println("[x] Button B was pressed."); 
    
    /* 
     * Turning on the LCD backlight 
     */
    Tft_Backlight = true;
    if (Tft_Backlight == true)  { 
      M5.Lcd.wakeup(); 
      M5.Lcd.setBrightness(255); 
    }  
  }

  // check if some one has pressed a button
  if (M5.BtnC.wasPressed()) 
  {
    PreviousTFTMillis = currentmillis;
    Serial.println("[x] Button C was pressed");
    
    /* 
     * Turning on the LCD backlight 
     */
    Tft_Backlight = true;
    if (Tft_Backlight == true)  { 
      M5.Lcd.wakeup(); 
      M5.Lcd.setBrightness(255); 
    }
  }

  /* 
   * It is checked whether the time for the transmission interval has already expired
   * If the time difference between the last save and the current time is greater
   * as the interval, the following function is executed.
  */
  if (currentmillis - PreviousTTNMillis >= TTNInterval) { 
      PreviousTTNMillis = currentmillis;       
      SendToTTN();
  }
   
  /*
   * Check for new data
   */
  if (IaqSensor.run()) {
    if (currentmillis - PreviousMillis >= Interval) { 
      PreviousMillis = currentmillis; 
      
      EvaluateIAQ();
  
      /* 
       * display sensor data 
       */
      TftDisplayUpdateM5Stack();  
      PrintValues();
    }
  }
  
  /* 
   * TFT goes to sleep after 1 minute 
   */
  if (currentmillis - PreviousTFTMillis >= TFTInterval) {
    if (Tft_Backlight == true) { 
      Tft_Backlight=false; 
    }   
    
    /* 
     * Turning off the LCD backlight 
     */
    if (Tft_Backlight == false) 
    {
      M5.Lcd.sleep();  
      M5.Lcd.setBrightness(0);
    }   
  }   
 
  M5.update();
}

/*
 * Send data to TTN
 */
void SendToTTN(void) 
{  
  /* 
   * activate communication 
   */
  Serial.println(F("[!] LoraSet=?")); 
  ATCommand("LoraSet", "?");
  delay(500);
  
  Serial.println(F(" "));
  Serial.print(F(Application)); Serial.print(F(" Version ")); Serial.println(F(Sw_Version));
  Serial.println(F(" "));

  /* 
   * check if we can access battery functions 
   */
  if(!M5.Power.canControl())
  {
    Serial.println(F("[!] No communication with IP5306 chip"));
  }

  /* 
   * actual battery level  
   */
  uint8_t bat = M5.Power.getBatteryLevel();
  Serial.print(F("[?] M5STACK BATT LEVEL  --> "));
  Serial.print(bat);
  Serial.println(F(" %"));
  M5Stack_Battery = bat;
  int32_t battery_int = bat * 100; 
    
  PackData();

  Serial.print(F("[x] actual TTN payload  --> "));
  char str[32] = "";
  ArrayToString(MyData, 12, str);
  Serial.println(str);

  /* 
   * now send all to TTN 
   */
  ATCommand("SendHex", str);  
}


/*
 * Translation value to text according to the table 4 BME680_ED-EN.pdf
 */
void EvaluateIAQ(void){
  if ((IaqSensor.staticIaq > 0)  && (IaqSensor.staticIaq  <= 50)) {
    IAQStatus = "Good";
  }
  if ((IaqSensor.staticIaq > 51)  && (IaqSensor.staticIaq  <= 100)) {
    IAQStatus = "Average";
  }
  if ((IaqSensor.staticIaq > 101)  && (IaqSensor.staticIaq  <= 150)) {
    IAQStatus = "Little Bad";
  }
  if ((IaqSensor.staticIaq > 151)  && (IaqSensor.staticIaq  <= 200)) {
    IAQStatus = "Bad";
  }
  if ((IaqSensor.staticIaq > 201)  && (IaqSensor.staticIaq  <= 300)) {
    IAQStatus = "Worse";
  }
  if ((IaqSensor.staticIaq > 301)  && (IaqSensor.staticIaq  <= 500)) {
    IAQStatus = "Very Bad";
  }
  if ((IaqSensor.staticIaq > 500)){
    IAQStatus = "Very Very Bad";
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
 * Evaluate the sensor status
 */
void CheckIaqSensorStatus(void){
  
  if (IaqSensor.status != BSEC_OK) {
    if (IaqSensor.status < BSEC_OK) {
      Output = "BSEC error code : " + String(IaqSensor.status);
      Serial.println(Output);
    } else {
      Output = "BSEC warning code : " + String(IaqSensor.status);
      Serial.println(Output);
    }
  }

  if (IaqSensor.bme680Status != BME680_OK) {
    if (IaqSensor.bme680Status < BME680_OK) {
      Output = "BME680 error code : " + String(IaqSensor.bme680Status);
      Serial.println(Output);
    } else {
      Output = "BME680 warning code : " + String(IaqSensor.bme680Status);
      Serial.println(Output);
    }
  }
}

/*
 * Pack data for TTN
 */
void PackData(void){
  uint16_t temperature = (IaqSensor.temperature + 100) * 100;
  uint16_t humidity = IaqSensor.humidity * 100;
  uint32_t iaq = IaqSensor.staticIaq;
  uint16_t pressure = round(IaqSensor.pressure / 10);
  uint16_t co2 = round(IaqSensor.co2Equivalent);
  uint16_t voc = IaqSensor.breathVocEquivalent * 100;
  
  MyData[0] = temperature &  0x00FF;
  MyData[1] = (temperature & 0xFF00) >> 8;
  MyData[2] = humidity & 0x00FF;
  MyData[3] = (humidity & 0xFF00) >> 8;
  MyData[4] = pressure & 0x00FF;
  MyData[5] = (pressure & 0xFF00) >> 8;

  if(IaqSensor.iaqAccuracy > 0){
    MyData[6] = iaq & 0x00FF;
    MyData[7] = (iaq & 0xFF00) >> 8;
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
