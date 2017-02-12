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

        binaryCycle = (long)Konnekting.getUINT32Param(1);  //polling cycle

        for (byte i= 0;i<INPUTS;i++){
            inputActive[i]  = (bool)Konnekting.getUINT8Param(i*3+2);  //input active?
            invertOutput[i] = (bool)Konnekting.getUINT8Param(i*3+3);  //invert binary output 2: 0=normal|1=inverted
            outputDelay[i]  = (long)Konnekting.getUINT32Param(i*3+4); //send delay for binary inputs
        
        }
#ifdef KDEBUG  
        for (byte i= 0;i<INPUTS;i++){
            Debug.println(F("Input: %d  active: %d  invert: %d  delay: %d ms"),i,inputActive[i],invertOutput[i],outputDelay[i]);
        }
        Debug.println(F("Setup is ready. go to loop..."));
#endif
    }
}
