/*
 * RotoChannel.cpp
 *
 *  Created on: 27.03.2017
 *      Author: achristian
 */

#include "RotoChannel.h"
#include "Constants.h"

RotoChannel::RotoChannel(int group, int openTime, int closeTime, int setPinOpen, int resetPinOpen, int setPinClose, int resetPinClose) {

    _group = group;

    _openTime = openTime;
    _closeTime = closeTime;

    _openStep = (100.0 / (openTime * SECOND)) / 100.0;
    _closeStep = (100.0 / (closeTime * SECOND)) / 100.0;

    _setPinOpen = setPinOpen;
    _resetPinOpen = resetPinOpen;

    _setPinClose = setPinClose;
    _resetPinClose = resetPinClose;

    _manualMoveRequest = false;

    _moveStatus = MS_STOP;
    _status = CS_UNDEFINED;

    _initDone = false;
    _startupAction = A_NONE;
    _lastBlinkMillis = millis();
    _lastBlinkState = false;
    _lastMoveStatus = MS_STOP;

    _position = 0.0;

    _startMoveMillis = 0;

    _isStopping = false;

    _config = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
}

RotoChannel::~RotoChannel() {
    // TODO Auto-generated destructor stub
}

void RotoChannel::setConfig(ChannelConfig config) {
    _config = config;
    _enabled = true;
}

ChannelConfig RotoChannel::getConfig() {
    return _config;
}

void RotoChannel::init(Adafruit_MCP23017& mcp, Frontend8Btn8Led& frontend) {

    if (!_enabled) {
        SerialUSB.println("no config, stopping init");
        return;
    }

    switch (_config.setting) {
        case OPTION_SETTINGS_WINDOW:
            _startupAction = A_CLOSE;
            break;
        case OPTION_SETTINGS_SHUTTER:
            _startupAction = A_OPEN;
            break;
    }

    _mcp = mcp;
    _frontend = frontend;

    // configure MCP on relay board
    _mcp.pinMode(_setPinOpen, OUTPUT);
    _mcp.pinMode(_resetPinOpen, OUTPUT);

    _mcp.pinMode(_setPinClose, OUTPUT);
    _mcp.pinMode(_resetPinClose, OUTPUT);

    SerialUSB.print("Init of group ");
    SerialUSB.print(_group);
    switch (_startupAction) {
        case A_OPEN:
            _initDone = false;
            SerialUSB.println(" to OPEN");
            doOpen();
            break;
        case A_CLOSE:
            _initDone = false;
            SerialUSB.println(" to CLOSE");
            doClose();
            break;
        case A_NONE:
        default:
            _initDone = true;
            SerialUSB.println(" to NONE");
            _status = CS_UNDEFINED;
            _moveStatus = MS_STOP;
            break;
    }

    SerialUSB.print("open Step=");
    SerialUSB.println(_openStep);
    SerialUSB.print("close Step=");
    SerialUSB.println(_openStep);
}

void RotoChannel::work() {

    if (!_enabled) {
        // nothing do to, channel is disabled
        return;
    }

    if (!_initDone) {

        // check how long we have to wait for final end position
        unsigned long waitTime = 0;
        switch (_startupAction) {
            case A_OPEN:
                waitTime = _openTime;
                break;
            case A_CLOSE:
                waitTime = _closeTime;
                break;
            case A_NONE:
                waitTime = 0;
                break;

        }

        // if time is over
        if (millis() - _startMoveMillis > waitTime * SECOND) {

            // set final status
            switch (_startupAction) {
                case A_OPEN:
                    _position = 1.0; // open will reduce to 0
                    _status = CS_OPENED;
                    break;
                case A_CLOSE:
                    _position = 0.0; // close will increase to 1
                    _status = CS_CLOSED;
                    break;
                case A_NONE:
                    _status = CS_UNDEFINED;
                    break;
            }

            // ... and finalize init
            _initDone = true;
            SerialUSB.print("Init of group ");
            SerialUSB.print(_group);
            SerialUSB.print(" *done*");
            SerialUSB.print(" Pos =  ");
            SerialUSB.println(_position);

        }

    } else if (_manualMoveRequest) {

        SerialUSB.print("manualMoveRequest on group: ");
        SerialUSB.println(_group);

        SerialUSB.print("current move status: ");
        SerialUSB.println(_moveStatus);

        SerialUSB.print("current status: ");
        SerialUSB.println(_status);

        if (_moveStatus != MS_STOP) {
            doStop(); // wenn wir noch fahren, anhalten
        } else if (_manualMoveRequestOpenButton) { // ansonsten je nach button öffnen oder schließen
            doOpen();
        } else {
            doClose();
        }

        _manualMoveRequest = false;

    } else {
        //SerialUSB.print("work on channel ");
        //SerialUSB.println(_channel);
    }

    if (_initDone) {
        updateStatus();
    }

    updateLEDs();

    _lastMoveStatus = _moveStatus;

}

