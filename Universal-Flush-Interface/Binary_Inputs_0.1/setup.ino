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


binaryCycle = (long)Tools.getUINT32Param(1);  //polling cycle

for (byte i= 0;i<inputs;i++){
    InputActive[i]  = (bool)Tools.getUINT8Param(i*3+2);  //input active?
    InvertOutput[i] = (bool)Tools.getUINT8Param(i*3+3);  //invert binary output 2: 0=normal|1=inverted
    OutputDelay[i]  = (long)Tools.getUINT32Param(i*3+4); //send delay for binary inputs

}
#ifdef DEBUG  
for (byte i= 0;i<inputs;i++){
            DEBUG.print("Input ");
            DEBUG.print(i);
            DEBUG.print(" Active: ");
            DEBUG.print(InputActive[i]);
            DEBUG.print(" Invert: ");
            DEBUG.print(InvertOutput[i]);
            DEBUG.print(" Delay: ");
            DEBUG.print(OutputDelay[i]);
            DEBUG.println(" ms");
}
#endif

/*if (binaryDelay<binaryCycle){
    binaryDelay = binaryCycle;
}
*/
}
