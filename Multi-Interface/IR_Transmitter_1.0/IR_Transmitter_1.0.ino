#include <IRremote.h> //https://github.com/adafruit/Arduino-IRremote
#include <KonnektingDevice.h>
#include "mi.h"

// include device related configuration code, created by "KONNEKTING CodeGenerator"
#include "kdevice_IR_Transmitter.h"

// ################################################
// ### DEBUG CONFIGURATION
// ################################################
//#define KDEBUG // comment this line to disable DEBUG mode
#ifdef KDEBUG
#include <DebugUtil.h>
#define DEBUGSERIAL SerialUSB
#endif

// ################################################
// ### Global variables, sketch related
// ################################################
#define MAX_INDEX 40 // Max IR codes to save.
#define MAX_SCRIPTS 5
#define FIRST_PARAM 1 // depends on xml. First param id for first ir code
#define PARAM_AMOUNT 5 // amount of params per ir code
#define PARAM_SCRIPT 10 // amount of params per script

#ifdef KDEBUG
String        irText[MAX_INDEX];
String        irScriptText[MAX_SCRIPTS];
#endif
byte          irType[MAX_INDEX];
unsigned long irData[MAX_INDEX];
int           irBits[MAX_INDEX];
byte          irRept[MAX_INDEX];
bool          irScriptActive[MAX_SCRIPTS];
byte          irScriptCode1[MAX_SCRIPTS];
bool          irScriptSend1[MAX_SCRIPTS];
word          irScriptDelay1[MAX_SCRIPTS];
byte          irScriptCode2[MAX_SCRIPTS];
bool          irScriptSend2[MAX_SCRIPTS];
word          irScriptDelay2[MAX_SCRIPTS];
byte          irScriptCode3[MAX_SCRIPTS];
bool          irScriptSend3[MAX_SCRIPTS];
word          irScriptDelay3[MAX_SCRIPTS];
byte          irScriptCode4[MAX_SCRIPTS];
bool          irScriptSend4[MAX_SCRIPTS];
word          irScriptDelay4[MAX_SCRIPTS];
byte          irScriptCode5[MAX_SCRIPTS];
bool          irScriptSend5[MAX_SCRIPTS];

int startDelay = 0;
unsigned long lastMillis = 0;
unsigned long currMillis = 0;

// ################################################
// ### SETUP
// ################################################


IRsend knxSendIr(true,3);

