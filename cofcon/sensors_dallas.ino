//
// COFCON Controller for Espresso Machines
// 2014 by Roman Schmitz
//
// Code for: 
// Dallas 1-Wire Temperature-Sensors (DS18x20) using the DallasTemperature and OneWire Libraries for Arduino
//

#ifndef SIMULATION_MODE
#ifdef SENS_DALLAS

#include <OneWire.h>
#include <DallasTemperature.h>

// pin for one-wire-bus
#define ONE_WIRE_BUS 10
// index of sensor to be used, normally first sensor (index 0) is ised
#define DALLAS_IDX 0
// conversion time depends on precision (12 bit=750ms, 11 bit=375ms, 10 bit=187.5ms...)
#define TEMPERATURE_PRECISION 10
#define DALLAS_SMP_TIME 200

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float lastT = 0.0;
float SumT = 0.0;
int CntT = 0;
unsigned long lastSensTime;

void setupSensor() {
  sensors.begin();
  while(sensors.getDeviceCount()<1) {
    Serial.println("No sensors connected !");
    delay(1000);
    setupSensor();
  }
  
  sensors.setResolution(TEMPERATURE_PRECISION);		
  lastSensTime=millis();
}

void updateTempSensor() {
  if (abs(millis() - lastSensTime) >= DALLAS_SMP_TIME) {
    if(sensors.requestTemperaturesByIndex(DALLAS_IDX)) {
      float curT = sensors.getTempCByIndex(DALLAS_IDX);
      
      // very simple selection of noise hits/invalid values
      if(abs(curT-lastT)<1.0 || lastT<1) {
        SumT+=curT;
        CntT++;        
      }     
      lastSensTime=millis();
    }
  }  
}

float getTemp() {
  float retVal=gInputTemp;

  if(CntT>=1) {
    retVal=(SumT/CntT);
    SumT=0.;
    CntT=0;
  }  

  return retVal;
}

#endif
#endif
