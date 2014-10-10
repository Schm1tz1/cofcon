//
// COFCON Controller for Espresso Machines
// 2014 by Roman Schmitz
//
// Uses PID and PID-Autotune library
//

// Simulation mode deactivates physical sensors and heater output 
#define SIMULATION_MODE
//

// Select standard serial interface type
//#define SERIAL_BBCC
#define SERIAL_CC

// defines to select sensor types or interface parameters
#define SENS_TSIC
//#define SENS_DALLAS

#include <PID_v1.h>

// global variables
double gTargetTemp;
double gOvershoot;
double gInputTemp=0.0;
double gOutputPwr;
double gP, gI, gD;

double aggKp=35.0, aggKi=0.0, aggKd=0.0;
unsigned int gSampleTime=200;

double aTuneStep=100.0, aTuneNoise=0.2;

unsigned long time_now=0;
unsigned long time_last=0;

boolean tuning = false;
boolean osmode = false;

// gloabl classes
PID CofConPID(&gInputTemp, &gOutputPwr, &gTargetTemp, gP, gI, gD, DIRECT);

void setup()
{
  // read value from EEPROM
  EEreadValues();
  gOutputPwr=0;

  // initialize PID
  CofConPID.SetTunings(gP, gI, gD);
  CofConPID.SetSampleTime(gSampleTime);
  CofConPID.SetOutputLimits(0, 1000);
  CofConPID.SetMode(AUTOMATIC);

  // module setup calls
  setupHeater();
  setupSerialInterface();
  setupSensor();
  
  time_now=millis();
  time_last=time_now;
    
}

void loop()
{  

  updateTempSensor(); 
  updateSerialInterface();

  time_now=millis();

  if(abs(time_now-time_last)>=gSampleTime) {

    time_last=time_now;   
    gInputTemp=getTemp();
    
    if(tuning==true)
    {
      // re-do tuning loop !!!
      CofConPID.SetMode(MANUAL);
      if(gInputTemp<(gTargetTemp-aTuneNoise)) setHeatPowerPercentage(aTuneStep);
      else if(gInputTemp>(gTargetTemp+aTuneNoise)) setHeatPowerPercentage(0);      
    }
    else  {
        if( !osmode && abs(gTargetTemp-gInputTemp)>=gOvershoot ) {        
          CofConPID.SetTunings(aggKp, aggKi, aggKd);
          osmode=true;
        }
        else if( osmode && abs(gTargetTemp-gInputTemp)<gOvershoot ) {
          CofConPID.SetTunings(gP,gI,gD);
          osmode=false;
        }
        if(CofConPID.Compute()==true) {   
          setHeatPowerPercentage(gOutputPwr);
        }
    } 
  }

  updateHeater();
}

