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
#define BLINK_DELAY 500L

/**
 * delay for blinking while init
 */
#define BLINK_INIT_DELAY 250L

/**
 * One second in milliseconds -> 1000ms
 */
#define SECOND 1000L

/**
 * 100% / 255(0xff) = 0,39% per 1(0x01)
 */
#define BYTE_PERCENT 0.392156863

/**
 * Means: Value is not defined
 */
#define NOT_DEFINED -1



/**
 * The offset for comobj calculation.
 * 
 * f.i. group 0 has it's ComObj stating at index=8, then ComObjOffset is set to 8.
 * Depending on the group and offset, the Channel can calculate it's ComObj indizes itself.
 * So main-sketch just needs to forward all knxEvents to all channels and each channel filters out it's ComObj events.
 */
#define COMOBJ_OFFSET 8
#define COMOBJ_PER_CHANNEL 16

/**
 * time interval in which status updates are sent
 */
#define STATUS_UPDATE_INTERVAL 1000

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

enum RotoLock {
    /**
     * 0: shutter is locked (lock action = done)
     */
    LCK_LOCKED,
    /**
     * 1: shutter is doing lock-action
     */
    LCK_LOCKING,
    /**
     * 2: shutter is unlocked (unlock action = done)
     */
    LCK_UNLOCKED,
    /**
     * 3: shutter is doing unlock-action
     */
    LCK_UNLOCKING,
    /**
     * 4: Reference run is active
     */
    LCK_REFERENCE_RUN
};

enum RotoReferenceRun {
    REF_NONE,
    /**
     * 
     */
    REF_START,
    /**
     * 
     */
    REF_CLOSING,
    /**
     * 
     */
    REF_RESTORING,
    /**
     * 
     */
    REF_DONE
};

enum RotoVentilate {
    VENT_NONE,
    /**
     * 
     */
    VENT_START,
    /**
     * 
     */
    VENT_VENTILATE,
    /**
     * 
     */
    VENT_RESTORING,
    /**
     * 
     */
    VENT_DONE
};

enum FrontendLed {
    /**
     * LED for open-status
     */
    LED_OPEN,
    /**
     * LED for close-status
     */
    LED_CLOSE
};

typedef struct {
    
    /**
     * 
     */
    unsigned long ventilationTime;
    
    /**
     */
    uint8_t triggerTime;
    
    /**
     * Settings: Channel setting: 
     * 0 = unused
     * 1 = roof-light
     * 2 = shutter
     * 3 = clone of ...
     */
    uint8_t setting;
    
    /**
     * Settings: Time in seconds the channel requires for full close
     */
    uint8_t runTimeClose;
    
    /**
     * Settings: Time in seconds the channel requires for full open
     */
    uint8_t runTimeOpen;
    
    /**
     * Settings: "Fahrzeitverlängerung" in %
     * 2=2%, 5=5%, 10=10%, 15=15%, 20=20%
     */
    uint8_t runTimeRollover;
       
    /**
     * Settings: Verhalten bei Sperren: 0=keine Änderung, 1=auf fahren, 2=zu fahren
     */
    uint8_t lockAction;
    
    /**
     * Settings: Verhalten bei Rückname der Sperre / Alarme, 00=keine Aktion|01=auf fahren|02=zu fahren|03=vorherige Position anfahren
     */
    uint8_t unlockAction;
    
    /**
     * Settings: Regenalarm, 00=nicht aktiv|01=aktiv
     */
    uint8_t rainAlarm;
    
    /**
     * Settings: Regenalarm: Überwachungszeit [0..120min, 0=aus], Default="1E" Min="00" Max="78"
     */
    uint8_t rainAlarmObservationTime;
    
    /**
     * Settings: Regenalarm: Aktion, 00=keine Aktion|01=auf fahren|02=zu fahren
     */
    uint8_t rainAlarmAction;
    
    /**
     * Settings: Windalarm, 00=nicht aktiv|01=aktiv
     */
    uint8_t windAlarm;
    
    /**
     * Settings: Windalarm: Überwachungszeit [0..120min, 0=aus], Default="1E" Min="00" Max="78"
     */
    uint8_t windAlarmObservationTime;
    
    /**
     * Settings: Windalarm: Aktion, 00=keine Aktion|01=auf fahren|02=zu fahren
     */
    uint8_t windAlarmAction;
    
    /**
     * Settings: Lüften per zentralem KO 0=false, 1=true
     */
    uint8_t ventByComObj;
    
    /**
     * Ansteuerung: KO für absolute Position 0=nicht aktiv|1=aktiv
     */
    uint8_t absPositionComObj;
    
    /**
     * Ansteuerung: KO für "Position anfahren" 0=nicht aktiv|1=aktiv
     */
    uint8_t driveToPositionComObj;
    
    /**
     * Settings: Wert für "Position anfahren" 00=0%|05=5%|0A=10%|0F=15%|14=20%|19=25%|1E=30%|23=35%|28=40%|2D=45%|32=50%|37=55%|3C=60%|41=65%|46=70%|4B=75%|50=80%|55=85%|5A=90%|5F=95%|64=100%
     */
    uint8_t driveToPositionValue;
    
    /**
     * Ansteuerung: KO für Referenzfahrt 0=nicht aktiv|1=aktiv
     */
    uint8_t referenceRunComObj;
    
    /**
     * Status: KO für Verfahrstatus 0=nicht aktiv|1=fährt (1 KO)|2=Auffahrt+Zufahrt (2 KOs)
     */
    uint8_t runStatusComObj;
    
    /**
     * Status: Status für aktuelle Position 0=nicht aktiv|1=aktiv
     */
    uint8_t absPosStatusComObj;
    
    /**
     * Status: Status für akt. Richtung & Position auf/zu 0=nicht aktiv|1=aktiv
     */
    uint8_t runStatusPositionComObj;
    
} ChannelConfig;

