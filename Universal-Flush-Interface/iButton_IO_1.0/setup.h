void setup(void) {
  // if debug mode is enabled, setup serial port with 9600 baud
#ifdef KDEBUG
    DEBUGSERIAL.begin(9600);
    while (!DEBUGSERIAL)
    Debug.setPrintStream(&DEBUGSERIAL);
#endif
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
        init_iButton();
        init_inputs();
        init_outputs();
#ifdef KDEBUG  
        Debug.println(F("Setup is ready. go to loop..."));
#endif
    }
}
