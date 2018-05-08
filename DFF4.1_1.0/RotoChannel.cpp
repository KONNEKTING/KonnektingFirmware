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

    _referenceRun = REF_NONE;

    _config = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    _lock = LCK_UNLOCKED;
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

    Debug.print(F("[%i] Init"), _group);
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

    Debug.println(F("[%i] setConfig:\n"
                            "\t_config.runtimeRollover=%is \n"
                            "\t_config.runTimeOpen=%ims \n"
                            "\t_config.runTimeClose=%ims"), _group, _config.runTimeRollover, getTime(_config.runTimeOpen), getTime(_config.runTimeClose));

    _openStep = (100.0 / (getTime(_config.runTimeOpen))) / 100.0;
    _closeStep = (100.0 / (getTime(_config.runTimeClose))) / 100.0;

    Debug.println(F("\t_openStep=%3.9f%%/ms"), _group, _openStep);
    Debug.println(F("\t_closeStep=%3.9f%%/ms"), _group, _closeStep);

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

            Debug.println(F("[%i] Init *done*. Pos=%3.9f"), _group, _position);

        }

    } else if (_manualMoveRequest) {

        Debug.print(F("[%i] manualMoveRequest: "), _group);

        Debug.print(F("current move status: %i"), _moveStatus);

        Debug.println(F("; current status: %i"), _status);

        if (_moveStatus != MS_STOP) {
            doStop(); // wenn wir noch fahren, anhalten
        } else if (_manualMoveRequestOpenButton) { // ansonsten je nach button öffnen oder schließen
            doOpen();
        } else {
            doClose();
        }

        _manualMoveRequest = false;

    } else {

        switch (_referenceRun) {
            case REF_START:
                Debug.println(F("[%i] Reference run START"), _group);
                _previousPosition = _position;
                doClose();
                // next step:
                _referenceRun = REF_CLOSING;
                break;
            case REF_CLOSING:
                if (_position == 1.0f) {
                    Debug.println(F("[%i] Reference CLOSED pos reached, restoring ..."), _group);
                    doPosition(_previousPosition);
                    // next step:
                    _referenceRun = REF_RESTORING;
                }
                break;
            case REF_RESTORING:
                if (isJustStopped()) {
                    Debug.println(F("[%i] Reference PREV pos reached, DONE ..."), _group);
                    // next step:
                    _referenceRun = REF_DONE;
                }
                break;
            case REF_DONE:
                Debug.println(F("[%i] Reference DONE"), _group);
                _lock = LCK_UNLOCKED;
                _referenceRun = REF_NONE;
                break;
            default:
                // irrelevant
                break;
        }

        switch (_ventilate) {
            case VENT_START:
                Debug.println(F("[%i] Ventilate START"), _group);
                _previousPosition = _position;
                doOpen();
                // next step:
                _ventilate = VENT_VENTILATE;
                _ventilateStart = millis();
                break;
            case VENT_VENTILATE:
                if (millis() - _ventilateStart > _config.ventilationTime) {
                    _ventilate = VENT_RESTORING;
                } else {

                    unsigned long remain = _config.ventilationTime - (millis() - _ventilateStart);
                    if (remain % 5000 == 0) {
                        Debug.println(F("[%i] Ventilate remaining time: %lu"), _group, remain);
                    }
                }
                break;
            case VENT_RESTORING:
                Debug.println(F("[%i] Ventilate RESTORE"), _group);
                doPosition(_previousPosition);
                _ventilate = VENT_DONE;
                break;
            case VENT_DONE:
                Debug.println(F("[%i] Ventilate DONE"), _group);
                _ventilate = VENT_NONE;
                break;
            default:
                break;
        }

    }


    if (_initDone) {
        workPosition();
        workStatus();
    }

    if (isJustStopped()) {
        // if required, set end of locking/unlocking as channel movement has stopped 
        bool updated = false;
        switch (_lock) {
            case LCK_LOCKING:
                _lock = LCK_LOCKED;
                updated = true;
                break;
            case LCK_UNLOCKING:
                _lock = LCK_UNLOCKED;
                updated = true;
                break;
        }
        if (updated) {
            Debug.println(F("[%i] Updated lock status [locked=%i]"), _group, isLocked());
        }
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
    if (_lock == LCK_LOCKED) {
        return;
    }


    if (isJustStopped()) {
        Debug.print("[%i] STATUS just stopped: [", _group);
        /*
         * 0-en werden gesendet wenn abgehalten wird, passiert in workStatus()
         */
        switch (_config.runStatusComObj) {
            case 0x01:
                // 1KO: Verfahrstatus
                Knx.write(getComObjIndex(COMOBJ_abStatusMovement), 0);
                Debug.print(" COMOBJ_abStatusMovement=0", _group);
                break;
            case 0x02:
                // 2KO: Auffahrt + Zufahrt
                Knx.write(getComObjIndex(COMOBJ_abStatusMovementOpen), 0);
                Knx.write(getComObjIndex(COMOBJ_abStatusMovementClose), 0);
                Debug.print(" COMOBJ_abStatusMovement[Open|Close]=0", _group);
                break;
            case 0x00:
            default:
                // do nothing
                break;
        }

        /*
         * Status: Status für akt. Richtung & Position auf/zu 
         * --> 3 KOs: 
         * Status akt. Richtung -> abStatusMovementDirection
         * Position auf -> abStatusOpenPos
         * Position zu -> abStatusClosePos
         */
        if (_config.runStatusPositionComObj != 0x00) { // if not OFF
            if (_position == 0.0f) { // = window closed | shutter opened
                if (isWindow()) {
                    Knx.write(getComObjIndex(COMOBJ_abStatusClosePos), DPT1_001_on);
                    Debug.print(" COMOBJ_abStatusMovementClosePos=1", _group);
                } else { //isShutter
                    Knx.write(getComObjIndex(COMOBJ_abStatusOpenPos), DPT1_001_on);
                    Debug.print(" COMOBJ_abStatusMovementOpenPos=1", _group);
                }
            } else if (_position == 1.0f) { // = window opened| shutter closed
                if (isWindow()) {
                    Knx.write(getComObjIndex(COMOBJ_abStatusOpenPos), DPT1_001_on);
                    Debug.print(" COMOBJ_abStatusMovementOpenPos=1", _group);
                } else { //isShutter
                    Knx.write(getComObjIndex(COMOBJ_abStatusClosePos), DPT1_001_on);
                    Debug.print(" COMOBJ_abStatusMovementClosePos=1", _group);
                }
            }
        }
        Debug.println("]", _group);
    }


    // if we are in an solid stop, don't send status updates
    if (_lastMoveStatus == MS_STOP && _moveStatus == MS_STOP) {
        return;
    }

    // current position is located in idfferent variables, depending on move-status
    float currPos = isMoving() ? _newPosition : _position;

    uint8_t positionValueToSend = currPos * 255; // map 0..100% to 0..255 unsigned byte

    //    Debug.println("[%i] STATUS curr pos: %f -> 0x%02x; lastSentPos=0x%02x; millis=%6ld newMillis=%6ld; lastMoveStatus=%i movestatus=%i",
    //            _group, currPos, positionValueToSend, _lastSentPosition, millis(), (_lastStatusUpdate + STATUS_UPDATE_INTERVAL), _lastMoveStatus, _moveStatus);

    if (_initDone && // only send when init is done AND
            _config.absPosStatusComObj == 0x01 && // currentpos status comobj is active AND
            _lastSentPosition != positionValueToSend && // we need to have different value as last time AND
            (
            (millis() > (_lastStatusUpdate + STATUS_UPDATE_INTERVAL)) || // if it's time for an update OR
            (_lastMoveStatus != MS_STOP && _moveStatus == MS_STOP) // if we just reached STOP status
            )) {

        if (_config.absPosStatusComObj == 0x01) {
            Knx.write((byte) (_baseIndex + (COMOBJ_abStatusCurrentPos - COMOBJ_OFFSET)), positionValueToSend);
            Debug.println("[%i] STATUS SEND curr pos: %f -> 0x%02x", _group, currPos, positionValueToSend);
            // store last sent status. Ensures with if-clause that we don't send the same value twice (especially on stop)
            _lastSentPosition = positionValueToSend;
            _lastStatusUpdate = millis();
        }

    }
}

