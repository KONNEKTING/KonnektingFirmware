void setup(void) {
  // if debug mode is enabled, setup serial port with 9600 baud
#ifdef DEBUG
  DEBUG.begin(9600);
#endif
  // Initialize KNX enabled Arduino Board
  Tools.init(KNX_SERIAL, PROG_BUTTON_PIN, PROG_LED_PIN, MANUFACTURER_ID, DEVICE_ID, REVISION);

  if (!Tools.isFactorySetting()) {
    byte startDelay = (byte)Tools.getUINT8Param(0);
    if (startDelay < 61 && startDelay > 0) {
#ifdef DEBUG
      DEBUG.print("delay for ");
      DEBUG.print(startDelay);
      DEBUG.println("s");
#endif
      delay(startDelay * 1000);

#ifdef DEBUG
      DEBUG.println("ready!");
#endif
    }
  }
  pinMode(ONE_WIRE_PULLUP, OUTPUT);
  digitalWrite(ONE_WIRE_PULLUP, HIGH);

  cycleTime = (long)Tools.getUINT32Param(1); //polling cycle
  present = (bool)Tools.getUINT8Param(2);    //send "all present"
  one_absent = (bool)Tools.getUINT8Param(3); //send "at least one absent"
  absent = (bool)Tools.getUINT8Param(4);     //send "all absent"
  unreg = (bool)Tools.getUINT8Param(5);      //send "unregistered iButton founded"

  for (byte i = 0; i < iButtons; i++) {
    iButtonActive[i]  = (bool)Tools.getUINT8Param(i * 3 + 6); //input active?
    iButtonInvOutput[i] = (bool)Tools.getUINT8Param(i * 3 + 7); //invert binary output 2: 0=normal|1=inverted
  }
  Tools.getParamValue(8, iButton0);
  Tools.getParamValue(11,iButton1);
  Tools.getParamValue(14,iButton2);
  Tools.getParamValue(17,iButton3);
  Tools.getParamValue(20,iButton4);
  Tools.getParamValue(23,iButton5);
  Tools.getParamValue(26,iButton6);
  Tools.getParamValue(29,iButton7);
  Tools.getParamValue(32,iButton8);
  Tools.getParamValue(35,iButton9);
#ifdef DEBUG
      DEBUG.println("Registered iButtons:");
      DEBUG.println("Nr.  Active  Inverterted  iButton ID");
      DEBUG.print  (" 1     ");
      DEBUG.print  (iButtonActive[0]);
      DEBUG.print  ("          ");
      DEBUG.print  (iButtonInvOutput[0]);
      DEBUG.print  ("       ");
      print_reg_iButton(iButton0);
      DEBUG.print  (" 2     ");
      DEBUG.print  (iButtonActive[1]);
      DEBUG.print  ("          ");
      DEBUG.print  (iButtonInvOutput[1]);
      DEBUG.print  ("       ");
      print_reg_iButton(iButton1);
      DEBUG.print  (" 3     ");
      DEBUG.print  (iButtonActive[2]);
      DEBUG.print  ("          ");
      DEBUG.print  (iButtonInvOutput[2]);
      DEBUG.print  ("       ");
      print_reg_iButton(iButton2);
      DEBUG.print  (" 4     ");
      DEBUG.print  (iButtonActive[3]);
      DEBUG.print  ("          ");
      DEBUG.print  (iButtonInvOutput[3]);
      DEBUG.print  ("       ");
      print_reg_iButton(iButton3);
      DEBUG.print  (" 5     ");
      DEBUG.print  (iButtonActive[4]);
      DEBUG.print  ("          ");
      DEBUG.print  (iButtonInvOutput[4]);
      DEBUG.print  ("       ");
      print_reg_iButton(iButton4);
      DEBUG.print  (" 6     ");
      DEBUG.print  (iButtonActive[5]);
      DEBUG.print  ("          ");
      DEBUG.print  (iButtonInvOutput[5]);
      DEBUG.print  ("       ");
      print_reg_iButton(iButton5);
      DEBUG.print  (" 7     ");
      DEBUG.print  (iButtonActive[6]);
      DEBUG.print  ("          ");
      DEBUG.print  (iButtonInvOutput[6]);
      DEBUG.print  ("       ");
      print_reg_iButton(iButton6);
      DEBUG.print  (" 8     ");
      DEBUG.print  (iButtonActive[7]);
      DEBUG.print  ("          ");
      DEBUG.print  (iButtonInvOutput[7]);
      DEBUG.print  ("       ");
      print_reg_iButton(iButton7);
      DEBUG.print  (" 9     ");
      DEBUG.print  (iButtonActive[8]);
      DEBUG.print  ("          ");
      DEBUG.print  (iButtonInvOutput[8]);
      DEBUG.print  ("       ");
      print_reg_iButton(iButton8);
      DEBUG.print  ("10     ");
      DEBUG.print  (iButtonActive[9]);
      DEBUG.print  ("          ");
      DEBUG.print  (iButtonInvOutput[9]);
      DEBUG.print  ("       ");
      print_reg_iButton(iButton9);
#endif
  
}
