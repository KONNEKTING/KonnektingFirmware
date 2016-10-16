/* ---------------------------------------------------------------------------------------------
iButtons v1.1
Hardware/Firmware/Sketch/kdevice.xml by E.Burkowski / e.burkowski@konnekting.de, GPL Licensed
Compatible with: KONNEKTING Device Library 1.0.0 beta 4

Universal Flush Interface:
       _______USB__________
    E1/                    \
     1|                    |E4
     2|                    |+3.3V
     3|                    |OWRight
 +3.3V|                    |GND
OWLeft|                    |
   GND|                    |KNX+
     7|                    |KNX-
     8|                    |
     9|                    |
    10|                    |
      \____________________/
    E2    1 2 3 4 5 6 7
------------------------------------------------------------------------------------------------*/
//Define OneWire connection. Please check ufi_rev1.h
//#define ONE_WIRE OWLeft 
//#define ONE_WIRE_PULLUP OWLeftPullUp

#define ONE_WIRE OWRight 
#define ONE_WIRE_PULLUP OWRightPullUp

#define iButtons 10 //iButtons count depends on kdevice.xml

#include <KnxDevice.h>
#include "kdevice_iButton.h"
#include "ufi_rev1.h"

//#define KDEBUG // comment this line to disable DEBUG mode
#ifdef KDEBUG
#include <DebugUtil.h>
#define DEBUGSERIAL Serial
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
