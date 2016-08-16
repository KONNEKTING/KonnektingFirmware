void loop(void) {
    Knx.task();
    unsigned long currTime = millis();
    if (!Tools.getProgState() && !Tools.isFactorySetting()) {//check programming mode
        if (currTime - prevTime >= cycleTime){

            ow_search(); //search for iButtons

            iButton_state(); //send states
            bool tmp_present = all_present();
            bool tmp_absent = all_absent();
            if (tmp_present != lastState_present){
                if (tmp_present){
#ifdef DEBUG 
                    DEBUG.println("Send message: all iButtons are present");
#endif
                    if (present){
                        Knx.write(comObj_present,true);
                    }
                   
                }else{
#ifdef DEBUG 
                    DEBUG.println("Send message: at least one of iButtons is absent");
#endif
                    if (one_absent){
                        Knx.write(comObj_one_absent,true);
                    }
                }
                lastState_present = tmp_present;
            }
    
            if (tmp_absent != lastState_absent){
                if(tmp_absent){
#ifdef DEBUG
                    DEBUG.println("Send message: all iButtons are absent");
#endif
                    if (absent){
                        Knx.write(comObj_absent,true);
                    }
                }
                lastState_absent = tmp_absent;
            }
        
        for (byte i = 0; i<iButtons;i++){
            iButtonState[i] = false;
        }
        prevTime = currTime;
        }
    }
}