void RotoChannel::doButton(bool openButton) {
    SerialUSB.print("doButton on group=");
    SerialUSB.print(_group);
    SerialUSB.print(" openButton=");
    SerialUSB.println(openButton);

    _manualMoveRequest = true; // signal move request
    _manualMoveRequestOpenButton = openButton;

}

void RotoChannel::doOpen() {

    /*
     * The delay in here will definitley block the loop(),
     * bus this happens not that often, so we won't miss a telegram
     */

    SerialUSB.print("doOpen on group=");
    SerialUSB.println(_group);

    _mcp.digitalWrite(_setPinOpen, HIGH);
    delay(RELAY_SET_TIME);
    _mcp.digitalWrite(_setPinOpen, LOW);

    delay(ROTO_TRIGGER_DURATION);

    _mcp.digitalWrite(_resetPinOpen, HIGH);
    delay(RELAY_SET_TIME);
    _mcp.digitalWrite(_resetPinOpen, LOW);

    _moveStatus = MS_OPENING;
    _lastAction = A_OPEN;

    if (!_isStopping) {
        _startMoveMillis = millis();
    }

}

void RotoChannel::doClose() {

    SerialUSB.print("doClose on group=");
    SerialUSB.println(_group);

    _mcp.digitalWrite(_setPinClose, HIGH);
    delay(RELAY_SET_TIME);
    _mcp.digitalWrite(_setPinClose, LOW);

    delay(ROTO_TRIGGER_DURATION);

    _mcp.digitalWrite(_resetPinClose, HIGH);
    delay(RELAY_SET_TIME);
    _mcp.digitalWrite(_resetPinClose, LOW);

    _moveStatus = MS_CLOSING;
    _lastAction = A_CLOSE;

    if (!_isStopping) {
        _startMoveMillis = millis();
    }


}

void RotoChannel::doStop() {

    _isStopping = true;
    SerialUSB.print("doStop on group ");
    SerialUSB.print(_group);
    SerialUSB.print(" with move status ");

    switch (_moveStatus) {
        case MS_CLOSING:
            SerialUSB.println("CLOSING");
            doClose();
            _moveStatus = MS_STOP;
            break;
        case MS_OPENING:
            SerialUSB.println("OPENING");
            doOpen();
            _moveStatus = MS_STOP;
            break;
        default:
            SerialUSB.println("STOP");
            break;
    }
    _isStopping = false;

}

/**
 *
 */
void RotoChannel::updateLEDs() {

    // Status Blinken
    if (_moveStatus != MS_STOP) {

        if (millis() - _lastBlinkMillis > BLINK_DELAY) {

            //      SerialUSB.print("BLINK ");

            _lastBlinkState = !_lastBlinkState;

            // get correct LED for blinking
            int led = _group * 2;
            switch (_moveStatus) {
                case MS_CLOSING:
                    //        SerialUSB.println("CLOSING");
                    led += 1;
                    break;
                case MS_OPENING:
                    //        SerialUSB.println("OPENING");
                    led += 0;
                    break;
                default:
                    //        SerialUSB.println("NONE?");
                    break;
            }
            _frontend.setLED(led, _lastBlinkState);
            _lastBlinkMillis = millis();
        }

    }

    // ensure LEDs are off after moving
    if (_lastMoveStatus != MS_STOP && _moveStatus == MS_STOP) {
        int led = _group * 2;
        SerialUSB.print("STOP reached. Set LEDs to ");

        switch (_status) {
            case CS_OPENED:
                _frontend.setLED(led + 0, true); // auf
                _frontend.setLED(led + 1, false); // zu
                SerialUSB.println("OPENED");
                break;
            case CS_CLOSED:
                _frontend.setLED(led + 0, false); // auf
                _frontend.setLED(led + 1, true); // zu
                SerialUSB.println("CLOSED");
                break;
            case CS_UNDEFINED:
            default:
                _frontend.setLED(led + 0, false); // auf
                _frontend.setLED(led + 1, false); // zu
                SerialUSB.println("UNDEFINED");
        }

        _lastBlinkState = false;
    }

}

