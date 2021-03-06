#define MANUFACTURER_ID 7070
#define DEVICE_ID 120
#define REVISION 1

#define COMOBJ_ibutton1 0
#define COMOBJ_ibutton2 1
#define COMOBJ_ibutton3 2
#define COMOBJ_ibutton4 3
#define COMOBJ_ibutton5 4
#define COMOBJ_ibutton6 5
#define COMOBJ_ibutton7 6
#define COMOBJ_ibutton8 7
#define COMOBJ_ibutton9 8
#define COMOBJ_ibutton10 9
#define COMOBJ_allPresent 10
#define COMOBJ_oneAbsent 11
#define COMOBJ_allAbsent 12
#define COMOBJ_allAbsentScene 13
#define COMOBJ_notRegistered 14
#define COMOBJ_notRegisteredId 15
#define PARAM_initialDelay 0
#define PARAM_cycle 1
#define PARAM_sendAllPresent 2
#define PARAM_sendOneAbsent 3
#define PARAM_sendAllAbsent 4
#define PARAM_sendAllAbsentDelay 5
#define PARAM_sendAllAbsentValue 6
#define PARAM_sendAllAbsentScene 7
#define PARAM_sendNotRegistered 8
#define PARAM_iB1isActive 9
#define PARAM_iB1invOutput 10
#define PARAM_iB1id 11
#define PARAM_iB12sActive 12
#define PARAM_iB2invOutput 13
#define PARAM_iB2id 14
#define PARAM_iB3isActive 15
#define PARAM_iB3invOutput 16
#define PARAM_iB3id 17
#define PARAM_iB4isActive 18
#define PARAM_iB4invOutput 19
#define PARAM_iB4id 20
#define PARAM_iB5isActive 21
#define PARAM_iB5invOutput 22
#define PARAM_iB5id 23
#define PARAM_iB6isActive 24
#define PARAM_iB6invOutput 25
#define PARAM_iB6id 26
#define PARAM_iB7isActive 27
#define PARAM_iB7invOutput 28
#define PARAM_iB7id 29
#define PARAM_iB8isActive 30
#define PARAM_iB8invOutput 31
#define PARAM_iB8id 32
#define PARAM_iB9isActive 33
#define PARAM_iB9invOutput 34
#define PARAM_iB9id 35
#define PARAM_iB10isActive 36
#define PARAM_iB10invOutput 37
#define PARAM_iB10id 38
        
KnxComObject KnxDevice::_comObjectsList[] = {
    /* Index 0 - ibutton1 */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 1 - ibutton2 */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 2 - ibutton3 */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 3 - ibutton4 */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 4 - ibutton5 */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 5 - ibutton6 */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 6 - ibutton7 */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 7 - ibutton8 */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 8 - ibutton9 */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 9 - ibutton10 */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 10 - allPresent */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 11 - oneAbsent */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 12 - allAbsent */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 13 - allAbsentScene */ KnxComObject(KNX_DPT_17_001, 0x34),
    /* Index 14 - notRegistered */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 15 - notRegisterd */ KnxComObject(KNX_DPT_16_001, 0x34)
};
const byte KnxDevice::_numberOfComObjects = sizeof (_comObjectsList) / sizeof (KnxComObject); // do not change this code
       
byte KonnektingDevice::_paramSizeList[] = {
    /* Index 0 - initialDelay */ PARAM_UINT16,
    /* Index 1 - cycle */ PARAM_UINT32,
    /* Index 2 - sendAllPresent */ PARAM_UINT8,
    /* Index 3 - sendOneAbsent */ PARAM_UINT8,
    /* Index 4 - sendAllAbsent */ PARAM_UINT8,
    /* Index 5 - sendAllAbsentDelay */ PARAM_UINT8,
    /* Index 6 - sendAllAbsentValue */ PARAM_UINT8,
    /* Index 7 - sendAllAbsentScene */ PARAM_UINT8,
    /* Index 8 - sendNotRegistered */ PARAM_UINT8,
    /* Index 9 - iB1isActive */ PARAM_UINT8,
    /* Index 10 - iB1invOutput */ PARAM_UINT8,
    /* Index 11 - iB1id */ PARAM_RAW8,
    /* Index 12 - iB12sActive */ PARAM_UINT8,
    /* Index 13 - iB2invOutput */ PARAM_UINT8,
    /* Index 14 - iB2id */ PARAM_RAW8,
    /* Index 15 - iB3isActive */ PARAM_UINT8,
    /* Index 16 - iB3invOutput */ PARAM_UINT8,
    /* Index 17 - iB3id */ PARAM_RAW8,
    /* Index 18 - iB4isActive */ PARAM_UINT8,
    /* Index 19 - iB4invOutput */ PARAM_UINT8,
    /* Index 20 - iB4id */ PARAM_RAW8,
    /* Index 21 - iB5isActive */ PARAM_UINT8,
    /* Index 22 - iB5invOutput */ PARAM_UINT8,
    /* Index 23 - iB5id */ PARAM_RAW8,
    /* Index 24 - iB6isActive */ PARAM_UINT8,
    /* Index 25 - iB6invOutput */ PARAM_UINT8,
    /* Index 26 - iB6id */ PARAM_RAW8,
    /* Index 27 - iB7isActive */ PARAM_UINT8,
    /* Index 28 - iB7invOutput */ PARAM_UINT8,
    /* Index 29 - iB7id */ PARAM_RAW8,
    /* Index 30 - iB8isActive */ PARAM_UINT8,
    /* Index 31 - iB8invOutput */ PARAM_UINT8,
    /* Index 32 - iB8id */ PARAM_RAW8,
    /* Index 33 - iB9isActive */ PARAM_UINT8,
    /* Index 34 - iB9invOutput */ PARAM_UINT8,
    /* Index 35 - iB9id */ PARAM_RAW8,
    /* Index 36 - iB10isActive */ PARAM_UINT8,
    /* Index 37 - iB10invOutput */ PARAM_UINT8,
    /* Index 38 - iB10id */ PARAM_RAW8
};
const byte KonnektingDevice::_numberOfParams = sizeof (_paramSizeList); // do not change this code
