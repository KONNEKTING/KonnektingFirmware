/* ---------------------------------------------------------------------------------------------
iButtons v1.2
Hardware/Firmware/Sketch/kdevice.xml by E.Burkowski / e.burkowski@konnekting.de, GPL Licensed
Compatible with: KONNEKTING Device Library 1.0.0 beta 4

------------------------------------------------------------------------------------------------*/
//OneWire on pin 3 (you need a 4.7 kOhm PullUp)
#define ONE_WIRE 3 




#define iButtons 10 //iButtons count depends on kdevice.xml

#include <KnxDevice.h>
#include "kdevice_iButton.h"
#include "mi.h"

//#define KDEBUG // comment this line to disable DEBUG mode
#ifdef KDEBUG
#include <DebugUtil.h>
#define DEBUGSERIAL SerialUSB
#endif

#include <OneWire.h>

OneWire oneWire(ONE_WIRE);

byte iButtonComObj[] = {COMOBJ_ibutton1,
                        COMOBJ_ibutton2,
                        COMOBJ_ibutton3,
                        COMOBJ_ibutton4,
                        COMOBJ_ibutton5,
                        COMOBJ_ibutton6,
                        COMOBJ_ibutton7,
                        COMOBJ_ibutton8,
                        COMOBJ_ibutton9,
                        COMOBJ_ibutton10};

byte iButton_addr[8];
byte iButton0[8];
byte iButton1[8];
byte iButton2[8];
byte iButton3[8];
byte iButton4[8];
byte iButton5[8];
byte iButton6[8];
byte iButton7[8];
byte iButton8[8];
byte iButton9[8];

bool iButtonActive[]     = {false,false,false,false,false,false,false,false,false,false};
bool iButtonState[]      = {false,false,false,false,false,false,false,false,false,false};
bool iButtonLastState[]  = {false,false,false,false,false,false,false,false,false,false};
bool iButtonInvOutput[]  = {false,false,false,false,false,false,false,false,false,false};

bool lastState_present = false;
bool lastState_one_absent = true;
bool lastState_absent = true;
bool lastState_unreg = true;

bool present = false;
bool one_absent = false;
bool absent = false;
int absentValue = 255;
int absentScene = 255;
bool lastStateAbsent = false;
bool unreg = false;

unsigned long prevTime= 0;
unsigned long firstAbsent = 0;
bool firstAbsentSet = false;
long absentDelay[] = {0,10,30,60,120,180,240,300,600,900,1200,1800};
byte absentDelayNummer = 0;
long cycleTime = 500;

#include "iButtonFunctions.h"

void knxEvents(byte index) {
    // nothing to do in this sketch
};