void setup()
{
        // debug related stuff
#ifdef KDEBUG

    // Start debug serial with 9600 bauds
    DEBUGSERIAL.begin(115200);
    // wait for serial port to connect. Needed for Leonardo/Micro/ProMicro/Zero only
    while (!DEBUGSERIAL)
    // make debug serial port known to debug class
    // Means: KONNEKTING will use the same serial port for console debugging
    Debug.setPrintStream(&DEBUGSERIAL);
#endif
    Wire.begin();
    Konnekting.setMemoryReadFunc(&readMemory);
    Konnekting.setMemoryWriteFunc(&writeMemory);
    Konnekting.setMemoryUpdateFunc(&updateMemory);
    Konnekting.setMemoryCommitFunc(&commitMemory);


    // Initialize KNX enabled Arduino Board
    Konnekting.init(KNX_SERIAL,
            PROG_BUTTON_PIN,
            PROG_LED_PIN,
            MANUFACTURER_ID,
            DEVICE_ID,
            REVISION);

    // If device has been parametrized with KONNEKTING Suite, read params from EEPROM
    // Otherwise continue with global default values from sketch
    if (!Konnekting.isFactorySetting()) {
        startDelay = (int) Konnekting.getUINT16Param(PARAM_initialDelay); 

    		for(byte i = 0; i < MAX_INDEX;i++){
#ifdef KDEBUG
            //first param is the comment to ir code, we don't need it, it is only for the suite or debug
            irText[i] = Konnekting.getSTRING11Param(i*PARAM_AMOUNT+FIRST_PARAM); //comment
#endif
            irType[i] = (byte)Konnekting.getUINT8Param(i*PARAM_AMOUNT+FIRST_PARAM + 1);  //type, e.g. NEC, SONY...
            byte tempArray[] = {0,0,0,0};
            Konnekting.getParamValue(i*PARAM_AMOUNT+FIRST_PARAM + 2,tempArray);  //data comes as byte array and will be converted to unsigned long
            irData[i] = byteArrayToULong(tempArray);
            irBits[i] = Konnekting.getUINT8Param(i*PARAM_AMOUNT+FIRST_PARAM + 3);  //amount of data bits
            irRept[i] = Konnekting.getUINT8Param(i*PARAM_AMOUNT+FIRST_PARAM + 4);  //repeat or not repeat
    		}
        for(byte s = 0;s < MAX_SCRIPTS;s++){
            irScriptActive[s] = false;
            irScriptSend1[s] = false;
            irScriptSend2[s] = false;
            irScriptSend3[s] = false;
            irScriptSend4[s] = false;
            irScriptSend5[s] = false;
#ifdef KDEBUG
            //first param is the comment to script, we don't need it, it is only for the suite or debug
            irScriptText[s] = Konnekting.getSTRING11Param(s*PARAM_SCRIPT + MAX_INDEX*PARAM_AMOUNT+FIRST_PARAM); //comment
#endif
            irScriptCode1[s]  = (byte)Konnekting.getUINT8Param (s*PARAM_SCRIPT + MAX_INDEX*PARAM_AMOUNT+FIRST_PARAM + 1);
            irScriptDelay1[s] = (word)Konnekting.getUINT16Param(s*PARAM_SCRIPT + MAX_INDEX*PARAM_AMOUNT+FIRST_PARAM + 2);
            irScriptCode2[s]  = (byte)Konnekting.getUINT8Param (s*PARAM_SCRIPT + MAX_INDEX*PARAM_AMOUNT+FIRST_PARAM + 3);
            irScriptDelay2[s] = (word)Konnekting.getUINT16Param(s*PARAM_SCRIPT + MAX_INDEX*PARAM_AMOUNT+FIRST_PARAM + 4);
            irScriptCode3[s]  = (byte)Konnekting.getUINT8Param (s*PARAM_SCRIPT + MAX_INDEX*PARAM_AMOUNT+FIRST_PARAM + 5);
            irScriptDelay3[s] = (word)Konnekting.getUINT16Param(s*PARAM_SCRIPT + MAX_INDEX*PARAM_AMOUNT+FIRST_PARAM + 6);
            irScriptCode4[s]  = (byte)Konnekting.getUINT8Param (s*PARAM_SCRIPT + MAX_INDEX*PARAM_AMOUNT+FIRST_PARAM + 7);
            irScriptDelay4[s] = (word)Konnekting.getUINT16Param(s*PARAM_SCRIPT + MAX_INDEX*PARAM_AMOUNT+FIRST_PARAM + 8);
            irScriptCode5[s]  = (byte)Konnekting.getUINT8Param (s*PARAM_SCRIPT + MAX_INDEX*PARAM_AMOUNT+FIRST_PARAM + 9);
        }
    }
#ifdef KDEBUG
    Debug.println(F("Scene\tType\tData\t\tBits\tRepeats\tComment"));
    for(byte i = 0; i < MAX_INDEX;i++){
        char temp[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
        irText[i].toCharArray(temp, 12);
        Debug.println(F("%d\t%d\t%08X\t%d\t%d\t%s"),i+1,irType[i],irData[i],irBits[i],irRept[i],temp);
    }
    Debug.println(F("Scene\tCode 1\tDelay\tCode 2\tDelay\tCode 3\tDelay\tCode 4\tDelay\tCode 5\tComment"));
    for(byte s = 0; s < MAX_SCRIPTS;s++){
        char temp[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
        irScriptText[s].toCharArray(temp, 12);
        Debug.println(F("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s"),MAX_INDEX+s+1,irScriptCode1[s],irScriptDelay1[s],irScriptCode2[s],irScriptDelay2[s],irScriptCode3[s],irScriptDelay3[s],irScriptCode4[s],irScriptDelay4[s],irScriptCode5[s],temp);
    }
#endif    
    if (startDelay > 0) {
        Debug.println(F("delay for %d ms"),startDelay);
        delay(startDelay);
        Debug.println(F("ready!"));
    }
    Debug.println(F("Setup is ready. go to loop..."));
}


void loop() {
    Knx.task();
    if (Konnekting.isReadyForApplication()) {
        currMillis = millis();
        for(byte s = 0;s < MAX_SCRIPTS;s++){
            if(irScriptActive[s] && irScriptCode1[s] != 0){
                if(!irScriptSend1[s]){
                    Debug.println(F("Script %d, IR code number 1"),s+1);
                    sendIr(irType[irScriptCode1[s]-1],irData[irScriptCode1[s]-1],irBits[irScriptCode1[s]-1]);
                    irScriptSend1[s] = true;
                    lastMillis = currMillis;
                }
                if(irScriptCode2[s] != 0){
                    if(!irScriptSend2[s] && irScriptSend1[s] && (currMillis - lastMillis) >= irScriptDelay1[s]){
                        Debug.println(F("Script %d, IR code number 2"),s+1);
                        sendIr(irType[irScriptCode2[s]-1],irData[irScriptCode2[s]-1],irBits[irScriptCode2[s]-1]);
                        irScriptSend2[s] = true;
                        lastMillis = currMillis;
                    }
                }else{
                    irScriptSend2[s] = true;
                }
                if(irScriptCode3[s] != 0){
                    if(!irScriptSend3[s] && irScriptSend1[s] && irScriptSend2[s] && (currMillis - lastMillis) >= irScriptDelay2[s]){
                        Debug.println(F("Script %d, IR code number 3"),s+1);
                        sendIr(irType[irScriptCode3[s]-1],irData[irScriptCode3[s]-1],irBits[irScriptCode3[s]-1]);
                        irScriptSend3[s] = true;
                        lastMillis = currMillis;
                    }
                }else{
                    irScriptSend3[s] = true;
                }
                if(irScriptCode4[s] != 0){
                    if(!irScriptSend4[s] && irScriptSend1[s] && irScriptSend2[s] && irScriptSend3[s] && (currMillis - lastMillis) >= irScriptDelay3[s]){
                        Debug.println(F("Script %d, IR code number 4"),s+1);
                        sendIr(irType[irScriptCode4[s]-1],irData[irScriptCode4[s]-1],irBits[irScriptCode4[s]-1]);
                        irScriptSend4[s] = true;
                        lastMillis = currMillis;
                    }
                }else{
                    irScriptSend4[s] = true;
                }
                if(irScriptCode5[s] != 0){
                    if(irScriptSend1[s] && irScriptSend2[s] && irScriptSend3[s] && irScriptSend4[s] && (currMillis - lastMillis) >= irScriptDelay4[s]){
                        Debug.println(F("Script %d, IR code number 5"),s+1);
                        sendIr(irType[irScriptCode5[s]-1],irData[irScriptCode5[s]-1],irBits[irScriptCode5[s]-1]);
                        irScriptSend5[s] = true;
                }
                }else{
                    irScriptSend5[s] = true;
                }
                if(irScriptSend1[s] && irScriptSend2[s] && irScriptSend3[s] && irScriptSend4[s] && irScriptSend5[s]){
                    irScriptActive[s] = false;
                    irScriptSend1[s] = false;
                    irScriptSend2[s] = false;
                    irScriptSend3[s] = false;
                    irScriptSend4[s] = false;
                    irScriptSend5[s] = false;
                    Debug.println(F("Script %d is ready"),s+1);
                }
            }else{
                irScriptActive[s] = false;
                irScriptSend1[s] = false;
                irScriptSend2[s] = false;
                irScriptSend3[s] = false;
                irScriptSend4[s] = false;
                irScriptSend5[s] = false;
            }
        }
    }
}

void sendIr(byte type, unsigned long data, int bits){
    switch (type){
        case NEC:
            Debug.println(F("sendNEC(%08X,%i)"),data,bits);
            knxSendIr.sendNEC(data, bits);
            break;
        case SAMSUNG:
            Debug.println(F("sendSAMSUNG(%08X,%i)"),data,bits);
            knxSendIr.sendSAMSUNG(data, bits);
            break;
        case SONY:
            Debug.println(F("sendSony(%08X,%i)"),data,bits);
            knxSendIr.sendSony(data, bits);
            break;
        case RC5:
            Debug.println(F("sendRC5(%08X,%i)"),data,bits);
            knxSendIr.sendRC5(data, bits);
            break;
        case RC6:
            Debug.println(F("sendRC6(%08X,%i)"),data,bits);
            knxSendIr.sendRC6(data, bits);
            break;
        case DENON:
            Debug.println(F("sendDenon(%08X,%i)"),data,bits);
            knxSendIr.sendDenon(data, bits);
            break;
        case AIWA_RC_T501:
            Debug.println(F("sendAiwaRCT501(%08X)"),data);
            knxSendIr.sendAiwaRCT501(data);
            break;
        case JVC:
            Debug.println(F("sendJVC(%08X,%i,false)"),data,bits);
            knxSendIr.sendJVC(data, bits, false);
            break;
        case LG:
            Debug.println(F("sendLG(%08X,%i)"),data,bits);
            knxSendIr.sendLG(data, bits);
            break;
        default:
            break;
    }
}

//convert a byte array to unsigned long
unsigned long byteArrayToULong(byte *dataarray) {
    unsigned long value = 0;
    value = ((uint32_t) dataarray[0] << 24) + ((uint32_t) dataarray[1] << 16) + ((uint32_t) dataarray[2] << 8) + ((uint32_t) dataarray[3] << 0);
    return value;
}


// ################################################
// ### KNX EVENT CALLBACK
// ################################################

void knxEvents(byte comBoj) {
    byte index = 0;
    // nothing to do in this sketch
    switch (comBoj) {
        case COMOBJ_sendIr: // object index has been updated
            index = Knx.read(COMOBJ_sendIr);
            if (index < MAX_INDEX){ 
                for(byte r = 0;r<irRept[index];r++){
                    sendIr(irType[index],irData[index],irBits[index]);
                    if (irRept[index] != 1){
                       delay(5);
                    }
                }
            }
            if (index >= MAX_INDEX && index < MAX_INDEX+MAX_SCRIPTS){
                irScriptActive[index-MAX_INDEX] = true;
                Debug.println(F("Script %d is activ"),index-MAX_INDEX+1);
            }
            break;
        default:
            break;
    }
};

