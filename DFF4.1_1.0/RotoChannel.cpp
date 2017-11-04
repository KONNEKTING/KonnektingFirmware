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

    _position = 0.0f;

    _startMoveMillis = NOT_DEFINED;

    _isStopping = false;

    _targetPosition = NOT_DEFINED;

    _config = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    _shutterLock = LCK_UNLOCKED;
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

    Debug.print(F("[%i] Init "), _group);
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

    Debug.println(F("_config.runtimeRollover=%i s,_config.runTimeOpen=%i ms, _config.runTimeClose=%i ms"), _config.runTimeRollover, getTime(_config.runTimeOpen), getTime(_config.runTimeClose));

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
        if (_startMoveMillis != NOT_DEFINED && millis() - _startMoveMillis > waitTime) {

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


    if (_initDone) {
        workPosition();
        workStatus();
    }

    // update LEDs in any case, needs to be done at last
    workLEDs();

    _lastMoveStatus = _moveStatus;
}

/**
 * Send status updates, if required
 */
void RotoChannel::workStatus() {
    // if locked, just don't send any updates, we're locked!
    if (_shutterLock == LCK_LOCKED) {
        return;
    }

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


    //    if (_config.setting == OPTION_SETTINGS_SHUTTER) {
    //        currPos = 1.0f - currPos; // invert the position as shutter has different 100%-meaning than window
    //        //Debug.println(F("pos inverted due to shutter to: %f"), currPos);
    //    }

    uint8_t positionValueToSend = currPos * 255; // map 0..100% to 0..255 unsigned byte
    if (_initDone && // only send when init is done AND
            _lastSentPosition != positionValueToSend && // we need to have different value as last time
            (sendStatus && millis() > (_lastStatusUpdate + STATUS_UPDATE_INTERVAL)) || // if it's time for an update and we really have an update OR
            (_lastMoveStatus != MS_STOP && _moveStatus == MS_STOP) // if we just stopped
            ) {
        //        if (_config.setting == OPTION_SETTINGS_SHUTTER) {
        //            Debug.println(F("pos inverted due to shutter to: %f"), currPos);
        //        }
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

/**
 * 
 * @param targetPosition
 */
void RotoChannel::doPosition(float targetPosition) {

    Debug.println(F("[%i] doPosition(%f) currPos=%f setting=%i"), _group, targetPosition, _position, _config.setting);

    _targetPosition = targetPosition;
    // window: 0% = closed, 100% = opened
    // shutter: 0% = opened, 100% = closed
    if (_targetPosition > _position) {

        if (isWindow()) {
            doOpen();
        } else { // shutter
            doClose();
        }
    } else {
        if (isWindow()) {
            doClose();
        } else { // shutter
            doOpen();
        }
    }

}

void RotoChannel::doOpen() {

    /*
     * TODO The delay in here will definitley block the loop(),
     * bus this happens not that often, so we won't miss a telegram
     */

    Debug.println(F("[%i] doOpen() locked=%i"), _group, _shutterLock);
    if (isMoving() && !_isStopping) {
        Debug.println(F("[%i] Stop moving first ..."), _group);
        doStop();
        Debug.println(F("[%i] Stop moving first ...*done*"), _group);
    }
    // it's okay to trigger relais in unlocked, locking and unlocking
    if (_shutterLock != LCK_LOCKED) {
        _mcp.digitalWrite(_setPinOpen, HIGH);
        delay(RELAY_SET_TIME);
        _mcp.digitalWrite(_setPinOpen, LOW);

        delay(ROTO_TRIGGER_DURATION);

        _mcp.digitalWrite(_resetPinOpen, HIGH);
        delay(RELAY_SET_TIME);
        _mcp.digitalWrite(_resetPinOpen, LOW);

        // we're no longer "closed", so turn off that status LED
        _frontend.setLED(getLed(LED_CLOSE), false);
    }

    _moveStatus = MS_OPENING;
    //    _lastAction = A_OPEN;

    if (_startMoveMillis == NOT_DEFINED) {
        _startMoveMillis = millis();
    }

}

void RotoChannel::doClose() {

    Debug.println(F("[%i] doClose() locked=%i"), _group, _shutterLock);
    if (isMoving() && !_isStopping) {
        Debug.println(F("[%i] Stop moving first ..."), _group);
        doStop();
        Debug.println(F("[%i] Stop moving first ...*done*"), _group);
    }
    // it's okay to trigger relais in unlocked, locking and unlocking
    if (_shutterLock != LCK_LOCKED) {
        _mcp.digitalWrite(_setPinClose, HIGH);
        delay(RELAY_SET_TIME);
        _mcp.digitalWrite(_setPinClose, LOW);

        delay(ROTO_TRIGGER_DURATION);

        _mcp.digitalWrite(_resetPinClose, HIGH);
        delay(RELAY_SET_TIME);
        _mcp.digitalWrite(_resetPinClose, LOW);

        // we're no longer "opened", so turn off that status LED
        _frontend.setLED(getLed(LED_OPEN), false);
    }

    _moveStatus = MS_CLOSING;
    //    _lastAction = A_CLOSE;

    if (_startMoveMillis == NOT_DEFINED) {
        _startMoveMillis = millis();
    }



}

void RotoChannel::doStop() {

    _isStopping = true;
    Debug.print(F("[%i] doStop() moveStatus=%i ->"), _group, _moveStatus);

    switch (_moveStatus) {
        case MS_CLOSING:
            Debug.println(F("CLOSING to STOP"));
            doClose(); // call close again to stop motor
            _moveStatus = MS_STOP;
            break;
        case MS_OPENING:
            Debug.println(F("OPENING to STOP"));
            doOpen(); // call close again to stop motor
            break;
        default:
            Debug.println(F("STOP"));
            break;
    }
    _moveStatus = MS_STOP;
    //_startMoveMillis = NOT_DEFINED;
    _isStopping = false;

}

/**
 *
 */
void RotoChannel::workLEDs() {

    // while not yet done with init, led all LEDs blink to signal init
    if (!_initDone) {
        if (millis() - _lastBlinkMillis > BLINK_INIT_DELAY) {
            _frontend.setLED(getLed(LED_CLOSE), !_lastBlinkState);
            _frontend.setLED(getLed(LED_OPEN), _lastBlinkState);
            _lastBlinkMillis = millis();
            _lastBlinkState = !_lastBlinkState;
        }
        return;
    }

    // Status Blinken
    if (_moveStatus != MS_STOP) {

        if (millis() - _lastBlinkMillis > BLINK_DELAY) {

            //      Debug.println(F("BLINK "));

            _lastBlinkState = !_lastBlinkState;

            // set blink status on correct LED
            switch (_moveStatus) {
                case MS_CLOSING:
                    _frontend.setLED(getLed(LED_CLOSE), _lastBlinkState);
                    break;
                case MS_OPENING:
                    _frontend.setLED(getLed(LED_OPEN), _lastBlinkState);
                    break;
                default:
                    break;
            }
            _lastBlinkMillis = millis();
        }

    }

    // ensure LEDs are off after moving
    if (_lastMoveStatus != MS_STOP && _moveStatus == MS_STOP) {
        Debug.print(F("workLEDs: STOP reached. Set LEDs to "));

        switch (_status) {
            case CS_OPENED:
                _frontend.setLED(getLed(LED_OPEN), true); // auf
                _frontend.setLED(getLed(LED_CLOSE), false); // zu
                Debug.println(F("OPENED"));
                break;
            case CS_CLOSED:
                _frontend.setLED(getLed(LED_OPEN), false); // auf
                _frontend.setLED(getLed(LED_CLOSE), true); // zu
                Debug.println(F("CLOSED"));

                break;
            case CS_UNDEFINED:
            default:
                _frontend.setLED(getLed(LED_OPEN), false); // weder
                _frontend.setLED(getLed(LED_CLOSE), false); // noch
                Debug.println(F("UNDEFINED"));
        }

        _lastBlinkState = false;
    }

}

void RotoChannel::workPosition() {

#define DEBUG_UPDATE_STATUS    

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
     * - öffnen, target position reached/exceeded
     * - schließen timeout
     * - schließen, target position reached/exceeded
     *
     * - weder noch
     */



    /* ********************************************
     * Fall: Öffne ODER öffnen gestoppt
     * ********************************************/

    if ((_lastMoveStatus == MS_OPENING && _moveStatus == MS_OPENING) || (_lastMoveStatus == MS_OPENING && _moveStatus == MS_STOP)) {

        unsigned long duration = millis() - _startMoveMillis;
        float delta = (float) duration * _openStep;

        if (isWindow()) {
            _newPosition = _position + delta;
        } else {
            _newPosition = _position - delta;
        }

#ifdef DEBUG_UPDATE_STATUS
        Debug.println(F("[%i] O: duration: %i"), _group, duration);
        Debug.println(F("[%i] O: position: %3.9f"), _group, _position);
        Debug.println(F("[%i] O: delta: %3.9f"), _group, delta);
        Debug.println(F("[%i] O: new position: %3.9f"), _group, _newPosition);
#endif        

        // keep pos in range
        _newPosition = limitPos(_newPosition);

        unsigned long allowedTime = 0.0;
        if (isWindow()) {
            allowedTime = (1.0 - _position) * getTime(_config.runTimeOpen);
        } else {
            allowedTime = (_position) * (getTime(_config.runTimeClose));
        }

#ifdef DEBUG_UPDATE_STATUS        
        Debug.println(F("[%i] O: allowed time open: %i"), _group, allowedTime);
#endif

        // if open move time exceeds "open time", force position to "completely open"
        if (duration > allowedTime) {
            if (isWindow()) {
                _newPosition = 1.0;
            } else {
                _newPosition = 0.0;
            }
#ifdef DEBUG_UPDATE_STATUS
            Debug.println(F("[%i] O: Time limit for OPEN reached"), _group);
#endif            
        }

        // opened condition
        bool openedCondition = false;
        if (isWindow()) {
            openedCondition = _newPosition == 1.0 || (_targetPosition != NOT_DEFINED && _newPosition >= _targetPosition);
        } else {
            openedCondition = _newPosition == 0.0 || (_targetPosition != NOT_DEFINED && _newPosition <= _targetPosition);
        }
        if (openedCondition) {
            _status = CS_OPENED;
            _moveStatus = MS_STOP;
            _targetPosition = NOT_DEFINED;
#ifdef DEBUG_UPDATE_STATUS            
            Debug.println(F("[%i] Is now OPENED or at abs. target pos"), _group);
#endif            
        } else {
            _status = CS_OPEN;
        }

    } else


        /* ********************************************
         * Fall: Schließe ODER schließen gestoppt
         * ********************************************/

        if ((_lastMoveStatus == MS_CLOSING && _moveStatus == MS_CLOSING) || (_lastMoveStatus == MS_CLOSING && _moveStatus == MS_STOP)) {


        unsigned long duration = millis() - _startMoveMillis;
        float delta = (float) duration * _closeStep;


        if (isWindow()) {
            _newPosition = _position - delta;
        } else {
            _newPosition = _position + delta;
        }
#ifdef DEBUG_UPDATE_STATUS        
        Debug.println(F("[%i] C: duration: %i"), _group, duration);
        Debug.println(F("[%i] C: position: %3.9f"), _group, _position);
        Debug.println(F("[%i] C: delta: %3.9f"), _group, delta);
        Debug.println(F("[%i] C: new position: %3.9f"), _group, _newPosition);
#endif

        // keep pos in range
        _newPosition = limitPos(_newPosition);

        unsigned long allowedTime = 0.0;
        if (isWindow()) {
            allowedTime = (_position) * (getTime(_config.runTimeClose));
        } else {
            allowedTime = (1.0 - _position) * getTime(_config.runTimeOpen);
        }
#ifdef DEBUG_UPDATE_STATUS        
        Debug.println(F("[%i] C: allowed time close: %i"), _group, allowedTime);
#endif        

        // if open move time exceeds "open time", force position to "completely close"
        if (duration > allowedTime) {
            if (isWindow()) {
                _newPosition = 0.0;
            } else {
                _newPosition = 1.0;
            }
#ifdef DEBUG_UPDATE_STATUS            
            Debug.println(F("[%i] C: Time limit for CLOSE reached"), _group);
#endif            
        }

        // closed condition
        bool closedCondition = false;
        if (isWindow()) {
            closedCondition = _newPosition == 0.0 || (_targetPosition != NOT_DEFINED && _newPosition <= _targetPosition);
        } else {
            closedCondition = _newPosition == 1.0 || (_targetPosition != NOT_DEFINED && _newPosition >= _targetPosition);
        }
        if (closedCondition) {
            _status = CS_CLOSED;
            _moveStatus = MS_STOP;
            _targetPosition = NOT_DEFINED;
#ifdef DEBUG_UPDATE_STATUS            
            Debug.println(F("[%i] Is now CLOSED or at abs. target pos"), _group);
#endif            
        } else {
            _status = CS_OPEN;
        }

    }


    // if we just stopped NOW ...
    if (_lastMoveStatus != MS_STOP && _moveStatus == MS_STOP) {

        // apply position
        _position = _newPosition;
        _startMoveMillis = NOT_DEFINED;

        // if required, set end of locking/unlocking as channel movement has stopped 
        switch (_shutterLock) {
            case LCK_LOCKING:
                _shutterLock = LCK_LOCKED;
                break;
            case LCK_UNLOCKING:
                _shutterLock = LCK_UNLOCKED;
        }

        Debug.println(F("[%i] Finally on position %3.9f [locked=%i]"), _group, _position, isLocked());

    }


}

/**
 * Returns true, if shutter is locked or doiong lock-action,
 * false if unlocked, or doing unlock-action
 * @return bool
 */
bool RotoChannel::isLocked() {
    switch (_shutterLock) {
        case LCK_LOCKED:
        case LCK_LOCKING:
            return true;
        case LCK_UNLOCKED:
        case LCK_UNLOCKING:
        default:
            return false;
    }
}

/**
 * returns true, if channel is in move state
 * @return bool
 */
bool RotoChannel::isMoving() {
    switch (_moveStatus) {
        case MS_CLOSING:
        case MS_OPENING:
            return true;
        case MS_STOP:
        default:
            return false;
    }
}

float RotoChannel::limitPos(float pos) {
    if (pos > 1.0f) {
        pos = 1.0f;
    } else if (pos < 0.0f) {
        pos = 0.0f;
    }
    return pos;
}

/**
 * Check for window setup
 * @return true, if channel/group is window, false if it's shutter
 */
bool RotoChannel::isWindow() {
    return _config.setting == OPTION_SETTINGS_WINDOW;
}

/**
 * Returns frontend LED index
 * @param led enum for open and close LED
 * @return frontend LED index
 */
int RotoChannel::getLed(FrontendLed led) {
    int ledIndex = _group * 2;
    switch (led) {
        case LED_OPEN:
            ledIndex += 0;
            break;
        case LED_CLOSE:
            ledIndex += 1;
            break;
        default:
            break;
    }
    return ledIndex;
}

/**
 * Gets given time in milliseconds, incl. rollover time
 * @param time time in sec
 * @return time in ms incl. rollovertime
 */
uint32_t RotoChannel::getTime(uint8_t time) {
    return time * SECOND * (1.0f + (_config.runTimeRollover / 100.0f));
}

bool RotoChannel::knxEvents(byte index) {

    if (!_enabled) {
        return false;
    }

    // common comobjects
    switch (index) {

        case (COMOBJ_centralShutterLock):
        {
            // lock can be applied to shutter only! No lock for window!
            if (_config.setting == OPTION_SETTINGS_SHUTTER) {

                /*
                 * - store lock state
                 * - block external action on lock! (in doOpen/doClose)
                 */
                byte actionValue = Knx.read(index);
                if (actionValue == DPT1_003_disable) { // true if "locked", false if "unlocked"
                    _shutterLock = LCK_LOCKED;
                } else {
                    _shutterLock = LCK_UNLOCKED;
                };

                Debug.println(F("[%i] shutter locked: %i"), _group, isLocked());
                if (isLocked()) {
                    //locked
                    _previousPosition = _position;
                    switch (_config.lockAction) {
                        case OPTION_LOCK_ACTION_NONE:
                            // nothing to do for us
                            Debug.println(F("[%i] locked: none"), _group);
                            break;
                        case OPTION_LOCK_ACTION_OPEN:
                            Debug.println(F("[%i] locked: open"), _group);
                            _shutterLock = LCK_LOCKING;
                            doOpen();
                            break;
                        case OPTION_LOCK_ACTION_CLOSE:
                            Debug.println(F("[%i] locked: close"), _group);
                            _shutterLock = LCK_LOCKING;
                            doClose();
                            break;
                    }
                } else {
                    //unlocked
                    _shutterLock = LCK_UNLOCKING;
                    switch (_config.unlockAction) {
                        case OPTION_UNLOCK_ACTION_NONE:
                            // set the position we reached during lock
                            Debug.println(F("[%i] unlocked: none"), _group);
                            doPosition(_position);
                            break;
                        case OPTION_UNLOCK_ACTION_OPEN:
                            Debug.println(F("[%i] unlocked: open"), _group);
                            doOpen();
                            break;
                        case OPTION_UNLOCK_ACTION_CLOSE:
                            Debug.println(F("[%i] unlocked: close"), _group);
                            doClose();
                            break;
                        case OPTION_UNLOCK_ACTION_PREVIOUS_POS:
                            Debug.println(F("[%i] unlocked: restore prev position: %i"), _group, _previousPosition);
                            doPosition(_previousPosition);
                            break;
                    }

                }
            }
            return false;
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
            return true;
        }

        case (COMOBJ_abStop - COMOBJ_OFFSET):
        {
            byte value = Knx.read(index);
            Debug.println(F("[%i] doStop: %i"), _group, value);
            if (value == DPT1_010_stop) {
                doStop();
            }
            return true;
        }

        case (COMOBJ_abAbsPosition - COMOBJ_OFFSET):
        {
            byte value = Knx.read(index);
            float absPos = (value * (BYTE_PERCENT)) / 100.0f;
            Debug.println(F("[%i] absPos: %f"), _group, absPos);
            doPosition(absPos);
            return true;
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
                // --> Use separate/new enum to follow reference drive steps
            }
            return true;
        }

        case (COMOBJ_abFixPosition - COMOBJ_OFFSET):
        {
            byte value = Knx.read(index);
            if (value == DPT1_001_on) {
                Debug.println(F("[%i] fix position: %i"), _group, value);

                /*
                 * TODO
                 * - get predefined position from settings
                 * - call doPosition()
                 */
                doPosition(0.5f); // for now, we use 50%
            }
            return true;
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
            return true;
        }

        case (COMOBJ_abWindAlarm - COMOBJ_OFFSET):
        {
            byte value = Knx.read(index);
            Debug.println(F("[%i] windalarm: %i"), _group, value);
            /*
             * TODO
             * Introduce param that controls what to do? Like with lock?
             */
            return true;
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
            return true;
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

}