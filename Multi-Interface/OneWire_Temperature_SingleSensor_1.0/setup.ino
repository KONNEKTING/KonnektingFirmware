void setup() {
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

    //setup sensors
    configureOW();
    
    typeTemp = (int) Konnekting.getUINT8Param(PARAM_tempSendUpdate);
    
    //temperature polling interval (ms)
    intervalTempUser = (long) Konnekting.getUINT32Param(PARAM_tempPollingTime)*1000; 
    
    //minimal difference between previous and current temperature [°C]
    diffTempUser = (float) Konnekting.getUINT8Param(PARAM_tempDiff)*0.1; 
    valueTempMin = Konnekting.getUINT8Param(PARAM_tempMinValue);
    limitTempMin = Konnekting.getINT16Param(PARAM_tempMinLimit);
    valueTempMax = Konnekting.getUINT8Param(PARAM_tempMaxValue);
    limitTempMax = Konnekting.getINT16Param(PARAM_tempMaxLimit);
    }
    Debug.println(F("Setup is ready. go to loop..."));
}
