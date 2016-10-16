#define MANUFACTURER_ID 7070
#define DEVICE_ID 30
#define REVISION 1

#define COMOBJ_A 0
#define COMOBJ_B 1
#define COMOBJ_C 2
#define COMOBJ_D 3
#define COMOBJ_E 4
#define COMOBJ_F 5
#define COMOBJ_G 6
#define COMOBJ_H 7
#define COMOBJ_I 8
#define COMOBJ_J 9
#define PARAM_initialDelay 0
#define PARAM_cyleDelay 1
#define PARAM_inputA_active 2
#define PARAM_inputA_inv 3
#define PARAM_inputA_delay 4
#define PARAM_inputB_active 5
#define PARAM_inputB_inv 6
#define PARAM_inputB_delay 7
#define PARAM_inputC_active 8
#define PARAM_inputC_inv 9
#define PARAM_inputC_delay 10
#define PARAM_inputD_active 11
#define PARAM_inputD_inv 12
#define PARAM_inputD_delay 13
#define PARAM_inputE_active 14
#define PARAM_inputE_inv 15
#define PARAM_inputE_delay 16
#define PARAM_inputF_active 17
#define PARAM_inputF_inv 18
#define PARAM_inputF_delay 19
#define PARAM_inputG_active 20
#define PARAM_inputG_inv 21
#define PARAM_inputG_delay 22
#define PARAM_inputH_active 23
#define PARAM_inputH_inv 24
#define PARAM_inputH_delay 25
#define PARAM_inputI_active 26
#define PARAM_inputI_inv 27
#define PARAM_inputA_delay 28
#define PARAM_inputJ_active 29
#define PARAM_inputJ_inv 30
#define PARAM_inputJ_delay 31
        
KnxComObject KnxDevice::_comObjectsList[] = {
    /* Index 0 - A */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 1 - B */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 2 - C */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 3 - D */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 4 - E */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 5 - F */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 6 - G */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 7 - H */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 8 - I */ KnxComObject(KNX_DPT_1_001, 0x34),
    /* Index 9 - J */ KnxComObject(KNX_DPT_1_001, 0x34)
};
const byte KnxDevice::_numberOfComObjects = sizeof (_comObjectsList) / sizeof (KnxComObject); // do not change this code
       
byte KonnektingDevice::_paramSizeList[] = {
    /* Index 0 - initialDelay */ PARAM_UINT8,
    /* Index 1 - cyleDelay */ PARAM_UINT32,
    /* Index 2 - inputA_active */ PARAM_UINT8,
    /* Index 3 - inputA_inv */ PARAM_UINT8,
    /* Index 4 - inputA_delay */ PARAM_UINT32,
    /* Index 5 - inputB_active */ PARAM_UINT8,
    /* Index 6 - inputB_inv */ PARAM_UINT8,
    /* Index 7 - inputB_delay */ PARAM_UINT32,
    /* Index 8 - inputC_active */ PARAM_UINT8,
    /* Index 9 - inputC_inv */ PARAM_UINT8,
    /* Index 10 - inputC_delay */ PARAM_UINT32,
    /* Index 11 - inputD_active */ PARAM_UINT8,
    /* Index 12 - inputD_inv */ PARAM_UINT8,
    /* Index 13 - inputD_delay */ PARAM_UINT32,
    /* Index 14 - inputE_active */ PARAM_UINT8,
    /* Index 15 - inputE_inv */ PARAM_UINT8,
    /* Index 16 - inputE_delay */ PARAM_UINT32,
    /* Index 17 - inputF_active */ PARAM_UINT8,
    /* Index 18 - inputF_inv */ PARAM_UINT8,
    /* Index 19 - inputF_delay */ PARAM_UINT32,
    /* Index 20 - inputG_active */ PARAM_UINT8,
    /* Index 21 - inputG_inv */ PARAM_UINT8,
    /* Index 22 - inputG_delay */ PARAM_UINT32,
    /* Index 23 - inputH_active */ PARAM_UINT8,
    /* Index 24 - inputH_inv */ PARAM_UINT8,
    /* Index 25 - inputH_delay */ PARAM_UINT32,
    /* Index 26 - inputI_active */ PARAM_UINT8,
    /* Index 27 - inputI_inv */ PARAM_UINT8,
    /* Index 28 - inputA_delay */ PARAM_UINT32,
    /* Index 29 - inputJ_active */ PARAM_UINT8,
    /* Index 30 - inputJ_inv */ PARAM_UINT8,
    /* Index 31 - inputJ_delay */ PARAM_UINT32
};
const byte KonnektingDevice::_numberOfParams = sizeof (_paramSizeList); // do not change this code