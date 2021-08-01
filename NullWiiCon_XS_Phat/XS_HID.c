#include "XS_HID.h"
static USB_JoystickReport_Input_t SwitchReportData;
static USB_JoystickReport_XInput_t XInputSwitchReportData;

// Configures hardware and peripherals, such as the USB peripherals.
void SetupHardware(bool xinput_mode) {

  //set xinput_mode for descriptors.h/.c
  xs_xinput = xinput_mode;
  desc_set_xinput_mode(xinput_mode);
  // We need to disable watchdog if enabled by bootloader/fuses.
  MCUSR &= ~(1 << WDRF);
  wdt_disable();
  // We need to disable clock division before initializing the USB hardware.
  clock_prescale_set(clock_div_1);
  // We can then initialize our hardware and peripherals, including the USB stack.
  // The USB stack should be initialized last.
  USB_Init();
}

// Fired to indicate that the device is enumerating.
void EVENT_USB_Device_Connect(void) {
  // We can indicate that we're enumerating here (via status LEDs, sound, etc.).
}

// Fired to indicate that the device is no longer connected to a host.
void EVENT_USB_Device_Disconnect(void) {
  // We can indicate that our device is not ready (via status LEDs, sound, etc.).
}

// Fired when the host set the current configuration of the USB device after enumeration.
void EVENT_USB_Device_ConfigurationChanged(void) {

  if (!xs_xinput) Endpoint_ConfigureEndpoint(JOYSTICK_OUT_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);
  else Endpoint_ConfigureEndpoint((ENDPOINT_DIR_IN | 3), EP_TYPE_INTERRUPT, 32, 1);

  Endpoint_ConfigureEndpoint(JOYSTICK_IN_EPADDR, EP_TYPE_INTERRUPT, xs_xinput ? JOYSTICK_EPSIZE_XINPUT : JOYSTICK_EPSIZE, 1);

}

// Process control requests sent to the device from the USB host.
void EVENT_USB_Device_ControlRequest(void) {
  PORTB ^= (1 << (0));
  //No controlRequest received from the switch, so only handled in xinput mode
  if (xs_xinput) {
    /* Handle HID Class specific requests */
    switch (USB_ControlRequest.bRequest) {
      case HID_REQ_GetReport:
        if (USB_ControlRequest.bmRequestType
            == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE)) {
          Endpoint_ClearSETUP();

          /* Write the report data to the control endpoint */
          Endpoint_Write_Control_Stream_LE(&XInputSwitchReportData, 20);
          Endpoint_ClearOUT();
        }
        break;
    }
  }
}

// Process and deliver data from IN and OUT endpoints.
void HID_Task(void) {
  /* Device must be connected and configured for the task to run */
  if (USB_DeviceState != DEVICE_STATE_Configured)
    return;

  void* Address = &XInputSwitchReportData;
  uint16_t    Size    = 20;

  //no OUT endpoint for xinput in this firmware
  if (!xs_xinput) {
    Address = &SwitchReportData;
    Size = sizeof(SwitchReportData);

    // We'll start with the OUT endpoint.
    Endpoint_SelectEndpoint(JOYSTICK_OUT_EPADDR);
    // We'll check to see if we received something on the OUT endpoint.
    if (Endpoint_IsOUTReceived())
    {
      // If we did, and the packet has data, we'll react to it.
      if (Endpoint_IsReadWriteAllowed())
      {
        // We'll create a place to store our data received from the host.
        USB_JoystickReport_Output_t JoystickOutputData;
        // We'll then take in that data, setting it up in our storage.
        Endpoint_Read_Stream_LE(&JoystickOutputData, sizeof(JoystickOutputData), NULL);
        // At this point, we can react to this data.
        // However, since we're not doing anything with this data, we abandon it.
      }
      // Regardless of whether we reacted to the data, we acknowledge an OUT packet on this endpoint.
      Endpoint_ClearOUT();
    }
  }
  /* Select the Joystick Report Endpoint */
  Endpoint_SelectEndpoint(JOYSTICK_IN_EPADDR);

  /* Check to see if the host is ready for another packet */
  if (Endpoint_IsINReady()) {
    /* Write Joystick Report Data */
    bit_toggle(PORTD, 5);
    Endpoint_Write_Stream_LE(Address, Size, NULL);

    /* Finalize the stream transfer to send the last packet */
    Endpoint_ClearIN();
    /* Clear the report data afterwards */
    memset(Address, 0, Size);
    if (xs_xinput) (*((USB_JoystickReport_XInput_t *) Address)).rsize = 20;
  }
}

void send_pad_state(void) {
  generate_report();
  HID_Task();
  USB_USBTask();
}

