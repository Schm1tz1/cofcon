//
// COFCON Controller for Espresso Machines
// 2014 by Roman Schmitz
//
// Bluetooth interface via serial pins or software serial
//

#ifdef SERIAL_BT

#define SERIAL_BAUD 9600
#define SERIAL_PLOT_INTERVAL 200

#define BT_MOD_PWR_PIN 10
#define BT_MOD_RST_PIN 11

String inStr = "";
boolean newlineFlag = false;
unsigned long lastPlotTime;

void setupSerialInterface()  {
  
  pinMode(BT_MOD_PWR_PIN, OUTPUT);
  digitalWrite(BT_MOD_PWR_PIN, HIGH);
 
  inStr.reserve(128);
  Serial.begin(SERIAL_BAUD);
  lastPlotTime=millis();
}

void updateSerialInterface() {

  unsigned long now = millis();  
  
  if(newlineFlag==true) {
    if(inStr=="reset") {
      resetParameters();
    }
    else if(inStr=="load") {      
      EEreadValues();
    }
    else if(inStr=="save") {
      EEstoreValues();
    }
    else if(inStr.startsWith("set")) {
      serialSetValue(inStr);
    }
    else if(inStr.startsWith("tune")) {
      serialTuningMode(inStr);
    }
    else if(inStr=="FLASH") {
        digitalWrite(BT_MOD_PWR_PIN, LOW);
        pinMode(BT_MOD_RST_PIN, OUTPUT);
        digitalWrite(BT_MOD_RST_PIN, LOW);
    }
    else if(inStr=="RST") {
        pinMode(BT_MOD_RST_PIN, OUTPUT);
        digitalWrite(BT_MOD_RST_PIN, LOW);
    }
    
    inStr="";
    newlineFlag=false;
  }
  
  if( (now - lastPlotTime) >= SERIAL_PLOT_INTERVAL) {
    lastPlotTime = now;
    Serial.print("%");
    Serial.print(now); Serial.print(" ");
    Serial.print(gInputTemp, 2); Serial.print(" ");
    Serial.print(gTargetTemp, 2); Serial.print(" ");
    Serial.print(gOvershoot, 2); Serial.print(" ");
    Serial.print(CofConPID.GetKp(), 2); Serial.print(" ");
    Serial.print(CofConPID.GetKi(), 2); Serial.print(" ");
    Serial.print(CofConPID.GetKd(), 2); Serial.print(" ");
    Serial.print(gP, 2); Serial.print(" ");
    Serial.print(gI, 2); Serial.print(" ");
    Serial.print(gD, 2); Serial.print(" ");
    Serial.print(aggKp, 2); Serial.print(" ");
    Serial.print(aggKi, 2); Serial.print(" ");
    Serial.print(aggKd, 2); Serial.print(" ");
    Serial.print(getHeatCycles()); Serial.println("&");
  }
}

void serialSetValue(String &aConfStr) {
  if(aConfStr.startsWith("Tset",4)) {
    gTargetTemp=StrToFloat(aConfStr.substring(9));
  }
  else if(aConfStr.startsWith("Tband",4)) {
    gOvershoot=StrToFloat(aConfStr.substring(9));
  }
  else if(aConfStr.startsWith("Kp",4)) {
    gP=StrToFloat(aConfStr.substring(7));
  }
  else if(aConfStr.startsWith("Ki",4)) {
    gI=StrToFloat(aConfStr.substring(7));
  }
  else if(aConfStr.startsWith("Kd",4)) {
    gD=StrToFloat(aConfStr.substring(7));
  }
  else if(aConfStr.startsWith("aKp",4)) {
    aggKp=StrToFloat(aConfStr.substring(8));
  }  
  else if(aConfStr.startsWith("aKi",4)) {
    aggKi=StrToFloat(aConfStr.substring(8));
  }
  else if(aConfStr.startsWith("aKd",4)) {
    aggKd=StrToFloat(aConfStr.substring(8));
  }  

  // overshoot mode or not
  if(osmode==true) CofConPID.SetTunings(aggKp, aggKi, aggKd);
  else CofConPID.SetTunings(gP, gI, gD);
}

void serialTuningMode(String &aConfStr) {
  if(aConfStr.startsWith("status",5)) {
    if(tuning==true) Serial.print(1); 
    else Serial.print(0);
    Serial.print(" ");
    Serial.print(aTuneStep, 3); Serial.print(" ");
    Serial.println(aTuneNoise, 3);
  }
  else if(aConfStr.startsWith("on",5)) {
    tuning=true;
  }
  else if(aConfStr.startsWith("off",5)) {
    tuning=false;
  }
  else if(aConfStr.startsWith("step",5)) {
    aTuneStep=StrToFloat(aConfStr.substring(10));
  }
  else if(aConfStr.startsWith("thres",5)) {
    aTuneNoise=StrToFloat(aConfStr.substring(11));
  }  
}

void serialPrintStatus() {  
  Serial.print("%");
  Serial.print(gTargetTemp, 2); Serial.print(" ");
  Serial.print(gOvershoot, 2); Serial.print(" ");
  Serial.print(gP, 2); Serial.print(" ");
  Serial.print(gI, 2); Serial.print(" ");
  Serial.print(gD, 2); Serial.print(" ");
  Serial.print(aggKp, 2); Serial.print(" ");
  Serial.print(aggKi, 2); Serial.print(" ");
  Serial.print(aggKd, 2); Serial.print(" ");
  Serial.print(CofConPID.GetKp(), 2); Serial.print(" ");
  Serial.print(CofConPID.GetKi(), 2); Serial.print(" ");
  Serial.print(CofConPID.GetKd(), 2);
  Serial.println("&");
}

void resetParameters() {
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
// END Serial Interface

