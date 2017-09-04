/*---------------------------------------------------------
You don't need KONNEKTING Suite for this test :)
Connect UFI with microUSB to you PC and open Serial-Monitor
----------------------------------------------------------*/


#include <KonnektingDevice.h>

// ################################################
// ### DEBUG CONFIGURATION
// ################################################
#define KDEBUG // comment this line to disable DEBUG mode
#ifdef KDEBUG
#include <DebugUtil.h>

#define DEBUGSERIAL Serial
#endif

// ################################################
// ### IO Configuration
// ################################################
#define PROG_LED_PIN 13
#define PROG_BUTTON_PIN 2
#define PROG_TOGGLE_BUTTON 7

// defaults to on-board LED for AVR Arduinos
#define TEST_LED 13 //or change it to another pin


//(use knx_address_calculator.html to calculate your own address/GA)
//define hardcoded address 1.1.199 
byte hiAddr = 0x11;
byte loAddr = 0xc7;
//define hardcoded listen GA 7/7/7 for LED toggle
byte hiGA1 = 0x3F;
byte loGA1 = 0x07;
//define hardcoded GA 7/7/8 for sending button state
byte hiGA2 = 0x3F;
byte loGA2 = 0x08;



// Define KONNEKTING Device related IDs
#define MANUFACTURER_ID 57005
#define DEVICE_ID 255
#define REVISION 0

// ################################################
// ### KONNEKTING Configuration
// ################################################
#define KNX_SERIAL Serial1

// Definition of the Communication Objects attached to the device
KnxComObject KnxDevice::_comObjectsList[] = {
    /* Suite-Index 0 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_LOGIC_IN),
    /* Suite-Index 1 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR),
};
const byte KnxDevice::_numberOfComObjects = sizeof (_comObjectsList) / sizeof (KnxComObject); // do no change this code

// Definition of parameter size
byte KonnektingDevice::_paramSizeList[] = {
    /* Param Index 0 */ PARAM_UINT16
};
const byte KonnektingDevice::_numberOfParams = sizeof (_paramSizeList); // do no change this code

unsigned long blinkDelay = 2500;
unsigned long lastmillis = millis(); 
int lastState = false;


// ################################################
// ### KNX EVENT CALLBACK
// ################################################

void knxEvents(byte index) {
    // nothing to do in this sketch
    switch (index) {

        case 0: // object index has been updated

            if (Knx.read(0)) {
                digitalWrite(TEST_LED, HIGH);
                Debug.println(F("Toggle LED: on"));
            } else {
                digitalWrite(TEST_LED, LOW);
                Debug.println(F("Toggle LED: off"));
            }
            break;

        default:
            break;
    }
}

void setup() {

//write hardcoded PA and GAs
EEPROM.write(0, hiAddr);  //Set "not factory" flag
EEPROM.write(1, hiAddr);  //hiAddr
EEPROM.write(2, loAddr);  //loAddr
EEPROM.write(10, hiGA1); //hi GA1
EEPROM.write(11, loGA1); //lo GA1
EEPROM.write(12, 0x80);  //activate GA1
EEPROM.write(13, hiGA2); //hi GA2
EEPROM.write(14, loGA2); //lo GA2
EEPROM.write(15, 0x80);  //activate GA2

    pinMode(PROG_TOGGLE_BUTTON,INPUT);

    // debug related stuff
#ifdef KDEBUG

    // Start debug serial with 9600 bauds
    DEBUGSERIAL.begin(9600);

    while (!DEBUGSERIAL)
    // make debug serial port known to debug class
    // Means: KONNEKTING will sue the same serial port for console debugging
    Debug.setPrintStream(&DEBUGSERIAL);
#endif
    // Initialize KNX enabled Arduino Board
    Konnekting.init(KNX_SERIAL, PROG_BUTTON_PIN, PROG_LED_PIN, MANUFACTURER_ID, DEVICE_ID, REVISION);


    Debug.println(F("Toggle LED every %d ms."), blinkDelay);
    Debug.println(F("Setup is ready. go to loop..."));

}

void loop() {

    // Do KNX related stuff (like sending/receiving KNX telegrams)
    // This is required in every KONNEKTING aplication sketch
    Knx.task();

    unsigned long currentmillis = millis();

    /*
     * only do measurements and other sketch related stuff if not in programming mode
     * means: only when konnekting is ready for appliction
     */
    if (Konnekting.isReadyForApplication()) {


            bool actualState = digitalRead(PROG_TOGGLE_BUTTON);
            if (lastState != actualState){
                Knx.write(1, actualState);
                lastState = actualState;
                Debug.println(F("Actual state: %d"), actualState);
            }
            
        if (currentmillis - lastmillis >= blinkDelay) {
            digitalWrite(TEST_LED, HIGH);
            lastmillis = currentmillis;
        }
    }

}
