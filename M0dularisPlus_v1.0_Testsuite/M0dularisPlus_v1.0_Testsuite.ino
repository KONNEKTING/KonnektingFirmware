/* *****************************************************************************
   Includes
 */
#include <Arduino.h>
#include <KonnektingDevice.h>
#include "Memory.h"



/* *****************************************************************************
   Defines
 */

#define KNX_SERIAL Serial1
#define PROG_LED 19 // A1
#define PROG_BTN 15 // A5

#define MANUFACTURER_ID 49374
#define DEVICE_ID 0
#define REVISION 0


KnxComObject KnxDevice::_comObjectsList[] = {
    /* Index 0 - Test Receive Object 15/7/253 */ KnxComObject(KNX_DPT_1_001, 0x2a),
    /* Index 1 - Test Send Object 15/7/254 */ KnxComObject(KNX_DPT_1_001, 0x34),
};
const byte KnxDevice::_numberOfComObjects = sizeof (_comObjectsList) / sizeof (KnxComObject); // do not change this code
       
byte KonnektingDevice::_paramSizeList[] = {
    /* Index 0 - startupDelay */ PARAM_UINT8,
};
const int KonnektingDevice::_numberOfParams = sizeof (_paramSizeList); // do not change this code


void writeComObj(byte comObjId, word ga) { 
    byte gaHi = (ga >> 8) & 0xff;
    byte gaLo = (ga >> 0) & 0xff;
    byte settings = 0x80;
    writeMemory(EEPROM_COMOBJECTTABLE_START + (comObjId * 3) + 0, gaHi);
    writeMemory(EEPROM_COMOBJECTTABLE_START + (comObjId * 3) + 1, gaLo);
    writeMemory(EEPROM_COMOBJECTTABLE_START + (comObjId * 3) + 2, settings);
}

/* *****************************************************************************
   S E T U P
 */
void setup() {

    SerialUSB.begin(115200);
    pinMode(PROG_LED, OUTPUT);
    digitalWrite(PROG_LED, HIGH);
    long start = millis();
    while (!SerialUSB && (millis()-start<5000)) { 
        // wait for serial connection at most 2sec
    }
    digitalWrite(PROG_LED, LOW);
    /*
       make debug serial port known to debug class
       Means: KONNEKTING will use this serial port for console debugging
     */
    Debug.setPrintStream(&SerialUSB);

    Debug.println(F("Test-Suite: M0dularis+ v1.0"));

    /*
       Set memory functions
     */
    Debug.println(F("Setup Memory ..."));
    setupMemory();
    Debug.println(F("--> DONE"));

    /* 
     * Fake parametrization 
     */
    Debug.println(F("Fake Memory Data..."));
    word individualAddress = P_ADDR(1,1,1);
    byte iaHi = (individualAddress >> 8) & 0xff;
    byte iaLo = (individualAddress >> 0) & 0xff;
    byte deviceFlags = 0xFE;
    writeMemory(EEPROM_DEVICE_FLAGS, deviceFlags);
    writeMemory(EEPROM_INDIVIDUALADDRESS_HI, iaHi);
    writeMemory(EEPROM_INDIVIDUALADDRESS_LO, iaLo);

    writeComObj(0, G_ADDR(15,7,253));
    writeComObj(1, G_ADDR(15,7,254));
    Debug.println(F("--> DONE"));


    Debug.println(F("Setup Memory Fctptr ..."));
    Konnekting.setMemoryReadFunc(&readMemory);
    Konnekting.setMemoryWriteFunc(&writeMemory);
    Konnekting.setMemoryUpdateFunc(&updateMemory);
    Konnekting.setMemoryCommitFunc(&commitMemory);
    Debug.println(F("--> DONE"));
    
    // M0dularis+ uses "Serial1" !!!
    Konnekting.init(KNX_SERIAL, PROG_BTN, PROG_LED, MANUFACTURER_ID, DEVICE_ID, REVISION);
    
    Debug.println(F("--> setup() DONE"));
}

long last = 0;
bool state = false;

/* *****************************************************************************
   L O O P
 */
void loop() {
    
    Knx.task();
    // it's not ready, unless it's programmed via Suite
    if (Konnekting.isReadyForApplication()) {

        // Blink Prog-LED SLOW for testing purpose
        if (millis() - last > 1000) {

            // send state every second to bus
            Knx.write(1, state);
            Debug.println(F("Sending %i on 15/7/254 from 1.1.1"), state);
          
            if (state) {
                digitalWrite(PROG_LED, HIGH);
                state = false;
            } else {
                digitalWrite(PROG_LED, LOW);
                state = true;
            }
            last = millis();
        }
    } else {
        // Blink Prog-LED FAST for testing purpose
        if (Konnekting.isProgState() && (millis() - last > 200)) {
            if (state) {
                digitalWrite(PROG_LED, HIGH);
                state = false;
            } else {
                digitalWrite(PROG_LED, LOW);
                state = true;
            }
            last = millis();
        }
    }

}

// Callback function to handle com objects updates
void knxEvents(byte index) {

    Debug.println(F("knxEvents: i=%i"), index);
    

};