void generate_report_xinput() {
  // HAT
  if (buttonStatus[BUTTONUP]) {
    XInputSwitchReportData.digital_buttons_1 |= XBOX_DPAD_UP;
  }
  if (buttonStatus[BUTTONDOWN]) {
    XInputSwitchReportData.digital_buttons_1 |= XBOX_DPAD_DOWN;
  }
  if (buttonStatus[BUTTONLEFT]) {
    XInputSwitchReportData.digital_buttons_1 |= XBOX_DPAD_LEFT;
  }
  if (buttonStatus[BUTTONRIGHT]) {
    XInputSwitchReportData.digital_buttons_1 |= XBOX_DPAD_RIGHT;
  }

  // analogs
  XInputSwitchReportData.l_x = LeftX * 257 + -32768;
  XInputSwitchReportData.l_y = LeftY * -257 + 32767;
  XInputSwitchReportData.r_x = RightX * 257 + -32768;
  XInputSwitchReportData.r_y = RightY * -257 + 32767;

  // buttons
  if (buttonStatus[BUTTONSTART]) {
    XInputSwitchReportData.digital_buttons_1 |= XBOX_START;
  }
  if (buttonStatus[BUTTONSELECT]) {
    XInputSwitchReportData.digital_buttons_1 |= XBOX_BACK;
  }
  if (buttonStatus[BUTTONL3]) {
    XInputSwitchReportData.digital_buttons_1 |= XBOX_LEFT_STICK;
  }
  if (buttonStatus[BUTTONR3]) {
    XInputSwitchReportData.digital_buttons_1 |= XBOX_RIGHT_STICK;
  }

  /* xbox buttons are not in the same position as nintendo controllers */
  if (buttonStatus[BUTTONA]) {
    XInputSwitchReportData.digital_buttons_2 |= XBOX_B;
  }
  if (buttonStatus[BUTTONB]) {
    XInputSwitchReportData.digital_buttons_2 |= XBOX_A;
  }
  if (buttonStatus[BUTTONX]) {
    XInputSwitchReportData.digital_buttons_2 |= XBOX_Y;
  }
  if (buttonStatus[BUTTONY]) {
    XInputSwitchReportData.digital_buttons_2 |= XBOX_X;
  }

  if (buttonStatus[BUTTONLB]) {
    XInputSwitchReportData.digital_buttons_2 |= XBOX_LB;
  }
  if (buttonStatus[BUTTONRB]) {
    XInputSwitchReportData.digital_buttons_2 |= XBOX_RB;
  }
  if (buttonStatus[BUTTONHOME]) {
    XInputSwitchReportData.digital_buttons_2 |= XBOX_HOME;
  }

  if (buttonStatus[BUTTONZL]) {
    XInputSwitchReportData.lt = 0xFF;
  }
  if (buttonStatus[BUTTONZR]) {
    XInputSwitchReportData.rt = 0xFF;
  }
}

void generate_report_switch() {
  // HAT
  if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONRIGHT])) {
    SwitchReportData.HAT = DPAD_UPRIGHT_MASK_ON;
  }
  else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONRIGHT])) {
    SwitchReportData.HAT = DPAD_DOWNRIGHT_MASK_ON;
  }
  else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONLEFT])) {
    SwitchReportData.HAT = DPAD_DOWNLEFT_MASK_ON;
  }
  else if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONLEFT])) {
    SwitchReportData.HAT = DPAD_UPLEFT_MASK_ON;
  }
  else if (buttonStatus[BUTTONUP]) {
    SwitchReportData.HAT = DPAD_UP_MASK_ON;
  }
  else if (buttonStatus[BUTTONDOWN]) {
    SwitchReportData.HAT = DPAD_DOWN_MASK_ON;
  }
  else if (buttonStatus[BUTTONLEFT]) {
    SwitchReportData.HAT = DPAD_LEFT_MASK_ON;
  }
  else if (buttonStatus[BUTTONRIGHT]) {
    SwitchReportData.HAT = DPAD_RIGHT_MASK_ON;
  }
  else {
    SwitchReportData.HAT = DPAD_NOTHING_MASK_ON;
  }

  // analogs
  SwitchReportData.LX = LeftX;
  SwitchReportData.LY = LeftY;
  SwitchReportData.RX = RightX;
  SwitchReportData.RY = RightY;

  // Buttons
  bit_writemask(buttonStatus[BUTTONA], SwitchReportData.Button, A_MASK_ON);
  bit_writemask(buttonStatus[BUTTONB], SwitchReportData.Button, B_MASK_ON);
  bit_writemask(buttonStatus[BUTTONX], SwitchReportData.Button, X_MASK_ON);
  bit_writemask(buttonStatus[BUTTONY], SwitchReportData.Button, Y_MASK_ON);
  bit_writemask(buttonStatus[BUTTONLB], SwitchReportData.Button, LB_MASK_ON);
  bit_writemask(buttonStatus[BUTTONRB], SwitchReportData.Button, RB_MASK_ON);
  bit_writemask(buttonStatus[BUTTONZL], SwitchReportData.Button, ZL_MASK_ON);
  bit_writemask(buttonStatus[BUTTONZR], SwitchReportData.Button, ZR_MASK_ON);
  bit_writemask(buttonStatus[BUTTONSTART], SwitchReportData.Button,
            START_MASK_ON);
  bit_writemask(buttonStatus[BUTTONSELECT], SwitchReportData.Button,
            SELECT_MASK_ON);
  bit_writemask(buttonStatus[BUTTONHOME], SwitchReportData.Button, HOME_MASK_ON);
  bit_writemask(buttonStatus[BUTTONCAPTURE], SwitchReportData.Button,
            CAPTURE_MASK_ON);
  bit_writemask(buttonStatus[BUTTONL3], SwitchReportData.Button, L3_MASK_ON);
  bit_writemask(buttonStatus[BUTTONR3], SwitchReportData.Button,
            R3_MASK_ON);
}

static void generate_report() {
  if (xs_xinput) generate_report_xinput();
  else generate_report_switch();
}
