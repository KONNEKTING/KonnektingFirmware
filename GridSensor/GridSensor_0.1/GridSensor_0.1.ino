/*----------
  Grid Sensor 1.0

Sensors that can be used and i2c address:
T/RH:
HDC1080 0x40
HUT21D  0x40
VOC:
CCS811  0x5A 
AMS iAq Core  0x5A
Light: 
TSL2561 0x29
OPT3001 0x44
  -----------*/
#define ADDRESS_HDC1080 0x40
#define ADDRESS_HUT21D  0x40
#define ADDRESS_CCS811  0x5A 
#define ADDRESS_IAQ     0x5A
#define ADDRESS_TSL2561 0x29
#define ADDRESS_OPT3001 0x44


//debug
//#define KDEBUG

//define used sensors
//#define HDC1080 //Temp + RH
#define HTU21 //Temp + RH 
//#define TSL2561 //Light
#define OPT3001 //Light

//DOSN'T WORK YET:
//#define ANALOG_T_RH //Temp TMP36 + RH HIH4030/5030
//#define VOC_CCS811 //VOC
//#define VOC_IAQ //VOC
//#define ONEWIRE
//#define BINARY
//#define I2C_EEPROM


#define ERROR_TRH -127.00
#define ERROR_LUX -1
#define ERROR_VOC -1
#define ERROR_TVOC -1
#define MEASURE_CYCLE 30000 //30 sec
#define INPUTS 2


//global varibales

unsigned long lastmillis = 0;
float t = ERROR_TRH;
float t_last = ERROR_TRH;
float rh = ERROR_TRH;
float rh_last = ERROR_TRH;
float owt = ERROR_TRH;
float owt_last = ERROR_TRH;
int lux = ERROR_LUX;
int lux_last = ERROR_LUX;
int voc = ERROR_VOC;
int voc_last = ERROR_VOC;
int tvoc = ERROR_TVOC;
int tvoc_last = ERROR_TVOC;

//temperature
byte typeTemp;
long intervalTempUser;
float diffTempUser;
float offsetTemp = 0.0;
byte valueTempMin;
float limitTempMin;
bool lastStateTempMinMax = false;
byte valueTempMax;
float limitTempMax;

//humidity
byte typeHumd;
long intervalHumdUser;
float diffHumdUser;
float offsetHumd = 0.0;
byte valueHumdMin;
float limitHumdMin;
bool lastStateHumdMinMax = false;
byte valueHumdMax;
float limitHumdMax;

//light
int offsetLux = 0;

//OneWire temperature
byte typeOwTemp;
long intervalOwTempUser;
float diffOwTempUser;
float offsetOwTemp = 0.0;
byte valuOwTempMin;
float limitOwTempMin;
bool lastStateOwTempMinMax = false;
byte valueOwTempMax;
float limitOwTempMax;

//button
int typeBtn = 0;
int longClickDurationBtn = 450;
int valueShortClickBtn;
int valueLongClickBtn;
int valueShortClick = 255;
int valueLongClick = 255;
bool boolButtonPressed = false;
bool boolShortClick = false; //Dimmer: false => off, true => on
bool boolLongClick = false;
//int dimDirection = true; //false => off, true => on

//rgb
byte typeRgb = 0;
byte brightnessTypeRgb = 0;
byte colorOnClickRgb = 2;
byte colorStateShortRgb = 1;
byte colorStateLongRgb = 0;
bool alarmRgb = false;

#include <KonnektingDevice.h>
#include "kdelay.h"
#include "kdevice_Gridsensor.h"
#include "gridsensor.h"
#include "rgb.h"
#include "button.h"
#include "binary.h"
#include "onewire.h"

#if defined(HDC1080) || defined(HTU21) || defined(ANALOG_T_RH)
#include "temp_rh.h"
#define TRH
#endif
#if defined(OPT3001) || defined(TSL2561)
#include "light.h"
#define LIGHT
#endif
#if defined(VOC_CCS811) || defined(VOC_IAQ)
#include "voc.h"
#define VOC
#endif

