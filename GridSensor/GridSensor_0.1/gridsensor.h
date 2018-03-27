/*Grid Sensor header file
Author: E.Burkowski / e.burkowski@konnekting.de, GPL Licensed
*/


//don't change this lines!
#include <Wire.h>


#define KNX_SERIAL Serial
#define PIN_PROG_LED 6
#define PIN_PROG_BUTTON 5
#define PIN_KNX_SAVE 7
#define PIN_FLASH_CS 9 

#define PIN_VCC A2
#define PIN_GND A3
#define PIN_OW A4
#define PIN_SW_TL 11
#define PIN_SW_TR 13
#define PIN_SW_BL 12
#define PIN_SW_BR 10
#define PIN_AN_RH A5
#define PIN_AN_T A0
#define PIN_LIGHT_INT A1
#define PIN_VOC_INT 8
#define PIN_VOC_WAKE 9
#define PIN_LED_R 4
#define PIN_LED_G 3
#define PIN_LED_B 6
#define PIN_BIN0 0
#define PIN_BIN1 1




#ifdef KDEBUG
#include <DebugUtil.h>
#define DEBUGSERIAL SerialUSB
#endif


#ifdef I2C_EEPROM
// MI uses 24AA256 I2C EEPROM
int readMemory(int index)
{
    Debug.println(F("i2c EEPROM read on index %d"),index);
    byte data = 0xFF;
    if(index >= 0 && index < 32768){
        Wire.beginTransmission(0x50);
        Wire.write((int) (index >> 8));
        Wire.write((int) (index & 0xFF));
        Wire.endTransmission();
        Wire.requestFrom(0x50, 1);
        if (Wire.available()) {
            data = Wire.read();
        }
    }
    return data;
}

void writeMemory(int index, int val) {
    Debug.println(F("i2c EEPROM write value %d on index %d"),val, index);
    if(index >= 0 && index < 32768 && val >= 0 && val < 256){
        Wire.beginTransmission(0x50);
        Wire.write((int) (index >> 8));
        Wire.write((int) (index & 0xFF));
        Wire.write(val);
        Wire.endTransmission();
        delay(5); //is it needed?!
    }
}

void updateMemory(int index, int val) {
    Debug.println(F("i2c EEPROM update"));
    if (readMemory(index) != val) {
        writeMemory(index, val);
    }
}

void commitMemory() {
    Debug.println(F("i2c EEPROM commit"));
    // EEPROM needs no commit, so this function is intentionally left blank 
}
#else

#include "src/FlashStorage/FlashAsEEPROM.h"
#define EEPROM_EMULATION_SIZE 2048


int readMemory(int index) {
    Debug.println(F("FLASH read on index %d"),index);
    return EEPROM.read(index);
}

void writeMemory(int index, int val) {
    Debug.println(F("FLASH write value %d on index %d"),val, index);
    EEPROM.write(index, val);
}

void updateMemory(int index, int val) {
    Debug.println(F("FLASH update"));
    if (EEPROM.read(index) != val) {
        EEPROM.write(index, val);
    }
}

void commitMemory() {
    Debug.println(F("FLASH commit"));
    EEPROM.commit();
}
#endif

void kinit(){
    Wire.begin();
#ifdef KDEBUG

    // Start debug serial with 9600 bauds
    DEBUGSERIAL.begin(115200);
    // wait for serial port to connect. Needed for Leonardo/Micro/ProMicro/Zero only
    while (!DEBUGSERIAL)
    // make debug serial port known to debug class
    // Means: KONNEKTING will use the same serial port for console debugging
    Debug.setPrintStream(&DEBUGSERIAL);
#endif
    
    Konnekting.setMemoryReadFunc(&readMemory);
    Konnekting.setMemoryWriteFunc(&writeMemory);
    Konnekting.setMemoryUpdateFunc(&updateMemory);
    Konnekting.setMemoryCommitFunc(&commitMemory);


    // Initialize KNX enabled Arduino Board
    Konnekting.init(KNX_SERIAL,
            PIN_PROG_BUTTON,
            PIN_PROG_LED,
            MANUFACTURER_ID,
            DEVICE_ID,
            REVISION); 
}

