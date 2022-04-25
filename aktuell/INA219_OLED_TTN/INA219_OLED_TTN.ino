/*
 * INA219 based on Adafruit library example
 */
#include <Wire.h>

/* 
 * Adafruit libraries 
 */
#include <Adafruit_INA219.h>

/*
 * INA219 instance
 */
Adafruit_INA219 ina219;

/*
 * OLED library
 */
#include "SSD1306Wire.h"
SSD1306Wire display(0x3c, SDA, SCL); 

/*
 * global variables
 */
float g_Shuntvoltage = 0;
float g_Busvoltage = 0;
float g_Current_mA = 0;
float g_Loadvoltage = 0;
float g_Power_mW = 0;
  
/*
 * Prototypes 
 */
void PrintValues(void);
void DisplayValues(void);

/*
 * LoRa Module
 */
#include <SPI.h>

/*
 * TTN LMIC includes
 */
#include <lmic.h>                                 
#include <hal/hal.h>                     

/*
 * LoRaWAN
 */
#include "ttn.h"
uint8_t MyData[16];

/*
 * scheduling variables
 */
const long Interval = 1000; /* ms */
unsigned long PreviousMillis = 0;

const long TTNInterval = 60000; /* ms */
unsigned long PreviousTTNMillis = 0;

/*
 * If PLOTTER is defined, the serial output is used for the Arduino Plotter visualisation
 */
//#define PLOTTER

/*
 * Entry point
 */
void setup(void){

  Serial.begin(115200);
  
#ifndef PLOTTER
  Serial.println(F("Starting..."));
#endif /* PLOTTER */

  // Initialize the INA219.
  // By default the initialization will use the largest range (32V, 2A).  However
  // you can call a setCalibration function to change this range (see comments).
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
  
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();

#ifndef PLOTTER
  Serial.println("Measuring voltage and current with INA219 ...");
#endif /* PLOTTER */

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
  
  display.init();                                                                                 
  display.flipScreenVertically();
  display.setFont( ArialMT_Plain_16 );
  display.clear();
  display.display();
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
void loop(void) {
  
  /*
  * LMIC runloop
  */
  os_runloop_once();

  unsigned long currentmillis = millis();

  /* 
   *  Check if timer expired
   */
  if (currentmillis - PreviousMillis >= Interval) { 
      PreviousMillis = currentmillis; 
      
    /*
     * Start measuring
    */
    g_Shuntvoltage = ina219.getShuntVoltage_mV();
    g_Busvoltage = ina219.getBusVoltage_V();
    g_Current_mA = ina219.getCurrent_mA();
    g_Power_mW = ina219.getPower_mW();
    g_Loadvoltage = g_Busvoltage + (g_Shuntvoltage / 1000);
    
#ifndef PLOTTER
    /*
     * Use PrintValues for "normal" output
     */
    PrintValues();
#else
    /*
     * Use PrintValuesPlotter if Arduino IDE Plotter shall be used
     */
    PrintValuesPlotter();
#endif /* PLOTTER */

    DisplayValues();
   
    if (currentmillis - PreviousTTNMillis >= TTNInterval) { 
      PreviousTTNMillis = currentmillis;       
      SendToTTN();
    }
  }
}

/*
 * Output to serial interface
 */
void PrintValues(void){
  Serial.print("Bus Voltage:   "); Serial.print(g_Busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(g_Shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(g_Loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(g_Current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(g_Power_mW); Serial.println(" mW");
  Serial.println("");
}


/*
 * Output to serial interface formatted for plotter usage
 */
void PrintValuesPlotter(void){
  Serial.print("Bus_Voltage:"); Serial.print(g_Busvoltage); Serial.print("\t");
  Serial.print("Shunt_Voltage:"); Serial.print(g_Shuntvoltage); Serial.print("\t");
  Serial.print("Load_Voltage:"); Serial.print(g_Loadvoltage); Serial.print("\t");
  Serial.print("Current:"); Serial.print(g_Current_mA); Serial.print("\t");
  Serial.print("Power:"); Serial.println(g_Power_mW); 
}

/*
 * Output to OLED display
 */
void DisplayValues(void){

  display.clear();
  String tempString;

  display.setFont( ArialMT_Plain_10 );
  tempString = "Bus Voltage:   " + String(g_Busvoltage, 2 ) + " V";
  display.drawString( 0, 0, tempString );
  
  tempString = "Shunt Voltage: " + String(g_Shuntvoltage, 2 ) + " mV";
  display.drawString( 0, 12, tempString );

  tempString = "Load Voltage:  " + String(g_Loadvoltage, 2 ) + " V";
  display.drawString( 0, 24, tempString );

  tempString = "Current:       " + String(g_Current_mA, 2 ) + " mA";
  display.drawString( 0, 36, tempString );

  tempString = "Power:         " + String(g_Power_mW, 2 ) + " mW";
  display.drawString( 0, 48, tempString );
  
  display.display();
}

/*
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

/*
 * Send data to the TTN network
 */
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
  
  uint16_t busvoltage = g_Busvoltage * 1000;
  int32_t shuntvoltage = g_Shuntvoltage * 1000;  
  uint16_t loadvoltage = g_Loadvoltage * 1000;
  int32_t current_mA = g_Current_mA * 1000;
  uint32_t power_mW = g_Power_mW * 1000;

  MyData[0] = busvoltage & 0x00FF;
  MyData[1] = (busvoltage & 0xFF00) >> 8;
  
  MyData[2] = shuntvoltage & 0x00FF;
  MyData[3] = (shuntvoltage & 0xFF00) >> 8;
  MyData[4] = (shuntvoltage & 0x00FF0000) >> 16;
  MyData[5] = (shuntvoltage & 0xFF000000) >> 24;
  
  MyData[6] = loadvoltage & 0x00FF;
  MyData[7] = (loadvoltage & 0xFF00) >> 8; 
  
  MyData[8] = current_mA & 0x000000FF;
  MyData[9] = (current_mA & 0x0000FF00) >> 8;
  MyData[10] = (current_mA & 0x00FF0000) >> 16;
  MyData[11] = (current_mA & 0xFF000000) >> 24;
  
  MyData[12] = power_mW & 0x000000FF;
  MyData[13] = (power_mW & 0x0000FF00) >> 8;
  MyData[14] = (power_mW & 0x00FF0000) >> 16;
  MyData[15] = (power_mW & 0xFF000000) >> 24;
}
