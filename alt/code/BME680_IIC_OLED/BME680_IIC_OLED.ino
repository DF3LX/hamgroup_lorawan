/*
 * BME680 with Adafruit library
 */
#include <Wire.h>
#include <SPI.h>

/* 
 * Adafruit libraries 
 */
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

/*
 * OLED library
 */
#include "SSD1306Wire.h"
SSD1306Wire display(0x3c, SDA, SCL); 

/*
 * Prototypes 
 */
void PrintValues(void);
void DisplayValues(void);

/*
 * BME680 Instance
 */
Adafruit_BME680 bme;

const long interval = 5000; /* ms */
unsigned long previousMillis = 0;

/*
 * Entry point
 */
void setup(void){

  Serial.begin(115200);
  Serial.println(F("Starting..."));
  Serial.println(F("BME680"));
  
  if (!bme.begin())
  {
      Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
      while (1)
          ;
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  display.init();                                                                                 
  display.flipScreenVertically();
  display.setFont( ArialMT_Plain_16 );
  display.clear();
  display.display(); 
}

/*
 * endles loop
 */
void loop(void){
  
  unsigned long currentMillis  = millis();

  /* 
   *  Check if timer expired
   */
  if (currentMillis - previousMillis >= interval) { 
      previousMillis = currentMillis; 
      
    /*
     * Start measuring
     */
    unsigned long endTime = bme.beginReading();
  
    if (endTime == 0) {
      Serial.println(F("Failed to begin reading :("));
      return;
    }
    
    if (!bme.endReading()) {
      Serial.println(F("Failed to complete reading :("));
      return;
    }
      
    DisplayValues();   
    PrintValues();
  }
}

/*
 * Output to serial interface
 */
void PrintValues(void){
  Serial.print(F("Temperature = "));
  Serial.print(bme.temperature);
  Serial.println(F(" *C"));

  Serial.print(F("Pressure = "));
  Serial.print(bme.pressure / 100.0);
  Serial.println(F(" hPa"));

  Serial.print(F("Humidity = "));
  Serial.print(bme.humidity);
  Serial.println(F(" %"));

  Serial.print(F("Gas = "));
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(F(" kOhms"));
}

/*
 * Output to OLED display
 */
void DisplayValues(void){

  display.clear();
  String tempString;

  display.setFont( ArialMT_Plain_16 );
  tempString = String( bme.temperature, 2 ) + " ºC";
  display.drawString( 0, 0, tempString );
  
  tempString = String( bme.pressure/100.0, 2 ) + " hPa";
  display.drawString( 0, 15, tempString );

  tempString = String( bme.humidity, 2 ) + " %";
  display.drawString( 0, 30, tempString );

  tempString = String( bme.gas_resistance / 1000.0, 2 ) + " kOhm";
  display.drawString( 0, 45, tempString );
  
  display.display();
}
