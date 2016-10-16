void loop() {
    Knx.task();
    unsigned long currentTime = millis();
    if (Konnekting.isReadyForApplication()) {//check programming mode
        currentTime = millis();
        if ((currentTime - binaryPreviousTime) >= binaryCycle){
            for(byte i=0;i<INPUTS;i++){
                currentState[i] = checkBinaryInput(pinNummber[i]);
            }
        binaryPreviousTime = currentTime;
        }
        
    }
    unsigned long currentTimeDelay = millis();
    for (byte i=0;i<INPUTS;i++){
        if ((currentTimeDelay - previousOutputDelay[i]) >= outputDelay[i]){
            if (currentState[i] != lastState[i]){
                if (invertOutput[i]){
                    knxSendBool(i,!currentState[i]); //send inverted state
                }else{
                    knxSendBool(i, currentState[i]); //send original state
                } 
                lastState[i] = currentState[i];
            }
            previousOutputDelay[i] = currentTimeDelay;
        }
    }
}

