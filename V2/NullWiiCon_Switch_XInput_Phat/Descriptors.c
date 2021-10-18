#include "Descriptors.h"

// HID Descriptors.

const USB_Descriptor_HIDReport_Datatype_t PROGMEM SwitchJoystickReport[] = {
    HID_RI_USAGE_PAGE(8,1),                         // Generic desktop controls
    HID_RI_USAGE(8,5),                              // Joystick
    HID_RI_COLLECTION(8,1),                         // Application

    // Buttons (2 bytes)
    HID_RI_LOGICAL_MINIMUM(8,0),                    // button off state
    HID_RI_LOGICAL_MAXIMUM(8,1),                    // button on state
    HID_RI_PHYSICAL_MINIMUM(8,0),                   // button off state
    HID_RI_PHYSICAL_MAXIMUM(8,1),                   // button on state
    HID_RI_REPORT_SIZE(8,1),                        // 1 bit per report field
    HID_RI_REPORT_COUNT(8,14),                      // 14 report fields (14 buttons)
    HID_RI_USAGE_PAGE(8,9),                         // Buttons (section 12)
    HID_RI_USAGE_MINIMUM(8,1),
    HID_RI_USAGE_MAXIMUM(8,14),
    HID_RI_INPUT(8,2),                              // Variable input
    HID_RI_REPORT_COUNT(8,2),                       // 2 report fields (empty 2 bits)
    HID_RI_INPUT(8,1),                              // Array input

    // HAT switch
    HID_RI_USAGE_PAGE(8,1),                         // Generic desktop controls
    HID_RI_LOGICAL_MAXIMUM(8,7),                    // 8 valid HAT states, sending 0x08 = nothing pressed
    HID_RI_PHYSICAL_MAXIMUM(16,315),                // HAT "rotation"
    HID_RI_REPORT_SIZE(8,4),                        // 4 bits per report field
    HID_RI_REPORT_COUNT(8,1),                       // 1 report field (a nibble containing entire HAT state)
    HID_RI_UNIT(8,20),                              // unit degrees
    HID_RI_USAGE(8,57),                             // Hat switch (section 4.3)
    HID_RI_INPUT(8,66),                             // Variable input, null state
    HID_RI_UNIT(8,0),                               // No units
    HID_RI_REPORT_COUNT(8,1),                       // 1 report field (empty upper nibble)
    HID_RI_INPUT(8,1),                              // Array input

    // Joystick (4 bytes)
    HID_RI_LOGICAL_MAXIMUM(16,255),                 // 0-255 for analog sticks
    HID_RI_PHYSICAL_MAXIMUM(16,255),
    HID_RI_USAGE(8,48),                             // X (left X)
    HID_RI_USAGE(8,49),                             // Y (left Y)
    HID_RI_USAGE(8,50),                             // Z (right X)
    HID_RI_USAGE(8,53),                             // Rz (right Y)
    HID_RI_REPORT_SIZE(8,8),                        // 1 byte per report field
    HID_RI_REPORT_COUNT(8,4),                       // 4 report fields (left X, left Y, right X, right Y)
    HID_RI_INPUT(8,2),                              // Variable input

    // I think this is the vendor spec byte.
    // On the Pokken pad this is usage page 0xFF00 which is vendor defined.
    // Usage is 0x20 on the Pokken pad, but since the usage page is vendor defined this is kind of meaningless.
    // Seems fine to just leave this byte set to 0.
    HID_RI_REPORT_SIZE(8,8),                        // 1 byte per report field
    HID_RI_REPORT_COUNT(8,1),                       // 1 report field
    HID_RI_INPUT(8,1),                              // Array input

    // On the Pokken pad apparently we also receive a mirror image of the sent inputs,
    // but according to the Hori pad USB descriptors this isn't necessary.
    // The usage page is still vendor defined, so the usage 0x2621 is meaningless to us.
    // On the Pokken pad this is an 8 byte report of dynamic output.

    /* This is where the Hori pad and modified Pokken pad diverge. Commenting out for future reference.
    // HAT Switch (1 nibble)
    HID_RI_USAGE_PAGE(8,1),
    HID_RI_LOGICAL_MAXIMUM(8,7),
    HID_RI_PHYSICAL_MAXIMUM(16,315),
    HID_RI_REPORT_SIZE(8,4),
    HID_RI_REPORT_COUNT(8,1),
    HID_RI_UNIT(8,20),
    HID_RI_USAGE(8,57),
    HID_RI_INPUT(8,66),
    // There's an additional nibble here that's utilized as part of the Switch Pro Controller.
    // I believe this -might- be separate U/D/L/R bits on the Switch Pro Controller, as they're utilized as four button descriptors on the Switch Pro Controller.
    HID_RI_UNIT(8,0),
    HID_RI_REPORT_COUNT(8,1),
    HID_RI_INPUT(8,1),
    // Joystick (4 bytes)
    HID_RI_LOGICAL_MAXIMUM(16,255),
    HID_RI_PHYSICAL_MAXIMUM(16,255),
    HID_RI_USAGE(8,48),
    HID_RI_USAGE(8,49),
    HID_RI_USAGE(8,50),
    HID_RI_USAGE(8,53),
    HID_RI_REPORT_SIZE(8,8),
    HID_RI_REPORT_COUNT(8,4),
    HID_RI_INPUT(8,2),
    // ??? Vendor Specific (1 byte)
    // This byte requires additional investigation.
    HID_RI_USAGE_PAGE(16,65280),
    HID_RI_USAGE(8,32),
    HID_RI_REPORT_COUNT(8,1),
    HID_RI_INPUT(8,2),
    // Output (8 bytes)
    // Original observation of this suggests it to be a mirror of the inputs that we sent.
    // The Switch requires us to have these descriptors available.
    HID_RI_USAGE(16,9761),
    HID_RI_REPORT_COUNT(8,8),
    HID_RI_OUTPUT(8,2),
    */

    HID_RI_END_COLLECTION(0),
};

