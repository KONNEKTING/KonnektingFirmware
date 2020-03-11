/* ---------------------------------------------------------------------------------------------
OneWire Temperature SingleSensor 1.1 (DS18B20, MAX31820) (Single sensor)
Hardware/Firmware/Sketch/kdevice.xml by E.Burkowski / e.burkowski@konnekting.de, GPL Licensed
Compatible with: KONNEKTING Device Library 1.0.0 beta 4b

------------------------------------------------------------------------------------------------*/

/* this libraries are required (click on link and download with library manager):
KONNEKTING Device Library:  http://librarymanager/All#Konnekting
FlashStorage:               http://librarymanager/All#FlashStorage
OneWire:                    http://librarymanager/All#OneWire
DallasTemperature:          http://librarymanager/All#DallasTemperature
*/

#include <KonnektingDevice.h>

// include device related configuration code, created by "KONNEKTING CodeGenerator"
#include "kdevice_OneWire_Temeprature_SingleSensor_1_1.h"
#include "mi.h"

#include <FlashAsEEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>


// ################################################
// ### DEBUG CONFIGURATION
// ################################################
//#define KDEBUG // comment this line to disable DEBUG mode
#ifdef KDEBUG
#include <DebugUtil.h>
#define DEBUGSERIAL SerialUSB
#endif

// ################################################
// ### IO Configuration
// ################################################
//OneWire on pin SDA (pin with approx 4k7 should be used)
#define ONE_WIRE_PIN SDA

#define TEMPERATURE_PRECISION 10 //9: 0.5°, 10: 0.25°, 11: 0.125°, 12: 0.0625°


OneWire oneWire(ONE_WIRE_PIN); 
DallasTemperature sensors(&oneWire);/* Dallas Temperature Library */

unsigned long owDelay = 750;
unsigned long previousTimeTemp = 0;
unsigned long previousTimeHumd = 0;
unsigned long tempRequestedTime = 0;
bool tempRequested = false;
bool sensorFound = false;

float previousTemp = 0;
float previousHumd = 0;
float currentTemp = 0;
float currentHumd = 0;

int typeTemp = 0;
long intervalTempUser = 5; //typical temperatur polling interval (ms)
float diffTempUser = 0.2; //minimal difference between previous and current temperature [°C]
uint8_t valueTempMin = 255;
int16_t limitTempMin = 0;
uint8_t valueTempMax = 255;
int16_t limitTempMax = 0;

#include "OneWire_functions.h"
#include "setup.h"
#include "loop.h"

// Callback function to handle com objects updates

void knxEvents(byte index) {
    // nothing to do in this sketch
};
