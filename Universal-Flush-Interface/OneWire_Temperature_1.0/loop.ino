void loop() {
    Knx.task();
    unsigned long currentTime = millis();
    
    // only do measurements and other sketch related stuff if not in programming mode & not factory settings
    if (!Tools.getProgState() && !Tools.isFactorySetting()) {
        

        if ((currentTime - previousTimeTemp) >= intervalTempUser) {
           Knx.task();
           // Get temperature
           currentTemp = getCurrentTemp(0);
           Knx.task();

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
                limitReached(currentTemp, limitTempMin, limitTempMax, comObj_Min, comObj_Max, valueTempMin, valueTempMax);
                previousTimeTemp = currentTime;
            }
        }
    }
}
