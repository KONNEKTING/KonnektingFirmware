//#define PULS_DELAY 30
byte lastR = 0;
byte lastG = 0;
byte lastB = 0;
byte color = 0;
byte buttonStateOn = 255;
byte buttonStateOff = 0;
bool lastStateShort = false;
bool lastStateLong = false;
//bool lastState = false;
//unsigned long lastMillis = 0;



void initRGB(){
//    pinMode(PIN_LED_R,OUTPUT);
//    pinMode(PIN_LED_G,OUTPUT);
//    pinMode(PIN_LED_B,OUTPUT);
}

void setR(byte r){
    if(r != lastR){
        analogWrite(PIN_LED_R,r);
        lastR = r;
    }
}
void setG(byte g){
    if(g != lastG){
        analogWrite(PIN_LED_G,g);
        lastG = g;
    }
}
void setB(byte b){
    if(b != lastB){
        analogWrite(PIN_LED_B,b);
        lastB = b;
    }
}

void setRGB(byte r, byte g, byte b){
    if(r != lastR){
        analogWrite(PIN_LED_R,r);
       lastR = r;
    }
    if(g != lastG){
        analogWrite(PIN_LED_G,g);
        lastG = g;
    }
    if(b != lastB){
        analogWrite(PIN_LED_B,b);
        lastB = b;
    }
}

/*

RGB Prios:
1. button pressed
2. alarm (blink)
3. long press
4. short press
*/




void taskRgb(){
    if(boolButtonPressed){//Prio 1
        switch (colorOnClickRgb){
            case 0:
                setRGB(buttonStateOn,0,0);
                break;
            case 1:
                setRGB(0,buttonStateOn,0);
                break;
            case 2:
                setRGB(0,0,buttonStateOn);
                break;
            default:
                setRGB(0,0,0);
                break;
        }
    }else{
        if(alarmRgb){//Prio 2
            //ToDo
        }else{
            if(boolLongClick){//Prio 3
                switch (colorStateLongRgb){
                    case 0:
                        setRGB(buttonStateOn,0,0);
                        break;
                    case 1:
                        setRGB(0,buttonStateOn,0);
                        break;
                    case 2:
                        setRGB(0,0,buttonStateOn);
                        break;
                    default:
                        setRGB(0,0,0);
                        break;
                }
            }else{
                if(boolShortClick){//Prio 4
                    switch (colorStateShortRgb){
                    case 0:
                        setRGB(buttonStateOn,0,0);
                        break;
                    case 1:
                        setRGB(0,buttonStateOn,0);
                        break;
                    case 2:
                        setRGB(0,0,buttonStateOn);
                        break;
                    default:
                        setRGB(0,0,0);
                        break;
                    }
                }else{//off
                    setRGB(0,0,0);
                }
            }
        }
    }
  
}


