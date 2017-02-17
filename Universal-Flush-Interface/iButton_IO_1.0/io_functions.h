void init_inputs(){
    inputCycle = (long)Konnekting.getUINT32Param(1);  //polling cycle

    for (byte i= 0;i<INPUTS;i++){
        activeInputs[i]  = (bool)Konnekting.getUINT8Param(i*IN_PARAM_COUNT+IN_FIRST_PARAM);  //input active?
        invertOutputInputs[i] = (bool)Konnekting.getUINT8Param(i*IN_PARAM_COUNT+IN_FIRST_PARAM + 1);  //invert binary output 2: 0=normal|1=inverted
        outputDelayInputs[i]  = (long)Konnekting.getUINT32Param(i*IN_PARAM_COUNT+IN_FIRST_PARAM + 2); //send delay for binary inputs
    
    }
#ifdef KDEBUG  
    for (byte i= 0;i<INPUTS;i++){
        Debug.println(F("Input: %d  active: %d  invert: %d  delay: %d ms"),i,activeInputs[i],invertOutput[i],outputDelayInputs[i]);
    }
#endif
}


void init_outputs(){
    for (byte o = 0;o<OUTPUTS;o++){
        pinMode(o,OUTPUT);
        digitalWrite(o,LOW);
        outputActive[o]  = (bool)Konnekting.getUINT8Param(o*OUT_PARAM_COUNT+OUT_FIRST_PARAM);  //input active?
        invertOutput[o] = (bool)Konnekting.getUINT8Param(o*OUT_PARAM_COUNT+OUT_FIRST_PARAM + 1);  //invert binary output 2: 0=normal|1=inverted
    }
}

void setOutput(byte pin, bool value){
    digitalWrite(pin,value);
#ifdef KDEBUG
    Debug.println(F("Output %d , value: %d"), pin, value);
#endif
}

bool checkInputs(byte PIN){
    pinMode(PIN, INPUT_PULLUP);
    bool actualState = !digitalRead(PIN);
    pinMode(PIN,OUTPUT);
    return actualState;
}

void knxSendBool(byte comObj, bool state){
    Knx.write(comObj,state);
#ifdef KDEBUG
    Debug.println(F("Send to KNX Object %d , state: %d"), comObj, state);
#endif
}

void task_inputs(){
    unsigned long currentTime = millis();
    if ((currentTime - inputPreviousTime) >= inputCycle){
        for(byte i=0;i<INPUTS;i++){
            currentStateInputs[i] = checkInputs(pinNummberInputs[i]);
        }
    inputPreviousTime = currentTime;
    }
    
    unsigned long currentTimeDelay = millis();
    for (byte i=0;i<INPUTS;i++){
        if ((currentTimeDelay - previousOutputDelayInputs[i]) >= outputDelayInputs[i]){
            if (currentStateInputs[i] != lastStateInputs[i]){
                if (invertOutput[i]){
                    knxSendBool(comObjToInputId[i],!currentStateInputs[i]); //send inverted state
                }else{
                    knxSendBool(comObjToInputId[i], currentStateInputs[i]); //send original state
                } 
                lastStateInputs[i] = currentStateInputs[i];
            }
            previousOutputDelayInputs[i] = currentTimeDelay;
        }
    }
}

void task_outputs(){
    for (byte o = 0;o<OUTPUTS;o++){
        //find output pin
        if (currentStateOutput[o] != lastStateOutput[o]){
          lastStateOutput[o] = currentStateOutput[o];
          setOutput(pinNumberOutput[o],currentStateOutput[o]);
#ifdef KDEBUG
    Debug.println(F("New state on output %d, state: %d"), pinNumberOutput[o], currentStateOutput[o]);
#endif
        }
    }
}

