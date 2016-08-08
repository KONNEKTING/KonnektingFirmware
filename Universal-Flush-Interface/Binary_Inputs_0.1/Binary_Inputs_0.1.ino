/* ---------------------------------------------------------------------------------------------
Binary Imputs v0.1 beta
Hardware/Firmware/Sketch/kdevice.xml by E.Burkowski / e.burkowski@konnekting.de, GPL Licensed
Compatible with: KONNEKTING Device Library 1.0.0 beta 3

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



// comment following line to disable DEBUG mode
#define DEBUG debugSerial

// no need to comment, you can leave it as it is as long you do not change the "#define DEBUG debugSerial" line
#ifdef DEBUG
#include <SoftwareSerial.h>
SoftwareSerial debugSerial(10, 11); // RX, TX
#endif

#include "Binary_Inputs.h"
#include "ufi_rev1.h"

// include KnxDevice library
#include <KnxDevice.h>

unsigned long currentTime = 0;
unsigned long binaryPreviousTime = 0;
unsigned long binaryPreviousDelayTime = 0;
long binaryCycle = 200;

byte PinNummber[]   = {BIN_A,BIN_B,BIN_C,BIN_D,BIN_E,BIN_F,BIN_G,BIN_H,BIN_I,BIN_J};
byte InputActive[]  = {0,0,0,0,0,0,0,0,0,0};
byte InvertOutput[] = {0,0,0,0,0,0,0,0,0,0};
bool lastState[]    = {0,0,0,0,0,0,0,0,0,0};
bool currentState[] = {0,0,0,0,0,0,0,0,0,0};
long OutputDelay[] =  {0,0,0,0,0,0,0,0,0,0};
long PreviousOutputDelay[] = {0,0,0,0,0,0,0,0,0,0};








