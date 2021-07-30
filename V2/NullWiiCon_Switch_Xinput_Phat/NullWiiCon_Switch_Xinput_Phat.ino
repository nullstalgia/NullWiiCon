#include <LUFA.h>
#include <NintendoExtensionCtrl.h>

#include "Joystick.h"
#include "LUFAConfig.h"

ClassicController classic;

bool buttonStartBefore;
bool buttonSelectBefore;
#define BUTTONCOUNT 18
byte buttonStatus[BUTTONCOUNT];

/*
  0x4000,
  0x8000,
  #define CAPTURE_MASK_ON 0x2000
  #define R3_MASK_ON 0x800
  #define L3_MASK_ON 0x400
*/
#define DIP1 13
#define DIP2 5
#define DIP3 10
#define DIP4 9
#define DIP5 8
#define XINPUT_SWITCH_SELECTION 6

#define DIPPIN1 PINC
#define DIPPIN2 PINC
#define DIPPIN3 PINB
#define DIPPIN4 PINB
#define DIPPIN5 PINB
#define SWITCHPIN PIND

#define DIPBIT1 (1 << 7)
#define DIPBIT2 (1 << 6)
#define DIPBIT3 (1 << 6)
#define DIPBIT4 (1 << 5)
#define DIPBIT5 (1 << 4)
#define SWITCHBIT (1 << 7)

bool SNES_PAD = false;
bool SNES_DPAD = false;
bool SNES_LSTICK = false;
bool SNES_L_R = false;
bool SNES_ZL_ZR = false;
bool HOME_CAP_EN = false;

#define L_MIN 34
#define L_MAX 220
#define R_MIN 34
#define R_MAX 220
//#define L_MIDDLE 32
//#define R_MIDDLE 16
#define MIDDLE_BUFFER 3

uint8_t LX_MIDDLE = 128;
uint8_t LY_MIDDLE = 128;
uint8_t RX_MIDDLE = 128;
uint8_t RY_MIDDLE = 128;

// BumperMode mode = BUMPER_TO_BUMPER;
bool have_switched_mode = false;

uint8_t LeftX = 128;
uint8_t LeftY = 128;
uint8_t RightX = 128;
uint8_t RightY = 128;

bool nintendoSwitchMode = false;

bool westernLayout = false;

void setupPins() {
  pinMode(DIP1, INPUT_PULLUP);
  pinMode(DIP2, INPUT_PULLUP);
  pinMode(DIP3, INPUT_PULLUP);
  pinMode(DIP4, INPUT_PULLUP);
  pinMode(DIP5, INPUT_PULLUP);
  pinMode(XINPUT_SWITCH_SELECTION, INPUT);
  // RX LED - PB0
  DDRB |= (1 << 0);
  // TX LED - PD5
  DDRD |= (1 << 5);

  //DDRB |= (1 << 3);
  // PORTD ^= (1 << (5));
}
void setup() {
  xbox_reset_pad_status();
  setupPins();
  // Serial.begin(115200);
  // setNintendoSwitchMode(false);

  classic.begin();

  //bit_toggle(PORTD, 5);

  while (!classic.connect()) {
    // Serial.println("Classic Controller not detected!");
    delay(1000);
    //PORTB ^= (1 << (0));
    bit_toggle(PORTD, 5);
    bit_toggle(PORTB, 0);
  }

  // classic.setHighRes(false, false);

  if (classic.update()) {
    /*
    uint8_t timesToAverage = 10;
    uint16_t LX;
    uint16_t LY;
    uint16_t RX;
    uint16_t RY;
    for(int i = 0; i < timesToAverage; i++){
      classic.update();
      LX += classic.leftJoyX();
      LY += classic.leftJoyY();
      RX += classic.rightJoyX();
      RY += classic.rightJoyY();
    }
    LX_MIDDLE = LX/10;
    LY_MIDDLE = LY/10;
    RX_MIDDLE = RX/10;
    RY_MIDDLE = RY/10;
    */
    LX_MIDDLE = classic.leftJoyX();
    LY_MIDDLE = classic.leftJoyY();
    RX_MIDDLE = classic.rightJoyX();
    RY_MIDDLE = classic.rightJoyY();
    westernLayout = (classic.buttonA() && classic.buttonB());
  } else {
    classic.connect();
  }

  // setupPins();
  nintendoSwitchMode = !(SWITCHPIN & SWITCHBIT);
  // previousSwitch = nintendoSwitchMode;
  SetupHardware();
  GlobalInterruptEnable();
}

