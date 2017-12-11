void setup() {
    kinit();
    initRGB();
    initButton();

#ifdef TRH
    initTempRH();
#endif
#ifdef LIGHT
    initLight();
#endif
#ifdef VOC
    initVOC();
#endif
  
    //ab hier KNX Sachen
    if (!Konnekting.isFactorySetting()) {
        int startDelay = (int) Konnekting.getUINT16Param(PARAM_initialDelay);
        if (startDelay > 0) {
            Debug.println(F("delay for %d ms"), startDelay);
            delay(3000);
            Debug.println(F("ready!"));
        }
        typeTemp = (int) Konnekting.getUINT8Param(PARAM_tempSendUpdate);
        intervalTempUser = (long) Konnekting.getUINT32Param(PARAM_tempCycle) * 1000;
        diffTempUser = (float) Konnekting.getUINT8Param(PARAM_tempDiff) * 0.1;
        offsetTemp   = (float) Konnekting.getINT8Param(PARAM_tempOffset) * 0.1;
        valueTempMin = Konnekting.getUINT8Param(PARAM_tempMinValue);
        limitTempMin = (float) Konnekting.getINT16Param(PARAM_tempMinLimit) * 0.1;
        valueTempMax = Konnekting.getUINT8Param(PARAM_tempMaxValue);
        limitTempMax = (float) Konnekting.getINT16Param(PARAM_tempMaxLimit) * 0.1;
    
        typeHumd = Konnekting.getUINT8Param(PARAM_rhSendUpdate);
        intervalHumdUser = (long) Konnekting.getUINT32Param(PARAM_rhCycle) * 1000;
        diffHumdUser = (float) Konnekting.getUINT8Param(PARAM_rhDiff) * 0.1;
        offsetHumd   = (float) Konnekting.getINT8Param(PARAM_rhOffset) * 0.1;
        valueHumdMin = Konnekting.getUINT8Param(PARAM_rhMinValue);
        limitHumdMin = (float) Konnekting.getINT16Param(PARAM_rhMinLimit) * 0.1;
        valueHumdMax = Konnekting.getUINT8Param(PARAM_rhMaxValue);
        limitHumdMax = (float) Konnekting.getINT16Param(PARAM_rhMaxLimit) * 0.1;
/*
        typeOwTemp = (int) Konnekting.getUINT8Param(PARAM_tempSendUpdate);
        intervalOwTempUser = (long) Konnekting.getUINT32Param(PARAM_tempCycle) * 1000;
        diffOwTempUser = (float) Konnekting.getUINT8Param(PARAM_tempDiff) * 0.1;
        offsetOwTemp   = (float) Konnekting.getINT8Param(PARAM_tempOffset) * 0.1;
        valueOwTempMin = Konnekting.getUINT8Param(PARAM_tempMinValue);
        limitOwTempMin = (float) Konnekting.getINT16Param(PARAM_tempMinLimit) * 0.1;
        valueOwTempMax = Konnekting.getUINT8Param(PARAM_tempMaxValue);
        limitOwTempMax = (float) Konnekting.getINT16Param(PARAM_tempMaxLimit) * 0.1;
  */     
        offsetLux = (int) Konnekting.getINT8Param(PARAM_lightOffset);
         
        typeBtn = (int) Konnekting.getUINT8Param(PARAM_btnType);
        longClickDurationBtn = (int) Konnekting.getUINT16Param(PARAM_btnLongClickDuration);
        valueShortClickBtn = (int) Konnekting.getUINT8Param(PARAM_btnValueShortClick);
        valueLongClickBtn = (int) Konnekting.getUINT8Param(PARAM_btnValueLongClick);
  
        typeRgb = Konnekting.getUINT8Param(PARAM_rgbType);
        brightnessTypeRgb = Konnekting.getUINT8Param(PARAM_rgbBrightness);
        colorOnClickRgb = Konnekting.getUINT8Param(PARAM_rgbColorOnClick);
        colorStateShortRgb = Konnekting.getUINT8Param(PARAM_rgbStateShortColor);
        colorStateLongRgb = Konnekting.getUINT8Param(PARAM_rgbStateLongColor);
    
    
    }
    
#ifdef KDEBUG
    printConfig();
#endif
    Debug.println(F("Setup is ready. go to loop..."));
}
