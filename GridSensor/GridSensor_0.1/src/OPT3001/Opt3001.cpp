/*
  OPT3001.cpp
  Created by Adrian Fernandez & Dung Dang, Dec 2013
  
  Released into the public domain.

  Modified and adapted by Pontus Oldberg for the EnvironOne Node.IT board.
*/

#include <Arduino.h>
#include "Opt3001.h"
#define slaveAdr 0x44

#define RESULT_REG 0x00
#define CONFIG_REG 0x01
#define LOWLIMIT_REG 0x02
#define HIGHLIMIT_REG 0x03
#define MANUFACTUREID_REG 0x7E
#define DEVICEID_REG 0x7F

void Opt3001::writeRegister(uint8_t registerName, uint16_t value)
{
	/* Begin Transmission at address of device on bus */
	Wire.beginTransmission(slaveAdr);

	/* Send Pointer Register Byte */
	Wire.write(registerName);

	/* Read*/
	Wire.write((uint8_t)(value >> 8));
	Wire.write((uint8_t)(value & 0x00FF));

	/* Sends Stop */
	Wire.endTransmission();
	return;    
}

uint16_t Opt3001::readRegister(uint8_t registerName)
{
	uint8_t lsb;
	uint8_t msb;

	/* Begin Transmission at address of device on bus */
	Wire.beginTransmission(slaveAdr);
	Wire.write(registerName);
	Wire.endTransmission();

	/* Requests 2 bytes from Slave */
	Wire.requestFrom(slaveAdr, 2);
	while(Wire.available() < 2)	{}

	/* Get data from sensor */
	msb = Wire.read();
	lsb = Wire.read();

	return (msb << 8) | lsb;
}

void Opt3001::begin(uint16_t config)
{
    writeRegister(CONFIG_REG, config);
}

void Opt3001::begin()
{
    begin(DEFAULT_CONFIG_800);
}

uint16_t Opt3001::readManufacturerId()
{

	return readRegister(MANUFACTUREID_REG);
	
}

uint16_t Opt3001::readDeviceId()
{
	return readRegister(DEVICEID_REG);
		
}

uint16_t Opt3001::readConfigReg()
{
	return readRegister(CONFIG_REG);
}

uint16_t Opt3001::readLowLimitReg()
{
	return readRegister(LOWLIMIT_REG);
	
	
}

uint16_t Opt3001::readHighLimitReg()
{
	return readRegister(HIGHLIMIT_REG);
}


float Opt3001::readResult()
{
	/* Read raw data from the hw register */
	uint16_t raw = readRegister(RESULT_REG);
	
	//uint16_t result;
	//uint16_t exponent;
	
	/* Convert to LUX */
	//result = raw & 0x0fff;
	//exponent = (raw & 0xf000) >> 12;
	
	//return (result << exponent) * 0.01;
	
	return ((raw & 0x0fff) << ((raw & 0xf000) >> 12)) * 0.01;
}

#if defined(OPT_INTERRUPT_PIN)
uint8_t Opt3001::interruptPin()
{
	return (digitalRead(OPT_INTERRUPT_PIN) == 0 ? 1 : 0);
}
#endif

boolean Opt3001::isConversionReady()
{
    return readConfigReg() & OPT3001_CFG_CRF ? true : false;
}

void Opt3001::startConversion()
{
    begin(DEFAULT_CONFIG_100_OS);
}

void Opt3001::startConversion(uint16_t startArg)
{
    begin(startArg);
}

void Opt3001::shutDown()
{
    begin(DEFAULT_CONFIG_SHDWN);
}

// EOF
