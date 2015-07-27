//
// COFCON Controller for Espresso Machines
// 2014 by Roman Schmitz
// (code based on the BBCC by Tim Hirzel)
//
// PWM Output for Heater
//

#define HEAT_RELAY_PIN 4

float heatcycles; // the number of millis out of 1000 for the current heat amount (percent * 10)

boolean heaterState = 0;
unsigned long heatCurrentTime, heatLastTime;

void setupHeater() {
  pinMode(HEAT_RELAY_PIN , OUTPUT);
}

#ifndef SIMULATION_MODE
void updateHeater() {
  boolean h;
  heatCurrentTime = millis();
  if(heatCurrentTime - heatLastTime >= 1000 or heatLastTime > heatCurrentTime) { //second statement prevents overflow errors
    // begin cycle
    _turnHeatElementOnOff(1);  // 
    heatLastTime = heatCurrentTime;   
  } 
  if (heatCurrentTime - heatLastTime >= heatcycles) {
    _turnHeatElementOnOff(0);
  }
}
#endif

void setHeatPowerPercentage(float power) {
  if (power < 0.0) {
    power = 0.0;
  }	
  if (power > 1000.0) {
    power = 1000.0;
  }
  heatcycles = power;
}

float getHeatCycles() {
  return heatcycles;
}

void _turnHeatElementOnOff(boolean on) {
  digitalWrite(HEAT_RELAY_PIN, on);	//turn pin high
  heaterState = on;
}

// End Heater Control
