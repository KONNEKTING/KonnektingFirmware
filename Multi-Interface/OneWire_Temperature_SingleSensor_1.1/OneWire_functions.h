void configureOW(){
#ifdef KDEBUG
    Debug.println(F("Searching for sensors..."));  
#endif  
    sensors.begin();
    sensors.setResolution(TEMPERATURE_PRECISION);  
    sensors.setWaitForConversion(false); //use it with care!!
    owDelay = 750 / (1 << (12 - TEMPERATURE_PRECISION));
    if(sensors.getDeviceCount() > 0){
        sensorFound = true;
    }
#ifdef KDEBUG
    Debug.println(F("Found %d sensor(s)"),sensors.getDeviceCount());  
    Debug.println(F("Resolution: %d"),sensors.getResolution());
#endif    
}

float getCurrentTemp(uint8_t index){
    float currTemp = -127;
    long start = millis();
    currTemp = sensors.getTempCByIndex(index);
    long end = millis();
#ifdef KDEBUG              
    Debug.println(F("currentTemp: %3.2f °C time: %d ms"), currTemp, 0x00B0, (end-start));
#endif
return currTemp;
}

void limitReached(float comVal, float comValMin, float comValMax, int minObj, int maxObj, int minVal, int maxVal) {
    if (minVal != 255) {
        if (comVal <= comValMin) {
            Knx.write(minObj, minVal);
        }
    }
    if (maxVal != 255) {
        if (comVal >= comValMax) {
            Knx.write(maxObj, maxVal);
        }
    }
};
