#include <XInput.h>
#include <NintendoExtensionCtrl.h> // Library for the Wii controller communication
#include <NullWiiCon_Options.h>

ClassicController classic;

#define STATUS_LED 6

NullWiiCon_Options options(STATUS_LED, 32, 100, 5000, 1000, 100);

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

int LeftX = 1;
int LeftY = 1;
int RightX = 1;
int RightY = 1;

bool current_select;
bool previous_select;
unsigned long select_start_hold;
bool simulate_select;

#define SELECT_DELAY 1000
#define SELECT_TIME_TO_SIMULATE 100

void setup() {
  classic.begin();
  options.begin();
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

  XInput.setAutoSend(false); // Wait for all controls before sending

  XInput.begin();

  while (!classic.connect()) {
    delay(1000); // Controller not connected
    digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
  }

  // Uh oh, looks like your controller isn't genuine?
  if (classic.isNESThirdParty()) {
    // Requires 8 or more bytes for third party controllers
    classic.setRequestSize(8);
  }
}

void loop() {
  dipRead();
  if (classic.update()) { // Get new data!
    classic.fixNESThirdPartyData();
    
    options.menu_check(classic);
    
    if (!options.in_menu) {
      if (SNES_PAD) {
        XInput.setRange(JOY_LEFT, 0, 2);
        XInput.setRange(JOY_RIGHT, 0, 2);

        if (SNES_DPAD) {
          XInput.setDpad(classic.dpadUp(), classic.dpadDown(),
                         classic.dpadLeft(), classic.dpadRight());
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
        if (options.peek_option == 0) {
          if (SNES_L_R) {
            XInput.setButton(BUTTON_LB, classic.buttonL());
            XInput.setButton(BUTTON_RB, classic.buttonR());
          } else {
            XInput.setButton(BUTTON_LB, false);
            XInput.setButton(BUTTON_RB, false);
          }

          if (SNES_ZL_ZR) {
            XInput.setTrigger(TRIGGER_LEFT, classic.buttonL() ? 26 : 4);
            XInput.setTrigger(TRIGGER_RIGHT, classic.buttonR() ? 26 : 4);
          } else {
            XInput.setTrigger(TRIGGER_LEFT, false);
            XInput.setTrigger(TRIGGER_RIGHT, false);
          }
        } else if (options.peek_option == 1) {
          XInput.setButton(BUTTON_LB, classic.buttonL());
          XInput.setButton(BUTTON_RB, classic.buttonR());
          if (classic.buttonSelect()) {
            XInput.setTrigger(TRIGGER_LEFT, classic.buttonL() ? 26 : 4);
            XInput.setTrigger(TRIGGER_RIGHT, classic.buttonR() ? 26 : 4);
          } else {
            XInput.setTrigger(TRIGGER_LEFT, false);
            XInput.setTrigger(TRIGGER_RIGHT, false);
          }
        } else if (options.peek_option == 2) {
          XInput.setTrigger(TRIGGER_LEFT, classic.buttonL() ? 26 : 4);
          XInput.setTrigger(TRIGGER_RIGHT, classic.buttonR() ? 26 : 4);
          if (classic.buttonSelect()) {
            XInput.setButton(BUTTON_LB, classic.buttonL());
            XInput.setButton(BUTTON_RB, classic.buttonR());
          } else {
            XInput.setButton(BUTTON_LB, false);
            XInput.setButton(BUTTON_RB, false);
          }
        }
        // End simple pad
      } else {
        XInput.setRange(JOY_LEFT, 8, 56);
        XInput.setRange(JOY_RIGHT, 4, 28);
        XInput.setJoystick(JOY_LEFT, classic.leftJoyX(), classic.leftJoyY());
        XInput.setJoystick(JOY_RIGHT, classic.rightJoyX(), classic.rightJoyY());

        XInput.setDpad(classic.dpadUp(), classic.dpadDown(), classic.dpadLeft(),
                       classic.dpadRight());

        XInput.setTrigger(TRIGGER_LEFT, classic.buttonZL() ? 26 : 4);
        XInput.setTrigger(TRIGGER_RIGHT, classic.buttonZR() ? 26 : 4);

        XInput.setButton(BUTTON_LB, classic.buttonL());
        XInput.setButton(BUTTON_RB, classic.buttonR());
      }
      if (!options.western_layout) {
        XInput.setButton(BUTTON_A, classic.buttonB());
        XInput.setButton(BUTTON_B, classic.buttonA());
        XInput.setButton(BUTTON_X, classic.buttonY());
        XInput.setButton(BUTTON_Y, classic.buttonX());
      } else {
        XInput.setButton(BUTTON_B, classic.buttonB());
        XInput.setButton(BUTTON_A, classic.buttonA());
        XInput.setButton(BUTTON_Y, classic.buttonY());
        XInput.setButton(BUTTON_X, classic.buttonX());
      }

      if (classic.dpadDown() && classic.buttonSelect() && HOME_CAP_EN) {
        XInput.setButton(BUTTON_LOGO, true);
        XInput.setButton(BUTTON_START, false);
        XInput.setButton(BUTTON_BACK, false);
      } else {
        XInput.setButton(BUTTON_LOGO, false);
        XInput.setButton(BUTTON_START, classic.buttonStart());
        if (options.peek_option == 0 || !options.simple_pad) {
          XInput.setButton(BUTTON_BACK, classic.buttonSelect());
        } else {
          XInput.setButton(BUTTON_BACK, false);
          current_select = classic.buttonSelect() && !classic.buttonL() &&
                           !classic.buttonR();
          if (!simulate_select) {
            if (current_select != previous_select) {
              if (current_select == true) {
                select_start_hold = options.current_millis;
              } else {
                if (options.current_millis - select_start_hold <=
                    SELECT_DELAY) {
                  simulate_select = true;
                  select_start_hold = options.current_millis;
                } else {
                  XInput.setButton(BUTTON_BACK, false);
                }
              }
            } else {
              if (current_select == true) {
                if (options.current_millis - select_start_hold >=
                    SELECT_DELAY) {
                  XInput.setButton(BUTTON_BACK, true);
                } else {
                  XInput.setButton(BUTTON_BACK, false);
                }
              }
            }
          } else {
            if (options.current_millis - select_start_hold <=
                    SELECT_TIME_TO_SIMULATE &&
                !classic.buttonL() && !classic.buttonR()) {
              XInput.setButton(BUTTON_BACK, true);
            } else {
              XInput.setButton(BUTTON_BACK, false);
              simulate_select = false;
            }
          }
          previous_select = current_select;
        }
      }

      if (classic.buttonHome()) {
        XInput.setButton(BUTTON_LOGO, true);
      }
    } else {
      XInput.releaseAll();
      options.menu_work(classic);
    }
  } else { // Data is bad :(
    XInput.releaseAll();
    classic.reconnect();
  }

  XInput.send();
}

void dipRead() {
  SNES_PAD = options.simple_pad;
  SNES_DPAD = options.simple_dpad;
  SNES_LSTICK = !options.simple_dpad;
  SNES_L_R = options.lr_enable;
  SNES_ZL_ZR = options.zlzr_enable;
  HOME_CAP_EN = options.home_cap_en;
}

void setupPins() {}