void RotoChannel::updateStatus() {

    /*
     * Es gibt die folgenden Fälle:
     *
     * - Öffne
     * - Schließe
     *
     * - öffnen gestoppt
     * - schließen gestoppt
     *
     * - öffnen timeout
     * - schließen timeout
     *
     * - weder noch
     */

    // Fall: Öffne ODER öffnen gestoppt
    if ((_lastMoveStatus == MS_OPENING && _moveStatus == MS_OPENING) || (_lastMoveStatus == MS_OPENING && _moveStatus == MS_STOP)) {

        unsigned long duration = millis() - _startMoveMillis;
        float delta = (float) duration * _openStep;

        _newPosition = _position + delta;
        SerialUSB.print("duration: ");
        SerialUSB.println(duration);

        SerialUSB.print("delta: ");
        SerialUSB.println(delta);

        SerialUSB.print("new position: ");
        SerialUSB.println(_newPosition);

        // limit to 1
        if (_newPosition > 1) {
            _newPosition = 1;
        }

        unsigned long allowedTime = _position * _openTime;
        SerialUSB.print("allowed time open: ");
        SerialUSB.println(allowedTime);



        // if open move time exceeds "open time", force position to "completely open"
        //    if (duration > _openTime * SECOND) {
        if (duration > allowedTime) {
            _newPosition = 1;
            SerialUSB.print("Time limit for OPEN reached on group ");
            SerialUSB.println(_group);
        }

        if (_newPosition == 1) {
            _status = CS_OPENED;
            _moveStatus = MS_STOP;
            SerialUSB.print("Group ");
            SerialUSB.print(_group);
            SerialUSB.println(" is now OPENED");
        } else {
            _status = CS_OPEN;
        }

    } else
        // Fall: Schließe ODER schließen gestoppt
        if ((_lastMoveStatus == MS_CLOSING && _moveStatus == MS_CLOSING) || (_lastMoveStatus == MS_CLOSING && _moveStatus == MS_STOP)) {


        unsigned long duration = millis() - _startMoveMillis;
        float delta = (float) duration * _closeStep;

        _newPosition = _position - delta;
        SerialUSB.print("duration: ");
        SerialUSB.println(duration);

        SerialUSB.print("delta: ");
        SerialUSB.println(delta);

        SerialUSB.print("new position: ");
        SerialUSB.println(_newPosition);

        // limit to 0
        if (_newPosition < 0) {
            _newPosition = 0;
        }

        unsigned long allowedTime = (1.0 - _position) * _closeTime;
        SerialUSB.print("allowed time close: ");
        SerialUSB.println(allowedTime);

        // if open move time exceeds "open time", force position to "completely open"
        //if (duration > _closeTime * SECOND) {
        if (duration > allowedTime) {
            _newPosition = 0;
            SerialUSB.print("Time limit for CLOSE reached on group ");
            SerialUSB.println(_group);
        }

        if (_position == 0) {
            _status = CS_CLOSED;
            _moveStatus = MS_STOP;
            SerialUSB.print("Group ");
            SerialUSB.print(_group);
            SerialUSB.println(" is now CLOSED");
        } else {
            _status = CS_OPEN;
        }

    }



    if (_lastMoveStatus != MS_STOP && _moveStatus == MS_STOP) {

        _position = _newPosition;

        SerialUSB.print("Group ");
        SerialUSB.print(_group);
        SerialUSB.print(" is finally on position ");

        SerialUSB.println(_position);
    }


}
