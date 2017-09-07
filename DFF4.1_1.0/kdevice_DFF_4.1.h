#define MANUFACTURER_ID 49374
#define DEVICE_ID 1
#define REVISION 0

#define COMOBJ_centralWindowOpenClose 0
#define COMOBJ_centralWindowStop 1
#define COMOBJ_centralWindowAbsPosition 2
#define COMOBJ_centralShutterOpenClose 3
#define COMOBJ_centralShutterStop 4
#define COMOBJ_centralShutterAbsPosition 5
#define COMOBJ_centralShutterLock 6
#define COMOBJ_centralVentilation 7
#define COMOBJ_abOpenClose 8
#define COMOBJ_abShortStop 9
#define COMOBJ_abStop 10
#define COMOBJ_abAbsPosition 11
#define COMOBJ_abReference 12
#define COMOBJ_abFixPosition 13
#define COMOBJ_abVentilation 14
#define COMOBJ_abWindAlarm 15
#define COMOBJ_abRainAlarm 16
#define COMOBJ_abStatusCurrentDirection 17
#define COMOBJ_abStatusMovement 18
#define COMOBJ_abStatusMovementOpen 19
#define COMOBJ_abStatusMovementClose 20
#define COMOBJ_abStatusCurrentPos 21
#define COMOBJ_abStatusLock 22
#define COMOBJ_abStatusOpenPos 23
#define COMOBJ_abStatusClosePos 24
#define PARAM_startupDelay 0
#define PARAM_triggerTime 1
#define PARAM_manualControl 2
#define PARAM_ventilationTime 3
#define PARAM_setting_channel_ab 4
#define PARAM_setting_channel_cd 5
#define PARAM_setting_channel_ef 6
#define PARAM_setting_channel_gh 7
#define PARAM_channel_runTimeOpen 8
#define PARAM_channel_runTimeClose 9
#define PARAM_channel_runTimeRollover 10
#define PARAM_channel_shortTimeRun 11
#define PARAM_channel_shortTimeRunTime 12
#define PARAM_channel_lockAction 13
#define PARAM_channel_ventByComObj 14
#define PARAM_channel_absPositionComObj 15
#define PARAM_channel_referenceRunComObj 16
#define PARAM_channel_runStatusComObj 17
#define PARAM_channel_absPosStatusComObj 18
#define PARAM_channel_runStatusPositionComObj 19

