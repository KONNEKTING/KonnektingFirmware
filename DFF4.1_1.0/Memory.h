#include <extEEPROM.h>    // http://librarymanager/All#extEEPROM Version 3.4.0
#include <SerialFlash.h> // http://librarymanager/All#SerialFlash Version 0.5.0
#include <SPI.h>

#define SPIFLASH_CSPIN 9
#define LED A1

#define FORMATTED_MARKER_FILE "FORMATTED"
#define PROG_LED A5 


//One 24LC256 EEPROMs on the bus
extEEPROM i2cEEPROM(kbits_256, 1, 64); //device size, number of devices, page size

void setupSpiFlash() {
    //We start by formatting the flash...
    uint8_t id[5];
    SerialFlash.readID(id);

    Debug.println(F("SerialFlash id0=%i"), id[0]);
    Debug.println(F("SerialFlash id1=%i"), id[1]);
    Debug.println(F("SerialFlash id2=%i"), id[2]);
    Debug.println(F("SerialFlash id3=%i"), id[3]);
    Debug.println(F("SerialFlash id4=%i"), id[4]);



    if (!SerialFlash.exists(FORMATTED_MARKER_FILE)) {

        // no format marker available: need to format!
        SerialFlash.eraseAll();
        pinMode(PROG_LED, OUTPUT);
        //Flash LED while formatting
        while (!SerialFlash.ready()) {
            Debug.println(F("SerialFlash Format in progress ..."));
            delay(50);
            digitalWrite(PROG_LED, HIGH);
            delay(50);
            digitalWrite(PROG_LED, LOW);
        }
        digitalWrite(PROG_LED, LOW);

        if (SerialFlash.create(FORMATTED_MARKER_FILE, 16)) {
            Debug.println(F("Created 'formatted marker' file"));
            SerialFlashFile file = SerialFlash.open(FORMATTED_MARKER_FILE);
            Debug.println(F("Opened 'formatted marker' file"));
            byte buffer[16] = {
                0x00, 0x01, 0x02, 0x03,
                0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B,
                0x0C, 0x0D, 0x0E, 0x0F
            };

            file.write(buffer, 16);
            Debug.println(F("Written data to 'formatted marker' file"));

            file.close();
            Debug.println(F("Closed 'formatted marker' file"));
        }
    } else {
        Debug.println(F("Found existing 'formatted marker' file"));
    }

    Debug.println(F("SerialFlash *DONE*"));
}

/* *****************************************************************************
 * I2C MEMORY FUNCTIONS
 */
void setupMemory() {
    //uint8_t status = i2cEEPROM.begin(i2cEEPROM.twiClock400kHz); // 400kHz Fast Mode
    byte status = i2cEEPROM.begin(i2cEEPROM.twiClock100kHz); // 100kHz Normal Mode
    if (status) {
        Debug.println(F("extEEPROM.begin() failed, status = %i"), status);
        while (1);
    }
    Debug.println(F("EEPROM initialized"));
    if (!SerialFlash.begin(SPIFLASH_CSPIN)) {
        Debug.println(F("SerialFlash.begin() failed: Unable to access SPI Flash chip"));
        while (1);
    }
    Debug.println(F("SPI Flash initialized"));
    setupSpiFlash();
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

