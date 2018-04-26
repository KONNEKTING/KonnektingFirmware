/* *****************************************************************************
   Includes
*/
#include <Arduino.h>
#include <KonnektingDevice.h>
#include "Memory.h"

#include <SerialFlash.h>
#include <SPI.h>

#include "AudioZeroSPI.h"



/* *****************************************************************************
   Defines
*/

#define KNX_SERIAL Serial1
#define PROG_LED 19 // D19 / A1
#define PROG_BTN 15 // D15 / A5

#define MANUFACTURER_ID 49374
#define DEVICE_ID 2
#define REVISION 0

#define FT_OBJ 0
#define PLAY_OBJ 1

#define LED1_R 3
#define LED1_G 4
#define LED1_B 5

#define LED2_R 13
#define LED2_G 11
#define LED2_B 6

#define SPK_ON 10

#define BUTTON1 2 // interrupt!
#define BUTTON2 7 // interrupt!

KnxComObject KnxDevice::_comObjectsList[] = {
  /* Index 0 - FileTransfer 15/7/254 */ KnxComObject(KNX_DPT_60000_60000, KNX_COM_OBJ_C_W_U_T_INDICATOR),
  /* Index 1 - Test Receive Object 15/7/253 */ KnxComObject(KNX_DPT_1_001, 0x2a),
};
const byte KnxDevice::_numberOfComObjects = sizeof (_comObjectsList) / sizeof (KnxComObject); // do not change this code

byte KonnektingDevice::_paramSizeList[] = {
  /* Index 0 - startupDelay */ PARAM_UINT8,
};
const int KonnektingDevice::_numberOfParams = sizeof (_paramSizeList); // do not change this code

void writeComObj(byte comObjId, word ga) {
  byte gaHi = (ga >> 8) & 0xff;
  byte gaLo = (ga >> 0) & 0xff;
  byte settings = 0x80;
  writeMemory(EEPROM_COMOBJECTTABLE_START + (comObjId * 3) + 0, gaHi);
  writeMemory(EEPROM_COMOBJECTTABLE_START + (comObjId * 3) + 1, gaLo);
  writeMemory(EEPROM_COMOBJECTTABLE_START + (comObjId * 3) + 2, settings);
}

/* *****************************************************************************
   S E T U P
*/
void setup() {

  SerialUSB.begin(115200);
  pinMode(PROG_LED, OUTPUT);
  digitalWrite(PROG_LED, HIGH);

  pinMode(SPK_ON, OUTPUT);
  digitalWrite(SPK_ON, LOW);

  pinMode(LED1_R, OUTPUT);
  pinMode(LED1_G, OUTPUT);
  pinMode(LED1_B, OUTPUT);
  setLED(1, 0, 0, 0);

  pinMode(LED2_R, OUTPUT);
  pinMode(LED2_G, OUTPUT);
  pinMode(LED2_B, OUTPUT);
  setLED(2, 0, 0, 0);

  long start = millis();
  while (!SerialUSB && (millis() - start < 15000)) {
    // wait for serial connection at most 5sec
  }
  digitalWrite(PROG_LED, LOW);

  /*
     make debug serial port known to debug class
     Means: KONNEKTING will use this serial port for console debugging
  */
  Debug.setPrintStream(&SerialUSB);

  Debug.println(F("DB2.1 v1.0"));

  /*
     Set memory functions
  */
  Debug.println(F("Setup Memory ..."));
  setupMemory();
  Debug.println(F("--> DONE"));

  /*
     Fake parametrization
  */
  Debug.println(F("Fake Memory Data..."));
  word individualAddress = P_ADDR(1, 1, 1);
  writeMemory(EEPROM_INDIVIDUALADDRESS_HI, (individualAddress >> 8) & 0xff);
  writeMemory(EEPROM_INDIVIDUALADDRESS_LO, (individualAddress >> 0) & 0xff);
  writeMemory(EEPROM_DEVICE_FLAGS, 0xFE);
  writeComObj(FT_OBJ, G_ADDR(15, 7, 254));
  writeComObj(1, G_ADDR(15, 7, 253));
  Debug.println(F("--> DONE"));


  /**
     Setup KONNEKTING
  */
  Debug.println(F("Setup Memory Fctptr ..."));
  Konnekting.setMemoryReadFunc(&readMemory);
  Konnekting.setMemoryWriteFunc(&writeMemory);
  Konnekting.setMemoryUpdateFunc(&updateMemory);
  Konnekting.setMemoryCommitFunc(&commitMemory);
  Debug.println(F("--> DONE"));
  Konnekting.init(KNX_SERIAL, PROG_BTN, PROG_LED, MANUFACTURER_ID, DEVICE_ID, REVISION);

  Debug.println(F("begin:"));
  analogWriteResolution(10);
  analogReference(AR_INTERNAL1V0);
  analogWrite(A0, 0);

  attachInterrupt(BUTTON1, interruptButton1, CHANGE);
  attachInterrupt(BUTTON2, interruptButton2, CHANGE);

  Debug.println(F("--> setup() DONE"));
}

