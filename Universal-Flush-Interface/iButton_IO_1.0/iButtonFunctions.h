bool check_iButton(byte ow_addr[8], byte iButton_addr[8]){
    if (ow_addr[0] == iButton_addr[0] &&
        ow_addr[6] == iButton_addr[1] &&
        ow_addr[5] == iButton_addr[2] &&
        ow_addr[4] == iButton_addr[3] &&
        ow_addr[3] == iButton_addr[4] &&
        ow_addr[2] == iButton_addr[5] &&
        ow_addr[1] == iButton_addr[6] &&
        ow_addr[7] == iButton_addr[7]){
        return true;
    }else{
        return false;
    }
}

void print_iButton(byte addr[8]){
      Debug.println(F("%02x%02x%02x%02x%02x%02x%02x%02x"),addr[0],addr[6],addr[5],addr[4],addr[3],addr[2],addr[1],addr[7]);
}
void print_reg_iButton(byte addr[8]){
      Debug.println(F("%02x%02x%02x%02x%02x%02x%02x%02x"),addr[0],addr[1],addr[2],addr[3],addr[4],addr[5],addr[6],addr[7]);
}

void unregistered_iButtons(bool tmpState[]){
    bool unregistered = true;
    for (byte i = 0;i<iButtons;i++){
        //if (iButtonActive[i]){
            if (tmpState[i]){
                unregistered = false;
            }
        //}
    }
    if (unregistered){
#ifdef KDEBUG    
        Debug.println(F("Not registered iButton was found!"));
        print_iButton(iButton_addr);
#endif
        if (unreg){
            Knx.write(COMOBJ_notRegistered,true);
            byte tmp[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
            tmp[0] = iButton_addr[0];
            tmp[1] = iButton_addr[6];
            tmp[2] = iButton_addr[5];
            tmp[3] = iButton_addr[4];
            tmp[4] = iButton_addr[3];
            tmp[5] = iButton_addr[2];
            tmp[6] = iButton_addr[1];
            tmp[7] = iButton_addr[7];
            Knx.write(COMOBJ_notRegisteredId,tmp);
        }
    }
}

bool all_absent(){
    bool absent = true;
    for (byte i = 0;i<iButtons;i++){
        if (iButtonActive[i]){
            if (iButtonState[i]){
                absent = false;
//                Debug.println(F("not absent: iButton %d"),i);
            }
        }
    }
    return absent;
}

bool all_present(){
    bool present = true;
    for (byte i = 0;i<iButtons;i++){
        if (iButtonActive[i]){
            if (!iButtonState[i]){
                present = false;
//                Debug.println(F("present: iButton %d"),i);
            }
        }
    }
    return present;
}


void ow_search(){
      while(oneWire.search(iButton_addr)){
          bool tmpState[] = {false,false,false,false,false,false,false,false,false,false};
          Knx.task();
#ifdef DEBUG 
//          print_iButton(iButton_addr); //use this to find out connected iButtons
#endif
          if (OneWire::crc8( iButton_addr, 7) != iButton_addr[7]){
#ifdef DEBUG             
              Debug.println(F("CRC is not valid!\n"));
#endif
          }else{
              tmpState[0] = check_iButton(iButton_addr,iButton0);
              tmpState[1] = check_iButton(iButton_addr,iButton1);
              tmpState[2] = check_iButton(iButton_addr,iButton2);
              tmpState[3] = check_iButton(iButton_addr,iButton3);
              tmpState[4] = check_iButton(iButton_addr,iButton4);
              tmpState[5] = check_iButton(iButton_addr,iButton5);
              tmpState[6] = check_iButton(iButton_addr,iButton6);
              tmpState[7] = check_iButton(iButton_addr,iButton7);
              tmpState[8] = check_iButton(iButton_addr,iButton8);
              tmpState[9] = check_iButton(iButton_addr,iButton9);
              unregistered_iButtons(tmpState);
              
              for (byte i = 0; i < iButtons;i++){
                  if (iButtonState[i] != true && tmpState[i]){
                      iButtonState[i] = true;
                  }
              }
          oneWire.reset();
          }
      }
      oneWire.reset_search();
}

void iButton_state(){
    for (byte i = 0;i < iButtons; i++){
        if (iButtonActive[i]){
            if (iButtonState[i] != iButtonLastState[i]){
                  if(iButtonInvOutput[i]){
#ifdef KDEBUG 
                       Debug.println(F("Send iButton state %d to Object %d"),!iButtonState[i],iButtonComObj[i]);
#endif
                       Knx.write(iButtonComObj[i],!iButtonState[i]);
                  }else{
#ifdef KDEBUG 
                       Debug.println(F("Send iButton state %d to Object %d"),iButtonState[i],iButtonComObj[i]);
#endif
                       Knx.write(iButtonComObj[i],iButtonState[i]);
                  }
    
                iButtonLastState[i] = iButtonState[i];
            }
        }
    }
}

void init_iButton(){
    //turn on OneWire pullup
    pinMode(ONE_WIRE_PULLUP, OUTPUT);
    digitalWrite(ONE_WIRE_PULLUP, HIGH);

    iButtonCycleTime = (long)Konnekting.getUINT32Param(PARAM_cycle);             //polling cycle
    present = (bool)Konnekting.getUINT8Param(PARAM_sendAllPresent);              //send "all present"
    one_absent = (bool)Konnekting.getUINT8Param(PARAM_sendOneAbsent);            //send "at least one absent"
    absent = (bool)Konnekting.getUINT8Param(PARAM_sendAllAbsent);                //send "all absent"
    absentDelayNummer = (byte)Konnekting.getUINT8Param(PARAM_sendAllAbsentDelay);//all absent delay
    absentValue = (byte)Konnekting.getUINT8Param(PARAM_sendAllAbsentValue);      //all absent value
    absentScene = (byte)Konnekting.getUINT8Param(PARAM_sendAllAbsentScene);      //all absent scene
    unreg = (bool)Konnekting.getUINT8Param(PARAM_sendNotRegistered);             //send "unregistered iButton founded"

    for (byte i = 0; i < iButtons; i++) {
        iButtonActive[i]  = (bool)Konnekting.getUINT8Param(i * IB_PARAM_COUNT + IB_FIRST_PARAM);  //input active?
        iButtonInvOutput[i] = (bool)Konnekting.getUINT8Param(i * IB_PARAM_COUNT + IB_FIRST_PARAM + 1);//invert binary output: 0=normal|1=inverted
    }
    Konnekting.getParamValue(PARAM_iB1id, iButton0);
    Konnekting.getParamValue(PARAM_iB2id, iButton1);
    Konnekting.getParamValue(PARAM_iB3id, iButton2);
    Konnekting.getParamValue(PARAM_iB4id, iButton3);
    Konnekting.getParamValue(PARAM_iB5id, iButton4);
    Konnekting.getParamValue(PARAM_iB6id, iButton5);
    Konnekting.getParamValue(PARAM_iB7id, iButton6);
    Konnekting.getParamValue(PARAM_iB8id, iButton7);
    Konnekting.getParamValue(PARAM_iB9id, iButton8);
    Konnekting.getParamValue(PARAM_iB10id,iButton9);
#ifdef KDEBUG
    Debug.println(F("Registered iButtons:"));
    Debug.println(F("Nr.  Active  Inverted  iButton ID"));
    Debug.print(F(" 1      %d        %d     "),iButtonActive[0],iButtonInvOutput[0]);
    print_reg_iButton(iButton0);
    Debug.print(F(" 2      %d        %d     "),iButtonActive[1],iButtonInvOutput[1]);
    print_reg_iButton(iButton1);
    Debug.print(F(" 3      %d        %d     "),iButtonActive[2],iButtonInvOutput[2]);
    print_reg_iButton(iButton2);
    Debug.print(F(" 4      %d        %d     "),iButtonActive[3],iButtonInvOutput[3]);
    print_reg_iButton(iButton3);
    Debug.print(F(" 5      %d        %d     "),iButtonActive[4],iButtonInvOutput[4]);
    print_reg_iButton(iButton4);
    Debug.print(F(" 6      %d        %d     "),iButtonActive[5],iButtonInvOutput[5]);
    print_reg_iButton(iButton5);
    Debug.print(F(" 7      %d        %d     "),iButtonActive[6],iButtonInvOutput[6]);
    print_reg_iButton(iButton6);
    Debug.print(F(" 8      %d        %d     "),iButtonActive[7],iButtonInvOutput[7]);
    print_reg_iButton(iButton7);
    Debug.print(F(" 9      %d        %d     "),iButtonActive[8],iButtonInvOutput[8]);
    print_reg_iButton(iButton8);
    Debug.print(F("10      %d        %d     "),iButtonActive[9],iButtonInvOutput[9]);
    print_reg_iButton(iButton9);
#endif  
}

void task_iButtons(){
unsigned long currTime = millis();
if (currTime - iButtonPrevTime >= iButtonCycleTime){

            ow_search(); //search for iButtons

            iButton_state(); //send states
            bool tmp_allpresent = all_present();
            bool tmp_allabsent = all_absent();

            if (tmp_allpresent != lastState_present){
                if (tmp_allpresent){
#ifdef KDEBUG 
                    Debug.println(F("Send message: all iButtons are present"));
#endif
                    if (present){
                        Knx.write(COMOBJ_allPresent,true);
                    }
                   
                }else{
#ifdef KDEBUG 
                    Debug.println(F("Send message: at least one of iButtons is absent"));
#endif
                    if (one_absent){
                        Knx.write(COMOBJ_oneAbsent,true);
                    }
                }
                lastState_present = tmp_allpresent;
            }

            if(tmp_allabsent && !firstAllAbsentSet){
                firstAllAbsent = millis();
                firstAllAbsentSet = true;
                lastAllAbsent = true;
            }
            if(!tmp_allabsent && lastAllAbsent){
                if(absentValue != 255){
                    Knx.write(COMOBJ_allAbsent,!absentValue);
                }
                lastAllAbsent = false;
            }

            if ((millis()-firstAllAbsent>=absentDelay[absentDelayNummer]*1000) && firstAllAbsentSet){
                firstAllAbsentSet = false;
                if (tmp_allabsent){
                      
#ifdef KDEBUG
                      Debug.println(F("Send message: all iButtons are absent"));
#endif
                

                      if (lastState_absent && absent){
                            allAbsentWithDelay = true;
                            if(absentValue != 255){
                                Knx.write(COMOBJ_allAbsent,absentValue);
                            }
                            if(absentScene != 255){
                                Knx.write(COMOBJ_allAbsentScene,absentScene);
                            }
                      }else{
                            allAbsentWithDelay = false;
                      }
                }
            }
        
        for (byte i = 0; i<iButtons;i++){
            iButtonState[i] = false;
        }
        iButtonPrevTime = currTime;
        }

  
}
