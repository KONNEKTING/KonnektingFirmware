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

#include "AudioZeroSPI.h"
#include "wav.h"
#include <SerialFlash.h>

#define NEUTRAL_SOUND 512 // = (2^10 / 2)
#define NUMBER_OF_SAMPLES 1024 // Number of samples to read in block

/* Global variables */
volatile bool __StartFlag; // Is started?
volatile uint32_t __SampleIndex; // current played sample
uint32_t __HeadIndex; // current start of buffer
volatile uint32_t __StopAtIndex; // Sample where to stop writing buffer into DAC

int16_t *__WavSamples; // buffer
riff_header *__RIFFHeader; // RIFF header
wav_header *__WavHeader; // Wav header
data_header *__DataHeader; // Wav header
uint32_t __SamplesPending;
float __Volume = 0.5;

SerialFlashFile __toPlay;
bool __Configured = false;

void AudioZeroClass::begin() {
    __StartFlag = false;
    __SampleIndex = 0;			//in order to start from the beginning
    __StopAtIndex = -1;
    __SamplesPending = 0;

    /*Allocate the buffer where the samples are stored*/
    __WavSamples = (int16_t *) malloc(NUMBER_OF_SAMPLES * sizeof(int16_t));
    __RIFFHeader = (riff_header *) malloc(RIFF_HEADER_SIZE);
    __WavHeader = (wav_header *) malloc(WAV_HEADER_SIZE);
    __DataHeader = (data_header *) malloc(DATA_HEADER_SIZE);

    /*Modules configuration */
    dacConfigure();


}

void AudioZeroClass::end() {
    
    if (__Configured) {
        tcDisable();
        tcReset();
    }
    for (int i=NEUTRAL_SOUND;i>0;i--) {
        analogWrite(A0, i);    
        delayMicroseconds(50);
    }
    analogWrite(A0, 0);
    free(__WavSamples);
    free(__RIFFHeader);
    free(__WavHeader);
    free(__DataHeader);
}

void AudioZeroClass::setVolume(float v) {
    __Volume = v;
}


int AudioZeroClass::prepare(SerialFlashFile toPlay){
    __StartFlag = false; // to stop writing from the buffer
    __SampleIndex = 0;	//in order to start from the beginning
    __StopAtIndex = -1;
    __SamplesPending = 0;
    __toPlay = toPlay;


    // Read header + first buffer
    int to_read = 0;
    int available = __toPlay.available();

    if (available > 0) {
        // READ RIFF Header
        __toPlay.read(__RIFFHeader, RIFF_HEADER_SIZE);

        // Read WAV Header
        __toPlay.read(__WavHeader, WAV_HEADER_SIZE);

        tcConfigure(__WavHeader->sample_rate);
        __toPlay.seek(RIFF_HEADER_SIZE + __WavHeader->data_header_length + 8);

        __toPlay.read(__DataHeader, DATA_HEADER_SIZE);
        __SamplesPending = __DataHeader->data_length / 2;

        to_read = NUMBER_OF_SAMPLES;
        __toPlay.read(__WavSamples, to_read  * sizeof(int16_t));
        __HeadIndex = 0;
        __SamplesPending -= to_read;
        tcStartCounter(); //start the interruptions, it will do nothing until __StartFlag is true
        return 0;
    } else {
        return -1; // File is empty
    }
}