#ifdef KDEBUG
void printConfig(){
    Debug.println(F("Configuration temperature:"));
    Debug.println(F("typeTemp: %d"),typeTemp);
    Debug.println(F("intervalTempUser: %d"),intervalTempUser);
    Debug.println(F("diffTempUser: %3.2f"),diffTempUser);
    Debug.println(F("offsetTemp: %3.2f"),offsetTemp);
    Debug.println(F("valueTempMin: %d"),valueTempMin);
    Debug.println(F("limitTempMin: %3.2f"),limitTempMin);
    Debug.println(F("valueTempMax: %d"),valueTempMax);
    Debug.println(F("limitTempMax: %3.2f"),limitTempMax);
    Debug.println(F("Configuration humidity:"));
    Debug.println(F("typeHumd: %d"),typeHumd);
    Debug.println(F("intervalHumdUser: %d"),intervalHumdUser);
    Debug.println(F("diffHumdUser: %3.2f"),diffHumdUser);
    Debug.println(F("offsetHumd: %3.2f"),offsetHumd);
    Debug.println(F("valueHumdMin: %d"),valueHumdMin);
    Debug.println(F("limitHumdMin: %3.2f"),limitHumdMin);
    Debug.println(F("valueHumdMax: %d"),valueHumdMax);
    Debug.println(F("limitHumdMax: %3.2f"),limitHumdMax);
    Debug.println(F("Configuration button:"));
    Debug.println(F("typeBtn: %d"),typeBtn);
    Debug.println(F("longClickDurationBtn: %d"),longClickDurationBtn);
    Debug.println(F("valueShortClickBtn: %d"),valueShortClickBtn);
    Debug.println(F("valueLongClickBtn: %d"),valueLongClickBtn);
#ifdef BINARY  
    for (byte i= 0;i<INPUTS;i++){
        Debug.println(F("Input: %d  active: %d  invert: %d  delay: %d ms"),i,inputActive[i],invertOutput[i],outputDelay[i]);
    }
#endif
}
#endif


#include "setup.h"
#include "loop.h"


void knxEvents(byte index) {
    switch (index) {
        case COMOBJ_tempInputValue:
            lastStateTempMinMax = Knx.read(COMOBJ_tempInputValue);
            break;
        case COMOBJ_rhInputValue:
            lastStateHumdMinMax = Knx.read(COMOBJ_rhInputValue);
            break;
/*        case COMOBJ_tempSwitch:
            lastStateTempMin = Knx.read(COMOBJ_tempSwitch);
            break;
        case COMOBJ_tempSwitch:
            lastStateTempMin = Knx.read(COMOBJ_tempSwitch);
            break;*/
        case COMOBJ_btnShortInput:
            boolShortClick = Knx.read(COMOBJ_btnShortInput);
            Debug.println(F("boolShortClick: %d"),boolShortClick);
//          stateShortLED(boolShortClick);
            break;
        case COMOBJ_btnLongInput:
            boolLongClick = Knx.read(COMOBJ_btnLongInput);
            Debug.println(F("boolLongClick: %d"),boolLongClick);
//          stateLongLED(boolLongClick);
            break;
        default:
          break;
    }

};

#if (defined(HTU21) && defined(HDC1080)) || (defined(HTU21) && defined(ANALOG_T_RH)) || (defined(HDC1080) && defined(ANALOG_T_RH))
#error "Please define only one of this sensors: HTU21, HDC1080 or ANALOG_T_RH"
#endif
#if (defined(TSL2561) && defined(OPT3001))
#error "Please define only one of this sensors: TSL2561 or OPT3001"
#endif
#if (defined(VOC_CCS811) && defined(VOC_IAQ))
#error "Please define only one of this sensors: VOC_CCS811 or VOC_IAQ"
#endif
