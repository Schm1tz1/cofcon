//
// COFCON Controller for Espresso Machines
// 2014 by Roman Schmitz
//
// Serial Interface for Configuration, PID tuning and Data Logging
//

#ifndef SERIAL_BT
#ifdef SERIAL_CC

#define SERIAL_PLOT_INTERVAL 200

String inStr = "";
boolean newlineFlag = false;
boolean UserMode = true;
unsigned long lastPlotTime;

void setupSerialInterface()  {
  Serial.begin(115200);
  inStr.reserve(128);
  Serial.println("\nWelcome to the COFCON\n*********************\nEnter 'help' for a list of commands.");
  lastPlotTime=millis();
}

void updateSerialInterface() {

  unsigned long now = millis();  
  
  if(newlineFlag==true) {
    if(inStr=="plotter") {
      UserMode=!UserMode;
      if(UserMode==false) {
        Serial.println("# COFCON plotter - columns: (time) (T) (Tset) (P) (I) (D) (Power)");
      }
    }
    else if(inStr=="help") {
      serialPrintHelp();
    }
    else if(inStr=="status") {
      serialPrintStatus();
    }
    else if(inStr=="reset") {
      resetParameters();
    }
    else if(inStr=="load") {
      if(UserMode==true) Serial.println("Loading parameters from EEPROM...");
      EEreadValues();
    }
    else if(inStr=="save") {
      if(UserMode==true) Serial.println("Saving parameters to EEPROM...");
      EEstoreValues();
    }
    else if(inStr.startsWith("set")) {
      serialSetValue(inStr);
    }
    else if(inStr.startsWith("tune")) {
      serialTuningMode(inStr);
    }
    
    inStr="";
    newlineFlag=false;
  }
  
  if(UserMode==false && ((now - lastPlotTime) >= SERIAL_PLOT_INTERVAL)) {
    lastPlotTime = now;
    Serial.print(now); Serial.print(" ");
    Serial.print(gInputTemp, 2); Serial.print(" ");
    Serial.print(gTargetTemp, 2); Serial.print(" ");
    Serial.print(CofConPID.GetKp(), 2); Serial.print(" ");
    Serial.print(CofConPID.GetKi(), 2); Serial.print(" ");
    Serial.print(CofConPID.GetKd(), 2); Serial.print(" ");
    Serial.println(getHeatCycles());
  }
}

void serialSetValue(String &aConfStr) {
  if(aConfStr.startsWith("Tset",4)) {
    gTargetTemp=StrToFloat(aConfStr.substring(9));
    if(UserMode==true) {
      Serial.print("setting Tset=");
      Serial.print(gTargetTemp,2);
    }
  }
  else if(aConfStr.startsWith("Tband",4)) {
    gOvershoot=StrToFloat(aConfStr.substring(9));
    if(UserMode==true) {
      Serial.println("setting Tband=");
      Serial.print(gOvershoot,2);
    }
  }
  else if(aConfStr.startsWith("Kp",4)) {
    gP=StrToFloat(aConfStr.substring(7));
    if(UserMode==true) {
      Serial.println("setting Kp=");
      Serial.print(gP,3);
    }
  }
  else if(aConfStr.startsWith("Ki",4)) {
    gI=StrToFloat(aConfStr.substring(7));
    if(UserMode==true) {
      Serial.println("setting Ki=");
      Serial.print(gI,3);
    }
  }
  else if(aConfStr.startsWith("Kd",4)) {
    gD=StrToFloat(aConfStr.substring(7));
    if(UserMode==true) {
      Serial.println("setting Kd=");
      Serial.print(gD,3);
    }
  }
  else if(aConfStr.startsWith("aKp",4)) {
    aggKp=StrToFloat(aConfStr.substring(8));
    if(UserMode==true) {
      Serial.println("setting aKp=");
      Serial.print(aggKp,3);
    }
  }  
  else if(aConfStr.startsWith("aKi",4)) {
    aggKi=StrToFloat(aConfStr.substring(8));
    if(UserMode==true) {
      Serial.println("setting aKp=");
      Serial.print(aggKi,3);
    }
  }
  else if(aConfStr.startsWith("aKd",4)) {
    aggKd=StrToFloat(aConfStr.substring(8));
    if(UserMode==true) {
      Serial.println("setting aKd=");
      Serial.print(aggKd,3);
    }
  }  
  else {
    if(UserMode==true) Serial.println("Unknown parameter. Valid parameters: Tset, Tband, Kp, Ki, Kd, aKp, aKi, aKd");
  }
  if(osmode==true) CofConPID.SetTunings(aggKp, aggKi, aggKd);
  else CofConPID.SetTunings(gP, gI, gD);
}

