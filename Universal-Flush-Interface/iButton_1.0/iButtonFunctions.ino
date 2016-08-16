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
      if (addr[0] <= 0xF){
            DEBUG.print("0");
      }
      DEBUG.print(addr[0], HEX); 
      for(byte i = 6; i >0; i--) {
           DEBUG.print(":");
           if (addr[i] <= 0xF){
            DEBUG.print("0");
           }
           DEBUG.print(addr[i], HEX);           
      }
      
      DEBUG.print(":");
      if (addr[7] <= 0xF){
            DEBUG.print("0");
      }
      DEBUG.print(addr[7], HEX); 
      DEBUG.println("");
}
void print_reg_iButton(byte addr[8]){
      for(byte i = 0; i < 8; i++) {
           if (addr[i] <= 0xF){
              DEBUG.print("0");
           }
           DEBUG.print(addr[i], HEX); 
           if (i<7){
              DEBUG.print(":");                     
           }
      }
      DEBUG.println("");
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
#ifdef DEBUG    
        DEBUG.println("Not registered iButton was found!");
        print_iButton(iButton_addr);
#endif
        if (unreg){
            Knx.write(comObj_unreg,true);
            byte tmp[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
            tmp[0] = iButton_addr[0];
            tmp[1] = iButton_addr[6];
            tmp[2] = iButton_addr[5];
            tmp[3] = iButton_addr[4];
            tmp[4] = iButton_addr[3];
            tmp[5] = iButton_addr[2];
            tmp[6] = iButton_addr[1];
            tmp[7] = iButton_addr[7];
            Knx.write(comObj_unreg_ID,tmp);
        }
    }
}

bool all_absent(){
    bool absent = true;
    for (byte i = 0;i<iButtons;i++){
        if (iButtonActive[i]){
            if (iButtonState[i]){
                absent = false;
//                DEBUG.print("absend: iButton ");
//                DEBUG.println(i);
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
//                DEBUG.print("presend: iButton ");
//                DEBUG.println(i);
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
              Serial.println("CRC is not valid!\n");
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
/*        Serial.print("iButton_state: iButtonActive ");
        Serial.print(i);
        Serial.print(" State: ");
        Serial.print(iButtonState[i]);
        Serial.print(" LastState: ");
        Serial.println(iButtonLastState[i]);
*/        if (iButtonState[i] != iButtonLastState[i]){
              if(iButtonInvOutput[i]){
#ifdef DEBUG 
                    DEBUG.print("Send iButton state ");
                    DEBUG.print(!iButtonState[i]);
                    DEBUG.print(" to Object ");
                    DEBUG.println(iButtonComObj[i]);
#endif

                    Knx.write(iButtonComObj[i],!iButtonState[i]);
              }else{
#ifdef DEBUG 
                    DEBUG.print("Send iButton state ");
                    DEBUG.print(iButtonState[i]);
                    DEBUG.print(" to Object ");
                    DEBUG.println(iButtonComObj[i]);
#endif

                    Knx.write(iButtonComObj[i],iButtonState[i]);
              }

            iButtonLastState[i] = iButtonState[i];
        }
      }
    }
}

