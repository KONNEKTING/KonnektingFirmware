#ifdef ONEWIRE
#include "src/OneWire/OneWire.h"
#include "src/OneWire/DallasTemperature.h"


#define CYCLE_OW 10000 //10s
#define TEMPERATURE_PRECISION 10 //9: 0.5°, 10: 0.25°, 11: 0.125°, 12: 0.0625°
OneWire oneWire(PIN_OW); 
DallasTemperature ow(&oneWire);

unsigned long owDelay = 750;
unsigned long tempRequestedTime = 0;
unsigned long previousOwTime = 0;
bool tempRequested = false;
bool sensorFound = false;

void initOneWire(){
    ow.begin();
    ow.setResolution(TEMPERATURE_PRECISION);  
    ow.setWaitForConversion(false); //use it with care!!
    owDelay = 750 / (1 << (12 - TEMPERATURE_PRECISION));
    if(ow.getDeviceCount() > 0){
        sensorFound = true;
    }
}

float getOneWireTemp(){
    if (-55.0 <= owt && owt <= 125.0)
        return owt += offsetOwTemp;
    else
        return ERROR_TRH;
}

void taskOneWire(){
    if(!tempRequested){
        if ((millis() - tempRequestedTime) >= CYCLE_OW){
            if(sensorFound){
                //request tempeatures
                tempRequestedTime = millis();
                ow.requestTemperaturesByIndex(0);
                tempRequested = true;
#ifdef KDEBUG
                Debug.println(F("OneWire Temperature requested"));
#endif 
            }else{
#ifdef KDEBUG
                Debug.println(F("No OneWire sensors found! Try one more time..."));
#endif
                initOneWire();
            }
        }
        
    }
    if((millis() - tempRequestedTime >= owDelay) && tempRequested){
#ifdef KDEBUG              
		Debug.println(F("Temp.request delay %d ms is ready."), owDelay);
#endif      
		    owt = ow.getTempCByIndex(0);
		}else{
#ifdef KDEBUG              
			  Debug.println(F("Something gone wrong... trying to reset OneWire..."));
#endif
			  initOneWire(); 
		}
	  tempRequested = false;
    Debug.println(F("OneWire Temp: %3.2f °C "), owt);
}

#endif
