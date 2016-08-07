// Definition of the Communication Objects attached to the device
KnxComObject KnxDevice::_comObjectsList[] = {
    /* don't change this */ Tools.createProgComObject(),
                            
    // Currently, Sketch Index and Suite Index differ for ComObjects :-(
                            
    /* Sketch-Index 1, Suite-Index 0 : */ KnxComObject(KNX_DPT_9_001, COM_OBJ_SENSOR),
    /* Sketch-Index 2, Suite-Index 1 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR),
    /* Sketch-Index 3, Suite-Index 2 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR),
    /* Sketch-Index 4, Suite-Index 3 : */ KnxComObject(KNX_DPT_9_007, COM_OBJ_SENSOR),
    /* Sketch-Index 5, Suite-Index 4 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR),
    /* Sketch-Index 6, Suite-Index 5 : */ KnxComObject(KNX_DPT_1_001, COM_OBJ_SENSOR),
};

#define comObj_Temp 1
#define comObj_TempMin 2
#define comObj_TempMax 3
#define comObj_RH 4
#define comObj_RHMin 5
#define comObj_RHMax 6

const byte KnxDevice::_numberOfComObjects = sizeof (_comObjectsList) / sizeof (KnxComObject); // do no change this code

// Definition of parameter size
byte KnxTools::_paramSizeList[] = {
    
    // For params, the index in Sketch and Suite is equal
    
    /* Param Index 0 */ PARAM_UINT8,
    /* Param Index 1 */ PARAM_UINT8,
    /* Param Index 2 */ PARAM_UINT32,
    /* Param Index 3 */ PARAM_UINT8,
    /* Param Index 4 */ PARAM_UINT8,
    /* Param Index 5 */ PARAM_INT16,
    /* Param Index 6 */ PARAM_UINT8,
    /* Param Index 7 */ PARAM_INT16,
    /* Param Index 8 */ PARAM_UINT8,
    /* Param Index 9 */ PARAM_UINT32,
    /* Param Index 10 */ PARAM_UINT8,
    /* Param Index 11 */ PARAM_UINT8,
    /* Param Index 12 */ PARAM_INT16,
    /* Param Index 13 */ PARAM_UINT8,
    /* Param Index 14 */ PARAM_INT16
};
const byte KnxTools::_numberOfParams = sizeof (_paramSizeList); // do no change this code

// Callback function to handle com objects updates

void knxEvents(byte index) {
    // nothing to do in this sketch
};
