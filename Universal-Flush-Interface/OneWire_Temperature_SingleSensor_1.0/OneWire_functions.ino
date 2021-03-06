void configureOW(){
    pinMode(ONE_WIRE_PULLUP, OUTPUT);
    digitalWrite(ONE_WIRE_PULLUP, HIGH); //enable OneWire PullUp
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
    char currTempChar[10];
    dtostrf(currTemp,6,2,currTempChar);
    Debug.println(F("currentTemp: %s time: %d ms"), currTempChar, (end-start));
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

