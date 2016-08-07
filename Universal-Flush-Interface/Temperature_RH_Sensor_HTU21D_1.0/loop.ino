void loop() {
    Knx.task();
    unsigned long currentTime = millis();
    
    // only do measurements and other sketch related stuff if not in programming mode
    if (!Tools.getProgState()) {
        
        // Get temperature
        if ((currentTime - previousTimeTemp) >= intervalTempUser) {
            
            long start = micros();
            currentTemp = htu.readTemperature();
            long end = micros();
#ifdef DEBUG  
            DEBUG.print("currentTemp: ");
            DEBUG.println(currentTemp);
            DEBUG.print("time: ");
            DEBUG.println((end-start));
#endif
            if (currentTemp < 200) {
                switch (typeTemp) {
                    case 0:
                        Knx.write(comObj_Temp, currentTemp);
                        break;
                    case 1:
                        if (abs(currentTemp * 100 - previousTemp * 100) >= diffTempUser * 100) {//"*100" => "float to int"
                            Knx.write(comObj_Temp, currentTemp);
                            previousTemp = currentTemp;
                        }
                        break;
                    default:
                        break;
                }
                limitReached(currentTemp, limitTempMin, limitTempMax, comObj_TempMin, comObj_TempMax, valueTempMin, valueTempMax);
                
            }
            previousTimeTemp = currentTime;
        }
        Knx.task();
        // Get humidity
        if ((currentTime - previousTimeHumd) >= intervalHumdUser) {
            long start = micros();
            currentHumd = htu.readHumidity();
            long end = micros();
            
#ifdef DEBUG 
            DEBUG.print("currentHumd: ");
            DEBUG.println(currentHumd);
            DEBUG.print("time: ");
            DEBUG.println((end-start));
#endif
            if (currentHumd < 101) {
                
                switch (typeHumd) {
                    case 0:
                        Knx.write(comObj_RH, currentHumd);
                        break;
                    case 1:
                        if (abs(currentHumd * 100 - previousHumd * 100) >= diffHumdUser * 100) {
                            Knx.write(comObj_RH, currentHumd);
                            previousHumd = currentHumd;
                        }
                    default:
                        break;
                }
                limitReached(currentHumd, limitHumdMin, limitHumdMax, comObj_RHMin, comObj_RHMax, valueHumdMin, valueHumdMax);
                
            }
            previousTimeHumd = currentTime;
        }
    }
}
