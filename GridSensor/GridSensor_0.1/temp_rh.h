#define CYCLE_TRH 10000 //10s
unsigned long previousTrhTime = 0;

#ifdef HDC1080
#include "src/HDC1080/ClosedCube_HDC1080.h"

ClosedCube_HDC1080 hdc;

void initTempRH(){
    hdc.begin(ADDRESS_HDC1080);
}

void taskTRH(){
    if ((millis() - previousTrhTime) >= CYCLE_TRH){
        previousTrhTime = millis();
        t = hdc.readTemperature();
        rh = hdc.readHumidity();
        Debug.println(F("Temp: %3.2f °C, RH: %3.2f %% "), t, rh); 
    }
}

float getTemp(){
    if (-45.0 <= t && t <= 127.0)
        return t += offsetTemp;
    else
        return ERROR_TRH;
}

float getRH(){
    if (0.0 <= rh && rh <= 100.0)
        return rh += offsetHumd;
    else
        return ERROR_TRH;
}
#endif


#ifdef HTU21
#include "src/HTU21D/SparkFunHTU21D.h"
HTU21D htu;

void initTempRH(){
    htu.begin();
}

void taskTRH(){
    if ((millis() - previousTrhTime) >= CYCLE_TRH){
        previousTrhTime = millis();
        t = htu.readTemperature();
        rh = htu.readHumidity();
        Debug.println(F("Temp: %3.2f °C, RH: %3.2f %% "), t, rh); 
    }
}

float getTemp(){
    if (-40.0 <= t && t <= 125.0)
        return t += offsetTemp;
    else
        return ERROR_TRH;
}

float getRH(){
    if (0.0 <= rh && rh <= 100.0)
        return rh += offsetHumd;
    else
        return ERROR_TRH;
}

#endif

#ifdef ANALOG_T_RH

#define SAMPLES 3
#define SAMPLE_DELAY 1
#define ANALOG_RESOLUTION 10
#define MAX_VALUE pow(2,ANALOG_RESOLUTION)-1
#define VOLTAGE 3.33
#define VOLTAGE_RH 3.99

void initTempRH(){
    analogReadResolution(12);
    pinMode(PIN_AN_T,INPUT);
    pinMode(PIN_AN_RH,INPUT);
}


float getTemp(){
    if (-55.0 <= t && t <= 150.0)
        return t += offsetTemp;
    else
        return ERROR_TRH;
}
float getRh(){
    if (0.0 <= rh && rh <= 100.0)
        return t += offsetTemp;
    else
        return ERROR_TRH;
}

float getATemp(){
    int tread = 0;
    float t = 0.0;
    for (int i = 0; i<SAMPLES;i++){
        tread += analogRead(PIN_AN_T);
        Debug.println(F("tread: %d"),tread);
        delay(SAMPLE_DELAY);
    }
    tread = (int)(tread  / SAMPLES) * 100;
    Debug.println(F("tread: %d"),tread);
    t = (float)map(tread, 0,MAX_VALUE*100,0,(VOLTAGE*100-50))/1000;  //(voltage - 0.5V offset) * 100 => °C
    return t;
}

float getARH(){
    int rhread = 0;
    float rhv = 0.0;
    float rh = 0.0;
    for (int i = 0; i<SAMPLES;i++){
        rhread += analogRead(PIN_AN_RH);
        delay(SAMPLE_DELAY);
    }
    rhread = (int)(rhread * 100 / SAMPLES);
    rhv = (float)map(rhread, 0,MAX_VALUE*100,0,VOLTAGE*100)/10000;
    Debug.println(F("rhread: %d"),rhread);
    //rh = ((rhv / (.0062 * VOLTAGE_RH)) - 25.81);
    rh = (161.29 * rhv / VOLTAGE_RH - 25.8065)/(1.0546 - 0.00216 * getTemp()); //with temperature compensation
    return rh;
}

void taskTRH(){
    if ((millis() - previousTrhTime) >= CYCLE_TRH){
        previousTrhTime = millis();
        t = getATemp();
        rh = getATemp();
        Debug.println(F("Analog Temp: %3.2f °C, Analog RH: %3.2f %% "), t, rh);
    }
}

#endif
