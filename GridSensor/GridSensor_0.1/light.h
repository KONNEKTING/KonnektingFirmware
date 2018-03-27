#define CYCLE_LUX 10000 //5s
unsigned long previousLuxTime = 0;

#ifdef OPT3001
#include "src/OPT3001/Opt3001.h"

Opt3001 opt;

void initLight(){
    opt.begin();
}

void taskLight(){
    if ((millis() - previousLuxTime) >= CYCLE_LUX){
        lux = (int)opt.readResult();
        previousLuxTime = millis();
        Debug.println(F("Light: %d lux "), lux);  
    }
}

int getLux(){
    return lux += offsetLux;
}
#endif

#ifdef TSL2561
#include "src/TSL2561/Adafruit_Sensor.h"
#include "src/TSL2561/Adafruit_TSL2561_U.h"

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_LOW, 11111);

void initLight(){
    tsl.begin();
    tsl.enableAutoRange(true);
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
}

void taskLight(){
      if ((millis() - previousLuxTime) >= CYCLE_LUX){
        sensors_event_t event;
        tsl.getEvent(&event);
        lux = event.light;
        previousLuxTime = millis();
        Debug.println(F("Light: %d lux "), lux);  
    }
}

int getLux(){
    return lux += luxOffset;
}
#endif
