void loop(void) {
    Knx.task();
    unsigned long currTime = millis();
    if (Konnekting.isReadyForApplication()) {
        if (currTime - prevTime >= cycleTime){

            ow_search(); //search for iButtons

            iButton_state(); //send states
            bool tmp_present = all_present();
            bool tmp_absent = all_absent();
//#ifdef KDEBUG
//            Debug.println(F("tmp_present=%d"),tmp_present);
//            Debug.println(F("tmp_absent=%d"),tmp_absent);
//#endif
            if (tmp_present != lastState_present){
                if (tmp_present){
#ifdef KDEBUG 
                    Debug.println("Send message: all iButtons are present");
#endif
                    if (present){
                        Knx.write(COMOBJ_allPresent,true);
                    }
                   
                }else{
#ifdef KDEBUG 
                    Debug.println("Send message: at least one of iButtons is absent");
#endif
                    if (one_absent){
                        Knx.write(COMOBJ_oneAbsent,true);
                    }
                }
                lastState_present = tmp_present;
            }
            if(tmp_absent && !firstAbsentSet){
                    firstAbsent = millis();
                    firstAbsentSet = true;
            }
            if ((millis()-firstAbsent>=absentDelay[absentDelayNummer]*1000) && firstAbsentSet){
                firstAbsentSet = false;
                if (tmp_absent){
                      
                      #ifdef KDEBUG
                      Debug.println("Send message: all iButtons are absent");
#endif
                

                      if (lastState_absent && lastState_absent && absent){
//                            Knx.write(COMOBJ_allAbsent,true);
                            if(absentValue != 255){
                                Knx.write(COMOBJ_allAbsent,absentValue);
                            }
                            if(absentScene != 255){
                                Knx.write(COMOBJ_allAbsentScene,absentScene);
                            }
                      }
                }
            }
        
        for (byte i = 0; i<iButtons;i++){
            iButtonState[i] = false;
        }
        prevTime = currTime;
        }
    }
}
