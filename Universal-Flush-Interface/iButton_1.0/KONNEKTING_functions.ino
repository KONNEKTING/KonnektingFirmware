// Definition of the Communication Objects attached to the device
KnxComObject KnxDevice::_comObjectsList[] = {
    /* don't change this */ Tools.createProgComObject(),
                            
    // Currently, Sketch Index and Suite Index differ for ComObjects :-(
                            
    /* Sketch-Index 1, Suite-Index 0 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR),
    /* Sketch-Index 2, Suite-Index 1 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR),
    /* Sketch-Index 3, Suite-Index 2 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR),
    /* Sketch-Index 4, Suite-Index 3 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR),
    /* Sketch-Index 5, Suite-Index 4 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR),
    /* Sketch-Index 6, Suite-Index 5 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR),
    /* Sketch-Index 7, Suite-Index 6 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR),
    /* Sketch-Index 8, Suite-Index 7 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR),
    /* Sketch-Index 9, Suite-Index 8 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR),
    /* Sketch-Index 10,Suite-Index 9 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR),
    /* Sketch-Index 11,Suite-Index 10: */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR),
    /* Sketch-Index 12,Suite-Index 11: */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR),
    /* Sketch-Index 13,Suite-Index 12: */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR),
    /* Sketch-Index 14,Suite-Index 13: */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR),
    /* Sketch-Index 15,Suite-Index 14: */ KnxComObject(KNX_DPT_60000_000, COM_OBJ_SENSOR),
};
const byte KnxDevice::_numberOfComObjects = sizeof (_comObjectsList) / sizeof (KnxComObject); // do no change this code


byte iButtonComObj[] = {1,2,3,4,5,6,7,8,9,10};
byte comObj_present = 11;
byte comObj_one_absent = 12;
byte comObj_absent = 13;
byte comObj_unreg = 14;
byte comObj_unreg_ID = 15;

void knxEvents(byte index) {
    // nothing to do in this sketch
};

// Definition of parameter size
byte KnxTools::_paramSizeList[] = {
    
    // For params, the index in Sketch and Suite is equal
    
    /* Param Index  0 */ PARAM_UINT8,  //Start delay
    /* Param Index  1 */ PARAM_UINT32, //iButton cycle
    /* Param Index  2 */ PARAM_UINT8,  //all present
    /* Param Index  3 */ PARAM_UINT8,  //at least one absent
    /* Param Index  4 */ PARAM_UINT8,  //all absent
    /* Param Index  5 */ PARAM_UINT8,  //all absent
    /* Param Index  6 */ PARAM_UINT8,  //iButton 1 active
    /* Param Index  7 */ PARAM_UINT8,  //iButton 1 invert output
    /* Param Index  8 */ PARAM_RAW8,   //iButton 1 ID
    /* Param Index  9 */ PARAM_UINT8,  //iButton 2 active
    /* Param Index 10 */ PARAM_UINT8,  //iButton 2 invert output
    /* Param Index 11 */ PARAM_RAW8,   //iButton 2 ID
    /* Param Index 12 */ PARAM_UINT8,  //iButton 3 active
    /* Param Index 13 */ PARAM_UINT8,  //iButton 3 invert output
    /* Param Index 14 */ PARAM_RAW8,   //iButton 3 ID
    /* Param Index 15 */ PARAM_UINT8,  //iButton 4 active
    /* Param Index 16 */ PARAM_UINT8,  //iButton 4 invert output
    /* Param Index 17 */ PARAM_RAW8,   //iButton 4 ID
    /* Param Index 18 */ PARAM_UINT8,  //iButton 5 active
    /* Param Index 19 */ PARAM_UINT8,  //iButton 5 invert output
    /* Param Index 20 */ PARAM_RAW8,   //iButton 5 ID
    /* Param Index 21 */ PARAM_UINT8,  //iButton 6 active
    /* Param Index 22 */ PARAM_UINT8,  //iButton 6 invert output
    /* Param Index 23 */ PARAM_RAW8,   //iButton 6 ID
    /* Param Index 24 */ PARAM_UINT8,  //iButton 7 active
    /* Param Index 25 */ PARAM_UINT8,  //iButton 7 invert output
    /* Param Index 26 */ PARAM_RAW8,   //iButton 7 ID
    /* Param Index 27 */ PARAM_UINT8,  //iButton 8 active
    /* Param Index 28 */ PARAM_UINT8,  //iButton 8 invert output
    /* Param Index 29 */ PARAM_RAW8,   //iButton 8 ID
    /* Param Index 30 */ PARAM_UINT8,  //iButton 9 active
    /* Param Index 31 */ PARAM_UINT8,  //iButton 9 invert output
    /* Param Index 32 */ PARAM_RAW8,   //iButton 9 ID
    /* Param Index 33 */ PARAM_UINT8,  //iButton 10 active
    /* Param Index 34 */ PARAM_UINT8,  //iButton 10 invert output
    /* Param Index 35 */ PARAM_RAW8,   //iButton 10 ID
};

// Callback function to handle com objects updates
const byte KnxTools::_numberOfParams = sizeof (_paramSizeList); // do not change this code


