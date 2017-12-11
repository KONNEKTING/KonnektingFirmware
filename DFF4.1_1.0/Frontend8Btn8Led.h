/*
 * Frontend8Btn8Led.h
 *
 *  Created on: 27.03.2017
 *      Author: achristian
 */

#ifndef FRONTEND8BTN8LED_H_
#define FRONTEND8BTN8LED_H_

#include "Arduino.h"
#include <Adafruit_MCP23017.h>

class Frontend8Btn8Led {
public:
    // constructor
    Frontend8Btn8Led();

    void init(Adafruit_MCP23017& mcp);

    void work();

    void setLED(int ledNum, bool state);

    void setButtonCallbackFct(void (*func)(int));

    void interrupt();
    void buttonPressedEvent();

    // -----------------------------------------------------

private:

    Adafruit_MCP23017 _mcp;
    void (*_buttonCallbackFunc)(int);
    bool _interrupt;

    uint8_t lastState[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    void clearInterrupt();


};

#endif /* FRONTEND8BTN8LED_H_ */
