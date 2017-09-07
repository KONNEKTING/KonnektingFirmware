/*
 * RotoChannel.cpp
 *
 *  Created on: 27.03.2017
 *      Author: achristian
 */

#include "RotoChannel.h"
#include "Constants.h"
#include "DebugUtil.h"
#include "kdevice_DFF_4.1.h"

RotoChannel::RotoChannel(int group, int setPinOpen, int resetPinOpen, int setPinClose, int resetPinClose) {

    _group = group;

    _setPinOpen = setPinOpen;
    _resetPinOpen = resetPinOpen;

    _openStep = 0;
    _closeStep = 0;

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


ChannelConfig RotoChannel::getConfig() {
    return _config;
}

void RotoChannel::init(Adafruit_MCP23017& mcp, Frontend8Btn8Led& frontend) {

    if (!_enabled) {
        Debug.println(F("no config, stopping init"));
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

    Debug.print(F("Init of group %i"), _group);
    switch (_startupAction) {
        case A_OPEN:
            _initDone = false;
            Debug.println(F(" to OPEN"));
            doOpen();
            break;
        case A_CLOSE:
            _initDone = false;
            Debug.println(F(" to CLOSE"));
            doClose();
            break;
        case A_NONE:
        default:
            _initDone = true;
            Debug.println(F(" to NONE"));
            _status = CS_UNDEFINED;
            _moveStatus = MS_STOP;
            break;
    }

}

void RotoChannel::setConfig(ChannelConfig config) {
    _config = config;
    _enabled = true;
    
    Debug.println(F("_config.runTimeOpen=%i _config.runTimeClose=%i"),_config.runTimeOpen, _config.runTimeClose);

    _openStep = (100.0 / (_config.runTimeOpen * SECOND)) / 100.0;
    _closeStep = (100.0 / (_config.runTimeClose * SECOND)) / 100.0;
    
    Debug.println(F("open Step=%3.9f%%/ms"),_openStep);
    Debug.println(F("close Step=%3.9f%%/ms"),_closeStep);

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
                waitTime = _config.runTimeOpen;
                break;
            case A_CLOSE:
                waitTime = _config.runTimeClose;
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
            Debug.println(F("Init of group %i *done*. Pos=%3.9f"), _group, _position);

        }

    } else if (_manualMoveRequest) {

        Debug.println(F("manualMoveRequest on group: %i"), _group);

        Debug.println(F("current move status: %i"), _moveStatus);

        Debug.println(F("current status: %i"), _status);

        if (_moveStatus != MS_STOP) {
            doStop(); // wenn wir noch fahren, anhalten
        } else if (_manualMoveRequestOpenButton) { // ansonsten je nach button öffnen oder schließen
            doOpen();
        } else {
            doClose();
        }

        _manualMoveRequest = false;

    } else {
        //Debug.println(F("work on channel %i"), _channel);
    }

    if (_initDone) {
        updateStatus();
    }

    updateLEDs();

    _lastMoveStatus = _moveStatus;

}

void RotoChannel::doButton(bool openButton) {
    Debug.println(F("doButton on group=%i openButton=%i"), _group, openButton);

    _manualMoveRequest = true; // signal move request
    _manualMoveRequestOpenButton = openButton;
}

