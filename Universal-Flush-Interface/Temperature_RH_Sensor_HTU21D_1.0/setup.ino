void setup() {

    // if debug mode is enabled, setup serial port with 9600 baud    
#ifdef DEBUG
    DEBUG.begin(9600);
#endif
    // Initialize KNX enabled Arduino Board
    Tools.init(KNX_SERIAL, PROG_BUTTON_PIN, PROG_LED_PIN, MANUFACTURER_ID, DEVICE_ID, REVISION);
    
    if (!Tools.isFactorySetting()){
        byte startDelay = (byte)Tools.getUINT8Param(0);
        if (startDelay < 61 && startDelay > 0) {
#ifdef DEBUG  
            DEBUG.print("delay for ");
            DEBUG.print(startDelay);
            DEBUG.println("s");
#endif
            delay(startDelay*1000);
    
#ifdef DEBUG  
            DEBUG.println("ready!");
#endif
        }
    }

    htu.begin();

    typeTemp = (int) Tools.getUINT8Param(1);
    
    //temperature polling interval (ms)
    intervalTempUser = (long) Tools.getUINT32Param(2)*1000; 
    
    //minimal difference between previous and current temperature [°C]
    diffTempUser = (float) Tools.getUINT8Param(3)*0.1; 
    valueTempMin = Tools.getUINT8Param(4);
    limitTempMin = Tools.getINT16Param(5);
    valueTempMax = Tools.getUINT8Param(6);
    limitTempMax = Tools.getINT16Param(7);

    typeHumd = Tools.getUINT8Param(8);
    intervalHumdUser = (long) Tools.getUINT32Param(9)*1000; //humidity polling interval (ms)
    diffHumdUser = (float) Tools.getUINT8Param(10)*0.1;
    valueHumdMin = Tools.getUINT8Param(11);
    limitHumdMin = Tools.getINT16Param(12);
    valueHumdMax = Tools.getUINT8Param(13);
    limitHumdMax = Tools.getINT16Param(14);
}
