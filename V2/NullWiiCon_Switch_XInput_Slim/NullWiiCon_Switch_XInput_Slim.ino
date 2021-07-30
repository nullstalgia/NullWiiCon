#include <LUFA.h>

#include "Joystick.h"
#include "LUFAConfig.h"
//#define Serial //Serial1
#include <NintendoExtensionCtrl.h>
#include <NullWiiCon_Options.h>

ClassicController classic;

#define CONFIG_LED 8
#define STATUS_LED 13
NullWiiCon_Options options(CONFIG_LED, 32, 100, 3000, 1000, 100);

bool buttonStartBefore;
bool buttonSelectBefore;
#define BUTTONCOUNT 18
byte buttonStatus[BUTTONCOUNT];

#define XINPUT_SWITCH_SELECTION 6
#define SWITCHPIN PIND
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

bool current_select;
bool previous_select;
unsigned long select_start_hold;
bool simulate_select;

bool nintendoSwitchMode = false;

void setupPins() {
  // RX LED - PB0
  DDRB |= (1 << 0);
  // TX LED
  DDRD |= (1 << 5);
  // PORTD ^= (1 << (5));
}
void setup() {
  // Serial1.begin(115200);
  classic.begin();
  options.begin();

  pinMode(CONFIG_LED, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);

  while (!classic.connect()) {
    // Serial1.println("Classic Controller not detected!");
    delay(1000);
    digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
  }

  if (classic.update()) {
    LX_MIDDLE = classic.leftJoyX();
    LY_MIDDLE = classic.leftJoyY();
    RX_MIDDLE = classic.rightJoyX();
    RY_MIDDLE = classic.rightJoyY();
  } else {
    classic.connect();
  }

  setupPins();
  nintendoSwitchMode = digitalRead(XINPUT_SWITCH_SELECTION);
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

  if (nintendoSwitchMode != bit_check(SWITCHPIN, 7)) {
    USB_Detach();
    /* Enable the watchdog and force a timeout to reset the AVR */
    // this is the simplest solution since it will clear all the hardware
    // setups
    wdt_enable(WDTO_250MS);
    while (true)
      ;
  }
}

void buttonRead() {
  boolean success = classic.update();  // Get new data from the controller

  if (success) {
    options.menu_check(classic);

    if (!options.in_menu) {
      if (SNES_PAD) {
        if (SNES_DPAD) {
          buttonStatus[BUTTONUP] = classic.dpadUp();
          buttonStatus[BUTTONDOWN] = classic.dpadDown();
          buttonStatus[BUTTONLEFT] = classic.dpadLeft();
          buttonStatus[BUTTONRIGHT] = classic.dpadRight();
        } else {
          buttonStatus[BUTTONUP] = false;
          buttonStatus[BUTTONDOWN] = false;
          buttonStatus[BUTTONLEFT] = false;
          buttonStatus[BUTTONRIGHT] = false;
        }

        if (SNES_LSTICK) {
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

        if (options.peek_option == 0) {
          if (SNES_L_R) {
            buttonStatus[BUTTONLB] = classic.buttonL();
            buttonStatus[BUTTONRB] = classic.buttonR();
          }
          if (SNES_ZL_ZR) {
            buttonStatus[BUTTONZL] = classic.buttonL();
            buttonStatus[BUTTONZR] = classic.buttonR();
          }
        } else if (options.peek_option == 1) {
          buttonStatus[BUTTONLB] = classic.buttonL();
          buttonStatus[BUTTONRB] = classic.buttonR();
          if (classic.buttonSelect()) {
            buttonStatus[BUTTONZL] = classic.buttonL();
            buttonStatus[BUTTONZR] = classic.buttonR();
          }
        } else if (options.peek_option == 2) {
          buttonStatus[BUTTONZL] = classic.buttonL();
          buttonStatus[BUTTONZR] = classic.buttonR();
          if (classic.buttonSelect()) {
            buttonStatus[BUTTONLB] = classic.buttonL();
            buttonStatus[BUTTONRB] = classic.buttonR();
          }
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
      }

      buttonStatus[BUTTONSTART] = classic.buttonStart();
      buttonStatus[BUTTONSELECT] = classic.buttonSelect();

      if (!options.western_layout) {
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

      buttonStatus[BUTTONSELECT] = options.peek_work(classic);

      if (HOME_CAP_EN) {
        if (classic.dpadDown() && classic.buttonSelect()) {
          buttonStatus[BUTTONSTART] = false;
          buttonStatus[BUTTONSELECT] = false;
          buttonStatus[BUTTONHOME] = true;
          buttonStatus[BUTTONCAPTURE] = false;

          buttonStatus[BUTTONDOWN] = false;
        } else if (classic.dpadUp() && classic.buttonStart()) {
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
      for (int i = 0; i < sizeof(buttonStatus); i++) {
        buttonStatus[i] = false;
      }
      options.menu_work(classic);
    }
  } else {
    classic.connect();
    digitalWrite(STATUS_LED, HIGH);
    digitalWrite(STATUS_LED, LOW);
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
  SNES_PAD = options.simple_pad;
  SNES_DPAD = options.simple_dpad;
  SNES_LSTICK = !options.simple_dpad;
  SNES_L_R = options.lr_enable;
  SNES_ZL_ZR = options.zlzr_enable;
  HOME_CAP_EN = options.home_cap_en;
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
    bit_write(buttonStatus[BUTTONA], XInputReportData.digital_buttons_2,
              XBOX_A);
    bit_write(buttonStatus[BUTTONB], XInputReportData.digital_buttons_2,
              XBOX_B);
    bit_write(buttonStatus[BUTTONX], XInputReportData.digital_buttons_2,
              XBOX_X);
    bit_write(buttonStatus[BUTTONY], XInputReportData.digital_buttons_2,
              XBOX_Y);
   /*
    bit_write(buttonStatus[BUTTONA], XInputReportData.digital_buttons_2,
              XBOX_B);
    bit_write(buttonStatus[BUTTONB], XInputReportData.digital_buttons_2,
              XBOX_A);
    bit_write(buttonStatus[BUTTONX], XInputReportData.digital_buttons_2,
              XBOX_Y);
    bit_write(buttonStatus[BUTTONY], XInputReportData.digital_buttons_2,
              XBOX_X);
    */
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