void RotoChannel::doButton(bool openButton) {

    Debug.println(F("[%i] doButton: openButton=%i"), _group, openButton);

    if (isLocked()) {
        Debug.println(F("[%i] doButton skipped due to lock: %i"), _group, _lock);
    }
    _manualMoveRequest = true; // signal move request
    _manualMoveRequestOpenButton = openButton;
}

/**
 * 
 * @param targetPosition
 */
void RotoChannel::doPosition(float targetPosition) {

    Debug.println(F("[%i] doPosition(%f): currPos=%f setting=%i"), _group, targetPosition, _position, _config.setting);

    
    if (!isLocked()) {
        
//        if (isMoving()) {
//            Debug.println(F("[%i] doPosition(%f): already moving. stop first."), _group, targetPosition);    
//            doStop();
//            Debug.println(F("[%i] doPosition(%f): stopped, continue with doPosition()"), _group, targetPosition);    
//        }
        
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
    } else {
        Debug.println(F("[%i] doPosition skipped due to lock."), _group);
    }

}

void RotoChannel::doOpen() {

    /*
     * TODO The delay in here will definitley block the loop(),
     * bus this happens not that often, so we won't miss a telegram
     */

    Debug.println(F("[%i] doOpen() locked=%i"), _group, _lock);
    if (isMoving() && !_isStopping) {
        Debug.println(F("[%i] Stop moving first ..."), _group);
        doStop();
        Debug.println(F("[%i] Stop moving first ...*done*"), _group);
    }
    // it's okay to trigger relais in unlocked, locking and unlocking state
    if (!isLocked()) {
        Debug.println(F("[%i] trigger relay to open"), _group);
        _mcp.digitalWrite(_setPinOpen, HIGH);
        delay(RELAY_SET_TIME);
        _mcp.digitalWrite(_setPinOpen, LOW);

        delay((unsigned long) _config.triggerTime);

        _mcp.digitalWrite(_resetPinOpen, HIGH);
        delay(RELAY_SET_TIME);
        _mcp.digitalWrite(_resetPinOpen, LOW);
        
        delay((unsigned long) _config.triggerTime); // ensure delay till next relais command

        // we're no longer "closed", so turn off that status LED
        _frontend.setLED(getLed(LED_CLOSE), false);

    } else {
        Debug.println(F("[%i] doOpen() locked=%s"), _group, isLocked() ? "true": "false");
    }
    _moveStatus = MS_OPENING;
    sendMovementStatus();

    if (_startMoveMillis == NOT_DEFINED) {
        _startMoveMillis = millis();
    }
}

