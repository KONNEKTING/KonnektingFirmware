#define MANUFACTURER_ID 7070
#define DEVICE_ID 180
#define REVISION 1

#define COMOBJ_tempValue 0
#define COMOBJ_tempInputValue 1
#define COMOBJ_tempSendValue 2
#define COMOBJ_rhValue 3
#define COMOBJ_rhInputValue 4
#define COMOBJ_rhSendValue 5
#define COMOBJ_luxValue 6
#define COMOBJ_lightMin 7
#define COMOBJ_lightMax 8
#define COMOBJ_lightSwitch 9
#define COMOBJ_vocValue 10
#define COMOBJ_vocMin 11
#define COMOBJ_vocMax 12
#define COMOBJ_vocSwitch 13
#define COMOBJ_btnShortInput 14
#define COMOBJ_btnShortValue 15
#define COMOBJ_btnShortScene 16
#define COMOBJ_btnLongInput 17
#define COMOBJ_btnLongValue 18
#define COMOBJ_btnLongScene 19
#define COMOBJ_btnDimSw 20
#define COMOBJ_btnDim 21
#define COMOBJ_tvocValue 22
#define COMOBJ_owtempValue 23
#define PARAM_initialDelay 0
#define PARAM_tempSendUpdate 1
#define PARAM_tempDiff 2
#define PARAM_tempCycle 3
#define PARAM_tempOffset 4
#define PARAM_tempMinValue 5
#define PARAM_tempMinLimit 6
#define PARAM_tempMaxValue 7
#define PARAM_tempMaxLimit 8
#define PARAM_rhSendUpdate 9
#define PARAM_rhDiff 10
#define PARAM_rhCycle 11
#define PARAM_rhOffset 12
#define PARAM_rhMinValue 13
#define PARAM_rhMinLimit 14
#define PARAM_rhMaxValue 15
#define PARAM_rhMaxLimit 16
#define PARAM_lightSendUpdate 17
#define PARAM_lightDiff 18
#define PARAM_lightCycle 19
#define PARAM_lightOffset 20
#define PARAM_lightMinValue 21
#define PARAM_lightMinLimit 22
#define PARAM_lightMaxValue 23
#define PARAM_lightMaxLimit 24
#define PARAM_vocSendUpdate 25
#define PARAM_vocDiff 26
#define PARAM_vocPollingTime 27
#define PARAM_vocOffset 28
#define PARAM_vocMinValue 29
#define PARAM_vocMinLimit 30
#define PARAM_vocMaxValue 31
#define PARAM_vocMaxLimit 32
#define PARAM_btnType 33
#define PARAM_btnLongClickDuration 34
#define PARAM_btnValueShortClick 35
#define PARAM_btnValueLongClick 36
#define PARAM_binD0Type 37
#define PARAM_binD0delay 38
#define PARAM_binD0open 39
#define PARAM_binD0close 40
#define PARAM_binD1Type 41
#define PARAM_binD1delay 42
#define PARAM_binD1open 43
#define PARAM_binD1close 44
#define PARAM_rgbType 45
#define PARAM_rgbBrightness 46
#define PARAM_rgbColorOnClick 47
#define PARAM_rgbStateShortColor 48
#define PARAM_rgbStateLongColor 49
        
KnxComObject KnxDevice::_comObjectsList[] = {
    /* Index 0 - tempValue */ KnxComObject(KNX_DPT_9_001, 0x34),
    /* Index 1 - tempInputValue */ KnxComObject(KNX_DPT_1_001, 0x2b),
    /* Index 2 - tempSendValue */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 3 - rhValue */ KnxComObject(KNX_DPT_9_007, 0x34),
    /* Index 4 - rhInputValue */ KnxComObject(KNX_DPT_1_001, 0x2b),
    /* Index 5 - rhSendValue */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 6 - luxValue */ KnxComObject(KNX_DPT_9_004, 0x34),
    /* Index 7 - lightMin */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 8 - lightMax */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 9 - lightSwitch */ KnxComObject(KNX_DPT_1_001, 0x2b),
    /* Index 10 - vocValue */ KnxComObject(KNX_DPT_9_008, 0x34),
    /* Index 11 - vocMin */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 12 - vocMax */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 13 - vocSwitch */ KnxComObject(KNX_DPT_1_001, 0x2b),
    /* Index 14 - btnShortInput */ KnxComObject(KNX_DPT_1_001, 0x2b),
    /* Index 15 - btnShortValue */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 16 - btnShortScene */ KnxComObject(KNX_DPT_17_001, 0x34),
    /* Index 17 - btnLongInput */ KnxComObject(KNX_DPT_1_001, 0x2b),
    /* Index 18 - btnLongValue */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 19 - btnLongScene */ KnxComObject(KNX_DPT_17_001, 0x34),
    /* Index 20 - btnDimSw */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 21 - btnDim */ KnxComObject(KNX_DPT_3_007, 0x34),
    /* Index 22 - tvocValue */ KnxComObject(KNX_DPT_9_008, 0x34),
    /* Index 23 - owtempValue */ KnxComObject(KNX_DPT_9_001, 0x34)
};
const byte KnxDevice::_numberOfComObjects = sizeof (_comObjectsList) / sizeof (KnxComObject); // do not change this code
       
