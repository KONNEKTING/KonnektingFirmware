/* ---------------------------------------------------------------------------------------------
OneWire Temperature Sensor (DS18B20, MAX31820)
Hardware/Firmware/Sketch/kdevice.xml by E.Burkowski / e.burkowski@konnekting.de, GPL Licensed
Compatible with: KONNEKTING Device Library 1.0.0 beta 3
------------------------------------------------------------------------------------------------*/

// comment following line to disable DEBUG mode
#define DEBUG debugSerial

// no need to comment, you can leave it as it is as long you do not change the "#define DEBUG debugSerial" line
#ifdef DEBUG
#include <SoftwareSerial.h>
SoftwareSerial debugSerial(10, 11); // RX, TX
#endif

#include "OneWire_Temperature.h"
#include "ufi_rev1.h"


// include KnxDevice library
#include <KnxDevice.h>
#include <OneWire.h>
#include <DallasTemperature.h> //https://github.com/adafruit/MAX31850_OneWire

OneWire oneWire(ONE_WIRE); 
DallasTemperature sensors(&oneWire);/* Dallas Temperature Library */

unsigned long previousTimeTemp = 0;
unsigned long previousTimeHumd = 0;

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









