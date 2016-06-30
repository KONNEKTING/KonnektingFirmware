// comment following line to disable DEBUG mode
#define DEBUG debugSerial

// no need to comment, you can leave it as it is as long you do not change the "#define DEBUG debugSerial" line
#ifdef DEBUG
#include <SoftwareSerial.h>
SoftwareSerial debugSerial(10, 11); // RX, TX
#define CONSOLE(...)  DEBUG.print(__VA_ARGS__);
#define CONSOLELN(...)  DEBUG.println(__VA_ARGS__);
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
    /* don't change this */ Tools.createProgComObject(),

    // Currently, Sketch Index and Suite Index differ for ComObjects :-(

    /* Sketch-Index 1, Suite-Index 0 : */ KnxComObject(KNX_DPT_9_001, COM_OBJ_SENSOR), // Temperatur Messwert
    /* Sketch-Index 2, Suite-Index 1 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR), // Temperatur unterer Meldewert
    /* Sketch-Index 3, Suite-Index 2 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR), // Temperatur oberer Meldewert
    /* Sketch-Index 4, Suite-Index 3 : */ KnxComObject(KNX_DPT_9_007, COM_OBJ_SENSOR), // rH Messwert
    /* Sketch-Index 5, Suite-Index 4 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR), // rH unterer Meldewert
    /* Sketch-Index 6, Suite-Index 5 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR), // rH oberer Meldewert
    /* Sketch-Index 7, Suite-Index 6 : */ KnxComObject(KNX_DPT_9_008, COM_OBJ_SENSOR), // VOC Messwert
    /* Sketch-Index 8, Suite-Index 7 : */ KnxComObject(KNX_DPT_9_008, COM_OBJ_SENSOR), // TVOC Messwert
    /* Sketch-Index 9, Suite-Index 8 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR), // T/VOC unterer Meldewert
    /* Sketch-Index 10, Suite-Index 9 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR), // T/VOC oberer Meldewert

};
const byte KnxDevice::_numberOfComObjects = sizeof (_comObjectsList) / sizeof (KnxComObject); // do no change this code

// Definition of parameter size
byte KnxTools::_paramSizeList[] = {

    // For params, the index in Sketch and Suite is equal

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
const byte KnxTools::_numberOfParams = sizeof (_paramSizeList); // do no change this code


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

    // if debug mode is enabled, setup serial port with 9600 baud    
#ifdef DEBUG
    DEBUG.begin(9600);
    CONSOLELN(F("Setup ..."));
#endif

    // Initialize KNX enabled Arduino Board
    Tools.init(KNX_SERIAL, PROG_BUTTON_PIN, PROG_LED_PIN, MANUFACTURER_ID, DEVICE_ID, REVISION);

    CONSOLELN(F("KNX init done"));

    if (!Tools.isFactorySetting()) {
        int startDelay = (int) Tools.getUINT8Param(0);
        if (startDelay > 0) {
            CONSOLE(F("delay for "));
            CONSOLE(startDelay);
            CONSOLELN(F("s"));
            delay(startDelay * 1000);
            CONSOLELN(F("ready!"));
        }

        previousMillisTemp = millis();
        previousMillisRH = millis();
        previousMillisVOC = millis();

        // Read Temp Sensor Params
        tempActive = Tools.getUINT8Param(1) == 1 ? true : false;
        tempCycleMins = Tools.getUINT8Param(2);
        tempOffset = Tools.getINT8Param(3);

        CONSOLE(F("tempOffset="));
        CONSOLELN(tempOffset);

        tempSendOnChangeDegree = Tools.getUINT8Param(4);

        CONSOLE(F("tempSendOnChangeDegree="));
        CONSOLELN(tempSendOnChangeDegree);

        tempUpperAlarmValue = Tools.getUINT8Param(5);
        tempLowerAlarmValue = Tools.getUINT8Param(6);

        // Read RH Sensor Params
        rhActive = Tools.getUINT8Param(7) == 1 ? true : false;
        rhCycleMins = Tools.getUINT8Param(8);
        rhOffset = Tools.getINT8Param(9);
        rhSendOnChangePercent = Tools.getUINT8Param(10);
        rhUpperAlarmValue = Tools.getUINT8Param(11);
        rhLowerAlarmValue = Tools.getUINT8Param(12);

        // Read VOC Sensor Params
        vocActive = Tools.getUINT8Param(13) == 1 ? true : false;
        vocSensorType = Tools.getUINT8Param(14);
        vocCycleMins = Tools.getUINT8Param(15);
        vocSendOnChangePPM = Tools.getUINT16Param(16);
        vocUpperAlarmValue = Tools.getUINT16Param(17);
        vocLowerAlarmValue = Tools.getUINT16Param(18);

    }

    CONSOLELN(F("Startup delay done"));

    Wire.begin();
    htu21d.begin();

    pinMode(LED_G, OUTPUT);
    pinMode(LED_Y, OUTPUT);
    pinMode(LED_R, OUTPUT);

    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_Y, HIGH);
    digitalWrite(LED_R, HIGH);

    CONSOLELN(F("I2C setup done."));

    CONSOLELN(F("Ready!"));
}

void loop() {
    Knx.task();

    // simple counter to reduce log output
    loopc++;

    // only do measurements and other sketch related stuff if not in programming mode
    if (!Tools.getProgState()) {

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
                    CONSOLE(F("Temperature by cycle:"));
                    CONSOLELN(tempCycleMins, DEC);

                    currentTemp = readTemp();

                    CONSOLE(F("Temperature:"));
                    CONSOLE(currentTemp, 1);
                    CONSOLELN();

                    Knx.write(0, currentTemp);
                } else {
                    if (loopc % 10000 == 1) {
//                        CONSOLE(F("Waiting Temp: "));
//                        CONSOLELN((tempCycleMins * 60 * 1000)-(currentMillis - previousMillisTemp), DEC);
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

                    CONSOLE(F("Temp Diff: current="));
                    CONSOLE(currentTemp, 1);
                    CONSOLE(F(" last="));
                    CONSOLE(lastTemp, 1);
                    CONSOLE(F(" requDiff="));
                    CONSOLE(requiredDiff, 1);
                    CONSOLE(F(" diff="));
                    CONSOLELN(diff, 1);


                    if (diff >= requiredDiff) {
                        CONSOLE(F("Temp by Diff: "));
                        CONSOLE(diff, 1);
                        CONSOLELN();
                        Knx.write(1, currentTemp);
                    }
                }
            }
            
            if (currentTemp != UNDEFINED_TEMP) {
                
                Knx.write(2, currentTemp<=tempLowerAlarmValue);
                Knx.write(3, currentTemp>=tempUpperAlarmValue);
                
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
                    CONSOLE(F("RH by cycle:"));
                    CONSOLELN(rhCycleMins, DEC);

                    currentRH = readRH();

                    CONSOLE(F("RH:"));
                    CONSOLE(currentRH, 1);
                    CONSOLELN();

                    Knx.write(5, currentRH);
                } else {
                    if (loopc % 10000 == 1) {
//                        CONSOLE(F("Waiting RH: "));
//                        CONSOLELN((rhCycleMins * 60 * 1000)-(currentMillis - previousMillisRH), DEC);
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

                    CONSOLE(F("RH Diff: current="));
                    CONSOLE(currentRH, 1);
                    CONSOLE(F(" last="));
                    CONSOLE(lastRH, 1);
                    CONSOLE(F(" requDiff="));
                    CONSOLE(requiredDiff, 1);
                    CONSOLE(F(" diff="));
                    CONSOLELN(diff, 1);


                    if (diff >= requiredDiff) {
                        CONSOLE(F("RH by Diff: "));
                        CONSOLE(diff, 1);
                        CONSOLELN();
                        Knx.write(5, currentRH);
                    }
                }
            }
            if (currentRH != UNDEFINED_RH) {
                
                Knx.write(5, currentRH<=rhLowerAlarmValue);
                Knx.write(6, currentRH>=rhUpperAlarmValue);
                
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

                    CONSOLE(F("VOC by cycle: "));
                    readVOC();
                    CONSOLE(F("CO2="));
                    CONSOLE(vocPredict);
                    CONSOLE(F(" TVoC="));
                    CONSOLELN(vocTVOC);
                    checkVocStatus();


                    if (vocState == 0x10) {
                        vocPredict = 0;
                        vocTVOC = 0;
                    }

                    CONSOLELN(F("Sending CO2"));
                    Knx.write(7, vocPredict);
                } else {
                    if (loopc % 10000 == 1) {
//                        CONSOLE(F("Waiting VOC: "));
//                        CONSOLELN((vocCycleMins * 60 * 1000)-(currentMillis - previousMillisVOC), DEC);
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

                    CONSOLE(F("VOC Diff:"));
                    CONSOLE(F(" current="));
                    CONSOLE(currentVOC, DEC);
                    CONSOLE(F(" last="));
                    CONSOLE(lastVOC, DEC);
                    CONSOLE(F(" requDiff="));
                    CONSOLE(requiredDiff, DEC);
                    CONSOLE(F(" diff="));
                    CONSOLELN(diff, DEC);


                    if (diff >= requiredDiff) {
                        CONSOLE(F("VOC by Diff: "));
                        CONSOLELN(currentVOC, DEC);
                        Knx.write(7, currentVOC);
                    }
                }
            }
            if (currentVOC != UNDEFINED_VOC) {
                
                Knx.write(8, currentVOC<=vocLowerAlarmValue);
                Knx.write(9, currentVOC>=vocUpperAlarmValue);
                
                lastVOC = currentVOC;
            }

        }

        /* ***************************
         * Ampel
         * ***************************/


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
        CONSOLELN(F("Warming up..."));
    } else if (vocState == 0x00) {
        CONSOLELN(F("Ready"));
    } else if (vocState == 0x01) {
        CONSOLELN(F("Busy"));
    } else if (vocState == 0x80) {
        CONSOLELN(F("Error"));
    } else
        CONSOLELN(F("No Status, check module"));
#endif
}

/**
 * Read temperatur and add the parametrized offset 
 * @return temp+offset
 */
float readTemp() {
    float temp = htu21d.readTemperature();
    float off = (float) tempOffset / 10;
//    CONSOLE(F("Adding temp offset: "));
//    CONSOLELN(off);
    temp += off;
    return temp;
}

/**
 * Read humidity and add the parametrized offset 
 * @return rh+offset
 */
float readRH() {
    float humd = htu21d.readHumidity();
//    CONSOLE(F("Adding RH offset: "));
//    CONSOLELN(rhOffset);
    humd += rhOffset;
    return humd;
}