byte KonnektingDevice::_paramSizeList[] = {
    /* Index 0 - initialDelay */ PARAM_UINT16,
    /* Index 1 - tempSendUpdate */ PARAM_UINT8,
    /* Index 2 - tempDiff */ PARAM_UINT8,
    /* Index 3 - tempCycle */ PARAM_UINT32,
    /* Index 4 - tempOffset */ PARAM_INT8,
    /* Index 5 - tempMinValue */ PARAM_UINT8,
    /* Index 6 - tempMinLimit */ PARAM_INT16,
    /* Index 7 - tempMaxValue */ PARAM_UINT8,
    /* Index 8 - tempMaxLimit */ PARAM_INT16,
    /* Index 9 - rhSendUpdate */ PARAM_UINT8,
    /* Index 10 - rhDiff */ PARAM_UINT8,
    /* Index 11 - rhCycle */ PARAM_UINT32,
    /* Index 12 - rhOffset */ PARAM_INT8,
    /* Index 13 - rhMinValue */ PARAM_UINT8,
    /* Index 14 - rhMinLimit */ PARAM_INT16,
    /* Index 15 - rhMaxValue */ PARAM_UINT8,
    /* Index 16 - rhMaxLimit */ PARAM_INT16,
    /* Index 17 - lightSendUpdate */ PARAM_UINT8,
    /* Index 18 - lightDiff */ PARAM_UINT8,
    /* Index 19 - lightCycle */ PARAM_UINT32,
    /* Index 20 - lightOffset */ PARAM_INT8,
    /* Index 21 - lightMinValue */ PARAM_UINT8,
    /* Index 22 - lightMinLimit */ PARAM_INT16,
    /* Index 23 - lightMaxValue */ PARAM_UINT8,
    /* Index 24 - lightMaxLimit */ PARAM_INT16,
    /* Index 25 - vocSendUpdate */ PARAM_UINT8,
    /* Index 26 - vocDiff */ PARAM_UINT8,
    /* Index 27 - vocPollingTime */ PARAM_UINT32,
    /* Index 28 - vocOffset */ PARAM_INT8,
    /* Index 29 - lightMinValue */ PARAM_UINT8,
    /* Index 30 - lightMinLimit */ PARAM_INT16,
    /* Index 31 - lightMaxValue */ PARAM_UINT8,
    /* Index 32 - lightMaxLimit */ PARAM_INT16,
    /* Index 33 - btnType */ PARAM_UINT8,
    /* Index 34 - btnLongClickDuration */ PARAM_UINT16,
    /* Index 35 - btnValueShortClick */ PARAM_UINT8,
    /* Index 36 - btnValueLongClick */ PARAM_UINT8,
    /* Index 37 - binD0Type */ PARAM_UINT8,
    /* Index 38 - binD0delay */ PARAM_INT16,
    /* Index 39 - binD0open */ PARAM_UINT8,
    /* Index 40 - binD0close */ PARAM_UINT8,
    /* Index 41 - binD1Type */ PARAM_UINT8,
    /* Index 42 - binD1delay */ PARAM_INT16,
    /* Index 43 - binD1open */ PARAM_UINT8,
    /* Index 44 - binD1close */ PARAM_UINT8,
    /* Index 45 - rgbType */ PARAM_UINT8,
    /* Index 46 - rgbBrightness */ PARAM_UINT8,
    /* Index 47 - rgbColorOnClick */ PARAM_UINT8,
    /* Index 48 - rgbStateShortColor */ PARAM_UINT8,
    /* Index 49 - rgbStateLongColor */ PARAM_UINT8
};
const byte KonnektingDevice::_numberOfParams = sizeof (_paramSizeList); // do not change this codenst byte KonnektingDevice::_numberOfParams = sizeof (_paramSizeList); // do not change this code