void RotoChannel::doOpen() {

    /*
     * The delay in here will definitley block the loop(),
     * bus this happens not that often, so we won't miss a telegram
     */

    Debug.println(F("doOpen on group=%i"), _group);

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

    Debug.println(F("doClose on group=%i"), _group);

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
    Debug.print(F("doStop on group %i with move status "), _group);

    switch (_moveStatus) {
        case MS_CLOSING:
            Debug.println(F("CLOSING"));
            doClose();
            _moveStatus = MS_STOP;
            break;
        case MS_OPENING:
            Debug.println(F("OPENING"));
            doOpen();
            _moveStatus = MS_STOP;
            break;
        default:
            Debug.println(F("STOP"));

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

            //      Debug.println(F("BLINK "));

            _lastBlinkState = !_lastBlinkState;

            // get correct LED for blinking
            int led = _group * 2;
            switch (_moveStatus) {
                case MS_CLOSING:
                    //        Debug.println(F("CLOSING"));
                    led += 1;
                    break;
                case MS_OPENING:
                    //        Debug.println(F("OPENING"));
                    led += 0;
                    break;
                default:
                    //        Debug.println(F("NONE?"));
                    break;
            }
            _frontend.setLED(led, _lastBlinkState);
            _lastBlinkMillis = millis();
        }

    }

    // ensure LEDs are off after moving
    if (_lastMoveStatus != MS_STOP && _moveStatus == MS_STOP) {
        int led = _group * 2;
        Debug.print(F("STOP reached. Set LEDs to "));

        switch (_status) {
            case CS_OPENED:
                _frontend.setLED(led + 0, true); // auf
                _frontend.setLED(led + 1, false); // zu
                Debug.println(F("OPENED"));
                break;
            case CS_CLOSED:
                _frontend.setLED(led + 0, false); // auf
                _frontend.setLED(led + 1, true); // zu
                Debug.println(F("CLOSED"));

                break;
            case CS_UNDEFINED:
            default:
                _frontend.setLED(led + 0, false); // auf
                _frontend.setLED(led + 1, false); // zu
                Debug.println(F("UNDEFINED"));
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
        Debug.println(F("O: duration: %i"), duration);
        Debug.println(F("O: position: %3.9f"), _position);
        Debug.println(F("O: delta: %3.9f"), delta);

        Debug.println(F("O: new position: %3.9f"), _newPosition);

        // limit to 1
        if (_newPosition > 1.0) {
            _newPosition = 1.0;
        }

        unsigned long allowedTime = (1.0-_position) * (_config.runTimeOpen*SECOND);
        Debug.println(F("O: allowed time open: %i"), allowedTime);



        // if open move time exceeds "open time", force position to "completely open"
        //    if (duration > _openTime * SECOND) {
        if (duration > allowedTime) {
            _newPosition = 1.0;
            Debug.println(F("O: Time limit for OPEN reached on group %i"), _group);
        }

        if (_newPosition == 1.0) {
            _status = CS_OPENED;
            _moveStatus = MS_STOP;
            Debug.println(F("O: Group %i  is now OPENED"), _group);
        } else {
            _status = CS_OPEN;
        }

    } else
        // Fall: Schließe ODER schließen gestoppt
        if ((_lastMoveStatus == MS_CLOSING && _moveStatus == MS_CLOSING) || (_lastMoveStatus == MS_CLOSING && _moveStatus == MS_STOP)) {


        unsigned long duration = millis() - _startMoveMillis;
        float delta = (float) duration * _closeStep;

        _newPosition = _position - delta;
        Debug.println(F("C: duration: %i"), duration);
        Debug.println(F("C: position: %3.9f"), _position);
        Debug.println(F("C: delta: %3.9f"), delta);

        Debug.println(F("C: new position: %3.9f"), _newPosition);

        // limit to 0
        if (_newPosition < 0.0) {
            _newPosition = 0.0;
        }

        unsigned long allowedTime = (_position) * (_config.runTimeClose*SECOND);
        Debug.println(F("C: allowed time close: %i"), allowedTime);

        // if open move time exceeds "open time", force position to "completely close"
        //if (duration > _closeTime * SECOND) {
        if (duration > allowedTime) {
            _newPosition = 0.0;
            Debug.println(F("C: Time limit for CLOSE reached on group %i"), _group);
        }

        if (_newPosition == 0.0) {
            _status = CS_CLOSED;
            _moveStatus = MS_STOP;
            Debug.println(F("C: Group %i is now CLOSED"), _group);
        } else {
            _status = CS_OPEN;
        }

    }


    // if we just stopped NOW ...
    if (_lastMoveStatus != MS_STOP && _moveStatus == MS_STOP) {

        // apply position
        _position = _newPosition;

        Debug.println(F("Group %i is finally on position %3.9f"), _group, _position);

    }


}

bool RotoChannel::knxEvents(byte index) {
    
    int baseIndex = COM_OBJ_OFFSET + (_group * COM_OBJ_PER_CHANNEL);
            
    if (index<baseIndex || index>baseIndex+COM_OBJ_PER_CHANNEL) {
        // nothing to do for this channel
        return false;
    }
    
    byte myIndex=index-baseIndex;
    
    switch(myIndex) {
        case COMOBJ_abOpenClose-COM_OBJ_OFFSET:
            // handle open close
            break;
        case COMOBJ_abShortStop-COM_OBJ_OFFSET:
            // handle short-stop
            break;
        // ....
        default:
            Debug.println("ComObj %i(%i) is not yet implemented for channel with group %i", index, myIndex, _group);
            // not implemented yet?!
            return false;
    }
    return true;
}