/* **********************************
 *  
 *   ###### BETA 4 Firmware ######
 * 
 * **********************************/
 
/// ################################################
// ### DEBUG CONFIGURATION
// ################################################
#define KDEBUG // comment this line to disable DEBUG mode
#ifdef KDEBUG
#include <DebugUtil.h>

// All other, (ATmega328P f.i.) use software serial
#include <SoftwareSerial.h>
SoftwareSerial softserial(11, 10); // RX, TX
#define DEBUGSERIAL softserial

// end of debugging defs
#endif

// include KnxDevice library
#include <KnxDevice.h>

// Define KONNEKTING Device related IDs
#define MANUFACTURER_ID 257
#define DEVICE_ID 0
#define REVISION 0

// defaults to on-board LED for AVR Arduinos
#define PROG_LED_PIN 8

// define programming-button PIN
#define PROG_BUTTON_PIN 3

// define KNX Transceiver serial port
#define KNX_SERIAL Serial // Nano/ProMini etc. use Serial

#include "Wire.h"
#include "SparkFunHTU21D.h"

#define iaqaddress 0x5A

#define LED_G 4
#define LED_Y 5
#define LED_R 6

#define UNDEFINED_TEMP -123.4567890 // it's very unlikely that this temperature is measured
#define UNDEFINED_RH 101 // it's very unlikely that the humidity goes that high ;-)
#define UNDEFINED_VOC 0 // The VOC sensor will not deliver values lower than 450 or 125

// Definition of the Communication Objects attached to the device
KnxComObject KnxDevice::_comObjectsList[] = {
    /* Sketch-Index 1, Suite-Index 0 : */ KnxComObject(KNX_DPT_9_001, COM_OBJ_SENSOR), // Temperatur Messwert
    /* Sketch-Index 2, Suite-Index 1 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR), // Temperatur unterer Meldewert
    /* Sketch-Index 3, Suite-Index 2 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR), // Temperatur oberer Meldewert
    /* Sketch-Index 4, Suite-Index 3 : */ KnxComObject(KNX_DPT_9_007, COM_OBJ_SENSOR), // rH Messwert
    /* Sketch-Index 5, Suite-Index 4 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR), // rH unterer Meldewert
    /* Sketch-Index 6, Suite-Index 5 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR), // rH oberer Meldewert
    /* Sketch-Index 7, Suite-Index 6 : */ KnxComObject(KNX_DPT_9_008, COM_OBJ_SENSOR), // VOC Messwert
    /* Sketch-Index 8, Suite-Index 7 : */ KnxComObject(KNX_DPT_9_008, COM_OBJ_SENSOR), // TVOC Messwert
    /* Sketch-Index 9, Suite-Index 8 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR), // T/VOC unterer Meldewert
    /* Sketch-Index 10, Suite-Index 9 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR) // T/VOC oberer Meldewert

};
const byte KnxDevice::_numberOfComObjects = sizeof (_comObjectsList) / sizeof (KnxComObject); // do no change this code

// Definition of parameter size
byte KonnektingDevice::_paramSizeList[] = {
    /* Param Index 0 */ PARAM_UINT8,
    /* Param Index 1 */ PARAM_UINT8,
    /* Param Index 2 */ PARAM_UINT8,
    /* Param Index 3 */ PARAM_INT8,
    /* Param Index 4 */ PARAM_UINT8,
    /* Param Index 5 */ PARAM_UINT8,
    /* Param Index 6 */ PARAM_UINT8,
    /* Param Index 7 */ PARAM_UINT8,
    /* Param Index 8 */ PARAM_UINT8,
    /* Param Index 9 */ PARAM_INT8,
    /* Param Index 10 */ PARAM_UINT8,
    /* Param Index 11 */ PARAM_UINT8,
    /* Param Index 12 */ PARAM_UINT8,
    /* Param Index 13 */ PARAM_UINT8,
    /* Param Index 14 */ PARAM_UINT8, // UNUSED
    /* Param Index 15 */ PARAM_UINT8,
    /* Param Index 16 */ PARAM_UINT16,
    /* Param Index 17 */ PARAM_UINT16,
    /* Param Index 18 */ PARAM_UINT16,
    /* Param Index 19 */ PARAM_UINT8,
    /* Param Index 20 */ PARAM_UINT8,
    /* Param Index 21 */ PARAM_UINT8,
    /* Param Index 22 */ PARAM_UINT8,
    /* Param Index 23 */ PARAM_UINT8,
    /* Param Index 24 */ PARAM_UINT8,
    /* Param Index 25 */ PARAM_UINT8,
    /* Param Index 26 */ PARAM_UINT16,
    /* Param Index 27 */ PARAM_UINT8,
    /* Param Index 28 */ PARAM_UINT8,
    /* Param Index 29 */ PARAM_UINT8,
    /* Param Index 30 */ PARAM_UINT8,
    /* Param Index 31 */ PARAM_UINT8,
    /* Param Index 32 */ PARAM_UINT8,
    /* Param Index 33 */ PARAM_UINT8,
    /* Param Index 34 */ PARAM_UINT8,
    /* Param Index 35 */ PARAM_UINT8,
};
const byte KonnektingDevice::_numberOfParams = sizeof (_paramSizeList); // do no change this code


