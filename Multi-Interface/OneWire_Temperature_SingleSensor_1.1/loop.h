void loop() {
    Knx.task();
    unsigned long currentTime = millis();
    
    // only do measurements and other sketch related stuff if not in programming mode & not factory settings
    if (Konnekting.isReadyForApplication()) {
        if ((currentTime - previousTimeTemp) >= intervalTempUser) {
             Knx.task();
             if(!tempRequested){
                 if(sensorFound){
                     //request tempeatures
                     long tempRequestStart = millis();
                     sensors.requestTemperatures();
                     tempRequestedTime = millis();
                     tempRequested = true;
#ifdef KDEBUG              
                     Debug.println(F("Temp.requested. Time: %d ms"), (tempRequestedTime-tempRequestStart));
#endif 
                 }else{
#ifdef KDEBUG              
                     Debug.println(F("No sensors found! Try one more time..."));
#endif
                      configureOW();
                 }
             }
        previousTimeTemp = currentTime;  
        }
        
        if ((millis() - tempRequestedTime >= owDelay) && tempRequested){
#ifdef KDEBUG              
            Debug.println(F("Temp.request delay %d ms is ready."), owDelay);
#endif      
            // Get temperature
            currentTemp = getCurrentTemp(0);
            if (currentTemp < 200 && currentTemp != -127) {
                switch (typeTemp) {
                    case 0:
                        Knx.write(COMOBJ_tempValue, currentTemp);
                        break;
                    case 1:
                        if (abs(currentTemp * 100 - previousTemp * 100) >= diffTempUser * 100) {//"*100" => "float to int"
                            Knx.write(COMOBJ_tempValue, currentTemp);
                            previousTemp = currentTemp;
                        }
                        break;
                    default:
                        break;
                }
                limitReached(currentTemp, limitTempMin, limitTempMax, COMOBJ_tempMin, COMOBJ_tempMax, valueTempMin, valueTempMax);
                previousTimeTemp = currentTime;
            }else{
#ifdef KDEBUG              
                Debug.println(F("Something gone wrong... trying to reset OneWire..."));
#endif
                configureOW(); 
            }
        tempRequested = false;
        }
    }
}
