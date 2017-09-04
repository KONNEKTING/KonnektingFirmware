/*
 * RotoChannel.h
 *
 *  Created on: 27.03.2017
 *      Author: achristian
 */

#ifndef ROTOCHANNEL_H_
#define ROTOCHANNEL_H_

#include "Arduino.h"
#include "Frontend8Btn8Led.h"
#include <Adafruit_MCP23017.h>


/**
 * Time in ms the relay needs to be triggered for set
 */
#define RELAY_SET_TIME 50L

/**
 * Time in ms roto requires to trigger open/close action
 */
#define ROTO_TRIGGER_DURATION 175L

/**
 * delay for blinking
 */
#define BLINK_DELAY 300L

/**
 * One second in milliseconds -> 1000ms
 */
#define SECOND 1000L

/**
 * Move status, like opening, closing, ..
 */
enum RotoChannelMoveStatus {
    /**
     * channel currently opens
     */
    MS_OPENING,
    /**
     * channel currently closes
     */
    MS_CLOSING,
    /**
     * channel is not moving/is stopped
     */
    MS_STOP
};

/**
 * Status like, opened, closed, ...
 */
enum RotoChannelStatus {
    /**
     * completely opened
     */
    CS_OPENED = 0,
    /**
     * completely closed
     */
    CS_CLOSED,
    /**
     * somewhere between completely opened and completely closed
     */
    CS_OPEN,
    /**
     * position unknown
     */
    CS_UNDEFINED
};

/**
 * Default action on power up
 */
enum RotoAction {
    /**
     * Do open
     */
    A_OPEN,
    /**
     * Do close
     */
    A_CLOSE,
    /**
     * Do nothing
     */
    A_NONE
};

typedef struct {
    uint8_t setting;
    uint8_t runTimeClose;
    uint8_t runTimeOpen;
    uint8_t runTimeRollover;
    uint8_t shortTimeRun;
    uint8_t shortTimeRunTime;
    uint8_t lockAction;
    uint8_t ventByComObj;
    uint8_t absPositionComObj;
    uint8_t referenceRunComObj;
    uint8_t runStatusComObj;
    uint8_t absPosStatusComObj;
    uint8_t runStatusPositionComObj;
} ChannelConfig;

class RotoChannel {
public:
    RotoChannel(int group, int openTime, int closeTime, int setPinOpen, int resetPinOpen, int setPinClose, int resetPinClose);
    virtual ~RotoChannel();
    void work();
    /**
     * initialize and set the relay channel
     */
    void init(Adafruit_MCP23017& mcp, Frontend8Btn8Led& frontend);

    void setConfig(ChannelConfig config);
    ChannelConfig getConfig();

    void doButton(bool openButton);
    void doOpen();
    void doClose();
    void doStop();

private:
    Adafruit_MCP23017 _mcp;
    Frontend8Btn8Led _frontend;
    ChannelConfig _config;

    /**
     * enabled/disabled state of channel
     */
    bool _enabled;

    int _group; // 0..3

    /**
     * Pin which is connected to SET of the OPEN relay
     */
    int _setPinOpen;

    /**
     * Pin which is connected to RESET of the OPEN relay
     */
    int _resetPinOpen;

    /**
     * Pin which is connected to SET of the CLOSE relay
     */
    int _setPinClose;

    /**
     * Pin which is connected to RESET of the CLOSE relay
     */
    int _resetPinClose;

    /**
     * Time in seconds the channel requires for full open
     */
    int _openTime;
    float _openStep;

    /**
     * Time in secinds the channel requires for full close
     */
    int _closeTime;

    /**
     * 1ms of movement in percent
     */
    float _closeStep;

    /**
     * Flag that inidcates the the init process is DONE
     */
    bool _initDone;

    /**
     * The action that is triggered when the device powers up
     */
    RotoAction _startupAction;

    /**
     * last millis() of blinking
     */
    unsigned long _lastBlinkMillis;

    unsigned long _startMoveMillis;

    /*
     * last blink state
     */
    bool _lastBlinkState;

    RotoAction _lastAction;

    /*
     * the status of the channel
     */
    RotoChannelStatus _status;

    /*
     * the move status of the channel
     */
    RotoChannelMoveStatus _moveStatus;

    /*
     * the last move status
     */
    RotoChannelMoveStatus _lastMoveStatus;

    volatile bool _manualMoveRequest;
    volatile bool _manualMoveRequestOpenButton;

    /**
     * Position if window or Shutter
     * 0.0 = window closed / shutter open
     * 1.0 = window fully opened / shutter fully closed
     */
    float _position;
    float _newPosition;

    boolean _isStopping;

    void updateLEDs();
    void updateStatus();

};

#endif /* ROTOCHANNEL_H_ */
