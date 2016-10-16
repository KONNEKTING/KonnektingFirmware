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
        Debug.println("Not registered iButton was found!");
        print_iButton(iButton_addr);
#endif
        if (unreg){
            Knx.write(COMOBJ_notRegistered,true);
            byte tmp[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
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
              Debug.println("CRC is not valid!\n");
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

