void loop() {
    Knx.task();
    if (Konnekting.isReadyForApplication()) {
    taskButton();
    taskRgb();
#ifdef TRH
    taskTRH();
#endif
#ifdef VOC
    taskVOC();
#endif
#ifdef LIGHT
    taskLight();
#endif
#ifdef ONEWIRE
    taskOneWire();
#endif
         unsigned long currentmillis = millis();
         if (currentmillis - lastmillis >= MEASURE_CYCLE) {
            unsigned long measBegin = millis();
            Debug.println(F("Send values"));
/*
            t = getTemp();
            rh = getRH();
#endif
#ifdef LIGHT
            lux = getLux();
#endif
#ifdef VOC
            voc = getVOC();
#endif
*/

            lastmillis = currentmillis;
#ifdef TRH
            Knx.write(COMOBJ_tempValue, getTemp());
            Knx.write(COMOBJ_rhValue, getRH());
#endif
#ifdef VOC
            Knx.write(COMOBJ_vocValue, getVOC());
            Knx.write(COMOBJ_tvocValue, getTVOC());
#endif
#ifdef LIGHT
            Knx.write(COMOBJ_luxValue, getLux());
#endif
#ifdef ONEWIRE
            Knx.write(COMOBJ_owtempValue, getOneWireTemp());
#endif
        }
#ifdef BINARY

#endif

    }//isReadyForApplication
}
