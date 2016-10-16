/* ---------------------------------------------------------------------------------------------
OneWire Temperature SingleSensor 1.0 (DS18B20, MAX31820) (Single sensor)
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

#include <KonnektingDevice.h>

// include device related configuration code, created by "KONNEKTING CodeGenerator"
#include "kdevice_OneWire_Temeprature_SingleSensor_1_0.h"
#include "ufi_rev1.h"

#include <OneWire.h> //https://github.com/PaulStoffregen/OneWire
#include <DallasTemperature.h> //https://github.com/milesburton/Arduino-Temperature-Control-Library


// ################################################
// ### DEBUG CONFIGURATION
// ################################################
//#define KDEBUG // comment this line to disable DEBUG mode
#ifdef KDEBUG
#include <DebugUtil.h>
#define DEBUGSERIAL Serial
#endif

// ################################################
// ### IO Configuration
// ################################################
//Define OneWire connection. Please check ufi_rev1.h
//#define ONE_WIRE OWLeft 
//#define ONE_WIRE_PULLUP OWLeftPullUp

#define ONE_WIRE OWRight 
#define ONE_WIRE_PULLUP OWRightPullUp

#define TEMPERATURE_PRECISION 10 //9: 0.5°, 10: 0.25°, 11: 0.125°, 12: 0.0625°


OneWire oneWire(ONE_WIRE); 
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

// Callback function to handle com objects updates

void knxEvents(byte index) {
    // nothing to do in this sketch
};
