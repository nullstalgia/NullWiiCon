#include <LUFA.h>
#include <NintendoExtensionCtrl.h>

#include "XS_HID.h"
#include "LUFAConfig.h"

ClassicController classic;

/*
  0x4000,
  0x8000,
  #define CAPTURE_MASK_ON 0x2000
  #define R3_MASK_ON 0x800
  #define L3_MASK_ON 0x400
*/

#define XINPUT_SWITCH_SELECTION 11
#define SWITCHPIN PINB
#define SWITCHBIT (1 << 7)
// Comment out to reverse the DIP direction.
#define FLIP_DIP

#if defined(FLIP_DIP)
#define DIP6 12
#define DIP5 6
#define DIP4 8
#define DIP3 9
#define DIP2 10
#define DIP1 5

#define DIPPIN6 PIND
#define DIPPIN5 PIND
#define DIPPIN4 PINB
#define DIPPIN3 PINB
#define DIPPIN2 PINB
#define DIPPIN1 PINC

#define DIPBIT6 (1 << 6)
#define DIPBIT5 (1 << 7)
#define DIPBIT4 (1 << 4)
#define DIPBIT3 (1 << 5)
#define DIPBIT2 (1 << 6)
#define DIPBIT1 (1 << 6)
#else
#define DIP1 12
#define DIP2 6
#define DIP3 8
#define DIP4 9
#define DIP5 10
#define DIP6 5

#define DIPPIN1 PIND
#define DIPPIN2 PIND
#define DIPPIN3 PINB
#define DIPPIN4 PINB
#define DIPPIN5 PINB
#define DIPPIN6 PINC

#define DIPBIT1 (1 << 6)
#define DIPBIT2 (1 << 7)
#define DIPBIT3 (1 << 4)
#define DIPBIT4 (1 << 5)
#define DIPBIT5 (1 << 6)
#define DIPBIT6 (1 << 6)
#endif

bool SNES_PAD = false;
bool SNES_DPAD = false;
bool SNES_LSTICK = false;
bool SNES_L_R = false;
bool SNES_ZL_ZR = false;
bool HOME_CAP_EN = false;
bool WESTERN_LAYOUT = false;

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

bool switchStatus = false;
bool XS_XINPUT = true;

void setupPins() {
  pinMode(DIP1, INPUT_PULLUP);
  pinMode(DIP2, INPUT_PULLUP);
  pinMode(DIP3, INPUT_PULLUP);
  pinMode(DIP4, INPUT_PULLUP);
  pinMode(DIP5, INPUT_PULLUP);
  pinMode(DIP6, INPUT_PULLUP);
  pinMode(XINPUT_SWITCH_SELECTION, INPUT_PULLUP);
  // RX LED - PB0
  DDRB |= (1 << 0);
  // TX LED - PD5
  DDRD |= (1 << 5);

  //DDRB |= (1 << 3);
  // PORTD ^= (1 << (5));
}
void setup() {
  setupPins();
  // Serial.begin(115200);
  
  classic.begin();

  bit_toggle(PORTD, 5);
  bit_toggle(PORTB, 0);
  

  while (!classic.connect()) {
    // Serial.println("Classic Controller not detected!");
    delay(300);
    //PORTB ^= (1 << (0));
    //bit_toggle(PORTD, 5);
    //bit_toggle(PORTB, 0);
    bit_toggle(PORTE, 6);
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
  } else {
    classic.connect();
  }

  switchStatus = !(SWITCHPIN & SWITCHBIT);
  XS_XINPUT = switchStatus;
  SetupHardware(XS_XINPUT);
  GlobalInterruptEnable();
}

void loop() {
  dipRead();
  clearButtons();
  buttonRead();
  send_pad_state();
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
        buttonStatus[BUTTONLB] = classic.buttonL() || classic.buttonZL();
        buttonStatus[BUTTONRB] = classic.buttonR() || classic.buttonZR();
      }

      if (SNES_ZL_ZR) {
        buttonStatus[BUTTONZL] = classic.buttonL() || classic.buttonZL();
        buttonStatus[BUTTONZR] = classic.buttonR() || classic.buttonZR();
      }

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
      // buttonStatus[BUTTONL3] = classic.buttonZL();
      // buttonStatus[BUTTONR3] = classic.buttonZR();
    }

    buttonStatus[BUTTONSTART] = classic.buttonStart();
    buttonStatus[BUTTONSELECT] = classic.buttonSelect();
    if (!WESTERN_LAYOUT) {
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
                 !XS_XINPUT) {
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

void clearButtons() {
  for (int i = 0; i < BUTTONCOUNT; i++) {
    buttonStatus[i] = false;
  }
  LeftX = 128;
  LeftY = 128;
  RightX = 128;
  RightY = 128;
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
  WESTERN_LAYOUT = !(DIPPIN6 & DIPBIT6);

  switchStatus = !(SWITCHPIN & SWITCHBIT);

  if (XS_XINPUT != switchStatus) {
    USB_Detach();
    /* Enable the watchdog and force a timeout to reset the AVR */
    // this is the simplest solution since it will clear all the hardware
    // setups
    wdt_enable(WDTO_250MS);
    while (true)
      ;
  }
}