/** Device descriptor structure. This descriptor, located in FLASH memory, describes the overall
 *  device characteristics, including the supported USB version, control endpoint size and the
 *  number of device configurations. The descriptor is read out by the USB host when the enumeration
 *  process begins.
 */
const USB_Descriptor_Device_t PROGMEM SwitchDeviceDescriptor = {
  .Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

  .USBSpecification       = VERSION_BCD(2,0,0),
  .Class                  = USB_CSCP_NoDeviceClass,
  .SubClass               = USB_CSCP_NoDeviceSubclass,
  .Protocol               = USB_CSCP_NoDeviceProtocol,

  .Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

  .VendorID               = 0x0F0D,
  .ProductID              = 0x00C1,
  .ReleaseNumber          = VERSION_BCD(1,0,0),

  .ManufacturerStrIndex   = STRING_ID_Manufacturer,
  .ProductStrIndex        = STRING_ID_Product,
  .SerialNumStrIndex      = NO_DESCRIPTOR,

  .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

const uint8_t PROGMEM XInputDeviceDescriptor[] =
{
	0x12,        // bLength
	0x01,        // bDescriptorType (Device)
	0x00, 0x02,  // bcdUSB 2.00
	0xFF,        // bDeviceClass 
	0xFF,        // bDeviceSubClass 
	0xFF,        // bDeviceProtocol 
	0x40,        // bMaxPacketSize0 64
	0x5E, 0x04,  // idVendor 0x045E
	0x8E, 0x02,  // idProduct 0x028E
	0x14, 0x01,  // bcdDevice 2.14
	0x01,        // iManufacturer (String Index)
	0x02,        // iProduct (String Index)
	0x03,        // iSerialNumber (String Index)
	0x01        // bNumConfigurations 1
};

/** Configuration descriptor structure. This descriptor, located in FLASH memory, describes the usage
 *  of the device in one of its supported configurations, including information about any device interfaces
 *  and endpoints. The descriptor is read out by the USB host during the enumeration process when selecting
 *  a configuration so that the host may correctly communicate with the USB device.
 */
const USB_Descriptor_Configuration_t PROGMEM SwitchConfigurationDescriptor = {
  .Config =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

      .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
      .TotalInterfaces        = 1,

      .ConfigurationNumber    = 1,
      .ConfigurationStrIndex  = NO_DESCRIPTOR,

      .ConfigAttributes       = 0x80,

      .MaxPowerConsumption    = USB_CONFIG_POWER_MA(500)
    },

  .HID_Interface =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

      .InterfaceNumber        = INTERFACE_ID_Joystick,
      .AlternateSetting       = 0x00,

      .TotalEndpoints         = 2,

      .Class                  = HID_CSCP_HIDClass,
      .SubClass               = HID_CSCP_NonBootSubclass,
      .Protocol               = HID_CSCP_NonBootProtocol,

      .InterfaceStrIndex      = NO_DESCRIPTOR
    },

  .HID_JoystickHID =
    {
      .Header                 = {.Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID},

      .HIDSpec                = VERSION_BCD(1,1,1),
      .CountryCode            = 0x00,
      .TotalReportDescriptors = 1,
      .HIDReportType          = HID_DTYPE_Report,
      .HIDReportLength        = sizeof(SwitchJoystickReport)
    },

  .HID_ReportINEndpoint =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

      .EndpointAddress        = JOYSTICK_IN_EPADDR,
      .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
      .EndpointSize           = JOYSTICK_EPSIZE,
      .PollingIntervalMS      = 0x01 //0x04
    },

  .HID_ReportOUTEndpoint =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

      .EndpointAddress        = JOYSTICK_OUT_EPADDR,
      .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
      .EndpointSize           = JOYSTICK_EPSIZE,
      .PollingIntervalMS      = 0x10
    },
};

