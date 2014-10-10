 //
// COFCON Controller for Espresso Machines
// 2014 by Roman Schmitz
//
// Serial Interface for Configuration, PID tuning and Data Logging
//

#ifdef SERIAL_BBCC

#define AUTO_PRINT_INTERVAL 200  // milliseconds
#define MAX_DELTA  1000
#define MIN_DELTA  0.01
#define PRINT_PLACES_AFTER_DECIMAL 2  // set to match MIN_DELTA


int incomingByte = 0;
float delta = 1.0;
boolean autoupdate;
boolean printmode = 0;

unsigned long lastUpdateTime = 0;
void setupSerialInterface()  {
  Serial.begin(115200);
  Serial.println("\nWelcome to the COFCON !");
  Serial.println("Send back one or more characters to setup the controller.");
  Serial.println("If this is your initial run, please enter 'R' to Reset the EEPROM.");
  Serial.println("Enter '?' for help.  Here's to a great cup!");
}

void printHelp() {
  Serial.println("Send these characters for control:");
  Serial.println("<space> : print status now");
  Serial.println("u : toggle periodic status update");
  Serial.println("g : toggle update style between human and graphing mode");
  Serial.println("R : reset/initialize PID gain values");
  Serial.println("b : print PID debug values");
  Serial.println("# : run auto-tuning");
  Serial.println("? : print help");  
  Serial.println("+/- : adjust delta by a factor of ten");
  Serial.println("P/p : up/down adjust p gain by delta");
  Serial.println("I/i : up/down adjust i gain by delta");
  Serial.println("D/d : up/down adjust d gain by delta");
  Serial.println("T/t : up/down adjust set temp by delta");
  Serial.println("O/o : up/down adjust overshoot temp by delta");
}

void updateSerialInterface() {
  while(Serial.available()){

    incomingByte = Serial.read();
    if (incomingByte == 'R') {
      gP=30.0;
      gI=0.55;
      gD=500.0;
      gTargetTemp=100.0;
      gOvershoot=1.5;
      aggKp=35.0;
      aggKi=0.0;
      aggKd=0.0;
      EEstoreValues();
    } 
    if (incomingByte == 'P') {
      gP+=delta;
      CofConPID.SetTunings(gP, gI, gD);
      EEstoreValues();
    } 
    if (incomingByte == 'p') {
      gP-=delta;
      CofConPID.SetTunings(gP, gI, gD);
      EEstoreValues();
    } 
    if (incomingByte == 'I') {
      gI+=delta;
      CofConPID.SetTunings(gP, gI, gD);
      EEstoreValues();
    } 
    if (incomingByte == 'i') {
      gI-=delta;
      CofConPID.SetTunings(gP, gI, gD);
      EEstoreValues();
    } 
    if (incomingByte == 'D') {
      gD+=delta;
      CofConPID.SetTunings(gP, gI, gD);
      EEstoreValues();
    } 
    if (incomingByte == 'd' ){
      gD-=delta;
      CofConPID.SetTunings(gP, gI, gD);
      EEstoreValues();
    } 
    if (incomingByte == 'T') {
      gTargetTemp += delta;
      EEstoreValues();
    } 
    if (incomingByte == 't') {
      gTargetTemp -= delta;
      EEstoreValues();
    }
    if (incomingByte == 'O') {
      gOvershoot += delta;
      EEstoreValues();
    } 
    if (incomingByte == 'o') {
      gOvershoot -= delta;
      EEstoreValues();
    }
    if (incomingByte == '+') {
      delta *= 10.0;
      if (delta > MAX_DELTA)
        delta = MAX_DELTA;
    } 
    if (incomingByte == '-') {
      delta /= 10.0;
      if (delta < MIN_DELTA)
        delta = MIN_DELTA;
    }
    if (incomingByte == 'u') {
      autoupdate = not autoupdate;
    }
    if (incomingByte == 'g') {
      printmode = not printmode;
    }
    if (incomingByte == ' ') {
      printStatus();
    }
    if (incomingByte == '?') {
      printHelp(); 
    }
    if (incomingByte == '#') {
      changeAutoTune();
    }
  }

  if (millis() < lastUpdateTime) {
    lastUpdateTime = 0;
  }
  if ((millis() - lastUpdateTime) > AUTO_PRINT_INTERVAL) {
    // this is triggers every slightly more than a second from the delay between these two millis() calls
    lastUpdateTime += AUTO_PRINT_INTERVAL;
    if (autoupdate) {
      if (printmode) {
        printStatusForGraph();
      }
      else {
        printStatus();
      }
    } 
  }
}

