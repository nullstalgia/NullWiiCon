#include "LUFAConfig.h"
#include <LUFA.h>
#include "Joystick.h"
#define Serial Serial1
#include <NintendoExtensionCtrl.h>

ClassicController classic;

bool buttonStartBefore;
bool buttonSelectBefore;
byte buttonStatus[18];

/*
  0x4000,
  0x8000,
  #define CAPTURE_MASK_ON 0x2000
  #define R3_MASK_ON 0x800
  #define L3_MASK_ON 0x400
*/
#define DPAD_UP_MASK_ON 0x00
#define DPAD_UPRIGHT_MASK_ON 0x01
#define DPAD_RIGHT_MASK_ON 0x02
#define DPAD_DOWNRIGHT_MASK_ON 0x03
#define DPAD_DOWN_MASK_ON 0x04
#define DPAD_DOWNLEFT_MASK_ON 0x05
#define DPAD_LEFT_MASK_ON 0x06
#define DPAD_UPLEFT_MASK_ON 0x07
#define DPAD_NOTHING_MASK_ON 0x08
#define A_MASK_ON 0x04
#define B_MASK_ON 0x02
#define X_MASK_ON 0x08
#define Y_MASK_ON 0x01
#define LB_MASK_ON 0x10
#define RB_MASK_ON 0x20
#define ZL_MASK_ON 0x40
#define ZR_MASK_ON 0x80
#define START_MASK_ON 0x200
#define SELECT_MASK_ON 0x100
#define SWITCH_LCLICK 0x400
#define SWITCH_RCLICK 0x800
#define HOME_MASK_ON 0x1000
#define CAPTURE_MASK_ON 0x2000

#define BUTTONUP 0
#define BUTTONDOWN 1
#define BUTTONLEFT 2
#define BUTTONRIGHT 3
#define BUTTONA 4
#define BUTTONB 5
#define BUTTONX 6
#define BUTTONY 7
#define BUTTONLB 8
#define BUTTONRB 9
#define BUTTONZL 10
#define BUTTONZR 11
#define BUTTONSTART 12
#define BUTTONSELECT 13
#define BUTTONHOME 14
#define BUTTONCAPTURE 15
#define LEFT_CLICK 16
#define RIGHT_CLICK 17

#define DIP1 13
#define DIP2 5
#define DIP3 10
#define DIP4 9
#define DIP5 8
#define DIP6 6

bool SNES_PAD = false;
bool SNES_DPAD = false;
bool SNES_LSTICK = false;
bool SNES_L_R = false;
bool SNES_ZL_ZR = false;
bool HOME_CAP_EN = false;

#define L_MIN 8
#define L_MAX 56
#define R_MIN 4
#define R_MAX 28

//BumperMode mode = BUMPER_TO_BUMPER;
bool have_switched_mode = false;

int LeftX = 128;
int LeftY = 128;
int RightX = 128;
int RightY = 128;

void checkModeChange() {
  /*
    if (classic.buttonStart() && classic.buttonSelect()) {
      if (!have_switched_mode) {
        have_switched_mode = true;
        switch (mode) {
          case BUMPER_TO_BUMPER:
            mode = BUMPER_TO_TRIGGER;
            break;
          case BUMPER_TO_TRIGGER:
            mode = BUMPER_TO_BOTH;
            break;
          case BUMPER_TO_BOTH:
            mode = BUMPER_TO_BUMPER;
            break;
        }
      }
    } else if (!classic.buttonStart() && !classic.buttonSelect()) {
      have_switched_mode = false;
    }*/
}

void setupPins() {
  pinMode(DIP1, INPUT_PULLUP);
  pinMode(DIP2, INPUT_PULLUP);
  pinMode(DIP3, INPUT_PULLUP);
  pinMode(DIP4, INPUT_PULLUP);
  pinMode(DIP5, INPUT_PULLUP);
  pinMode(DIP6, INPUT_PULLUP);
  // RX LED - PB0
  DDRB |= (1 << 0);
  // TX LED
  DDRD |= (1 << 5);
  //PORTD ^= (1 << (5));
}
void setup() {
  Serial1.begin(115200);
  classic.begin();

  while (!classic.connect()) {
    Serial1.println("Classic Controller not detected!");
    delay(1000);
  }

  if (classic.isNESThirdParty()) {  // Uh oh, looks like your controller isn't genuine?
    classic.setRequestSize(8);  // Requires 8 or more bytes for third party controllers
  }

  setupPins();
  SetupHardware();
  GlobalInterruptEnable();
}


void loop() {
  dipRead();
  buttonRead();
  checkModeChange();
  processButtons();
  HID_Task();
  // We also need to run the main USB management task.
  USB_USBTask();
}

