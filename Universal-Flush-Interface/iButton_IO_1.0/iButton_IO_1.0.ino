/* ---------------------------------------------------------------------------------------------
iButtons v1.1
Hardware/Firmware/Sketch/kdevice.xml by E.Burkowski / e.burkowski@konnekting.de, GPL Licensed
Compatible with: KONNEKTING Device Library 1.0.0 beta 4

Universal Flush Interface:
       _______USB__________
    E1/                    \
   D14|                    |E4
   D16|                    |+3.3V
   D15|                    |OWRight
 +3.3V|                    |GND
    D4|                    |
   GND|                    |KNX+
    D6|                    |KNX-
   D22|                    |
   GND|   D23              |
   D21|                    |
      \____________________/
    E2    1 2 3 4 5 6 7
          | | \ D9
          | \GND
          \D8
------------------------------------------------------------------------------------------------*/
//Define OneWire connection. Please check ufi_rev1.h
//#define ONE_WIRE OWLeft 
//#define ONE_WIRE_PULLUP OWLeftPullUp

#define ONE_WIRE OWRight 
#define ONE_WIRE_PULLUP OWRightPullUp

#define iButtons 10 //iButtons count depends on kdevice.xml
#define INPUTS 2 //Inputs count depends on kdevice.xml
#define OUTPUTS 7 //Outputs count depends on kdevice.xml

//depends on xml file:
#define IB_FIRST_PARAM 9
#define IB_PARAM_COUNT 3

#define IN_FIRST_PARAM 39
#define IN_PARAM_COUNT 3

#define OUT_FIRST_PARAM 45
#define OUT_PARAM_COUNT 2



#include <KnxDevice.h>
#include "kdevice_iButton_IO.h"
#include "ufi_rev1.h"

#define KDEBUG // comment this line to disable DEBUG mode
#ifdef KDEBUG
#include <DebugUtil.h>
#define DEBUGSERIAL Serial
#endif

#include <OneWire.h>

OneWire oneWire(ONE_WIRE);
//iButton stuff
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

unsigned long iButtonPrevTime= 0;
unsigned long firstAllAbsent = 0;
bool firstAllAbsentSet = false;
bool allAbsentWithDelay = false;
long absentDelay[] = {0,10,30,60,120,180,240,300,600,900,1200,1800};
byte absentDelayNummer = 0;
long iButtonCycleTime = 500;

#include "iButtonFunctions.h"

//input & output stuff
unsigned long inputPreviousTime = 0;
unsigned long inputPreviousDelayTime = 0;
long inputCycle = 200;
//define input pins
byte pinNummberInputs[]    = {8,9};               //input pins
byte comObjToInputId[]     = {COMOBJ_inputA,
                              COMOBJ_inputB};
byte activeInputs[]  = {0,0};
byte invertOutputInputs[] = {0,0};
bool lastStateInputs[]    = {0,0};
bool currentStateInputs[] = {0,0};
long outputDelayInputs[]  = {0,0};
long previousOutputDelayInputs[] = {0,0};

//define output pins
byte pinNumberOutput[]   = {21,22,6,4,15,16,14};  //outut pins
byte comObjToOutputId[]  = {COMOBJ_outputA,
                            COMOBJ_outputB,
                            COMOBJ_outputC,
                            COMOBJ_outputD,
                            COMOBJ_outputE,
                            COMOBJ_outputF,
                            COMOBJ_outputG};
byte outputActive[]      = {0,0,0,0,0,0,0};
byte invertOutput[]      = {0,0,0,0,0,0,0};
bool lastStateOutput[]   = {0,0,0,0,0,0,0};
bool currentStateOutput[]= {0,0,0,0,0,0,0};

//logic stuff
bool alarm1active = false;
bool alarm1state = false;
bool alarm1value = false;
int alarm1scene = 255;
byte alarm1inputNumber = 0;
byte alarm1delay = 5;
long alarm1delayValue[] = {0,10,30,60,120,180,240,300,360,420,480,540,600};
unsigned long inputAlarm1PreviousTime = 0;
unsigned long alarm1beginTime = 0;


#include "io_functions.h"

#include "setup.h"


void knxEvents(byte comObj) {
    for (byte o = 0;o<OUTPUTS;o++){
    //find output pin
        if (comObj = comObjToOutputId[o]){
          currentStateOutput[o] = Knx.read(comObj);
        }
    }
}



void loop(void) {
    Knx.task();
    
    if (Konnekting.isReadyForApplication()) {
        task_iButtons();
        Knx.task();
        task_inputs();
        task_outputs();
    }
}