unsigned long previousMillisVOC = 0;
unsigned long previousMillisVOC2 = 0;
unsigned long previousMillisTemp = 0;
unsigned long previousMillisTemp2 = 0;
unsigned long previousMillisRH = 0;
unsigned long previousMillisRH2 = 0;


float lastTemp = 0;
float lastRH = 0;

int lastVOC = 0;

// Param values
// 1
bool tempActive = false;
// 2
unsigned int tempCycleMins = 0;
// 3
signed int tempOffset = 0;
// 4
int tempSendOnChangeDegree = 0;
// 5
int tempUpperAlarmValue = 0;
// 6
int tempLowerAlarmValue = 0;

// 7
bool rhActive = false;
// 8
unsigned int rhCycleMins = 0;
// 9
signed int rhOffset = 0;
// 10
int rhSendOnChangePercent = 0;
// 11
int rhUpperAlarmValue = 0;
// 12
int rhLowerAlarmValue = 0;

// 13
bool vocActive = false;
// 14
byte vocSensorType = 0;
// 15
unsigned int vocCycleMins = 0;
// 16
int vocSendOnChangePPM = 0;
// 17
unsigned int vocUpperAlarmValue = 0;
// 18
unsigned int vocLowerAlarmValue = 0;


// simple counter to reduce debug output frequency
int loopc = 0;

HTU21D htu21d;

// VOC vars
uint16_t vocPredict;
uint8_t vocState;
int32_t vocResistance;
uint16_t vocTVOC;

void setup() {

    // debug related stuff
#ifdef KDEBUG

    // Start debug serial with 9600 bauds
    DEBUGSERIAL.begin(9600);

    // make debug serial port known to debug class
    // Means: KONNEKTING will use the same serial port for console debugging
    Debug.setPrintStream(&DEBUGSERIAL);    

#endif

    // Initialize KNX enabled Arduino Board
    Konnekting.init(KNX_SERIAL, PROG_BUTTON_PIN, PROG_LED_PIN, MANUFACTURER_ID, DEVICE_ID, REVISION);

    DEBUGSERIAL.println(F("KNX init done"));

    if (!Konnekting.isFactorySetting()) {
        int startDelay = (int) Konnekting.getUINT8Param(0);
        if (startDelay > 0 && startDelay < 255) {
            DEBUGSERIAL.print(F("delay for "));
            DEBUGSERIAL.print(startDelay);
            DEBUGSERIAL.println(F("s"));
            delay(startDelay * 1000);
            DEBUGSERIAL.println(F("ready!"));
        }

        previousMillisTemp = millis();
        previousMillisRH = millis();
        previousMillisVOC = millis();

        // Read Temp Sensor Params
        tempActive = Konnekting.getUINT8Param(1) == 1 ? true : false;
        tempCycleMins = Konnekting.getUINT8Param(2);
        tempOffset = Konnekting.getINT8Param(3);

        DEBUGSERIAL.print(F("tempOffset="));
        DEBUGSERIAL.println(tempOffset);

        tempSendOnChangeDegree = Konnekting.getUINT8Param(4);

        DEBUGSERIAL.print(F("tempSendOnChangeDegree="));
        DEBUGSERIAL.println(tempSendOnChangeDegree);

        tempUpperAlarmValue = Konnekting.getUINT8Param(5);
        tempLowerAlarmValue = Konnekting.getUINT8Param(6);

        // Read RH Sensor Params
        rhActive = Konnekting.getUINT8Param(7) == 1 ? true : false;
        rhCycleMins = Konnekting.getUINT8Param(8);
        rhOffset = Konnekting.getINT8Param(9);
        rhSendOnChangePercent = Konnekting.getUINT8Param(10);
        rhUpperAlarmValue = Konnekting.getUINT8Param(11);
        rhLowerAlarmValue = Konnekting.getUINT8Param(12);

        // Read VOC Sensor Params
        vocActive = Konnekting.getUINT8Param(13) == 1 ? true : false;
        vocSensorType = Konnekting.getUINT8Param(14);
        vocCycleMins = Konnekting.getUINT8Param(15);
        vocSendOnChangePPM = Konnekting.getUINT16Param(16);
        vocUpperAlarmValue = Konnekting.getUINT16Param(17);
        vocLowerAlarmValue = Konnekting.getUINT16Param(18);

    }

    DEBUGSERIAL.println(F("Startup delay done"));

    Wire.begin();
    htu21d.begin();

    pinMode(LED_G, OUTPUT);
    pinMode(LED_Y, OUTPUT);
    pinMode(LED_R, OUTPUT);

    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_Y, HIGH);
    digitalWrite(LED_R, HIGH);

    DEBUGSERIAL.println(F("I2C setup done."));

    DEBUGSERIAL.println(F("Ready!"));
}