int buttonState1 = 0;
int buttonState2 = 0;

void interruptButton1() {
  // read the state of the pushbutton value:
  buttonState1 = digitalRead(BUTTON1);
  Debug.println(F("b1=%i b2=%i"), buttonState1, buttonState2);
}

void interruptButton2() {
  buttonState2 = digitalRead(BUTTON2);
  Debug.println(F("b1=%i b2=%i"), buttonState1, buttonState2);
}

long last = 0;
bool state = false;

long ledDelay = millis();
int r = 0;
int g = 341;
int b = 682;

void setLED(int led, int r, int g, int b) {
  int pin_r = -1;
  int pin_g = -1;
  int pin_b = -1;

  switch (led) {
    case 1:
      pin_r = LED1_R;
      pin_g = LED1_G;
      pin_b = LED1_B;
      break;
      pin_r = LED2_R;
      pin_g = LED2_G;
      pin_b = LED2_B;
    case 2:
      break;
  }
  analogWrite(pin_r, r);
  analogWrite(pin_g, g);
  analogWrite(pin_b, b);
}

/* *****************************************************************************
   L O O P
*/
void loop() {

  Knx.task();
  // it's not ready, unless it's programmed via Suite
  if (Konnekting.isReadyForApplication()) {


    if (SerialUSB.available() > 0) {
      int val = SerialUSB.parseInt();
      r = val;
      g = val;
      b = val;
      Debug.println(F("LED: r=%03d g=%03d b=%03d"), r, g, b);
      setLED(1, r, g, b);
      setLED(2, r, g, b);
    }


        if (millis() - ledDelay > 100) {
    //      r += 1;
    //      g += 1;
    //      b += 1;
    //
    //      if (r > 1023) r = 0;
    //      if (g > 1023) g = 0;
    //      if (b > 1023) b = 0;
    //      Debug.println(F("LED: r=%03d g=%03d b=%03d"), r, g, b);
    //
    //      setLED(1, r, g, b);
    //      setLED(2, r, g, b);
    //
          //Debug.println(F("b1=%i b2=%i"), buttonState1, buttonState2);
          ledDelay = millis();
        }

    // Blink Prog-LED SLOW for testing purpose
    if (millis() - last > 1000) {

      // send state every second to bus
      Knx.write(1, state);
      //Debug.println(F("Sending %i on 15/7/254 from 1.1.1"), state);

      if (state) {
        digitalWrite(PROG_LED, HIGH);
        state = false;
      } else {
        digitalWrite(PROG_LED, LOW);
        state = true;
      }
      last = millis();
    }
  } else {
    // Blink Prog-LED FAST for testing purpose
    if (Konnekting.isProgState() && (millis() - last > 200)) {
      if (state) {
        digitalWrite(PROG_LED, HIGH);
        state = false;
      } else {
        digitalWrite(PROG_LED, LOW);
        state = true;
      }
      last = millis();
    }
  }

}

unsigned int _blockNr = 0;
unsigned int _currFileIdx;
unsigned int _currRemainingSize = -1;
SerialFlashFile _curFlashFile;

// Callback function to handle com objects updates

