bool checkBinaryInput(byte PIN){
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
