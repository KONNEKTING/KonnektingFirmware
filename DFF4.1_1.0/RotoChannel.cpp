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
#include "KonnektingDevice.h"

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

    _config = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
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

    _baseIndex = COMOBJ_OFFSET + (_group * COMOBJ_PER_CHANNEL);

}

void RotoChannel::setConfig(ChannelConfig config) {
    _config = config;
    _enabled = true;

    Debug.println(F("_config.runTimeOpen=%i ms, _config.runTimeClose=%i ms"), getTime(_config.runTimeOpen), getTime(_config.runTimeClose));

    _openStep = (100.0 / (getTime(_config.runTimeOpen))) / 100.0;
    _closeStep = (100.0 / (getTime(_config.runTimeClose))) / 100.0;

    Debug.println(F("open Step=%3.9f%%/ms"), _openStep);
    Debug.println(F("close Step=%3.9f%%/ms"), _closeStep);

}

void RotoChannel::work() {

    if (!_enabled) {
        // nothing do to, channel is disabled
        return;
    }

    // Do init-stuff, if not already done
    if (!_initDone) {

        // check how long we have to wait for final end position
        unsigned long waitTime = 0;
        switch (_startupAction) {
            case A_OPEN:
                waitTime = getTime(_config.runTimeOpen);
                break;
            case A_CLOSE:
                waitTime = getTime(_config.runTimeClose);
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

            int led = _group * 2;

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

    // update LEDs in any case
    workLEDs();

    if (_initDone) {
        workPosition();
        workStatus();
    }



    _lastMoveStatus = _moveStatus;
}

/**
 * Send status updates, if required
 */
void RotoChannel::workStatus() {
    bool sendStatus = false;
    float currPos = 0;

    switch (_moveStatus) {

            // while moving, the current position is available in "_newPosition" only
        case MS_CLOSING:
        case MS_OPENING:
            currPos = _newPosition;
            sendStatus = true;
            break;

        case MS_STOP:
            // once we stopped, the position is available in _position
            if (_lastMoveStatus != MS_STOP) {
                currPos = _position;
                sendStatus = true;
            }
            break;
    }

    uint8_t positionValueToSend = currPos * 255; // map 0..100% to 0..255 unsigned byte
    if (_initDone && // only send when init is done AND
            _lastSentPosition != positionValueToSend && // we need to have different value as last time
            (sendStatus && millis() > (_lastStatusUpdate + STATUS_UPDATE_INTERVAL)) || // if it's time for an update and we really have an update OR
            (_lastMoveStatus != MS_STOP && _moveStatus == MS_STOP) // if we just stopped
            ) {
        Knx.write((byte) (_baseIndex + (COMOBJ_abStatusCurrentPos - COMOBJ_OFFSET)), positionValueToSend);
        Debug.println("[%i] status curr pos: %f -> 0x%02x", _group, currPos, positionValueToSend);
        // store last sent status. Ensures with if.clause that we don't send the same value twice (especially on stop)
        _lastSentPosition = positionValueToSend;
        _lastStatusUpdate = millis();
    }
}

void RotoChannel::doButton(bool openButton) {

    Debug.println(F("doButton on group=%i openButton=%i"), _group, openButton);

    _manualMoveRequest = true; // signal move request
    _manualMoveRequestOpenButton = openButton;
}

void RotoChannel::doOpen() {

    /*
     * TODO The delay in here will definitley block the loop(),
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
void RotoChannel::workLEDs() {

    // while not yet done with init, led all LEDs blink so signal init
    if (!_initDone) {
        _lastBlinkState = !_lastBlinkState;
        if (millis() - _lastBlinkMillis > BLINK_DELAY) {
            int led = _group * 2;
            _frontend.setLED(led, !_lastBlinkState);
            _frontend.setLED(led + 1, _lastBlinkState);
            _lastBlinkMillis = millis();
        }
        return;
    }

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

void RotoChannel::workPosition() {

    //#define DEBUG_UPDATE_STATUS    

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
#ifdef DEBUG_UPDATE_STATUS
        Debug.println(F("O: duration: %i"), duration);
        Debug.println(F("O: position: %3.9f"), _position);
        Debug.println(F("O: delta: %3.9f"), delta);
        Debug.println(F("O: new position: %3.9f"), _newPosition);
#endif        

        // limit to 1
        if (_newPosition > 1.0) {
            _newPosition = 1.0;
        }

        unsigned long allowedTime = (1.0 - _position) * getTime(_config.runTimeOpen);
#ifdef DEBUG_UPDATE_STATUS        
        Debug.println(F("O: allowed time open: %i"), allowedTime);
#endif


        // if open move time exceeds "open time", force position to "completely open"
        //    if (duration > _openTime * SECOND) {
        if (duration > allowedTime) {
            _newPosition = 1.0;
#ifdef DEBUG_UPDATE_STATUS
            Debug.println(F("O: Time limit for OPEN reached on group %i"), _group);
#endif            
        }

        if (_newPosition == 1.0) {
            _status = CS_OPENED;
            _moveStatus = MS_STOP;
#ifdef DEBUG_UPDATE_STATUS            
            Debug.println(F("O: Group %i  is now OPENED"), _group);
#endif            
        } else {
            _status = CS_OPEN;
        }

    } else
        // Fall: Schließe ODER schließen gestoppt
        if ((_lastMoveStatus == MS_CLOSING && _moveStatus == MS_CLOSING) || (_lastMoveStatus == MS_CLOSING && _moveStatus == MS_STOP)) {


        unsigned long duration = millis() - _startMoveMillis;
        float delta = (float) duration * _closeStep;

        _newPosition = _position - delta;
#ifdef DEBUG_UPDATE_STATUS        
        Debug.println(F("C: duration: %i"), duration);
        Debug.println(F("C: position: %3.9f"), _position);
        Debug.println(F("C: delta: %3.9f"), delta);
        Debug.println(F("C: new position: %3.9f"), _newPosition);
#endif

        // limit to 0
        if (_newPosition < 0.0) {
            _newPosition = 0.0;
        }

        unsigned long allowedTime = (_position) * (getTime(_config.runTimeClose));
#ifdef DEBUG_UPDATE_STATUS        
        Debug.println(F("C: allowed time close: %i"), allowedTime);
#endif        

        // if open move time exceeds "open time", force position to "completely close"
        //if (duration > _closeTime * SECOND) {
        if (duration > allowedTime) {
            _newPosition = 0.0;
#ifdef DEBUG_UPDATE_STATUS            
            Debug.println(F("C: Time limit for CLOSE reached on group %i"), _group);
#endif            
        }

        if (_newPosition == 0.0) {
            _status = CS_CLOSED;
            _moveStatus = MS_STOP;
#ifdef DEBUG_UPDATE_STATUS            
            Debug.println(F("C: Group %i is now CLOSED"), _group);
#endif            
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

/**
 * Gets given time in milliseconds, incl. rollover time
 * @param time time in sec
 * @return time in ms incl. rollovertime
 */
uint32_t RotoChannel::getTime(uint8_t time) {
    return time * SECOND * (1.0f + (_config.runTimeRollover / 100));
}

bool RotoChannel::knxEvents(byte index) {

    if (!_enabled) {
        return false;
    }

    // common comobjects
    switch (index) {
        
        case (COMOBJ_centralShutterLock): // central lock 
        {
            /*
             * TODO
             * - store lock state?
             * - block external action on lock!
             * --> introduce channel lock flag
             */
            byte value = Knx.read(index);
            switch (_config.lockAction) {
                case OPTION_LOCK_ACTION_NONE:
                    // nothing to do for us
                    break;
                case OPTION_LOCK_ACTION_OPEN:
                    doOpen();
                    break;
                case OPTION_LOCK_ACTION_CLOSE:
                    doClose();
                    break;
            }
        }

    }

    // check if index is relevant for this channel
    if (index < _baseIndex || index > _baseIndex + COMOBJ_PER_CHANNEL) {
        // nothing to do for this channel
        Debug.println(F("[%i] nothing to do: baseIndex=%i"), _group, _baseIndex);
        return false;
    }

    // calculate help-index for this channel
    byte chIndex = index - _baseIndex;
    Debug.println(F("[%i] baseIndex=%i chIndex=%i"), _group, _baseIndex, chIndex);

    // channel specific comobjects
    switch (chIndex) {


            // handle open close
        case (COMOBJ_abOpenClose - COMOBJ_OFFSET):
        {
            byte value = Knx.read(index);
            if (value == DPT1_009_open) {
                Debug.println(F("[%i] open"), _group);
                doOpen();
            } else {
                Debug.println(F("[%i] close"), _group);
                doClose();
            }
            break;
        }

        case (COMOBJ_abStop - COMOBJ_OFFSET):
        {
            byte value = Knx.read(index);
            Debug.println(F("[%i] doStop: %i"), _group, value);
            if (value == DPT1_010_stop) {
                doStop();
            }
            break;
        }

        case (COMOBJ_abAbsPosition - COMOBJ_OFFSET):
        {
            byte value = Knx.read(index);
            float percentage = value * (BYTE_PERCENT);
            Debug.println(F("[%i] absPos: %f"), _group, percentage);
            break;
        }

        case (COMOBJ_abReference - COMOBJ_OFFSET):
        {
            byte value = Knx.read(index);
            if (value == DPT1_001_on) {
                Debug.println(F("[%i] reference: %i"), _group, value);

                /*
                 * TODO
                 * - store current position
                 * - block any external action
                 * - close window/shutter
                 * - restore position
                 * - unblock any external action
                 * --> block-flag required!
                 */
            }
            break;
        }

        case (COMOBJ_abFixPosition - COMOBJ_OFFSET):
        {
            byte value = Knx.read(index);
            if (value == DPT1_001_on) {
                Debug.println(F("[%i] doStop: %i"), _group, value);

                /*
                 * TODO
                 * - define exit-condition on doOpen/doClose based on targetted position
                 */
            }
            break;
        }

        case (COMOBJ_abVentilation - COMOBJ_OFFSET):
        {
            byte value = Knx.read(index);
            if (value == DPT1_001_on) {
                Debug.println(F("[%i] ventilation: %i"), _group, value);

                /*
                 * TODO
                 * - only work in window-mode! ignore on shutter mode!
                 * - store current time in channel-global variable
                 * - store current position
                 * - block external action
                 * - open window fully
                 * - check in work() for outdated time on global variable -> then close window/restore last position --> unblock external action
                 */
            }
            break;
        }

        case (COMOBJ_abWindAlarm - COMOBJ_OFFSET):
        {
            byte value = Knx.read(index);
            Debug.println(F("[%i] windalarm: %i"), _group, value);
            /*
             * TODO
             * Introduce param that controls what to do? Like with lock?
             */
            break;
        }

        case (COMOBJ_abRainAlarm - COMOBJ_OFFSET):
        {
            byte value = Knx.read(index);
            Debug.println(F("[%i] rainalarm: %i"), _group, value);
            /*
             * TODO
             * - Introduce param that controls what to do? Like with lock?
             * - convert it to central comobj instead of channel comobj --> channel just decided via param how to react on wind/rain alarm
             */
            break;
        }

            // status com obj are "outgoing" comobjs.
            //        case (COMOBJ_abStatusCurrentDirection - COMOBJ_OFFSET):
            //        case (COMOBJ_abStatusMovement - COMOBJ_OFFSET):
            //        case (COMOBJ_abStatusMovementOpen - COMOBJ_OFFSET):
            //        case (COMOBJ_abStatusMovementClose - COMOBJ_OFFSET):
            //        case (COMOBJ_abStatusCurrentPos - COMOBJ_OFFSET):
            //        case (COMOBJ_abStatusLock - COMOBJ_OFFSET):
            //        case (COMOBJ_abStatusOpenPos - COMOBJ_OFFSET):
            //        case (COMOBJ_abStatusClosePos - COMOBJ_OFFSET):

        default:
            Debug.println(F("ComObj %i(%i) is not yet implemented for channel with group %i"), index, chIndex, _group);
            // not implemented yet?!
            return false;
    }
    return true;
}