/*
 * TopPCB.cpp
 *
 *  Created on: 27.03.2017
 *      Author: achristian
 */

#include "Frontend8Btn8Led.h"
#include "DebugUtil.h"

Frontend8Btn8Led::Frontend8Btn8Led() {
    _buttonCallbackFunc = NULL;
    _interrupt = false;
}

void Frontend8Btn8Led::init(Adafruit_MCP23017& mcp) {
    _mcp = mcp;

    // No "mirroring" required, as buttons on port-b only used
    // The INTA/B will not be Floating
    // INTs will be signaled with a HIGH
    _mcp.setupInterrupts(false, false, HIGH);

    // setup LED Outputs
    for (int i = 0; i < 8; i++) {
        _mcp.pinMode(i, OUTPUT);
    }

    // setup Button Inputs
    for (int i = 8; i < 17; i++) {
        _mcp.pinMode(i, INPUT);
        _mcp.pullUp(i, HIGH);
        _mcp.setupInterruptPin(i, CHANGE);
    }

    clearInterrupt();

}

void Frontend8Btn8Led::work() {
    if (_interrupt) {

        // Get more information from the MCP from the INT
        uint8_t gpioPin = _mcp.getLastInterruptPin();
        uint8_t newState = _mcp.getLastInterruptPinValue();

        int btn = gpioPin - 8;

        Debug.println(F("Frontend work. Button=%i"), btn);

        Debug.println(F("lastState=%i newState=%i"), lastState[btn], newState);

        if (*_buttonCallbackFunc != NULL && (!lastState[btn] && newState == 1) /* rising state change required*/) {
            SerialUSB.println("FORWARD");
            _buttonCallbackFunc(btn);
        } else {
            SerialUSB.println("IGNORE");
        }
        lastState[btn] = newState;
        clearInterrupt();
    }
}

void Frontend8Btn8Led::setLED(int ledNum, bool state) {
    //  SerialUSB.print("setLED: ");
    //  SerialUSB.print(ledNum);
    //  SerialUSB.print(" -> ");
    //  SerialUSB.println(state);
    if (ledNum < 0 || ledNum > 7) {
        // do nothing!
        return;
    }
    _mcp.digitalWrite(ledNum, state);
}

void Frontend8Btn8Led::setButtonCallbackFct(void (*func)(int)) {
    _buttonCallbackFunc = func;
}

void Frontend8Btn8Led::interrupt() {
    _interrupt = true;
}

void Frontend8Btn8Led::clearInterrupt() {
    Debug.println(F("Clear Frontend ISR register..."));
    // ensure int register is cleared
    // we have to wait for the interrupt condition to finish
    // otherwise we might go to sleep with an ongoing condition and never wake up again.
    // as, an action is required to clear the INT flag, and allow it to trigger again.
    // see datasheet for datails.
    while (!(_mcp.readGPIO(1))) {
        // The interrupt condition will be cleared after the LSb of the data is
        // clocked out during a read command of GPIO or INTCAP.
    }

    _interrupt = false;
    Debug.println(F("Clear Frontend ISR register... *done*"));

}