void RotoChannel::doClose() {

    Debug.println(F("[%i] doClose() locked=%i"), _group, _lock);
    // it's okay to trigger relais in unlocked, locking and unlocking state
    if (isMoving() && !_isStopping) {
        Debug.println(F("[%i] Stop moving first ..."), _group);
        doStop();
        Debug.println(F("[%i] Stop moving first ...*done*"), _group);
    }
    if (_lock != LCK_LOCKED) {
        Debug.println(F("[%i] trigger relay to close"), _group);
        _mcp.digitalWrite(_setPinClose, HIGH);
        delay(RELAY_SET_TIME);
        _mcp.digitalWrite(_setPinClose, LOW);

        delay((unsigned long) _config.triggerTime);

        _mcp.digitalWrite(_resetPinClose, HIGH);
        delay(RELAY_SET_TIME);
        _mcp.digitalWrite(_resetPinClose, LOW);
        
        delay((unsigned long) _config.triggerTime); // ensure delay till next relais command

        // we're no longer "opened", so turn off that status LED
        _frontend.setLED(getLed(LED_OPEN), false);


    } else {
        Debug.println(F("[%i] doClose() locked=%s"), _group, isLocked() ? "true": "false");
    }
    _moveStatus = MS_CLOSING;
    sendMovementStatus();

    if (_startMoveMillis == NOT_DEFINED) {
        _startMoveMillis = millis();
    }
}