void loop() {
    Knx.task();

    // simple counter to reduce log output
    loopc++;

    // only do measurements and other sketch related stuff if not in programming mode
    if (Konnekting.isReadyForApplication()) {

        unsigned long currentMillis = millis();

        /* ***************************
         * Check Temp Sensor
         * ***************************/
        if (tempActive) {

            float currentTemp = UNDEFINED_TEMP;

            // Zyklisches Senden
            if (tempCycleMins > 0) {

                // prevent overflow problems
                if (previousMillisTemp > currentMillis) {
                    // skip one round in case of millis overflow
                    previousMillisTemp = currentMillis;
                }

                if (currentMillis - previousMillisTemp >= (tempCycleMins * 60 * 1000)) {

                    previousMillisTemp = currentMillis;
                    DEBUGSERIAL.print(F("Temperature by cycle:"));
                    DEBUGSERIAL.println(tempCycleMins, DEC);

                    currentTemp = readTemp();

                    DEBUGSERIAL.print(F("Temperature:"));
                    DEBUGSERIAL.print(currentTemp, 1);
                    DEBUGSERIAL.println();

                    Knx.write(0, currentTemp);
                } else {
                    if (loopc % 10000 == 1) {
                        DEBUGSERIAL.print(F("Waiting Temp: "));
                        DEBUGSERIAL.println((tempCycleMins * 60 * 1000)-(currentMillis - previousMillisTemp), DEC);
                    }
                }

            }

            // Senden bei Änderung
            if (tempSendOnChangeDegree > 0) {

                // prevent overflow problems
                if (previousMillisTemp2 > currentMillis) {
                    // skip one round in case of millis overflow
                    previousMillisTemp2 = currentMillis;
                }

                // we need a window of about 10sec to give the sensor a chance 
                // to collect a diff >0, otherwise temperatur rise or fall is 
                // not fast enough to send an update on the bus
                if (currentMillis - previousMillisTemp2 >= 10000 /* 10sec */) {

                    previousMillisTemp2 = currentMillis;

                    if (currentTemp == UNDEFINED_TEMP) {
                        currentTemp = readTemp();
                    }

                    float diff = (lastTemp - currentTemp);

                    // make it always a positive diff
                    if (diff < 0) {
                        diff *= -1;
                    }

                    float requiredDiff = (float) tempSendOnChangeDegree / 10;

                    DEBUGSERIAL.print(F("Temp Diff: current="));
                    DEBUGSERIAL.print(currentTemp, 1);
                    DEBUGSERIAL.print(F(" last="));
                    DEBUGSERIAL.print(lastTemp, 1);
                    DEBUGSERIAL.print(F(" requDiff="));
                    DEBUGSERIAL.print(requiredDiff, 1);
                    DEBUGSERIAL.print(F(" diff="));
                    DEBUGSERIAL.println(diff, 1);


                    if (diff >= requiredDiff) {
                        DEBUGSERIAL.print(F("Temp by Diff: "));
                        DEBUGSERIAL.print(diff, 1);
                        DEBUGSERIAL.println();
                        Knx.write(0, currentTemp);
                    }
                }
            }
            
            if (currentTemp != UNDEFINED_TEMP) {
                
                Knx.write(1, currentTemp<=tempLowerAlarmValue);
                Knx.write(2, currentTemp>=tempUpperAlarmValue);
                
                lastTemp = currentTemp;
            }

        }

        // give the device time to react on telegrams
        Knx.task();
        currentMillis = millis();

        /* ***************************
         * Check RH Sensor
         * ***************************/
        if (rhActive) {

            float currentRH = UNDEFINED_RH;

            // Zyklisches Senden
            if (rhCycleMins > 0) {

                // prevent overflow problems
                if (previousMillisRH > currentMillis) {
                    // skip one round in case of millis overflow
                    previousMillisRH = currentMillis;
                }

                if (currentMillis - previousMillisRH >= (rhCycleMins * 60 * 1000)) {

                    previousMillisRH = currentMillis;
                    DEBUGSERIAL.print(F("RH by cycle:"));
                    DEBUGSERIAL.println(rhCycleMins, DEC);

                    currentRH = readRH();

                    DEBUGSERIAL.print(F("RH:"));
                    DEBUGSERIAL.print(currentRH, 1);
                    DEBUGSERIAL.println();

                    Knx.write(3, currentRH);
                } else {
                    if (loopc % 10000 == 1) {
//                        DEBUGSERIAL.print(F("Waiting RH: "));
//                        DEBUGSERIAL.println((rhCycleMins * 60 * 1000)-(currentMillis - previousMillisRH), DEC);
                    }
                }

            }

            // Senden bei Änderung
            if (rhSendOnChangePercent > 0) {

                // prevent overflow problems
                if (previousMillisRH2 > currentMillis) {
                    // skip one round in case of millis overflow
                    previousMillisRH2 = currentMillis;
                }

                // we need a window of about 10sec to give the sensor a chance 
                // to collect a diff >0, otherwise RH rise or fall is 
                // not fast enough to send an update on the bus
                if (currentMillis - previousMillisRH2 >= 10000 /* 10sec */) {

                    previousMillisRH2 = currentMillis;

                    if (currentRH == UNDEFINED_RH) {
                        currentRH = readRH();
                    }

                    float diff = (lastRH - currentRH);

                    // make it always a positive diff
                    if (diff < 0) {
                        diff *= -1;
                    }

                    float requiredDiff = (float) tempSendOnChangeDegree;

                    DEBUGSERIAL.print(F("RH Diff: current="));
                    DEBUGSERIAL.print(currentRH, 1);
                    DEBUGSERIAL.print(F(" last="));
                    DEBUGSERIAL.print(lastRH, 1);
                    DEBUGSERIAL.print(F(" requDiff="));
                    DEBUGSERIAL.print(requiredDiff, 1);
                    DEBUGSERIAL.print(F(" diff="));
                    DEBUGSERIAL.println(diff, 1);


                    if (diff >= requiredDiff) {
                        DEBUGSERIAL.print(F("RH by Diff: "));
                        DEBUGSERIAL.print(diff, 1);
                        DEBUGSERIAL.println();
                        Knx.write(3, currentRH);
                    }
                }
            }
            if (currentRH != UNDEFINED_RH) {
                
                Knx.write(4, currentRH<=rhLowerAlarmValue);
                Knx.write(5, currentRH>=rhUpperAlarmValue);
                
                lastRH = currentRH;
            }

        }

        // give the device time to react on telegrams
        Knx.task();
        currentMillis = millis();

        /* ***************************
         * Check VOC Sensor
         * ***************************/

        if (vocActive) {
            
            int currentVOC = 0;
            vocPredict = UNDEFINED_VOC;
            vocTVOC = UNDEFINED_VOC;

            if (vocCycleMins > 0) {


                // prevent overflow problems
                if (previousMillisVOC > currentMillis) {
                    // skip one round in case of millis overflow
                    previousMillisVOC = currentMillis;
                }

                if (currentMillis - previousMillisVOC >= (vocCycleMins * 60 * 1000)) {

                    previousMillisVOC = currentMillis;

                    DEBUGSERIAL.print(F("VOC by cycle: "));
                    readVOC();
                    DEBUGSERIAL.print(F("CO2="));
                    DEBUGSERIAL.print(vocPredict);
                    DEBUGSERIAL.print(F(" TVoC="));
                    DEBUGSERIAL.println(vocTVOC);
                    checkVocStatus();


                    if (vocState == 0x10) {
                        vocPredict = 0;
                        vocTVOC = 0;
                    }

                    DEBUGSERIAL.println(F("Sending VOC CO2 equiv."));
                    Knx.write(6, vocPredict);
                } else {
                    if (loopc % 10000 == 1) {
                        DEBUGSERIAL.print(F("Waiting VOC: "));
                        DEBUGSERIAL.println((vocCycleMins * 60 * 1000)-(currentMillis - previousMillisVOC), DEC);
                    }
                }

            }

            // Senden bei Änderung
            if (vocSendOnChangePPM > 0) {

                // prevent overflow problems
                if (previousMillisVOC2 > currentMillis) {
                    // skip one round in case of millis overflow
                    previousMillisVOC2 = currentMillis;
                }

                // we need a window of about 10sec to give the sensor a chance 
                // to collect a diff >0, otherwise VOC rise or fall is 
                // not fast enough to send an update on the bus
                if (currentMillis - previousMillisVOC2 >= 10000 /* 10sec */) {

                    previousMillisVOC2 = currentMillis;

                    if (vocPredict == UNDEFINED_RH || vocTVOC == UNDEFINED_VOC) {
                        readVOC();
                        checkVocStatus();
                    }


                    if (vocState == 0x10) {
                        vocPredict = 0;
                        vocTVOC = 0;
                    }

                    currentVOC = vocPredict;

                    int diff = (lastVOC - currentVOC);

                    // make it always a positive diff
                    if (diff < 0) {
                        diff *= -1;
                    }

                    int requiredDiff = (int) vocSendOnChangePPM;

                    DEBUGSERIAL.print(F("VOC Diff:"));
                    DEBUGSERIAL.print(F(" current="));
                    DEBUGSERIAL.print(currentVOC, DEC);
                    DEBUGSERIAL.print(F(" last="));
                    DEBUGSERIAL.print(lastVOC, DEC);
                    DEBUGSERIAL.print(F(" requDiff="));
                    DEBUGSERIAL.print(requiredDiff, DEC);
                    DEBUGSERIAL.print(F(" diff="));
                    DEBUGSERIAL.println(diff, DEC);


                    if (diff >= requiredDiff) {
                        DEBUGSERIAL.print(F("VOC by Diff: "));
                        DEBUGSERIAL.println(currentVOC, DEC);
                        Knx.write(6, currentVOC);
                    }
                }
            }
            if (currentVOC != UNDEFINED_VOC) {
                
                Knx.write(7, currentVOC<=vocLowerAlarmValue);
                Knx.write(8, currentVOC>=vocUpperAlarmValue);
                
                lastVOC = currentVOC;
            }

        }

        /* ***************************
         * Ampel
         * ***************************/


    } else {
 
    }
}

