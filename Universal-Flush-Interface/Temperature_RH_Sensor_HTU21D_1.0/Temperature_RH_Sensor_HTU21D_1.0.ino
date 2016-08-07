/* ---------------------------------------------------------------------------------------------
Temperature and relative humidty Sensor (HTU21D, SHT21...)
Hardware/Firmware/Sketch/kdevice.xml by E.Burkowski / e.burkowski@konnekting.de, GPL Licensed
Compatible with: KONNEKTING Device Library 1.0.0 beta 3
------------------------------------------------------------------------------------------------*/

// comment following line to disable DEBUG mode
#define DEBUG debugSerial

// no need to comment, you can leave it as it is as long you do not change the "#define DEBUG debugSerial" line
#ifdef DEBUG
#include <SoftwareSerial.h>
SoftwareSerial debugSerial(11, 10); // RX, TX
#endif

#include "Temperature_RH_Sensor_HTU21D.h"
#include "ufi_rev1.h"

// include KnxDevice library
#include <KnxDevice.h>
#include <Wire.h>
#include "SparkFunHTU21D.h" //https://github.com/sparkfun/SparkFun_HTU21D_Breakout_Arduino_Library

// Create an instance for HTU21D sensor
HTU21D htu;

unsigned long previousTimeTemp = 0;
unsigned long previousTimeHumd = 0;

float previousTemp = 0;
float previousHumd = 0;
float currentTemp = 0;
float currentHumd = 0;

int typeTemp;
long intervalTempUser; //typical temperatur polling interval (ms)
float diffTempUser; //minimal difference between previous and current temperature [°C]
uint8_t valueTempMin;
int16_t limitTempMin;
uint8_t valueTempMax;
int16_t limitTempMax;

uint8_t typeHumd;
long intervalHumdUser; //typical temperatur polling interval (ms)
float diffHumdUser;
uint8_t valueHumdMin;
int16_t limitHumdMin;
uint8_t valueHumdMax;
int16_t limitHumdMax;
