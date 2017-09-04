void setup(void) {
    // debug related stuff
#ifdef KDEBUG
   // Start debug serial with 9600 bauds
    DEBUGSERIAL.begin(9600);
    while (!DEBUGSERIAL)
    // make debug serial port known to debug class
    // Means: KONNEKTING will sue the same serial port for console debugging
    Debug.setPrintStream(&DEBUGSERIAL);
#endif

    Wire.begin();
    Konnekting.setMemoryReadFunc(&readMemory);
    Konnekting.setMemoryWriteFunc(&writeMemory);
    Konnekting.setMemoryUpdateFunc(&updateMemory);
    Konnekting.setMemoryCommitFunc(&commitMemory);
    
    // Initialize KNX enabled Arduino Board
    Konnekting.init(KNX_SERIAL, PROG_BUTTON_PIN, PROG_LED_PIN, MANUFACTURER_ID, DEVICE_ID, REVISION);

    if (!Konnekting.isFactorySetting()){
        int startDelay = (int) Konnekting.getUINT16Param(PARAM_initialDelay);
        if (startDelay > 0) {
#ifdef KDEBUG  
        Debug.println(F("delay for %d ms"),startDelay);
#endif
        delay(startDelay);
#ifdef KDEBUG  
        Debug.println(F("ready!"));
#endif
        }
        //turn on OneWire pullup
        pinMode(ONE_WIRE_PULLUP, OUTPUT);
        digitalWrite(ONE_WIRE_PULLUP, HIGH);
    
        cycleTime = (long)Konnekting.getUINT32Param(PARAM_cycle);                    //polling cycle
        present = (bool)Konnekting.getUINT8Param(PARAM_sendAllPresent);              //send "all present"
        one_absent = (bool)Konnekting.getUINT8Param(PARAM_sendOneAbsent);            //send "at least one absent"
        absent = (bool)Konnekting.getUINT8Param(PARAM_sendAllAbsent);                //send "all absent"
        absentDelayNummer = (byte)Konnekting.getUINT8Param(PARAM_sendAllAbsentDelay);//all absent delay
        absentValue = (byte)Konnekting.getUINT8Param(PARAM_sendAllAbsentValue);      //all absent value
        absentScene = (byte)Konnekting.getUINT8Param(PARAM_sendAllAbsentScene);      //all absent scene
        unreg = (bool)Konnekting.getUINT8Param(PARAM_sendNotRegistered);             //send "unregistered iButton founded"
    
        for (byte i = 0; i < iButtons; i++) {
            iButtonActive[i]  = (bool)Konnekting.getUINT8Param(i * 3 + 9);  //input active?
            iButtonInvOutput[i] = (bool)Konnekting.getUINT8Param(i * 3 + 10);//invert binary output: 0=normal|1=inverted
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
}