void loop() {
  dipRead();
  clearButtons();
  buttonRead();
  processButtons();

  HID_Task();
  // We also need to run the main USB management task.
  USB_USBTask();
}

void buttonRead() {
  boolean success = classic.update();  // Get new data from the controller

  if (success) {
    if (SNES_PAD) {
      if (SNES_DPAD) {
        buttonStatus[BUTTONUP] = classic.dpadUp();
        buttonStatus[BUTTONDOWN] = classic.dpadDown();
        buttonStatus[BUTTONLEFT] = classic.dpadLeft();
        buttonStatus[BUTTONRIGHT] = classic.dpadRight();
      } else {
        if (classic.dpadUp()) {
          LeftY -= 127;
        }
        if (classic.dpadDown()) {
          LeftY += 127;
        }
        if (classic.dpadLeft()) {
          LeftX -= 127;
        }
        if (classic.dpadRight()) {
          LeftX += 127;
        }
      }

      if (SNES_L_R) {
        buttonStatus[BUTTONLB] = classic.buttonL();
        buttonStatus[BUTTONRB] = classic.buttonR();
      }

      if (SNES_ZL_ZR) {
        buttonStatus[BUTTONZL] = classic.buttonL();
        buttonStatus[BUTTONZR] = classic.buttonR();
      }

      // buttonStatus[BUTTONZL] = classic.buttonL();

      // buttonStatus[BUTTONZR] = classic.buttonR();

    } else {
      buttonStatus[BUTTONUP] = classic.dpadUp();
      buttonStatus[BUTTONDOWN] = classic.dpadDown();
      buttonStatus[BUTTONLEFT] = classic.dpadLeft();
      buttonStatus[BUTTONRIGHT] = classic.dpadRight();

      uint8_t leftJoyX = classic.leftJoyX();
      uint8_t leftJoyY = classic.leftJoyY();
      uint8_t rightJoyX = classic.rightJoyX();
      uint8_t rightJoyY = classic.rightJoyY();

      if (leftJoyX <= LX_MIDDLE + MIDDLE_BUFFER &&
          !(leftJoyX < LX_MIDDLE - MIDDLE_BUFFER)) {
      } else {
        LeftX = map_with_clamp(leftJoyX, L_MIN, L_MAX, 0, 255);
      }
      if (leftJoyY <= LY_MIDDLE + MIDDLE_BUFFER &&
          !(leftJoyY < LY_MIDDLE - MIDDLE_BUFFER)) {
      } else {
        LeftY = map_with_clamp(leftJoyY, L_MIN, L_MAX, 255, 0);
      }

      if (rightJoyX <= RX_MIDDLE + MIDDLE_BUFFER &&
          !(rightJoyX < RX_MIDDLE - MIDDLE_BUFFER)) {
      } else {
        RightX = map_with_clamp(rightJoyX, R_MIN, R_MAX, 0, 255);
      }
      if (rightJoyY <= RY_MIDDLE + MIDDLE_BUFFER &&
          !(rightJoyY < RY_MIDDLE - MIDDLE_BUFFER)) {
      } else {
        RightY = map_with_clamp(rightJoyY, R_MIN, R_MAX, 255, 0);
      }

      buttonStatus[BUTTONLB] = classic.buttonL();
      buttonStatus[BUTTONRB] = classic.buttonR();
      buttonStatus[BUTTONZL] = classic.buttonZL();
      buttonStatus[BUTTONZR] = classic.buttonZR();
      // buttonStatus[LEFT_CLICK] = classic.buttonZL();
      // buttonStatus[RIGHT_CLICK] = classic.buttonZR();
    }

    buttonStatus[BUTTONSTART] = classic.buttonStart();
    buttonStatus[BUTTONSELECT] = classic.buttonSelect();
    if (!westernLayout) {
      buttonStatus[BUTTONA] = classic.buttonA();
      buttonStatus[BUTTONB] = classic.buttonB();
      buttonStatus[BUTTONX] = classic.buttonX();
      buttonStatus[BUTTONY] = classic.buttonY();
    } else {
      buttonStatus[BUTTONB] = classic.buttonA();
      buttonStatus[BUTTONA] = classic.buttonB();
      buttonStatus[BUTTONY] = classic.buttonX();
      buttonStatus[BUTTONX] = classic.buttonY();
    }

    if (HOME_CAP_EN) {
      if (classic.dpadDown() && classic.buttonSelect()) {
        buttonStatus[BUTTONSTART] = false;
        buttonStatus[BUTTONSELECT] = false;
        buttonStatus[BUTTONHOME] = true;
        buttonStatus[BUTTONCAPTURE] = false;

        buttonStatus[BUTTONDOWN] = false;
      } else if (classic.dpadUp() && classic.buttonStart() &&
                 nintendoSwitchMode) {
        buttonStatus[BUTTONSTART] = false;
        buttonStatus[BUTTONSELECT] = false;
        buttonStatus[BUTTONHOME] = false;
        buttonStatus[BUTTONCAPTURE] = true;

        buttonStatus[BUTTONUP] = false;
      }
    }

    if (classic.buttonHome()) {
      buttonStatus[BUTTONHOME] = true;
    }
  } else {
    clearButtons();
    classic.connect();
  }
}

