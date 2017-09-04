// Include EEPROM-like API for FlashStorage
// let's have 2k of emulated EEPROM
#define EEPROM_EMULATION_SIZE 2048

// see: https://github.com/cmaglie/FlashStorage >0.7.0
#include <FlashAsEEPROM.h>

/* *****************************************************************************
 * MEMORY FUNCTIONS
 */
int readMemory(int index) {
    return EEPROM.read(index);
}

void writeMemory(int index, int val) {
    EEPROM.write(index, val);
}

void updateMemory(int index, int val) {
    EEPROM.write(index, val);
}

void commitMemory() {
    EEPROM.commit();
}