const uint8_t PROGMEM XInputConfigurationDescriptor[] =
{
	0x09,        // bLength
	0x02,        // bDescriptorType (Configuration)
	0x99, 0x00,  // wTotalLength 153
	0x04,        // bNumInterfaces 4
	0x01,        // bConfigurationValue
	0x00,        // iConfiguration (String Index)
	0xA0,        // bmAttributes Remote Wakeup
	0xFA,        // bMaxPower 500mA
	
	0x09,        // bLength
	0x04,        // bDescriptorType (Interface)
	0x00,        // bInterfaceNumber 0
	0x00,        // bAlternateSetting
	0x02,        // bNumEndpoints 2
	0xFF,        // bInterfaceClass
	0x5D,        // bInterfaceSubClass
	0x01,        // bInterfaceProtocol
	0x00,        // iInterface (String Index)
	
	0x11,        // bLength
	0x21,        // bDescriptorType (HID)
	0x00, 0x01,  // bcdHID 1.00
	0x01,        // bCountryCode
	0x25,        // bNumDescriptors
	0x81,        // bDescriptorType[0] (Unknown 0x81)
	0x14, 0x00,  // wDescriptorLength[0] 20
	0x00,        // bDescriptorType[1] (Unknown 0x00)
	0x00, 0x00,  // wDescriptorLength[1] 0
	0x13,        // bDescriptorType[2] (Unknown 0x13)
	0x01, 0x08,  // wDescriptorLength[2] 2049
	0x00,        // bDescriptorType[3] (Unknown 0x00)
	0x00, 
	0x07,        // bLength
	0x05,        // bDescriptorType (Endpoint)
	0x81,        // bEndpointAddress (IN/D2H)
	0x03,        // bmAttributes (Interrupt)
	0x20, 0x00,  // wMaxPacketSize 32
	0x04,        // bInterval 4 (unit depends on device speed)
	
	0x07,        // bLength
	0x05,        // bDescriptorType (Endpoint)
	0x01,        // bEndpointAddress (OUT/H2D)
	0x03,        // bmAttributes (Interrupt)
	0x20, 0x00,  // wMaxPacketSize 32
	0x08,        // bInterval 8 (unit depends on device speed)
	
	0x09,        // bLength
	0x04,        // bDescriptorType (Interface)
	0x01,        // bInterfaceNumber 1
	0x00,        // bAlternateSetting
	0x04,        // bNumEndpoints 4
	0xFF,        // bInterfaceClass
	0x5D,        // bInterfaceSubClass
	0x03,        // bInterfaceProtocol
	0x00,        // iInterface (String Index)
	
	0x1B,        // bLength
	0x21,        // bDescriptorType (HID)
	0x00, 0x01,  // bcdHID 1.00
	0x01,        // bCountryCode
	0x01,        // bNumDescriptors
	0x82,        // bDescriptorType[0] (Unknown 0x82)
	0x40, 0x01,  // wDescriptorLength[0] 320
	0x02, 0x20, 0x16, 0x83, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x07,        // bLength
	0x05,        // bDescriptorType (Endpoint)
	0x82,        // bEndpointAddress (IN/D2H)
	0x03,        // bmAttributes (Interrupt)
	0x20, 0x00,  // wMaxPacketSize 32
	0x02,        // bInterval 2 (unit depends on device speed)
	
	0x07,        // bLength
	0x05,        // bDescriptorType (Endpoint)
	0x02,        // bEndpointAddress (OUT/H2D)
	0x03,        // bmAttributes (Interrupt)
	0x20, 0x00,  // wMaxPacketSize 32
	0x04,        // bInterval 4 (unit depends on device speed)
	
	0x07,        // bLength
	0x05,        // bDescriptorType (Endpoint)
	0x83,        // bEndpointAddress (IN/D2H)
	0x03,        // bmAttributes (Interrupt)
	0x20, 0x00,  // wMaxPacketSize 32
	0x40,        // bInterval 64 (unit depends on device speed)
	
	0x07,        // bLength
	0x05,        // bDescriptorType (Endpoint)
	0x03,        // bEndpointAddress (OUT/H2D)
	0x03,        // bmAttributes (Interrupt)
	0x20, 0x00,  // wMaxPacketSize 32
	0x10,        // bInterval 16 (unit depends on device speed)
	
	0x09,        // bLength
	0x04,        // bDescriptorType (Interface)
	0x02,        // bInterfaceNumber 2
	0x00,        // bAlternateSetting
	0x01,        // bNumEndpoints 1
	0xFF,        // bInterfaceClass
	0x5D,        // bInterfaceSubClass
	0x02,        // bInterfaceProtocol
	0x00,        // iInterface (String Index)
	
	0x09,        // bLength
	0x21,        // bDescriptorType (HID)
	0x00, 0x01,  // bcdHID 1.00
	0x01,        // bCountryCode
	0x22,        // bNumDescriptors
	0x84,        // bDescriptorType[0] (Unknown 0x84)
	0x07, 0x00,  // wDescriptorLength[0] 7
	
	0x07,        // bLength
	0x05,        // bDescriptorType (Endpoint)
	0x84,        // bEndpointAddress (IN/D2H)
	0x03,        // bmAttributes (Interrupt)
	0x20, 0x00,  // wMaxPacketSize 32
	0x10,        // bInterval 16 (unit depends on device speed)
	
	0x09,        // bLength
	0x04,        // bDescriptorType (Interface)
	0x03,        // bInterfaceNumber 3
	0x00,        // bAlternateSetting
	0x00,        // bNumEndpoints 0
	0xFF,        // bInterfaceClass
	0xFD,        // bInterfaceSubClass
	0x13,        // bInterfaceProtocol
	0x04,        // iInterface (String Index)
	
	0x06,        // bLength
	0x41,        // bDescriptorType (Unknown)
	0x00, 0x01, 0x01, 0x03
};