void buttonRead()
{
  boolean success = classic.update();  // Get new data from the controller


  if (success) {
    classic.fixNESThirdPartyData();

    LeftX = 128;
    LeftY = 128;
    RightX = 128;
    RightY = 128;

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






      if (SNES_L_R) {
        buttonStatus[BUTTONLB] = classic.buttonL();
        buttonStatus[BUTTONRB] = classic.buttonR();
      } else {
        buttonStatus[BUTTONLB] = false;
        buttonStatus[BUTTONRB] = false;
      }

      if (SNES_ZL_ZR) {
        buttonStatus[BUTTONZL] = classic.buttonL();
        buttonStatus[BUTTONZR] = classic.buttonR();
      } else {
        buttonStatus[BUTTONZL] = false;
        buttonStatus[BUTTONZR] = false;
      }

      //buttonStatus[BUTTONZL] = classic.buttonL();

      //buttonStatus[BUTTONZR] = classic.buttonR();





    } else {
      buttonStatus[BUTTONUP] = classic.dpadUp();
      buttonStatus[BUTTONDOWN] = classic.dpadDown();
      buttonStatus[BUTTONLEFT] = classic.dpadLeft();
      buttonStatus[BUTTONRIGHT] = classic.dpadRight();

      LeftX = map_with_clamp(classic.leftJoyX(), L_MIN, L_MAX, 0, 254);
      LeftY = map_with_clamp(classic.leftJoyY(), L_MIN, L_MAX, 254, 0);
      RightX = map_with_clamp(classic.rightJoyX(), R_MIN, R_MAX, 0, 254);
      RightY = map_with_clamp(classic.rightJoyY(), R_MIN, R_MAX, 254, 0);

      buttonStatus[BUTTONLB] = classic.buttonL();
      buttonStatus[BUTTONRB] = classic.buttonR();
      buttonStatus[BUTTONZL] = classic.buttonZL();
      buttonStatus[BUTTONZR] = classic.buttonZR();


    }

    buttonStatus[BUTTONSTART] = classic.buttonStart();
    buttonStatus[BUTTONSELECT] = classic.buttonSelect();
    buttonStatus[BUTTONHOME] = false;
    buttonStatus[BUTTONCAPTURE] = false;

    buttonStatus[BUTTONA] = classic.buttonA();
    buttonStatus[BUTTONB] = classic.buttonB();
    buttonStatus[BUTTONX] = classic.buttonX();
    buttonStatus[BUTTONY] = classic.buttonY();

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
    classic.reconnect();
  }


}


void processDPAD() {
  ReportData.LX = LeftX;
  ReportData.LY = LeftY;
  ReportData.RX = RightX;
  ReportData.RY = RightY;

  if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONRIGHT])) {
    ReportData.HAT = DPAD_UPRIGHT_MASK_ON;
  }
  else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONRIGHT])) {
    ReportData.HAT = DPAD_DOWNRIGHT_MASK_ON;
  }
  else if ((buttonStatus[BUTTONDOWN]) && (buttonStatus[BUTTONLEFT])) {
    ReportData.HAT = DPAD_DOWNLEFT_MASK_ON;
  }
  else if ((buttonStatus[BUTTONUP]) && (buttonStatus[BUTTONLEFT])) {
    ReportData.HAT = DPAD_UPLEFT_MASK_ON;
  }
  else if (buttonStatus[BUTTONUP]) {
    ReportData.HAT = DPAD_UP_MASK_ON;
  }
  else if (buttonStatus[BUTTONDOWN]) {
    ReportData.HAT = DPAD_DOWN_MASK_ON;
  }
  else if (buttonStatus[BUTTONLEFT]) {
    ReportData.HAT = DPAD_LEFT_MASK_ON;
  }
  else if (buttonStatus[BUTTONRIGHT]) {
    ReportData.HAT = DPAD_RIGHT_MASK_ON;
  }
  else {
    ReportData.HAT = DPAD_NOTHING_MASK_ON;
  }
}

// Stolen from
// https://github.com/dmadison/NintendoExtensionCtrl/issues/51#issuecomment-578484566
// Which is from the dmadison xinput lib
int32_t map_with_clamp(int32_t val, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max) {
  if (val <= in_min) return out_min;  // Out of range -
  if (val >= in_max) return out_max;  // Out of range +
  if (in_min == out_min && in_max == out_max) return val;  // Ranges identical
  return map(val, in_min, in_max, out_min, out_max);
}

void processButtons() {
  processDPAD();
  buttonProcessing();
}

void dipRead() {
  SNES_PAD = !digitalRead(DIP1);
  SNES_DPAD = !digitalRead(DIP2);
  SNES_LSTICK = !digitalRead(DIP3);
  SNES_L_R = !digitalRead(DIP4);
  SNES_ZL_ZR = !digitalRead(DIP5);
  HOME_CAP_EN = !digitalRead(DIP6);
}

void buttonProcessing() {
  if (buttonStatus[BUTTONA]) {
    ReportData.Button |= A_MASK_ON;
  }
  if (buttonStatus[BUTTONB]) {
    ReportData.Button |= B_MASK_ON;
  }
  if (buttonStatus[BUTTONX]) {
    ReportData.Button |= X_MASK_ON;
    //ReportData.LY = ly;
  }
  if (buttonStatus[BUTTONY]) {
    ReportData.Button |= Y_MASK_ON;
  }
  if (buttonStatus[BUTTONLB]) {
    ReportData.Button |= LB_MASK_ON;
  }
  if (buttonStatus[BUTTONRB]) {
    ReportData.Button |= RB_MASK_ON;
  }
  if (buttonStatus[BUTTONZL]) {
    ReportData.Button |= ZL_MASK_ON;
  }
  if (buttonStatus[BUTTONZR]) {
    ReportData.Button |= ZR_MASK_ON;
  }
  if (buttonStatus[BUTTONSTART]) {
    ReportData.Button |= START_MASK_ON;
  }
  if (buttonStatus[BUTTONSELECT]) {
    ReportData.Button |= SELECT_MASK_ON;
  }
  if (buttonStatus[BUTTONHOME]) {
    ReportData.Button |= HOME_MASK_ON;
  }
  if (buttonStatus[BUTTONCAPTURE]) {
    ReportData.Button |= CAPTURE_MASK_ON;
  }
  if (buttonStatus[LEFT_CLICK]) {
    ReportData.Button |= SWITCH_LCLICK;
  }
  if (buttonStatus[RIGHT_CLICK]) {
    ReportData.Button |= SWITCH_RCLICK;
  }
}
