#include "Joystick.h"

#include "Arduino.h"
USB_SwitchJoystickReport_Input_t SwitchReportData;
USB_XInputJoystickReport_Data_t XInputReportData;

// Configures hardware and peripherals, such as the USB peripherals.
void SetupHardware(void) {
  // We need to disable watchdog if enabled by bootloader/fuses.
  MCUSR &= ~(1 << WDRF);
  wdt_disable();
  // We need to disable clock division before initializing the USB hardware.
  clock_prescale_set(clock_div_1);
  // We can then initialize our hardware and peripherals, including the USB
  // stack. The USB stack should be initialized last.
  USB_Init();
}

// Fired to indicate that the device is enumerating.
void EVENT_USB_Device_Connect(void) {
  // We can indicate that we're enumerating here (via status LEDs, sound, etc.).
}

// Fired to indicate that the device is no longer connected to a host.
void EVENT_USB_Device_Disconnect(void) {
  // We can indicate that our device is not ready (via status LEDs, sound,
  // etc.).
}

// Fired when the host set the current configuration of the USB device after
// enumeration.
void EVENT_USB_Device_ConfigurationChanged(void) {
  bool ConfigSuccess = true;

  // We setup the HID report endpoints.
  if (nintendoSwitchMode) {
    ConfigSuccess &= Endpoint_ConfigureEndpoint(
        JOYSTICK_OUT_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);
    ConfigSuccess &= Endpoint_ConfigureEndpoint(
        JOYSTICK_IN_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);
  } else {
    ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_IN_EPADDR,
                                                EP_TYPE_INTERRUPT, 20, 1);
    ConfigSuccess &= Endpoint_ConfigureEndpoint((ENDPOINT_DIR_IN | 3),
                                                EP_TYPE_INTERRUPT, 32, 1);
  }

  // We can read ConfigSuccess to indicate a success or failure at this point.
}

// Process control requests sent to the device from the USB host.
void EVENT_USB_Device_ControlRequest(void) {
  // We can handle two control requests: a GetReport and a SetReport.
  PORTB ^= (1 << (0));
  if (nintendoSwitchMode) {
    // Not used here, it looks like we don't receive control request from the
    // Switch.
  } else {
    switch (USB_ControlRequest.bRequest) {
      case HID_REQ_GetReport:
        if (USB_ControlRequest.bmRequestType ==
            (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE)) {
          Endpoint_ClearSETUP();

          /* Write the report data to the control endpoint */
          Endpoint_Write_Control_Stream_LE(&XInputReportData, 20);
          Endpoint_ClearOUT();
        }

        break;
    }
  }
}

// Process and deliver data from IN and OUT endpoints.
void HID_Task(void) {
  // If the device isn't connected and properly configured, we can't do anything
  // here.
  if (USB_DeviceState != DEVICE_STATE_Configured) return;

  if (nintendoSwitchMode) {
    // We'll start with the OUT endpoint.
    Endpoint_SelectEndpoint(JOYSTICK_OUT_EPADDR);
    // We'll check to see if we received something on the OUT endpoint.
    if (Endpoint_IsOUTReceived()) {
      // If we did, and the packet has data, we'll react to it.
      if (Endpoint_IsReadWriteAllowed()) {
        // We'll create a place to store our data received from the host.
        USB_SwitchJoystickReport_Output_t JoystickOutputData;
        // We'll then take in that data, setting it up in our storage.
        Endpoint_Read_Stream_LE(&JoystickOutputData, sizeof(JoystickOutputData),
                                NULL);
        // At this point, we can react to this data.
        // However, since we're not doing anything with this data, we abandon
        // it.
      }
      // Regardless of whether we reacted to the data, we acknowledge an OUT
      // packet on this endpoint.
      Endpoint_ClearOUT();
    }
  }

  // We'll then move on to the IN endpoint.
  Endpoint_SelectEndpoint(JOYSTICK_IN_EPADDR);
  // We first check to see if the host is ready to accept data.
  if (Endpoint_IsINReady()) {
    // Once populated, we can output this data to the host. We do this by first
    // writing the data to the control stream.
    if (nintendoSwitchMode) {
      Endpoint_Write_Stream_LE(&SwitchReportData, sizeof(SwitchReportData),
                               NULL);
      memset(&SwitchReportData, 0, sizeof(SwitchReportData));
    } else {
      Endpoint_Write_Stream_LE(&XInputReportData, sizeof(XInputReportData),
                               NULL);
      
    }
    // We then send an IN packet on this endpoint.
    Endpoint_ClearIN();
    /* Clear the report data afterwards */

    PORTD ^= (1 << (5));
  }
}

void xbox_reset_pad_status(void) {
  memset(&XInputReportData, 0, sizeof(XInputReportData));
  XInputReportData.rsize = 20;
}
