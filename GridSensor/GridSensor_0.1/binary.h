#ifdef BINARY
byte pinNumber[]    = {PIN_BIN0,PIN_BIN1};
byte inputActive[]  = {0,0};
byte invertOutput[] = {0,0};
bool lastStateBinary[]    = {0,0};
bool currentState[] = {0,0};
long outputDelay[]  = {0,0};
long previousOutputDelay[] = {0,0};

void initBinary(){
    
}

bool getBinaryState(byte binNumber){
    pinMode(pinNumber[binNumber], INPUT_PULLUP);
    bool actualState = !digitalRead(pinNumber[binNumber]);
    pinMode(pinNumber[binNumber], OUTPUT);
    return actualState;
}
#endif
