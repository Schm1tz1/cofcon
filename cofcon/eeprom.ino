//
// COFCON Controller for Espresso Machines
// 2014 by Roman Schmitz
//
// EEPROM routines
//

#include <avr/eeprom.h>

// configuraton data stored in EEPROM
// for PID: latest p,i,d, set temperature
#define PGAIN_ADR 0
#define IGAIN_ADR 4
#define DGAIN_ADR 8
#define TARGET_TEMP_ADDRESS 12
#define OVERSHOOT_ADDRESS 16
#define PAGG_ADR 20
#define IAGG_ADR 24
#define DAGG_ADR 28
//

float readFloat(int address) {
  float out;
  eeprom_read_block((void *) &out, (unsigned char *) address ,4);
  return out;
}

void writeFloat(float value, int address) {
  eeprom_write_block((void *) &value, (unsigned char *) address ,4);
}

void EEstoreValues() {
  writeFloat(gP, PGAIN_ADR);
  writeFloat(gI, IGAIN_ADR);
  writeFloat(gD, DGAIN_ADR);
  writeFloat(gTargetTemp, TARGET_TEMP_ADDRESS);
  writeFloat(gOvershoot, OVERSHOOT_ADDRESS);
  writeFloat(aggKp, PAGG_ADR);
  writeFloat(aggKi, IAGG_ADR);
  writeFloat(aggKd, DAGG_ADR);
}

void EEreadValues() {
  gP = readFloat(PGAIN_ADR);
  gI = readFloat(IGAIN_ADR);
  gD = readFloat(DGAIN_ADR);
  gTargetTemp = readFloat(TARGET_TEMP_ADDRESS);
  gOvershoot = readFloat(OVERSHOOT_ADDRESS);
  aggKp = readFloat(PAGG_ADR);
  aggKi = readFloat(IAGG_ADR);
  aggKd = readFloat(DAGG_ADR);
}

// END EEPROM Float