void printStatus() { 
  // A means for getting feedback on the current system status and controllable parameters
  Serial.print(" SET TEMP:");
  printFloat(gTargetTemp,PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(", CUR TEMP:");
  printFloat(gInputTemp,PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(", OVERSHOOT TEMP:");
  printFloat(gOvershoot,PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(", NORMAL GAINS p:");
  printFloat(gP,PRINT_PLACES_AFTER_DECIMAL);
    Serial.print(" i:");
  printFloat(gI,PRINT_PLACES_AFTER_DECIMAL);
    Serial.print(" d:");
  printFloat(gD,PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(", ADAPTIVE MODE GAINS p:");
  printFloat(aggKp,PRINT_PLACES_AFTER_DECIMAL);
    Serial.print(" i:");
  printFloat(aggKi,PRINT_PLACES_AFTER_DECIMAL);
    Serial.print(" d:");
  printFloat(aggKd,PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(", Delta: ");
  printFloat(delta,PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(", Power: ");
  printFloat((float)getHeatCycles(), 0);
  if(tuning) Serial.print(" - tuning mode - ");
  Serial.print(" \n");
}

void printStatusForGraph() {
  printFloat(gTargetTemp,PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(", ");
  printFloat(gInputTemp,PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(", ");
  printFloat(CofConPID.GetKp(),PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(", ");
  printFloat(CofConPID.GetKi(),PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(", ");
  printFloat(CofConPID.GetKd(),PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(", ");
  printFloat((float)getHeatCycles(), 0);
  if(tuning) Serial.print(" - tuning - ");
  Serial.println();
}

// printFloat prints out the float 'value' rounded to 'places' places after the decimal point
void printFloat(float value, int places) {
  // this is used to cast digits 
  int digit;
  float tens = 0.1;
  int tenscount = 0;
  int i;
  float tempfloat = value;

  // make sure we round properly. this could use pow from <math.h>, but doesn't seem worth the import
  // if this rounding step isn't here, the value  54.321 prints as 54.3209

  // calculate rounding term d:   0.5/pow(10,places)  
  float d = 0.5;
  if (value < 0)
    d *= -1.0;
  // divide by ten for each decimal place
  for (i = 0; i < places; i++)
    d/= 10.0;    
  // this small addition, combined with truncation will round our values properly 
  tempfloat +=  d;

  // first get value tens to be the large power of ten less than value
  // tenscount isn't necessary but it would be useful if you wanted to know after this how many chars the number will take

  if (value < 0)
    tempfloat *= -1.0;
  while ((tens * 10.0) <= tempfloat) {
    tens *= 10.0;
    tenscount += 1;
  }


  // write out the negative if needed
  if (value < 0)
    Serial.print('-');

  if (tenscount == 0)
    Serial.print(0, DEC);

  for (i=0; i< tenscount; i++) {
    digit = (int) (tempfloat/tens);
    Serial.print(digit, DEC);
    tempfloat = tempfloat - ((float)digit * tens);
    tens /= 10.0;
  }

  // if no places after decimal, stop now and return
  if (places <= 0)
    return;

  // otherwise, write the point and continue on
  Serial.print('.');  

  // now write out each decimal place by shifting digits one by one into the ones place and writing the truncated value
  for (i = 0; i < places; i++) {
    tempfloat *= 10.0; 
    digit = (int) tempfloat;
    Serial.print(digit,DEC);  
    // once written, subtract off that digit
    tempfloat = tempfloat - (float) digit; 
  }
}

#endif
// END Serial Interface