void RotoChannel::doStop() {

    if (isLocked()) {
        Debug.println(F("[%i] doStop skipped due to lock."), _group);
        return;
    }

    _isStopping = true;
    Debug.print(F("[%i] doStop() moveStatus=%i ->"), _group, _moveStatus);

    switch (_moveStatus) {
        case MS_CLOSING:
            Debug.println(F("CLOSING to STOP"));
            doClose(); // call close again to stop motor            
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
        Debug.print(F("[%i] workLEDs: STOP reached. Set LEDs to "), _group);

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
            case CS_OPEN:
            case CS_UNDEFINED:
            default:
                _frontend.setLED(getLed(LED_OPEN), false); // weder
                _frontend.setLED(getLed(LED_CLOSE), false); // noch
                Debug.println(F("OPEN|UNDEFINED"));
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
     * - öffnen, target position reached/exceeded
     * - schließen timeout
     * - schließen, target position reached/exceeded
     *
     * - weder noch
     */

    bool needStop = false;

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
        Debug.print(F("[%i] O: duration: %i"), _group, duration);
        Debug.print(F("\tposition: %3.9f"), _position);
        Debug.print(F("\tdelta: %3.9f"), delta);
        Debug.println(F("\tnew position: %3.9f"), _newPosition);
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

            if (_newPosition == 1.0 || _newPosition == 0.0) { // reached endpoint?
                _status = CS_OPENED; // opened
            } else {
                _status = CS_OPEN; // somewhere between opened and closed
            }
            //_moveStatus = MS_STOP;
            needStop = true;
            _targetPosition = NOT_DEFINED;
//#ifdef DEBUG_UPDATE_STATUS            
            Debug.println(F("[%i] Is now OPENED or at abs. target pos"), _group);
//#endif            
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
        Debug.print(F("[%i] C: duration: %i"), _group, duration);
        Debug.print(F("\tposition: %3.9f"), _position);
        Debug.print(F("\tdelta: %3.9f"), delta);
        Debug.println(F("\tnew position: %3.9f"), _newPosition);
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

            // reached endpoint?

            if (_newPosition == 1.0 || _newPosition == 0.0) {
                _status = CS_CLOSED; // closed
            } else {
                _status = CS_OPEN; // somewhere between opened and closed
            }
            //_moveStatus = MS_STOP;
            needStop = true;
            _targetPosition = NOT_DEFINED;
//#ifdef DEBUG_UPDATE_STATUS            
            Debug.println(F("[%i] Is now CLOSED or at abs. target pos"), _group);
//#endif            
        } else {
            _status = CS_OPEN;
        }

    }


    // if we just stopped NOW ...
    //if (_lastMoveStatus != MS_STOP && _moveStatus == MS_STOP) {    
    if (_lastMoveStatus != MS_STOP && needStop) {
        
        doStop();

        // apply position
        _position = _newPosition;
        _startMoveMillis = NOT_DEFINED;
        
        Debug.println(F("[%i] Finally on position %3.9f [locked=%s]"), _group, _position, isLocked()?"true":"false");

    }


}

/**
 * Returns true, if shutter is locked or doing lock-action,
 * false if unlocked, or doing unlock-action
 * @return bool
 */
