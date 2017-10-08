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
 * delay for blinking while init
 */
#define BLINK_INIT_DELAY 150L

/**
 * One second in milliseconds -> 1000ms
 */
#define SECOND 1000L

/**
 * 100% / 255(0xff) = 0,39% per 1(0x01)
 */
#define BYTE_PERCENT 0.392156863



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

typedef struct {
    
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
     * Settings: Lüften per zentralem KO 0=false, 1=true
     */
    uint8_t ventByComObj;
    
    /**
     * Ansteuerung: KO für absolute Position 0=nicht aktiv|1=aktiv
     */
    uint8_t absPositionComObj;
    
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
    void doOpen();
    void doClose();
    void doStop();
    
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
    /** millis when last status was sent */
    unsigned long _lastStatusUpdate;
    /** 
     * last status value that has been sent. 
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
    
    
    void workLEDs();
    void workStatus();
    void workPosition();

};

#endif /* ROTOCHANNEL_H_ */