/*
  // Stolen from
  //
  https://github.com/dmadison/NintendoExtensionCtrl/issues/51#issuecomment-578484566
  // Which is from the dmadison xinput lib
  uint8_t map_with_clamp(uint8_t val, uint8_t in_min, uint8_t in_max, uint8_t
  out_min, uint8_t out_max) { if (val <= in_min) return out_min;  // Out of
  range - if (val >= in_max) return out_max;  // Out of range +
  //if (in_min == out_min && in_max == out_max) return val;  // Ranges identical
  return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }
*/

// Stolen from @NicoHood and @st42 (github users) in a discussion about the
// map() function being weird
// https://github.com/arduino/ArduinoCore-API/issues/51#issuecomment-87432953
long map_with_clamp(long x, long in_min, long in_max, long out_min,
                    long out_max) {
  // if input is smaller/bigger than expected return the min/max out ranges
  // value
  if (x < in_min)
    return out_min;
  else if (x > in_max)
    return out_max;

  // map the input to the output range.
  // round up if mapping bigger ranges to smaller ranges
  else if ((in_max - in_min) > (out_max - out_min))
    return (x - in_min) * (out_max - out_min + 1) / (in_max - in_min + 1) +
           out_min;
  // round down if mapping smaller ranges to bigger ranges
  else
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void processButtons() {
  if (nintendoSwitchMode) {
    SwitchReportData.LX = LeftX;
    SwitchReportData.LY = LeftY;
    SwitchReportData.RX = RightX;
    SwitchReportData.RY = RightY;
  } else {
    XInputReportData.l_x = LeftX * 257 + -32768;
    XInputReportData.l_y = LeftY * -257 + 32767;
    XInputReportData.r_x = RightX * 257 + -32768;
    XInputReportData.r_y = RightY * -257 + 32767;
  }
  processDPAD();
  buttonProcessing();
}

void clearButtons() {
  LeftX = 128;
  LeftY = 128;
  RightX = 128;
  RightY = 128;
  for (int i = 0; i < BUTTONCOUNT; i++) {
    buttonStatus[i] = false;
  }
}

void dipRead() {
  /*
    SNES_PAD = !digitalRead(DIP1);
    SNES_DPAD = !digitalRead(DIP2);
    SNES_L_R = !digitalRead(DIP3);
    SNES_ZL_ZR = !digitalRead(DIP4);
    HOME_CAP_EN = !digitalRead(DIP5);
  */

  SNES_PAD = !(DIPPIN1 & DIPBIT1);
  SNES_DPAD = !(DIPPIN2 & DIPBIT2);
  SNES_L_R = !(DIPPIN3 & DIPBIT3);
  SNES_ZL_ZR = !(DIPPIN4 & DIPBIT4);
  HOME_CAP_EN = !(DIPPIN5 & DIPBIT5);

  if (nintendoSwitchMode != !(SWITCHPIN & SWITCHBIT)) {
    USB_Detach();
    /* Enable the watchdog and force a timeout to reset the AVR */
    // this is the simplest solution since it will clear all the hardware
    // setups
    wdt_enable(WDTO_250MS);
    while (true)
      ;
  }
}

void processDPAD() {
  if (nintendoSwitchMode) {
    if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONRIGHT])) {
      SwitchReportData.HAT = DPAD_UPRIGHT_MASK_ON;
    } else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONRIGHT])) {
      SwitchReportData.HAT = DPAD_DOWNRIGHT_MASK_ON;
    } else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONLEFT])) {
      SwitchReportData.HAT = DPAD_DOWNLEFT_MASK_ON;
    } else if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONLEFT])) {
      SwitchReportData.HAT = DPAD_UPLEFT_MASK_ON;
    } else if (buttonStatus[BUTTONUP]) {
      SwitchReportData.HAT = DPAD_UP_MASK_ON;
    } else if (buttonStatus[BUTTONDOWN]) {
      SwitchReportData.HAT = DPAD_DOWN_MASK_ON;
    } else if (buttonStatus[BUTTONLEFT]) {
      SwitchReportData.HAT = DPAD_LEFT_MASK_ON;
    } else if (buttonStatus[BUTTONRIGHT]) {
      SwitchReportData.HAT = DPAD_RIGHT_MASK_ON;
    } else {
      SwitchReportData.HAT = DPAD_NOTHING_MASK_ON;
    }
  } else {
    bit_write(buttonStatus[BUTTONUP], XInputReportData.digital_buttons_1,
              XBOX_DPAD_UP);
    bit_write(buttonStatus[BUTTONDOWN], XInputReportData.digital_buttons_1,
              XBOX_DPAD_DOWN);
    bit_write(buttonStatus[BUTTONLEFT], XInputReportData.digital_buttons_1,
              XBOX_DPAD_LEFT);
    bit_write(buttonStatus[BUTTONRIGHT], XInputReportData.digital_buttons_1,
              XBOX_DPAD_RIGHT);
  }
}

