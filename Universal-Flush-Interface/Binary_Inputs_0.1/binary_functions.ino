bool checkBinaryInput(byte PIN){
    pinMode(PIN, INPUT_PULLUP);
    bool actualState = !digitalRead(PIN); //returns 0 if open & 1 if close
    pinMode(PIN,OUTPUT);
/*    #ifdef DEBUG 
          DEBUG.print("State on pin ");
          DEBUG.print(PIN);
          DEBUG.print(": ");
          DEBUG.println(actualState);
    #endif
*/
  return actualState;
}


void knxSendBool(byte ComObj, bool State){
    Knx.write(ComObj+1,State); //ComObject+1 up to beta 4
    
    #ifdef DEBUG 
          DEBUG.print("Send to KNX Object# ");
          DEBUG.print(ComObj+1);
          DEBUG.print(", State: ");
          DEBUG.println(State);
    #endif
}
