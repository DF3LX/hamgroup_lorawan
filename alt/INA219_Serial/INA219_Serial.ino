
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

/*
 * scheduling variables
 */
const long interval = 500; /* ms */
unsigned long previousMillis = 0;

/*
 * Entry point
 */
void setup(void){

  Serial.begin(115200);
  Serial.println(F("Starting..."));
 
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

  Serial.println("Measuring voltage and current with INA219 ...");
}

/*
 * endles loop
 */
void loop(void) {
    
  unsigned long currentMillis  = millis();

  /* 
   *  Check if timer expired
   */
  if (currentMillis - previousMillis >= interval) { 
      previousMillis = currentMillis; 
      
    /*
     * Start measuring
    */
    g_Shuntvoltage = ina219.getShuntVoltage_mV();
    g_Busvoltage = ina219.getBusVoltage_V();
    g_Current_mA = ina219.getCurrent_mA();
    g_Power_mW = ina219.getPower_mW();
    g_Loadvoltage = g_Busvoltage + (g_Shuntvoltage / 1000);
    
    PrintValues();
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
