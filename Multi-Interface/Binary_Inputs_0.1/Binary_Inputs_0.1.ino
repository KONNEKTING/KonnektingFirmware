/* ---------------------------------------------------------------------------------------------
Binary Imputs v0.1 beta
Hardware/Firmware/Sketch/kdevice.xml by E.Burkowski / e.burkowski@konnekting.de, GPL Licensed
Compatible with: KONNEKTING Device Library 1.0.0 beta 4

For Pinout see mi.h
------------------------------------------------------------------------------------------------*/

#define INPUTS 12 //Inputs count depends on kdevice.xml

#include <KonnektingDevice.h>
#include "mi.h"

// include device related configuration code, created by "KONNEKTING CodeGenerator"
#include "kdevice_binary_inputs.h"

//#define KDEBUG // comment this line to disable DEBUG mode
#ifdef KDEBUG
#include <DebugUtil.h>
#define DEBUGSERIAL SerialUSB
#endif

unsigned long currentTime = 0;
unsigned long binaryPreviousTime = 0;
unsigned long binaryPreviousDelayTime = 0;
long binaryCycle = 200;

byte pinNummber[]   = {11,13,10,12,6,7,2,5,4,3,A1,A2};
byte inputActive[]  = {0,0,0,0,0,0,0,0,0,0,0,0};
byte invertOutput[] = {0,0,0,0,0,0,0,0,0,0,0,0};
bool lastState[]    = {0,0,0,0,0,0,0,0,0,0,0,0};
bool currentState[] = {0,0,0,0,0,0,0,0,0,0,0,0};
long outputDelay[] =  {0,0,0,0,0,0,0,0,0,0,0,0};
long previousOutputDelay[] = {0,0,0,0,0,0,0,0,0,0,0,0};

// Callback function to handle com objects updates
void knxEvents(byte index) {
    // nothing to do in this sketch
};

#include "binary_functions.h"
#include "setup.h"
#include "loop.h"