void knxEvents(byte index) {

  //Debug.println(F("knxEvents: i=%i"), index);

  bool debug = false;
  switch (index) {
    case FT_OBJ: // File Receive
      {
        byte buffer[14];
        Knx.read(FT_OBJ, buffer);

        if (debug) {
          for (int i = 0; i < 14; i++) {
            Debug.println(F("buffer[%d]\thex=0x%02x bin="BYTETOBINARYPATTERN), i, buffer[i], BYTETOBINARY(buffer[i]));
          }
        }

        byte cmd = buffer[0];

        //DEBUG_PRINTLN(F("cmd=0x%02x"), cmd);


        switch (cmd) {
          case 0: // erase file
            handleEraseFile(buffer);
            break;
          case 1: //write file
            handleWriteFile(buffer);
            break;
          case 2: // receive file data
            handleFileData(buffer);
            break;

          default:
            if (debug) Debug.println(F("Unsupported cmd: 0x%02x"), cmd);
            if (debug) Debug.println(F(" !!! Skipping message."));
            break;
        }
        break;
      }
    case PLAY_OBJ:
      {
        playFile();
        break;
      }
    default:
      if (debug) Debug.println(F("Unhandled comobj! index=%d"), index);
      break;

  }
}

void handleEraseFile(byte msg[]) {
  Debug.println(F("handleEraseFile"));



  // Knx.write(PROGCOMOBJ_INDEX, response);
  sendAck();
}

void handleWriteFile(byte msg[]) {
  Debug.println(F("handleWriteFile"));
  _currFileIdx = msg[1];

  _blockNr = 0;
  _currRemainingSize =
    (msg[2] << 24) +
    (msg[3] << 16) +
    (msg[4] << 8) +
    (msg[5] << 0);
  Debug.println(F("file size %d"), _currRemainingSize);

  formatSpiFlash();

  if (SerialFlash.exists("0.wav")) {
    Debug.println(F("delete file from Flash chip"));
    SerialFlash.remove("0.wav");
  }
  if (SerialFlash.create("0.wav", _currRemainingSize)) {
    _curFlashFile = SerialFlash.open("0.wav");
    Debug.println(F("created file with size %d"), _currRemainingSize);
    if (_curFlashFile) {
      sendAck();
    } else {
      Debug.println(F("error creating file on flash"));
    }

  }
}

void handleFileData(byte msg[]) {

  bool debug = false;

  if (debug) Debug.println(F("handleFileData"));

  if (debug) Debug.println(F("remaining before: %d"), _currRemainingSize);
  unsigned int toWrite = min(13, _currRemainingSize);
  if (debug) Debug.println(F("towrite: %d"), toWrite);
  byte data[13];

  // copy idx 1 to 13 from msg to data --> skip first byte
  memcpy(&data[0], &msg[1], toWrite);


  _curFlashFile.write(data, toWrite);

  _currRemainingSize -= toWrite;
  if (debug) Debug.println(F("remaining after: %d"), _currRemainingSize);

  if (_currRemainingSize == 0) {
    Debug.println(F("done. closing file"));
    _curFlashFile.close();
  }

  sendAck();
  _blockNr++;
}

void sendAck() {
  Debug.println(F("sendAck block=%d"), _blockNr);
  byte response[14];
  for (byte i = 0; i < 14; i++) {
    response[i] = 0x00;
  }

  response[1] = (_blockNr >> 24) & 0xff;
  response[2] = (_blockNr >> 16) & 0xff;
  response[3] = (_blockNr >> 8) & 0xff;
  response[4] = (_blockNr >> 0) & 0xff;

  Knx.write(FT_OBJ, response);
}

void playFile() {

  // 11025kHz stereo => 2* sample rate

  digitalWrite(SPK_ON, HIGH);
  Debug.println(F("open:"));
  SerialFlashFile ff = SerialFlash.open("0.wav");
  Debug.println(F("begin:"));
  AudioZero.begin();
  Debug.println(F("prepare:"));
  AudioZero.prepare(ff);
  Debug.println(F("playing:"));
  unsigned long start = millis();
  AudioZero.setVolume(1.0);
  AudioZero.play();
  unsigned long end = millis();

  Debug.println(F("end:"));
  AudioZero.end();
  Debug.println(F("*done*, played %dms"), (end - start));
  digitalWrite(SPK_ON, LOW);
}