// Callback function to handle com objects updates
void knxEvents(byte index) {
    // nothing to do in this sketch
};

void readVOC() {
    Wire.requestFrom(iaqaddress, 9);

    vocPredict = (Wire.read() << 8 | Wire.read());
    vocState = Wire.read();
    vocResistance = (Wire.read()& 0x00) | (Wire.read() << 16) | (Wire.read() << 8 | Wire.read());
    vocTVOC = (Wire.read() << 8 | Wire.read());
}

void checkVocStatus() {
#ifdef DEBUG    
    if (vocState == 0x10) {
        DEBUGSERIAL.println(F("Warming up..."));
    } else if (vocState == 0x00) {
        DEBUGSERIAL.println(F("Ready"));
    } else if (vocState == 0x01) {
        DEBUGSERIAL.println(F("Busy"));
    } else if (vocState == 0x80) {
        DEBUGSERIAL.println(F("Error"));
    } else
        DEBUGSERIAL.println(F("No Status, check module"));
#endif
}

/**
 * Read temperatur and add the parametrized offset 
 * @return temp+offset
 */
float readTemp() {
    float temp = htu21d.readTemperature();
    float off = (float) tempOffset / 10;
//    DEBUGSERIAL.print(F("Adding temp offset: "));
//    DEBUGSERIAL.println(off);
    temp += off;
    return temp;
}

/**
 * Read humidity and add the parametrized offset 
 * @return rh+offset
 */
float readRH() {
    float humd = htu21d.readHumidity();
//    DEBUGSERIAL.print(F("Adding RH offset: "));
//    DEBUGSERIAL.println(rhOffset);
    humd += rhOffset;
    return humd;
}