/** Language descriptor structure. This descriptor, located in FLASH memory, is returned when the host requests
 *  the string descriptor with index 0 (the first index). It is actually an array of 16-bit integers, which indicate
 *  via the language ID table available at USB.org what languages the device supports for its string descriptors.
 */
const USB_Descriptor_String_t PROGMEM SwitchLanguageString = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);
const USB_Descriptor_String_t PROGMEM XInputLanguageString =
{
	.Header                 = {.Size = USB_STRING_LEN(1), .Type = DTYPE_String},

	.UnicodeString          = {LANGUAGE_ID_ENG}
};


/** Manufacturer descriptor string. This is a Unicode string containing the manufacturer's details in human readable
 *  form, and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM SwitchManufacturerString = USB_STRING_DESCRIPTOR(L"Nullstalgia");
const USB_Descriptor_String_t PROGMEM XInputManufacturerString = USB_STRING_DESCRIPTOR(L"Nullstalgia");
/*
const USB_Descriptor_String_t PROGMEM XInputManufacturerString =
{
	.Header                 = {.Size = USB_STRING_LEN(12), .Type = DTYPE_String},

	.UnicodeString          = L"kadevice.net"
};
*/

/** Product descriptor string. This is a Unicode string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM SwitchProductString      = USB_STRING_DESCRIPTOR(L"NullWiiCon-S");
const USB_Descriptor_String_t PROGMEM XInputProductString      = USB_STRING_DESCRIPTOR(L"NullWiiCon-X");
/*
const USB_Descriptor_String_t PROGMEM XInputProductString =
{
	.Header                 = {.Size = USB_STRING_LEN(31), .Type = DTYPE_String},

	.UnicodeString          = L"KADE - Kick Ass Dynamic Encoder"
};
*/
const USB_Descriptor_String_t PROGMEM XInputVersionString =
{
	.Header                 = {.Size = USB_STRING_LEN(3), .Type = DTYPE_String},

	.UnicodeString          = L"1.0"
};