void AudioZeroClass::play() {
    int to_read = 0;
    int available = 0;
    __StartFlag = true;
    while ((available = __toPlay.available()) && __SamplesPending > 0) {
        uint32_t current__SampleIndex = __SampleIndex;

        if (current__SampleIndex > __HeadIndex) {
            to_read = min(current__SampleIndex - __HeadIndex, __SamplesPending);
            if (to_read > 0) { // First time this will be 0
                if (to_read > available) {
                    // If we have less bytes to read, read the missing and stop
                    to_read = available;
                }
                __toPlay.read(&__WavSamples[__HeadIndex], to_read * sizeof(int16_t));
                __SamplesPending -= to_read;
                __StopAtIndex = to_read + __HeadIndex-1;
                __HeadIndex = current__SampleIndex;
            }
        } else if (current__SampleIndex < __HeadIndex) {
            to_read = min(NUMBER_OF_SAMPLES-1 - __HeadIndex, __SamplesPending);
            if (to_read > 0) {
                if (to_read > available) {
                    // If we have less bytes to read, read the missing and stop
                    to_read = available;
                    __StopAtIndex = to_read + __HeadIndex - 1;
                } else {
                    // If not, we have less bytes available
                    available -= to_read;
                }
                __toPlay.read(&__WavSamples[__HeadIndex], to_read  * sizeof(int16_t));
                __SamplesPending -= to_read;
                if (available > 0) {
                    to_read = min(current__SampleIndex, __SamplesPending);
                    if (to_read > 0) {
                        if (to_read > available) {
                            // If we have less bytes to read, read the missing and stop
                            to_read = available;
                        }
                        __toPlay.read(__WavSamples, to_read  * sizeof(int16_t));
                        __SamplesPending -= to_read;
                        __StopAtIndex = to_read - 1;
                    }
                }
                __HeadIndex = current__SampleIndex;
            }
        }
    }
    // Serial.println(__SampleIndex);
    // Serial.println(__StopAtIndex);
    while (__SampleIndex != __StopAtIndex) {
        delay(1);
    }
}


/**
 * Configures the DAC in event triggered mode.
 *
 * Configures the DAC to use the module's default configuration, with output
 * channel mode configured for event triggered conversions.
 */
void AudioZeroClass::dacConfigure(void) {
    analogWriteResolution(10);
    for (int i=0;i<NEUTRAL_SOUND;i++) {
        analogWrite(A0, i);    
        delayMicroseconds(50);
    }
    analogWrite(A0, NEUTRAL_SOUND);
}

/**
 * Configures the TC to generate output events at the sample frequency.
 *
 * Configures the TC in Frequency Generation mode, with an event output once
 * each time the audio sample frequency period expires.
 */
 void AudioZeroClass::tcConfigure(uint32_t sampleRate) {
     __Configured = true;
    // Enable GCLK for TCC2 and TC5 (timer counter input clock)
    GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TC4_TC5)) ;
    while (GCLK->STATUS.bit.SYNCBUSY);

    tcReset();

    // Set Timer counter Mode to 16 bits
    TC5->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;

    // Set TC5 mode as match frequency
    TC5->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;

    TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_ENABLE;

    TC5->COUNT16.CC[0].reg = (uint16_t) (SystemCoreClock / sampleRate - 1);
    while (tcIsSyncing());

    // Configure interrupt request
    NVIC_DisableIRQ(TC5_IRQn);
    NVIC_ClearPendingIRQ(TC5_IRQn);
    NVIC_SetPriority(TC5_IRQn, 0);
    NVIC_EnableIRQ(TC5_IRQn);

    // Enable the TC5 interrupt request
    TC5->COUNT16.INTENSET.bit.MC0 = 1;
    while (tcIsSyncing());
}


bool AudioZeroClass::tcIsSyncing() {
    return TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY;
}

void AudioZeroClass::tcStartCounter(){
    // Enable TC
    TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
    while (tcIsSyncing());
}

void AudioZeroClass::tcReset() {
    // Reset TCx
    TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
    while (tcIsSyncing());
    while (TC5->COUNT16.CTRLA.bit.SWRST);
}

void AudioZeroClass::tcDisable() {
    // Disable TC5
    TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
    while (tcIsSyncing());
}

AudioZeroClass AudioZero;

#ifdef __cplusplus
extern "C" {
#endif
    
void Audio_Handler (void) {
    // Write next sample
    if ((__SampleIndex != __StopAtIndex) && __StartFlag != false) {
        analogWrite(A0, ((__WavSamples[__SampleIndex++] >> 6) + 512)*__Volume);
    } else {
        analogWrite(A0, NEUTRAL_SOUND);
    }
    // Clear interrupt
    TC5->COUNT16.INTFLAG.bit.MC0 = 1;
    // If it was the last sample in the buffer, start again
    if (__SampleIndex == NUMBER_OF_SAMPLES - 1) {
        __SampleIndex = 0;
    }
}

void TC5_Handler (void) __attribute__ ((weak, alias("Audio_Handler")));

#ifdef __cplusplus
}
#endif