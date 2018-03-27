#ifdef VOC_IAQ
#define CYCLE_VOC 15000 //15s
unsigned long previousVocTime = 0;
int vocPredict = 0;
byte vocState = 0;
int vocResistance = 0;
int vocTVOC = 0;

void initVOC(){
    //nothing to do, placeholder only
}

int getVOC(){
    return voc;
}

int getTVOC(){
    return tvoc;
}

void taskVOC(){
    if ((millis() - previousVocTime) >= CYCLE_VOC){
        previousVocTime = millis();
        //Datasheet page 6    
        Wire.requestFrom(ADDRESS_IAQ, 9);
        vocPredict = (Wire.read() << 8 | Wire.read());
        vocState = Wire.read();
        vocResistance = (Wire.read()& 0x00) | (Wire.read() << 16) | (Wire.read() << 8 | Wire.read());
        vocTVOC = (Wire.read() << 8 | Wire.read());
    }
#ifdef KDEBUG    
    if (vocState == 0x10) {
        Debug.println(F("VOC: Warming up..."));
    } else if (vocState == 0x00) {
        Debug.println(F("VOC: Ready"));
    } else if (vocState == 0x01) {
        Debug.println(F("VOC: Busy"));
    } else if (vocState == 0x80) {
        Debug.println(F("VOC: Error"));
    } else
        Debug.println(F("No Status, check module"));
#endif
    if(vocState == 0x00){
        voc = vocPredict;
        tvoc = vocTVOC;
        Debug.println(F("VOC: %d ppm, TVOC: %d ppm"), voc, tvoc);
    }
}

#endif



#ifdef VOC_CCS811
#include "src/CCS811/CCS811.h"      //https://github.com/AKstudios/CCS811-library

//Datasheet page 15
#define CCS811_STATUS            0x00
#define CCS811_MEANS_MODE        0x01
#define CCS811_ALG_RESULT_DATA   0x02
#define CCS811_ENV_DATA          0x05
#define CCS811_ERROR_ID          0xE0
#define CCS811_APP_START         0xF4

//STAUS bit mask
#define CCS811_STATUS_BM_FW_MODE    B10000000
#define CCS811_STATUS_BM_APP_VALID  B00010000
#define CCS811_STATUS_BM_DATA_READY B00001000
#define CCS811_STATUS_BM_ERROR      B00000001

//MEAS_MODE bits 7:4
#define CCS811_DRIVE_MODE0_IDLE  B0000
#define CCS811_DRIVE_MODE1_1S    B0001
#define CCS811_DRIVE_MODE2_10S   B0010
#define CCS811_DRIVE_MODE3_60S   B0011
//MEAS_MODE bits 3:0
#define CCS811_INT_DATARDY_ON    B0000
#define CCS811_INT_DATARDY_OFF   B1000


#define CYCLE_VOC 1000 //10s
unsigned long previousVocTime = 0;
bool vocOk = false;

CCS811 ccs;


void initVOC(){
    delay(5000);
    if(!ccs.begin(ADDRESS_CCS811,PIN_VOC_WAKE)){
          Debug.println(F("VOC: Initialization failed."));
          vocOk = false;
    }else{
          vocOk = true;
    }
/*      pinMode(PIN_VOC_WAKE, OUTPUT);
      digitalWrite(PIN_VOC_WAKE, LOW);
      Wire.beginTransmission(ADDRESS_CCS811);
      Wire.write(CCS811_STATUS);
      Wire.endTransmission();
      Wire.requestFrom(ADDRESS_CCS811, 1);
      if(Wire.available() == 1)
          if(Wire.read() & CCS811_STATUS_BM_APP_VALID == 1
*/
}

int getVOC(){
    return voc;
}

int getTVOC(){
    return tvoc;
}

int taskVOC(){
    if ((millis() - previousVocTime) >= CYCLE_VOC){
        previousVocTime = millis();
        if(vocOk){
/*        
        Wire.beginTransmission(ADDRESS_CCS811);
        Wire.write(ALG_RESULT_DATA);    // reading ALG_RESULT_DATA clears DATA_READY bit in 0x00
        Wire.endTransmission();
        Wire.requestFrom(ADDRESS_CCS811, 8);
            if (Wire.available() == 8){
            int vocPredict = (Wire.read() << 8 | Wire.read());
            int vocTVOC = (Wire.read() << 8 | Wire.read());
            byte vocState = Wire.read();
            byte vocErrorId = Wire.read()
            int vocRawData = (Wire.read() << 8 | Wire.read());
        }
*/      
#ifdef TRH
        ccs.compensate(getTemp(), getRH());
#endif
        
        Debug.println(F("Status %d"),(byte)ccs.readStatus());
        if(ccs.readStatus() == B10011000)
            Debug.println(F("VOC: get data"));
            ccs.getData();
            voc = ccs.readCO2();
            tvoc = ccs.readTVOC();
//        }
        
        Debug.println(F("VOC: %d ppm, TVOC: %d ppm"), voc, tvoc);
        }else{
            initVOC();
        }
    }
}

#endif
