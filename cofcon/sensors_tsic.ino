//
// COFCON Controller for Espresso Machines
// 2014 by Roman Schmitz
//
// Code for: 
// TSIC-Sensors 206/306
//

#ifndef SIMULATION_MODE
#ifdef SENS_TSIC

#include <TSIC.h>

// pins for power and signal
#define TSIC_VCC 12
#define TSIC_SIG 13
#define TSIC_SMP_TIME 100

#define ACCURACY 0.1

TSIC TSens1(TSIC_SIG, TSIC_VCC);

float lastT = 0.0;
float SumT = 0.0;
float lastErr = 0.0;
int CntT = 0;
uint16_t raw_temp = 0;
unsigned long lastSensTime;

void setupSensor() {	
  lastSensTime=millis();
}

void updateTempSensor() {
  if (abs(millis() - lastSensTime) >= TSIC_SMP_TIME) {
    if(TSens1.getTemperture(&raw_temp)) {
      float curT = TSens1.calc_Celsius(&raw_temp);

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
