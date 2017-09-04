/* *****************************************************************************
 * Includes
 */
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include "Constants.h"
#include "Frontend8Btn8Led.h"
#include "RotoChannel.h"
#include <KonnektingDevice.h>
#include "kdevice_DFF_4.1.h"
#include "Memory.h"


/* *****************************************************************************
 * Defines
 */

#define KNX_SERIAL Serial1

#define CHANNEL_PARAM_OFFSET 12

#define ENABLE_LEVELSHIFT A0

#define I2C_ADDR_RELAY 1
#define I2C_ADDR_FRONTEND 0

#define INT_PIN_FRONTEND 38

#define PROG_LED 19
#define PROG_BTN 15

// MCP23017 Output register<->pin map
#define OA0 0
#define OA1 1
#define OA2 2
#define OA3 3
#define OA4 4
#define OA5 5
#define OA6 6
#define OA7 7
#define OB0 8
#define OB1 9
#define OB2 10
#define OB3 11
#define OB4 12
#define OB5 13
#define OB6 14
#define OB7 15

#define WINDOW_OPEN_TIME 47 //sec
#define WINDOW_CLOSE_TIME 40 //sec
#define SHUTTER_OPEN_TIME 28 //sec
#define SHUTTER_CLOSE_TIME 26 //sec

/* *****************************************************************************
 * Variables
 */
Adafruit_MCP23017 mcpRelay;
Adafruit_MCP23017 mcpFrontend;
Frontend8Btn8Led frontend;
#define CHANNELS_COUNT 4
RotoChannel channels[CHANNELS_COUNT] = {
    //          relay group number (to calc relays, LED + button)
    //          |  set pin open
    //          |  |    reset pin open
    //          |  |    |    set pin close
    //          |  |    |    |    reset pin close
    //          |  |    |    |    |
    RotoChannel(0, OB1, OB0, OB3, OB2), // A+B 
    RotoChannel(1, OB5, OB4, OB7, OB6), // C+D 
    RotoChannel(2, OA0, OA1, OA2, OA3), // E+F 
    RotoChannel(3, OA4, OA5, OA6, OA7)  // G+H
};



int last = 0;
bool state = false;

/** 
 * Frontend Button Callback
 */
void buttonEvent(int i) {
    Debug.println(F("--> buttonEvent: %i"),i);

    int group = i / 2; // ganzzahlige division, ohne aufrunden bei ",5" ...
    bool openButton = even(i); // true -> open-button, false -> close-button

    channels[group].doButton(openButton);

    Debug.println(F("--> buttonEvent: %i done"), i);
}

/*
 * HELPER FUNCTION
 * returns true, if given integer is an even value, false if value is odd
 */
boolean even(int i) {
    if ((i & 1) == 0)
        return true;
    else {
        return false;
    }
}

/**
 * Interrupt Handler for Frontend PortExtender Interrupt Pin
 * Delegate to frontend control to check which button was pressed
 */
void frontendISR() {
    frontend.interrupt();
}

/* *****************************************************************************
 * S E T U P
 */
