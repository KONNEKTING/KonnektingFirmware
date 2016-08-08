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
};

const byte KnxDevice::_numberOfComObjects = sizeof (_comObjectsList) / sizeof (KnxComObject); // do no change this code

// Definition of parameter size
byte KnxTools::_paramSizeList[] = {
    
    // For params, the index in Sketch and Suite is equal
    
    /* Param Index  0 */ PARAM_UINT8,  //Start delay
    /* Param Index  1 */ PARAM_UINT32, //Binary cycle
    /* Param Index  2 */ PARAM_UINT8,  //Input A
    /* Param Index  3 */ PARAM_UINT8,  //Input A output
    /* Param Index  4 */ PARAM_UINT32, //Input A delay
    /* Param Index  5 */ PARAM_UINT8,  //Input B
    /* Param Index  6 */ PARAM_UINT8,  //Input B output
    /* Param Index  7 */ PARAM_UINT32, //Input B delay
    /* Param Index  8 */ PARAM_UINT8,  //Input C
    /* Param Index  9 */ PARAM_UINT8,  //Input C output
    /* Param Index 10 */ PARAM_UINT32, //Input C delay
    /* Param Index 11 */ PARAM_UINT8,  //Input D
    /* Param Index 12 */ PARAM_UINT8,  //Input D output
    /* Param Index 13 */ PARAM_UINT32, //Input D delay
    /* Param Index 14 */ PARAM_UINT8,  //Input E
    /* Param Index 15 */ PARAM_UINT8,  //Input E output
    /* Param Index 16 */ PARAM_UINT32, //Input E delay
    /* Param Index 17 */ PARAM_UINT8,  //Input E
    /* Param Index 18 */ PARAM_UINT8,  //Input F output
    /* Param Index 19 */ PARAM_UINT32, //Input F delay
    /* Param Index 20 */ PARAM_UINT8,  //Input G
    /* Param Index 21 */ PARAM_UINT8,  //Input G output
    /* Param Index 22 */ PARAM_UINT32, //Input G delay
    /* Param Index 23 */ PARAM_UINT8,  //Input H
    /* Param Index 24 */ PARAM_UINT8,  //Input H output
    /* Param Index 25 */ PARAM_UINT32, //Input H delay
    /* Param Index 26 */ PARAM_UINT8,  //Input I
    /* Param Index 27 */ PARAM_UINT8,  //Input I output
    /* Param Index 28 */ PARAM_UINT32, //Input I delay
    /* Param Index 29 */ PARAM_UINT8,  //Input J
    /* Param Index 30 */ PARAM_UINT8,  //Input J output
    /* Param Index 31 */ PARAM_UINT32, //Input J delay
};
const byte KnxTools::_numberOfParams = sizeof (_paramSizeList); // do not change this code

// Callback function to handle com objects updates

void knxEvents(byte index) {
    // nothing to do in this sketch
};