#ifdef KONNEKTING_h      
KnxComObject KnxDevice::_comObjectsList[] = {
    /* Index 0 - centralWindowOpenClose */ KnxComObject(KNX_DPT_1_007, 0x2a),
    /* Index 1 - centralWindowStop */ KnxComObject(KNX_DPT_1_001, 0x2a),
    /* Index 2 - centralWindowAbsPosition */ KnxComObject(KNX_DPT_5_001, 0x2a),
    /* Index 3 - centralShutterOpenClose */ KnxComObject(KNX_DPT_1_007, 0x2a),
    /* Index 4 - centralShutterStop */ KnxComObject(KNX_DPT_1_001, 0x2a),
    /* Index 5 - centralShutterAbsPosition */ KnxComObject(KNX_DPT_5_001, 0x2a),
    /* Index 6 - centralShutterLock */ KnxComObject(KNX_DPT_1_001, 0x2a),
    /* Index 7 - centralVentilation */ KnxComObject(KNX_DPT_1_001, 0x2a),
    /* Index 8 - abOpenClose */ KnxComObject(KNX_DPT_1_007, 0x2a),
    /* Index 9 - abShortStop */ KnxComObject(KNX_DPT_1_007, 0x2a),
    /* Index 10 - abStop */ KnxComObject(KNX_DPT_1_007, 0x2a),
    /* Index 11 - abAbsPosition */ KnxComObject(KNX_DPT_5_001, 0x2a),
    /* Index 12 - abReference */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 13 - abFixPosition */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 14 - abVentilation */ KnxComObject(KNX_DPT_1_001, 0x2a),
    /* Index 15 - abWindAlarm */ KnxComObject(KNX_DPT_1_005, 0x2a),
    /* Index 16 - abRainAlarm */ KnxComObject(KNX_DPT_1_005, 0x2a),
    /* Index 17 - abStatusCurrentDirection */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 18 - abStatusMovement */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 19 - abStatusMovementOpen */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 20 - abStatusMovementClose */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 21 - abStatusCurrentPos */ KnxComObject(KNX_DPT_5_001, 0x2a),
    /* Index 22 - abStatusLock */ KnxComObject(KNX_DPT_1_011, 0x2a),
    /* Index 23 - abStatusOpenPos */ KnxComObject(KNX_DPT_1_001, 0x2a),
    /* Index 24 - abStatusClosePos */ KnxComObject(KNX_DPT_1_001, 0x2a),
    /* Index 25 - null */ KnxComObject(KNX_DPT_1_007, 0x2a),
    /* Index 26 - null */ KnxComObject(KNX_DPT_1_007, 0x2a),
    /* Index 27 - null */ KnxComObject(KNX_DPT_1_007, 0x2a),
    /* Index 28 - null */ KnxComObject(KNX_DPT_5_001, 0x2a),
    /* Index 29 - null */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 30 - null */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 31 - null */ KnxComObject(KNX_DPT_1_001, 0x2a),
    /* Index 32 - null */ KnxComObject(KNX_DPT_1_005, 0x2a),
    /* Index 33 - null */ KnxComObject(KNX_DPT_1_005, 0x2a),
    /* Index 34 - null */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 35 - null */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 36 - null */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 37 - null */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 38 - null */ KnxComObject(KNX_DPT_5_001, 0x2a),
    /* Index 39 - null */ KnxComObject(KNX_DPT_1_011, 0x2a),
    /* Index 40 - null */ KnxComObject(KNX_DPT_1_001, 0x2a),
    /* Index 41 - null */ KnxComObject(KNX_DPT_1_001, 0x2a),
    /* Index 42 - null */ KnxComObject(KNX_DPT_1_007, 0x2a),
    /* Index 43 - null */ KnxComObject(KNX_DPT_1_007, 0x2a),
    /* Index 44 - null */ KnxComObject(KNX_DPT_1_007, 0x2a),
    /* Index 45 - null */ KnxComObject(KNX_DPT_5_001, 0x2a),
    /* Index 46 - null */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 47 - null */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 48 - null */ KnxComObject(KNX_DPT_1_001, 0x2a),
    /* Index 49 - null */ KnxComObject(KNX_DPT_1_005, 0x2a),
    /* Index 50 - null */ KnxComObject(KNX_DPT_1_005, 0x2a),
    /* Index 51 - null */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 52 - null */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 53 - null */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 54 - null */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 55 - null */ KnxComObject(KNX_DPT_5_001, 0x2a),
    /* Index 56 - null */ KnxComObject(KNX_DPT_1_011, 0x2a),
    /* Index 57 - null */ KnxComObject(KNX_DPT_1_001, 0x2a),
    /* Index 58 - null */ KnxComObject(KNX_DPT_1_001, 0x2a),
    /* Index 59 - null */ KnxComObject(KNX_DPT_1_007, 0x2a),
    /* Index 60 - null */ KnxComObject(KNX_DPT_1_007, 0x2a),
    /* Index 61 - null */ KnxComObject(KNX_DPT_1_007, 0x2a),
    /* Index 62 - null */ KnxComObject(KNX_DPT_5_001, 0x2a),
    /* Index 63 - null */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 64 - null */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 65 - null */ KnxComObject(KNX_DPT_1_001, 0x2a),
    /* Index 66 - null */ KnxComObject(KNX_DPT_1_005, 0x2a),
    /* Index 67 - null */ KnxComObject(KNX_DPT_1_005, 0x2a),
    /* Index 68 - null */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 69 - null */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 70 - null */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 71 - null */ KnxComObject(KNX_DPT_1_008, 0x2a),
    /* Index 72 - null */ KnxComObject(KNX_DPT_5_001, 0x2a),
    /* Index 73 - null */ KnxComObject(KNX_DPT_1_011, 0x2a),
    /* Index 74 - null */ KnxComObject(KNX_DPT_1_001, 0x2a),
    /* Index 75 - null */ KnxComObject(KNX_DPT_1_001, 0x2a)
};
const byte KnxDevice::_numberOfComObjects = sizeof (_comObjectsList) / sizeof (KnxComObject); // do not change this code
       
