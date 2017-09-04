void loop() {
    Knx.task();
    unsigned long currentTime = millis();
    
    // only do measurements and other sketch related stuff if not in programming mode
    if (Konnekting.isReadyForApplication()) {
        
        // Get temperature
        if ((currentTime - previousTimeTemp) >= intervalTempUser) {
           
            unsigned long start = millis();
            currentTemp = htu.readTemperature();
            unsigned long end = millis();
#ifdef KDEBUG              
            char currTempChar[10];
            dtostrf(currentTemp,6,2,currTempChar);
            Debug.println(F("currentTemp: %s time: %d ms"), currTempChar, (end-start));
#endif
            if (currentTemp < 900) {
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
                
            }else{
#ifdef KDEBUG              
            Debug.println(F("Sensor error!"));
#endif 
            }
            previousTimeTemp = currentTime;
        }
        // Get humidity
        if ((currentTime - previousTimeHumd) >= intervalHumdUser) {
            unsigned long start = millis();
            currentHumd = htu.readHumidity();
            unsigned long end = millis();
#ifdef KDEBUG              
            char currRHChar[10];
            dtostrf(currentHumd,6,2,currRHChar);
            Debug.println(F("currentHumd: %s time: %d ms"), currRHChar, (end-start));
#endif
            if (currentHumd < 900) {
                
                switch (typeHumd) {
                    case 0:
                        Knx.write(COMOBJ_rhValue, currentHumd);
                        break;
                    case 1:
                        if (abs(currentHumd * 100 - previousHumd * 100) >= diffHumdUser * 100) {
                            Knx.write(COMOBJ_rhValue, currentHumd);
                            previousHumd = currentHumd;
                        }
                    default:
                        break;
                }
                limitReached(currentHumd, limitHumdMin, limitHumdMax, COMOBJ_rhMin, COMOBJ_rhMax, valueHumdMin, valueHumdMax);
                
            }else{
#ifdef KDEBUG              
            Debug.println(F("Sensor error!"));
#endif 
            }
            previousTimeHumd = currentTime;
        }
    }
}
