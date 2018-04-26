/*
 * Copyright (c) 2015 by
 Arturo Guadalupi <a.guadalupi@arduino.cc>
 Angelo Scialabba <a.scialabba@arduino.cc>
 Claudio Indellicati <c.indellicati@arduino.cc> <bitron.it@gmail.com>
 * Audio library for Arduino Zero.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef AUDIOZEROSPI_H
#define AUDIOZEROSPI_H

#include <Arduino.h>
//#include <SdFat.h>

#include <SerialFlash.h>
// #include "Print.h"

class AudioZeroClass{
public:

	AudioZeroClass() {};
	void begin();
        void setVolume(float v);
	int prepare(SerialFlashFile myFile);
	void play() ;
	void end();

private:
	void dacConfigure(void);
	void tcConfigure(uint32_t sampleRate);
	bool tcIsSyncing(void);
	void tcStartCounter(void);
	void tcReset(void);
	void tcEnable(void);
	void tcDisable(void);
};

extern AudioZeroClass AudioZero;
#endif