/* ---------------------------------------------------------------------------------------------
Binary Imputs v0.1 beta
Hardware/Firmware/Sketch/kdevice.xml by E.Burkowski / e.burkowski@konnekting.de, GPL Licensed
Compatible with: KONNEKTING Device Library 1.0.0 beta 4

Universal Flush Interface:
       _______USB__________
      /                    \
 BIN_I|                    |
 BIN_H|                    |X
 BIN_G|                    |BIN_J
     X|                    |GND
 BIN_F|                    |
   GND|                    |KNX+
 BIN_E|                    |KNX-
 BIN_D|                    |
   GND|                    |
 BIN_C|                    |
      \____________________/
          | | \ BIN_A
          | \GND
          \BIN_B
------------------------------------------------------------------------------------------------*/

#define INPUTS 10 //Inputs count depends on kdevice.xml

#include <KonnektingDevice.h>
#include "ufi_rev1.h"

// include device related configuration code, created by "KONNEKTING CodeGenerator"
#include "kdevice_binary_inputs.h"


// ################################################
// ### DEBUG CONFIGURATION
// ################################################
//#define KDEBUG // comment this line to disable DEBUG mode
#ifdef KDEBUG
#include <DebugUtil.h>
#define DEBUGSERIAL Serial
#endif

unsigned long currentTime = 0;
unsigned long binaryPreviousTime = 0;
unsigned long binaryPreviousDelayTime = 0;
long binaryCycle = 200;

byte pinNummber[]   = {BIN_A,BIN_B,BIN_C,BIN_D,BIN_E,BIN_F,BIN_G,BIN_H,BIN_I,BIN_J};
byte inputActive[]  = {0,0,0,0,0,0,0,0,0,0};
byte invertOutput[] = {0,0,0,0,0,0,0,0,0,0};
bool lastState[]    = {0,0,0,0,0,0,0,0,0,0};
bool currentState[] = {0,0,0,0,0,0,0,0,0,0};
long outputDelay[] =  {0,0,0,0,0,0,0,0,0,0};
long previousOutputDelay[] = {0,0,0,0,0,0,0,0,0,0};

// Callback function to handle com objects updates
void knxEvents(byte index) {
    // nothing to do in this sketch
};

#include "binary_functions.h"
#include "setup.h"
#include "loop.h"




