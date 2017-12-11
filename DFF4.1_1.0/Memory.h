#define I2C_EEPROM


#ifdef I2C_EEPROM

#include <extEEPROM.h>    // "extEEPROM" via Arduino IDE Library Manager, see https://github.com/PaoloP74/extEEPROM v3.3.5


//One 24LC256 EEPROMs on the bus
extEEPROM i2cEEPROM(kbits_256, 1, 64); //device size, number of devices, page size

/* *****************************************************************************
 * I2C MEMORY FUNCTIONS
 * Later on, use without lib, like Eugen: https://github.com/KONNEKTING/KonnektingFirmware/blob/master/Multi-Interface/IR_Transmitter_1.0/mi.h#L102
 */
void setupMemory() {
    //uint8_t status = i2cEEPROM.begin(i2cEEPROM.twiClock400kHz); // 400kHz Fast Mode
    byte status = i2cEEPROM.begin(i2cEEPROM.twiClock100kHz); // 100kHz Normal Mode
    if (status) {
        Debug.println(F("extEEPROM.begin() failed, status = %i"), status);
        while (1);
    }
}

byte readMemory(int index) {
    return i2cEEPROM.read(index);
}

void writeMemory(int index, byte val) {
    i2cEEPROM.write(index, val);
}

void updateMemory(int index, byte val) {
    i2cEEPROM.update(index, val);
}

void commitMemory() {
    // nothing to do for I2C EEPROM
}



#else


// Include EEPROM-like API for FlashStorage
// let's have 2k of emulated EEPROM
#define EEPROM_EMULATION_SIZE 2048

// see: https://github.com/cmaglie/FlashStorage >0.7.0
#include <FlashAsEEPROM.h>

/* *****************************************************************************
 * SAMD Flash MEMORY FUNCTIONS
 */

void setupMemory() {
    // nothing to do when using SAMD Flash "EEPROM"
}

byte readMemory(int index) {
    return EEPROM.read(index);
}

void writeMemory(int index, int byte) {
    EEPROM.write(index, val);
}

void updateMemory(int index, int byte) {
    EEPROM.write(index, val);
}

void commitMemory() {
    EEPROM.commit();
}

#endif