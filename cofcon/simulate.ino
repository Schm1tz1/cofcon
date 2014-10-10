//
// COFCON Controller for Espresso Machines
// 2014 by Roman Schmitz
//
// Code for: 
// Simple simulation based on Gaggia CC Measurements
//

#ifdef SIMULATION_MODE

#define SIM_TIME 100
#define SIM_T_START 20
#define SIM_T_LOSS 2.9e-2
#define SIM_T_HEAT 0.7787

float heaterSavedState = 0.0;
float curTemp;
unsigned long lastSensTime;

void setupSensor() {
  curTemp = SIM_T_START;
}

void updateTempSensor() {
  if (abs(millis() - lastSensTime) >= SIM_TIME) {
      lastSensTime=millis();
      curTemp = curTemp + (heaterSavedState*SIM_T_HEAT*1e-3) - SIM_T_LOSS;
  }  
}

void updateHeater() {
  heatCurrentTime = millis();
  if(heatCurrentTime - heatLastTime >= 1000 or heatLastTime > heatCurrentTime) {
    heaterSavedState = getHeatCycles();
    heatLastTime = heatCurrentTime;   
  }
}

float getTemp() {
  return curTemp+((float)random(-10,10))/100;;
}

#endif