bool RotoChannel::isLocked() {
    switch (_lock) {
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
 * calculates the correct comobj index for the given "COMOBJ_...." variable
 */
byte RotoChannel::getComObjIndex(byte COMOBJ_var) {
    return (byte) (_baseIndex + (COMOBJ_var - COMOBJ_OFFSET));
}

/**
 * used by doOpen(), doClose() and doStop() to send one-time-status of movement 
 */
void RotoChannel::sendMovementStatus() {
    if (_config.runStatusPositionComObj != 0x00) {
        switch (_moveStatus) {
            case MS_OPENING:
                Knx.write(getComObjIndex(COMOBJ_abStatusMovementDirection), isWindow() ? DPT1_008_down : DPT1_008_up);
                break;
            case MS_CLOSING:
                Knx.write(getComObjIndex(COMOBJ_abStatusMovementDirection), isWindow() ? DPT1_008_up : DPT1_008_down);
                break;
            default:
                break;
        }
    }

    if (isMoving()) {

        /*
         * 0-en werden gesendet wenn abgehalten wird, passiert in workStatus()
         */
        switch (_config.runStatusComObj) {
            case 0x01:
                // 1KO: Verfahrstatus
                Knx.write(getComObjIndex(COMOBJ_abStatusMovement), 1);
                break;
            case 0x02:
                // 2KO: Auffahrt + Zufahrt
                switch (_moveStatus) {
                    case MS_OPENING:
                        Knx.write(getComObjIndex(COMOBJ_abStatusMovementOpen), 1);
                        break;
                    case MS_CLOSING:
                        Knx.write(getComObjIndex(COMOBJ_abStatusMovementClose), 1);
                        break;
                    case MS_STOP:
                    default:
                        // do nothing
                        break;
                }
                break;
            case 0x00:
            default:
                // do nothing
                break;
        }
    }
}

/**
 * Returns true if just reached stop
 * @return 
 */
bool RotoChannel::isJustStopped() {
    return _lastMoveStatus != MS_STOP && _moveStatus == MS_STOP;
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
                 * - block external action on lock! (in doOpen/doClose/doPosition/doStop)
                 */
                byte actionValue = Knx.read(index);
                if (actionValue == DPT1_003_disable) { // true if "locked", false if "unlocked"
                    _lock = LCK_LOCKED;
                } else {
                    _lock = LCK_UNLOCKED;
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
                            _lock = LCK_LOCKING;
                            doOpen();
                            break;
                        case OPTION_LOCK_ACTION_CLOSE:
                            Debug.println(F("[%i] locked: close"), _group);
                            _lock = LCK_LOCKING;
                            doClose();
                            break;
                    }
                } else {
                    //unlocked
                    _lock = LCK_UNLOCKING;
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
        
        // for remaining central objects, convert index to channelIndex --> just one implementation for central+channel!
        case (COMOBJ_centralWindowOpenClose):
            if (isWindow()) {
                index = _baseIndex + COMOBJ_abOpenClose - COMOBJ_OFFSET;
            }
            break;

        case (COMOBJ_centralWindowStop):
            if (isWindow()) {
                index = _baseIndex + COMOBJ_abStop - COMOBJ_OFFSET;
            }
            break;
        case (COMOBJ_centralWindowAbsPosition):
            if (isWindow()) {
                index = _baseIndex + COMOBJ_abAbsPosition - COMOBJ_OFFSET;
            }
            break;
        case (COMOBJ_centralShutterOpenClose):
            if (!isWindow()) {
                index = _baseIndex + COMOBJ_abOpenClose - COMOBJ_OFFSET;
            }
            break;
        case (COMOBJ_centralShutterStop):
            if (!isWindow()) {
                index = _baseIndex + COMOBJ_abStop - COMOBJ_OFFSET;
            }
            break;
        case (COMOBJ_centralShutterAbsPosition):
            if (!isWindow()) {
                index = _baseIndex + COMOBJ_abAbsPosition - COMOBJ_OFFSET;
            }
            break;
            
        case (COMOBJ_centralVentilation):
            index = _baseIndex + COMOBJ_abVentilation - COMOBJ_OFFSET;
            break;
            

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
            if (isLocked()) {
                Debug.println(F("[%i] skipping comobj %i due to lock"), _group, chIndex);
                return true;
            }
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
            if (isLocked()) {
                Debug.println(F("[%i] skipping comobj %i due to lock"), _group, chIndex);
                return true;
            }
            byte value = Knx.read(index);
            Debug.println(F("[%i] doStop: %i"), _group, value);
            if (value == DPT1_010_stop) {
                doStop();
            }
            return true;
        }

        case (COMOBJ_abAbsPosition - COMOBJ_OFFSET):
        {
            if (isLocked()) {
                Debug.println(F("[%i] skipping comobj %i due to lock"), _group, chIndex);
                return true;
            }
            byte value = Knx.read(index);
            float absPos = (value * (BYTE_PERCENT)) / 100.0f;
            Debug.println(F("[%i] absPos: %f"), _group, absPos);
            doPosition(absPos);
            return true;
        }

        case (COMOBJ_abReference - COMOBJ_OFFSET):
        {
            if (isLocked()) {
                Debug.println(F("[%i] skipping comobj %i due to lock"), _group, chIndex);
                return true;
            }
            byte value = Knx.read(index);
            if (value == DPT1_001_on) {
                Debug.println(F("[%i] reference: %i"), _group, value);
                _lock = LCK_REFERENCE_RUN;
                _referenceRun = REF_START;

            }
            return true;
        }

        case (COMOBJ_abDriveToPosition - COMOBJ_OFFSET):
        {
            if (isLocked()) {
                Debug.println(F("[%i] skipping comobj %i due to lock"), _group, chIndex);
                return true;
            }
            if (_config.driveToPositionComObj == 0x00) {
                Debug.println(F("[%i] drive to position comobj deactivated. Ignoring!"), _group);
            }
            byte value = Knx.read(index);
            if (value == DPT1_001_on) {
                float positionValue = _config.driveToPositionValue / 100.0f;
                Debug.println(F("[%i] drive to position: %f"), _group, positionValue);
                doPosition(positionValue);
            } else {
                Debug.println(F("[%i] drive to position: value=0 is not yet implemented. Ignoring!"), _group);
            }
            return true;
        }

        case (COMOBJ_abVentilation - COMOBJ_OFFSET):
        {
            if (isLocked()) {
                Debug.println(F("[%i] skipping comobj %i due to lock"), _group, chIndex);
                return true;
            }
            if (isWindow()) {
                byte value = Knx.read(index);
                if (value == DPT1_001_on) {
                    Debug.println(F("[%i] ventilation: %i"), _group, value);
                    _ventilate = VENT_START;
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
            } else {
                Debug.println(F("[%i] ignoring COMOBJ_abVentilation due to shutter!"), _group);
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