byte KonnektingDevice::_paramSizeList[] = {
    /* Index 0 - startupDelay */ PARAM_UINT8,
    /* Index 1 - triggerTime */ PARAM_UINT8,
    /* Index 2 - manualControl */ PARAM_UINT8,
    /* Index 3 - ventilationTime */ PARAM_UINT8,
    /* Index 4 - setting_channel_ab */ PARAM_UINT8,
    /* Index 5 - setting_channel_cd */ PARAM_UINT8,
    /* Index 6 - setting_channel_ef */ PARAM_UINT8,
    /* Index 7 - setting_channel_gh */ PARAM_UINT8,
    /* Index 8 - channel_runTimeOpen */ PARAM_UINT8,
    /* Index 9 - channel_runTimeClose */ PARAM_UINT8,
    /* Index 10 - channel_runTimeRollover */ PARAM_UINT8,
    /* Index 11 - channel_shortTimeRun */ PARAM_UINT8,
    /* Index 12 - channel_shortTimeRunTime */ PARAM_UINT8,
    /* Index 13 - channel_lockAction */ PARAM_UINT8,
    /* Index 14 - channel_ventByComObj */ PARAM_UINT8,
    /* Index 15 - channel_absPositionComObj */ PARAM_UINT8,
    /* Index 16 - channel_referenceRunComObj */ PARAM_UINT8,
    /* Index 17 - channel_runStatusComObj */ PARAM_UINT8,
    /* Index 18 - channel_absPosStatusComObj */ PARAM_UINT8,
    /* Index 19 - channel_runStatusPositionComObj */ PARAM_UINT8,
    /* Index 20 - null */ PARAM_UINT8,
    /* Index 21 - null */ PARAM_UINT8,
    /* Index 22 - null */ PARAM_UINT8,
    /* Index 23 - null */ PARAM_UINT8,
    /* Index 24 - null */ PARAM_UINT8,
    /* Index 25 - null */ PARAM_UINT8,
    /* Index 26 - null */ PARAM_UINT8,
    /* Index 27 - null */ PARAM_UINT8,
    /* Index 28 - null */ PARAM_UINT8,
    /* Index 29 - null */ PARAM_UINT8,
    /* Index 30 - null */ PARAM_UINT8,
    /* Index 31 - null */ PARAM_UINT8,
    /* Index 32 - null */ PARAM_UINT8,
    /* Index 33 - null */ PARAM_UINT8,
    /* Index 34 - null */ PARAM_UINT8,
    /* Index 35 - null */ PARAM_UINT8,
    /* Index 36 - null */ PARAM_UINT8,
    /* Index 37 - null */ PARAM_UINT8,
    /* Index 38 - null */ PARAM_UINT8,
    /* Index 39 - null */ PARAM_UINT8,
    /* Index 40 - null */ PARAM_UINT8,
    /* Index 41 - null */ PARAM_UINT8,
    /* Index 42 - null */ PARAM_UINT8,
    /* Index 43 - null */ PARAM_UINT8,
    /* Index 44 - null */ PARAM_UINT8,
    /* Index 45 - null */ PARAM_UINT8,
    /* Index 46 - null */ PARAM_UINT8,
    /* Index 47 - null */ PARAM_UINT8,
    /* Index 48 - null */ PARAM_UINT8,
    /* Index 49 - null */ PARAM_UINT8,
    /* Index 50 - null */ PARAM_UINT8,
    /* Index 51 - null */ PARAM_UINT8,
    /* Index 52 - null */ PARAM_UINT8,
    /* Index 53 - null */ PARAM_UINT8,
    /* Index 54 - null */ PARAM_UINT8,
    /* Index 55 - null */ PARAM_UINT8
};
const byte KonnektingDevice::_numberOfParams = sizeof (_paramSizeList); // do not change this code
#endif