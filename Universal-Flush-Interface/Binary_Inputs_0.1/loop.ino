void loop() {
    Knx.task();
    unsigned long currentTime = millis();
    if (!Tools.getProgState() && !Tools.isFactorySetting()) {//check programming mode
        currentTime = millis();
        if ((currentTime - binaryPreviousTime) >= binaryCycle){
            for(byte i=0;i<inputs;i++){
                currentState[i] = checkBinaryInput(PinNummber[i]);
            }
        binaryPreviousTime = currentTime;
        }
        
    }
    unsigned long currentTimeDelay = millis();
    for (byte i=0;i<inputs;i++){
        if ((currentTimeDelay - PreviousOutputDelay[i]) >= OutputDelay[i]){
            if (currentState[i] != lastState[i]){
                if (InvertOutput[i]){
                    knxSendBool(i,!currentState[i]); //send inverted state
                }else{
                    knxSendBool(i, currentState[i]); //send original state
                } 
                lastState[i] = currentState[i];
            }
            PreviousOutputDelay[i] = currentTimeDelay;
        }
    }
}