// USB Device Callback - Get Descriptor
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void** const DescriptorAddress)
{
  const uint8_t  DescriptorType   = (wValue >> 8);
  const uint8_t  DescriptorNumber = (wValue & 0xFF);

  const void* Address = NULL;
  uint16_t    Size    = NO_DESCRIPTOR;
  if(nintendoSwitchMode){
    switch (DescriptorType)
    {
      case DTYPE_Device:
        Address = &SwitchDeviceDescriptor;
        Size    = sizeof(USB_Descriptor_Device_t);
        break;
      case DTYPE_Configuration:
        Address = &SwitchConfigurationDescriptor;
        Size    = sizeof(USB_Descriptor_Configuration_t);
        break;
      case DTYPE_String:
        switch (DescriptorNumber)
        {
          case STRING_ID_Language:
            Address = &SwitchLanguageString;
            Size    = pgm_read_byte(&SwitchLanguageString.Header.Size);
            break;
          case STRING_ID_Manufacturer:
            Address = &SwitchManufacturerString;
            Size    = pgm_read_byte(&SwitchManufacturerString.Header.Size);
            break;
          case STRING_ID_Product:
            Address = &SwitchProductString;
            Size    = pgm_read_byte(&SwitchProductString.Header.Size);
            break;
        }

        break;
      case DTYPE_HID:
        Address = &SwitchConfigurationDescriptor.HID_JoystickHID;
        Size    = sizeof(USB_HID_Descriptor_HID_t);
        break;
      case DTYPE_Report:
        Address = &SwitchJoystickReport;
        Size    = sizeof(SwitchJoystickReport);
        break;
    }
  } else {
    switch (DescriptorType)
    {
      case DTYPE_Device:
        Address = &XInputDeviceDescriptor;
        Size    = sizeof(XInputDeviceDescriptor);
        break;
      case DTYPE_Configuration:
        Address = &XInputConfigurationDescriptor;
        Size    = sizeof(XInputConfigurationDescriptor);
        break;
      case DTYPE_String:
        switch (DescriptorNumber)
        {
          case STRING_ID_Language:
            Address = &XInputLanguageString;
            Size    = pgm_read_byte(&XInputLanguageString.Header.Size);
            break;
          case STRING_ID_Manufacturer:
            Address = &XInputManufacturerString;
            Size    = pgm_read_byte(&XInputManufacturerString.Header.Size);
            break;
          case STRING_ID_Product:
            Address = &XInputProductString;
            Size    = pgm_read_byte(&XInputProductString.Header.Size);
            break;
          case STRING_ID_Version:
            Address = &XInputVersionString;
            Size    = pgm_read_byte(&XInputVersionString.Header.Size);
            break;
      }

        break;
    }
  }

  *DescriptorAddress = Address;
  return Size;
}
