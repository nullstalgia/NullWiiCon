#include <XInput.h>
#include <NintendoExtensionCtrl.h>  // Library for the Wii controller communication

ClassicController classic;


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

int LeftX = 1;
int LeftY = 1;
int RightX = 1;
int RightY = 1;

void setup() {
  classic.begin();
  setupPins();

  XInput.setTriggerRange(4, 26);

  dipRead();

  if (SNES_PAD) {
    XInput.setRange(JOY_LEFT, 0, 2);
    XInput.setRange(JOY_RIGHT, 0, 2);
  } else {
    XInput.setRange(JOY_LEFT, 8, 56);
    XInput.setRange(JOY_RIGHT, 4, 28);
  }

  XInput.setAutoSend(false);  // Wait for all controls before sending

  XInput.begin();

  while (!classic.connect()) {
    delay(1000);  // Controller not connected
  }

  if (classic.isNESThirdParty()) {  // Uh oh, looks like your controller isn't genuine?
    classic.setRequestSize(8);  // Requires 8 or more bytes for third party controllers
  }
}

void loop() {
  dipRead();
  if (classic.update()) { // Get new data!
    classic.fixNESThirdPartyData();
    if (SNES_PAD) {
      XInput.setRange(JOY_LEFT, 0, 2);
      XInput.setRange(JOY_RIGHT, 0, 2);

      if (SNES_DPAD) {
        XInput.setDpad(classic.dpadUp(), classic.dpadDown(), classic.dpadLeft(), classic.dpadRight());
      } else {
        XInput.setDpad(false, false, false, false);
      }

      if (SNES_LSTICK) {
        LeftX = 1;
        LeftY = 1;
        if (classic.dpadUp()) {
          LeftY += 1;
        }
        if (classic.dpadDown()) {
          LeftY -= 1;
        }
        if (classic.dpadLeft()) {
          LeftX -= 1;
        }
        if (classic.dpadRight()) {
          LeftX += 1;
        }

        XInput.setJoystick(JOY_LEFT, LeftX, LeftY);
        XInput.setJoystick(JOY_RIGHT, 1, 1);
      } else {
        XInput.setJoystick(JOY_LEFT, 1, 1);
        XInput.setJoystick(JOY_RIGHT, 1, 1);
      }


      if (SNES_L_R) {
        XInput.setButton(BUTTON_LB, classic.buttonL());
        XInput.setButton(BUTTON_RB, classic.buttonR());
      } else {
        XInput.setButton(BUTTON_LB, false);
        XInput.setButton(BUTTON_RB, false);
      }

      if (SNES_ZL_ZR) {
        XInput.setTrigger(TRIGGER_LEFT, classic.buttonZL()  ? 26 : 4);
        XInput.setTrigger(TRIGGER_RIGHT, classic.buttonZR() ? 26 : 4);
      } else {
        XInput.setTrigger(TRIGGER_LEFT, false);
        XInput.setTrigger(TRIGGER_RIGHT, false);
      }



    } else {
      XInput.setRange(JOY_LEFT, 8, 56);
      XInput.setRange(JOY_RIGHT, 4, 28);
      XInput.setJoystick(JOY_LEFT, classic.leftJoyX(), classic.leftJoyY());
      XInput.setJoystick(JOY_RIGHT, classic.rightJoyX(), classic.rightJoyY());

      XInput.setDpad(classic.dpadUp(), classic.dpadDown(), classic.dpadLeft(), classic.dpadRight());

      XInput.setTrigger(TRIGGER_LEFT, classic.buttonZL());
      XInput.setTrigger(TRIGGER_RIGHT, classic.buttonZR());

      XInput.setButton(BUTTON_LB, classic.triggerL());
      XInput.setButton(BUTTON_RB, classic.triggerR());
    }
    XInput.setButton(BUTTON_A, classic.buttonB());
    XInput.setButton(BUTTON_B, classic.buttonA());
    XInput.setButton(BUTTON_X, classic.buttonY());
    XInput.setButton(BUTTON_Y, classic.buttonX());
    if (HOME_CAP_EN) {
      if (classic.dpadDown() && classic.buttonSelect()) {
        XInput.setButton(BUTTON_LOGO, true);
        XInput.setButton(BUTTON_START, false);
        XInput.setButton(BUTTON_BACK, false);
      } else {
        XInput.setButton(BUTTON_LOGO, false);
        XInput.setButton(BUTTON_START, classic.buttonStart());
        XInput.setButton(BUTTON_BACK, classic.buttonSelect());
      }
    }

    if (classic.buttonHome()) {
      XInput.setButton(BUTTON_LOGO, true);
    }
  }
  else {  // Data is bad :(
    XInput.releaseAll();
    classic.reconnect();
  }

  XInput.send();
}

void dipRead() {
  SNES_PAD = !digitalRead(DIP1);
  SNES_DPAD = !digitalRead(DIP2);
  SNES_LSTICK = !digitalRead(DIP3);
  SNES_L_R = !digitalRead(DIP4);
  SNES_ZL_ZR = !digitalRead(DIP5);
  HOME_CAP_EN = !digitalRead(DIP6);
}

void setupPins() {
  pinMode(DIP1, INPUT_PULLUP);
  pinMode(DIP2, INPUT_PULLUP);
  pinMode(DIP3, INPUT_PULLUP);
  pinMode(DIP4, INPUT_PULLUP);
  pinMode(DIP5, INPUT_PULLUP);
  pinMode(DIP6, INPUT_PULLUP);
}
