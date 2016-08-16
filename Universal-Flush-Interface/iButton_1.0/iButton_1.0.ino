/* ---------------------------------------------------------------------------------------------
iButtons v1.0
Hardware/Firmware/Sketch/kdevice.xml by E.Burkowski / e.burkowski@konnekting.de, GPL Licensed
Compatible with: KONNEKTING Device Library 1.0.0 beta 3

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

// comment following line to disable DEBUG mode
#define DEBUG debugSerial

// no need to comment, you can leave it as it is as long you do not change the "#define DEBUG debugSerial" line
#ifdef DEBUG
#include <SoftwareSerial.h>
SoftwareSerial debugSerial(10, 11); // RX, TX
#endif

#include "iButton.h"
#include "ufi_rev1.h"

#include <OneWire.h>

// include KnxDevice library
#include <KnxDevice.h>

OneWire oneWire(ONE_WIRE);
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
bool unreg = false;

unsigned long prevTime= 0;
long cycleTime = 500;