class RotoChannel {
public:
    RotoChannel(int group, int setPinOpen, int resetPinOpen, int setPinClose, int resetPinClose);
    virtual ~RotoChannel();
    void work();
    /**
     * initialize and set the relay channel
     */
    void init(Adafruit_MCP23017& mcp, Frontend8Btn8Led& frontend);

    void setConfig(ChannelConfig config);
    ChannelConfig getConfig();

    void doButton(bool openButton);
    
    
    /**
     * used by sketch to feed in com-obj events. If event is not used for this channel, method does nothing
     * @param index absolute comobj index
     * @return if event has been consumed or not
     */
    bool knxEvents(byte index);

private:
    Adafruit_MCP23017 _mcp;
    Frontend8Btn8Led _frontend;
    ChannelConfig _config;

    /**
     * enabled/disabled state of channel
     */
    bool _enabled;
    
    /**
     * Lock enum
     */
    RotoLock _lock;
    
    /**
     * Reference run enum
     */
    RotoReferenceRun _referenceRun;
    
    /**
     * Ventilate enum
     */
    RotoVentilate _ventilate;
    /**
     * millis() at which ventilate started
     */
    unsigned long _ventilateStart;
    
    /**
     * The comobj base index for this channel/group, means: at this absolute comobj index do the comobj start for this channel
     */
    byte _baseIndex;
    

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
     * 1ms of movement in percent
     */
    float _openStep;
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
     * Position of window or Shutter
     * 0.0 = window closed / shutter open
     * 1.0 = window fully opened / shutter fully closed
     */
    float _position;
    /**
     * Calculated new position, base on start-position + current_move_delta.
     * After move has been completed, _position is updated with _newPosition
     */
    float _newPosition;
    /**
     * Absolute position the window/shutter has to reach. Used/set with doPosition().
     */
    float _targetPosition;
    /**
     * Last position, set when f.i. lock happens, to remember "previous position"
     */
    float _previousPosition;
    
    /** millis when last status was sent */
    unsigned long _lastStatusUpdate;
    /** 
     * last status value that has been sent to bus as status update. 
     * Used to ensure that we don't send a value twice, especially when we 
     * reach stop, where the latest value is (caused by calculation 
     * from % to byte) the same
     */
    byte _lastSentPosition;

    bool _isStopping;
    
    /**
     * Open time in ms, including rollover
     * @return 
     */
    uint32_t getTime(uint8_t time);
    
    /**
     * Updating the LEDs to show current status
     */
    void workLEDs();
    /**
     * This functions handles status updated of the channel. 
     * It is f.i. responsible for sending position updates to the bus.
     */
    void workStatus();
    /**
     * The implementation is all about the position of window/shutter, so this method
     * is handling the position while moving the window
     */
    void workPosition();
    
    /**
     * Open shutter or window fully (if not stopped)
     */
    void doOpen();
    /**
     * Close shutter or window fully (if not stopped)
     */
    void doClose();
    /**
     * Stop movement
     */
    void doStop();
    /**
     * Move window/shutter to an absolute position
     * @param targetPosition the new absolute position the window/shutter should move to [0-1.0]
     */
    void doPosition(float targetPosition);
    
    /**
     * Returns true, if shutter is locked or doiong lock-action,
     * false if unlocked, or doing unlock-action
     * @return bool
     */
    bool isLocked();
    
    /**
     * returns true, if channel is in move state
     * @return bool
     */
    bool isMoving();
    
    /**
     * Keeps position in 0..1 range. If bigger, limit to 1, if smaller, limit to 0
     */
    float limitPos(float pos);
    
    /**
     * Check for window setup
     * @return true, if channel/group is window, false if it's shutter
     */
    bool isWindow();
    
    /**
     * Returns frontend LED index
     * @param led enum for open and close LED
     * @return frontend LED index
     */
    int getLed(FrontendLed led);
    
    /**
     * Returns true if just reached stop
     * @return 
     */
    bool isJustStopped();
    
    /**
     * calculates the correct comobj index for the given "COMOBJ_...." variable
     */
    byte getComObjIndex(byte comObj);
    
    /**
     * used by doOpen() and doClose() to send one-time-status of movement when movement begins
     */
    void sendMovementStatus();
    
};

#endif /* ROTOCHANNEL_H_ */
