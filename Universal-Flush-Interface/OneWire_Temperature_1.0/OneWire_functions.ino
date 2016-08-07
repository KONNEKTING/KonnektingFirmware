void configure_OW(){
    pinMode(ONE_WIRE_PULLUP, OUTPUT);
    digitalWrite(ONE_WIRE_PULLUP, HIGH); //enable OneWire PullUp
    sensors.begin();
    sensors.setResolution(10); // 9-Bit resolution for DS18B20: 9: 0.5° 94ms, 10: 0.25° 188ms, 11: 0.125° 375ms, 12: 0.0625° 750ms
#ifdef DEBUG  
            DEBUG.print("Resolution: ");
            DEBUG.println(sensors.getResolution());
#endif    
}

float getCurrentTemp(uint8_t index){
    float currTemp = 0;
           long start = micros();
           sensors.requestTemperatures();
           currTemp = sensors.getTempCByIndex(index);
           long endtime = micros();
#ifdef DEBUG 
            DEBUG.print("currentTemp: ");
            DEBUG.println(currTemp);
            DEBUG.print("time: ");
            DEBUG.println((endtime-start));
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