void buttonProcessing() {
  if (nintendoSwitchMode) {
    bit_write(buttonStatus[BUTTONA], SwitchReportData.Button, A_MASK_ON);
    bit_write(buttonStatus[BUTTONB], SwitchReportData.Button, B_MASK_ON);
    bit_write(buttonStatus[BUTTONX], SwitchReportData.Button, X_MASK_ON);
    bit_write(buttonStatus[BUTTONY], SwitchReportData.Button, Y_MASK_ON);
    bit_write(buttonStatus[BUTTONLB], SwitchReportData.Button, LB_MASK_ON);
    bit_write(buttonStatus[BUTTONRB], SwitchReportData.Button, RB_MASK_ON);
    bit_write(buttonStatus[BUTTONZL], SwitchReportData.Button, ZL_MASK_ON);
    bit_write(buttonStatus[BUTTONZR], SwitchReportData.Button, ZR_MASK_ON);

    bit_write(buttonStatus[BUTTONSTART], SwitchReportData.Button,
              START_MASK_ON);
    bit_write(buttonStatus[BUTTONSELECT], SwitchReportData.Button,
              SELECT_MASK_ON);
    bit_write(buttonStatus[BUTTONHOME], SwitchReportData.Button, HOME_MASK_ON);
    bit_write(buttonStatus[BUTTONCAPTURE], SwitchReportData.Button,
              CAPTURE_MASK_ON);
    bit_write(buttonStatus[LEFT_CLICK], SwitchReportData.Button, SWITCH_LCLICK);
    bit_write(buttonStatus[RIGHT_CLICK], SwitchReportData.Button,
              SWITCH_RCLICK);
  } else {
    /*
    bit_write(buttonStatus[BUTTONA], XInputReportData.digital_buttons_2,
              XBOX_A);
    bit_write(buttonStatus[BUTTONB], XInputReportData.digital_buttons_2,
              XBOX_B);
    bit_write(buttonStatus[BUTTONX], XInputReportData.digital_buttons_2,
              XBOX_X);
    bit_write(buttonStatus[BUTTONY], XInputReportData.digital_buttons_2,
              XBOX_Y);
    */
    bit_write(buttonStatus[BUTTONA], XInputReportData.digital_buttons_2,
              XBOX_B);
    bit_write(buttonStatus[BUTTONB], XInputReportData.digital_buttons_2,
              XBOX_A);
    bit_write(buttonStatus[BUTTONX], XInputReportData.digital_buttons_2,
              XBOX_Y);
    bit_write(buttonStatus[BUTTONY], XInputReportData.digital_buttons_2,
              XBOX_X);
    bit_write(buttonStatus[BUTTONLB], XInputReportData.digital_buttons_2,
              XBOX_LB);
    bit_write(buttonStatus[BUTTONRB], XInputReportData.digital_buttons_2,
              XBOX_RB);
    XInputReportData.lt = buttonStatus[BUTTONZL] * 0xFF;
    XInputReportData.rt = buttonStatus[BUTTONZR] * 0xFF;
    bit_write(buttonStatus[BUTTONSTART], XInputReportData.digital_buttons_1,
              XBOX_START);
    bit_write(buttonStatus[BUTTONSELECT], XInputReportData.digital_buttons_1,
              XBOX_BACK);
    bit_write(buttonStatus[BUTTONHOME], XInputReportData.digital_buttons_2,
              XBOX_HOME);
    bit_write(buttonStatus[LEFT_CLICK], XInputReportData.digital_buttons_1,
              XBOX_LEFT_STICK);
    bit_write(buttonStatus[RIGHT_CLICK], XInputReportData.digital_buttons_1,
              XBOX_RIGHT_STICK);
  }

  /*
  if (buttonStatus[BUTTONA]) {
    SwitchReportData.Button |= A_MASK_ON;
  }
  if (buttonStatus[BUTTONB]) {
    SwitchReportData.Button |= B_MASK_ON;
  }
  if (buttonStatus[BUTTONX]) {
    SwitchReportData.Button |= X_MASK_ON;
    //SwitchReportData.LY = ly;
  }
  if (buttonStatus[BUTTONY]) {
    SwitchReportData.Button |= Y_MASK_ON;
  }
  if (buttonStatus[BUTTONLB]) {
    SwitchReportData.Button |= LB_MASK_ON;
  }
  if (buttonStatus[BUTTONRB]) {
    SwitchReportData.Button |= RB_MASK_ON;
  }
  if (buttonStatus[BUTTONZL]) {
    SwitchReportData.Button |= ZL_MASK_ON;
  }
  if (buttonStatus[BUTTONZR]) {
    SwitchReportData.Button |= ZR_MASK_ON;
  }

  if (buttonStatus[BUTTONSTART]) {
    SwitchReportData.Button |= START_MASK_ON;
  }
  if (buttonStatus[BUTTONSELECT]) {
    SwitchReportData.Button |= SELECT_MASK_ON;
  }
  if (buttonStatus[BUTTONHOME]) {
    SwitchReportData.Button |= HOME_MASK_ON;
  }
  if (buttonStatus[BUTTONCAPTURE]) {
    SwitchReportData.Button |= CAPTURE_MASK_ON;
  }
  if (buttonStatus[LEFT_CLICK]) {
    SwitchReportData.Button |= SWITCH_LCLICK;
  }
  if (buttonStatus[RIGHT_CLICK]) {
    SwitchReportData.Button |= SWITCH_RCLICK;
  }
  */
}