void serialTuningMode(String &aConfStr) {
  if(aConfStr.startsWith("status",5)) {
    if(UserMode==true) {
      Serial.print("Tuning is ");
      if(tuning==true) Serial.println("on");
      else Serial.println("off");
      Serial.print("Step size: ");
      Serial.print(aTuneStep, 3);
      Serial.print(", Threshold: ");
      Serial.println(aTuneNoise, 3);
    }
    else {
      if(tuning==true) Serial.print(1); 
      else Serial.print(0);
      Serial.print(" ");
      Serial.print(aTuneStep, 3); Serial.print(" ");
      Serial.println(aTuneNoise, 3);
    }
  }
  else if(aConfStr.startsWith("on",5)) {
    tuning=true;
    if(UserMode==true) {
      Serial.print("starting tuning mode...");
    }
  }
  else if(aConfStr.startsWith("off",5)) {
    tuning=false;
    if(UserMode==true) {
      Serial.print("stopping tuning mode...");      
    }
  }
  else if(aConfStr.startsWith("step",5)) {
    aTuneStep=StrToFloat(aConfStr.substring(10));
    if(UserMode==true) {
      Serial.println("setting tuning step to ");
      Serial.print(aTuneStep,2);
    }
  }
  else if(aConfStr.startsWith("thres",5)) {
    aTuneNoise=StrToFloat(aConfStr.substring(11));
    if(UserMode==true) {
      Serial.println("setting tuning threshold to ");
      Serial.print(aTuneNoise,2);
    }
  }  
  else {
    if(UserMode==true) Serial.println("Unknown tuning command. Valid: status, on, off, step <stepsize>, thres <threshold>");
  }
}

void serialPrintHelp() {
  Serial.println("Valid commands: 'help' - this help, 'status' - show settings, 'load' - load setting from EEPROM, 'save' - save setting to EEPROM, 'set <parameter> <value>' - change parameters, 'reset' - reset parameters, 'plotter' - toggle plotter mode");
}

void serialPrintStatus() {
  if(UserMode==true) {
    Serial.println("COFCON Status\n*************");
    Serial.print("(normal PID) Kp: ");
    Serial.print(gP, 3);
    Serial.print(", Ki: ");
    Serial.print(gI, 3);
    Serial.print(", Kd: ");
    Serial.println(gD, 3);
    Serial.print("(adaptive mode) aKp: ");
    Serial.print(aggKp, 3);
    Serial.print(", aKi: ");
    Serial.print(aggKi, 3);
    Serial.print(", aKd: ");
    Serial.println(aggKd, 2);
    Serial.print("(current values) Kp: ");
    Serial.print(CofConPID.GetKp(), 3);
    Serial.print(", Ki: ");
    Serial.print(CofConPID.GetKi(), 3);
    Serial.print(", Kd: ");
    Serial.println(CofConPID.GetKd(), 3);
    Serial.print("Tset: ");
    Serial.print(gTargetTemp, 2);
    Serial.print(", Tband: ");
    Serial.print(gOvershoot, 2);
  }
  else {
    Serial.print(gTargetTemp, 2); Serial.print(" ");
    Serial.print(gOvershoot, 2); Serial.print(" ");
    Serial.print(gP, 2); Serial.print(" ");
    Serial.print(gI, 2); Serial.print(" ");
    Serial.println(gD, 2); Serial.print(" ");
    Serial.print(aggKp, 2); Serial.print(" ");
    Serial.print(aggKi, 2); Serial.print(" ");
    Serial.println(aggKd, 2); Serial.print(" ");
    Serial.print(CofConPID.GetKp(), 2); Serial.print(" ");
    Serial.print(CofConPID.GetKi(), 2); Serial.print(" ");
    Serial.println(CofConPID.GetKd(), 2);
  }
}

void resetParameters() {
  Serial.println("Parameter reset performed !");
  gP=30.0; 
  gI=0.55; 
  gD=500.0;
  gTargetTemp=100.0; 
  gOvershoot=1.5;
  aggKp=35.0; 
  aggKi=0.0; 
  aggKd=0.0;
}

// helper command to get float from string
float StrToFloat(String str){
  char carray[str.length() + 1];
  str.toCharArray(carray, sizeof(carray));
  return atof(carray);
}

// event handler to read full lines into buffer
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 

    if (inChar == '\n') {
      newlineFlag = true;
    } 
    else {
      inStr += inChar;
    }
  }
}

#endif
#endif
// END Serial Interface