void setup() {

    SerialUSB.begin(115200);
    while (!SerialUSB) {
        // wait for serial connection
    }

    /*
     * make debug serial port known to debug class
     * Means: KONNEKTING will use this serial port for console debugging
     */
    Debug.setPrintStream(&SerialUSB);

    Debug.println(F("Setup..."));
    
    /*
     * Set memory functions
     */
    Konnekting.setMemoryReadFunc(&readMemory);
    Konnekting.setMemoryWriteFunc(&writeMemory);
    Konnekting.setMemoryUpdateFunc(&updateMemory);
    Konnekting.setMemoryCommitFunc(&commitMemory);
    
    // M0dularis+ uses "Serial1" !!!
    Konnekting.init(KNX_SERIAL, PROG_BTN, PROG_LED, MANUFACTURER_ID, DEVICE_ID, REVISION);

    /*
     *  Read general settings
     */
    uint8_t val = Konnekting.getUINT8Param(PARAM_startupDelay);
    int paramStartupDelay = (val == 0xff ? 0 : val * 1000); //seconds
    Debug.println("P: startupDelay=%i", paramStartupDelay);

    bool paramManualControl = Konnekting.getUINT8Param(PARAM_manualControl) == 0x01 ? true : false;
    Debug.println("P: paramManualControl=%i", paramManualControl);

    val = Konnekting.getUINT8Param(PARAM_ventilationTime);
    int paramVentilationTime = (val == 0xff ? 5 * 60 * 1000 : val * 60 * 1000); // minutes
    Debug.println("P: paramVentilationTime=%i", paramVentilationTime);

    /* 
     * Startup MCP port extender on relay board and frontend
     */
    Debug.print(F("Setup MCPs"));
    Debug.print(F(" ...relays"));
    mcpRelay.begin(I2C_ADDR_RELAY);
    Debug.print(F(" ...frontend"));
    mcpFrontend.begin(I2C_ADDR_FRONTEND);
    frontend.init(mcpFrontend);
    // watch for interrupts (button events from frontend).
    if (paramManualControl) {
        pinMode(INT_PIN_FRONTEND, INPUT);
        attachInterrupt(INT_PIN_FRONTEND, frontendISR, RISING);
        frontend.setButtonCallbackFct(&buttonEvent);
        Debug.print(F(" ...interrupt+callback"));
    }
    Debug.println(F(" *done*"));

    /* 
     * enable I2C level shifter Vcc and port extender on relay board by dedicated GPIO pin
     */
    pinMode(ENABLE_LEVELSHIFT, OUTPUT);
    digitalWrite(ENABLE_LEVELSHIFT, HIGH);
    Debug.println(F("levelshift+enable %i"), ENABLE_LEVELSHIFT);

    /* 
     * init relay channels
     */
    for (int i = 0; i < CHANNELS_COUNT; i++) {

        Debug.println(F("Reading channel config for #%i"), i);
        uint8_t channelSetting = Konnekting.getUINT8Param(PARAM_setting_channel_ab + i);

        Debug.println(F("Channel #%i has setting 0x%02x"), i, channelSetting);

        switch (channelSetting) {

                // window and shutter needs full config
            case OPTION_SETTINGS_WINDOW:
            case OPTION_SETTINGS_SHUTTER:
                // fill struct with config data
                ChannelConfig config;
                config.setting = channelSetting;
                config.runTimeClose = Konnekting.getUINT8Param(PARAM_channel_runTimeClose + (i * CHANNEL_PARAM_OFFSET));
                Debug.println(F("runTimeClose: %i"), config.runTimeClose);
                config.runTimeOpen = Konnekting.getUINT8Param(PARAM_channel_runTimeOpen + (i * CHANNEL_PARAM_OFFSET));
                Debug.println(F("runTimeOpen: %i"), config.runTimeOpen);
                config.runTimeRollover = Konnekting.getUINT8Param(PARAM_channel_runTimeRollover + (i * CHANNEL_PARAM_OFFSET));
                Debug.println(F("runTimeRollover: %i"), config.runTimeRollover);
                config.shortTimeRun = Konnekting.getUINT8Param(PARAM_channel_shortTimeRun + (i * CHANNEL_PARAM_OFFSET));
                Debug.println(F("shortTimeRun: %i"), config.shortTimeRun);
                config.shortTimeRunTime = Konnekting.getUINT8Param(PARAM_channel_shortTimeRunTime + (i * CHANNEL_PARAM_OFFSET));
                Debug.println(F("shortTimeRunTime: %i"), config.shortTimeRunTime);
                config.lockAction = Konnekting.getUINT8Param(PARAM_channel_lockAction + (i * CHANNEL_PARAM_OFFSET));
                Debug.println(F("lockAction: %i"), config.lockAction);
                config.ventByComObj = Konnekting.getUINT8Param(PARAM_channel_ventByComObj + (i * CHANNEL_PARAM_OFFSET));
                Debug.println(F("ventByComObj: %i"), config.ventByComObj);
                config.absPositionComObj = Konnekting.getUINT8Param(PARAM_channel_absPositionComObj + (i * CHANNEL_PARAM_OFFSET));
                Debug.println(F("absPositionComObj: %i"), config.absPositionComObj);
                config.referenceRunComObj = Konnekting.getUINT8Param(PARAM_channel_referenceRunComObj + (i * CHANNEL_PARAM_OFFSET));
                Debug.println(F("referenceRunComObj: %i"), config.referenceRunComObj);
                config.runStatusComObj = Konnekting.getUINT8Param(PARAM_channel_runStatusComObj + (i * CHANNEL_PARAM_OFFSET));
                Debug.println(F("runStatusComObj: %i"), config.runStatusComObj);
                config.absPosStatusComObj = Konnekting.getUINT8Param(PARAM_channel_absPosStatusComObj + (i * CHANNEL_PARAM_OFFSET));
                Debug.println(F("absPosStatusComObj: %i"), config.absPosStatusComObj);
                config.runStatusPositionComObj = Konnekting.getUINT8Param(PARAM_channel_runStatusPositionComObj + (i * CHANNEL_PARAM_OFFSET));
                Debug.println(F("runStatusPositionComObj: %i"), config.runStatusPositionComObj);

                // set config struct on channel
                channels[i].setConfig(config);
                channels[i].init(mcpRelay, frontend);
                break;

                // "same as" duplicates/clones from Channel A+B
            case OPTION_SETTINGS_SAMEAS:
                if (i >= 2) {
                    ChannelConfig sameConfig;
                    sameConfig = channels[0].getConfig(); // same as channel A+B
                    channels[i].setConfig(sameConfig);
                    channels[i].init(mcpRelay, frontend);
                } else {
                    Debug.println(F("ERROR: SameConfig cannot be done with channel id<2 !!!"));
                }
                break;

            case OPTION_SETTINGS_UNUSED:
                // when channel is disabled, no need to read any config from memory --> faster
            default:
                Debug.println(F("No channel config for channel #%i"), i);
                break;

        }

    }

    // initial delay
    Debug.print(F("Initial delay %ims "), paramStartupDelay);
    delay(paramStartupDelay);
    Debug.println(F(" *done*"));

    // TODO get last state from eeprom
    // --> was window or shutter opened or closed before power down?

    last = millis();
    Debug.println(F("Setup *done*"));
}

/* *****************************************************************************
 * L O O P
 */
void loop() {
    Knx.task();
    // it's not ready, unless it's programmed via Suite
    if (Konnekting.isReadyForApplication()) {

        frontend.work();
        for (int i = 0; i < CHANNELS_COUNT; i++) {
            channels[i].work();
        }

        // Blink Prog-LED SLOW for testing purpose
        if (millis() - last > 1000) {
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
        if (!Konnekting.isProgState() && (millis() - last > 200)) {
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
    // nothing to do for now
};
