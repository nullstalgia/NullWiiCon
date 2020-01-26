/*
  NullWiiCon_Options.h - Library for a key combo "menu" for my NullWiiCon.
  Created by nullstalgia, January 2020.
*/
#ifndef NWC_O
#define NWC_O

#include "Arduino.h"
#include <NintendoExtensionCtrl.h>

#define OFF_BLINK 0
#define ON_BLINK 1

class NullWiiCon_Options {
public:
  NullWiiCon_Options(int status_led, int eeprom_offset, int flash_millis_delay,
                     int how_long_to_hold, int select_delay,
                     int select_time_to_sim);
  void begin();
  bool menu_check(ClassicController &controller);
  void flash_amount(uint8_t amount);
  void flash_option(uint8_t option);
  void change_option(uint8_t option);
  void menu_work(ClassicController &controller);
  void fill_touched(ClassicController &controller);
  bool peek_work(ClassicController &controller);

  // If the connected pad lacks analog sticks and extra trigger buttons
  bool simple_pad;
  /*
    True: Western
      A is on the bottom
    False: Japanese
      A is on the right button
  */
  bool western_layout;
  /*
    0: Off
    1: LR is main, applies ZL ZR as well when holding Select
    2. ZLZR is main...
  */
  uint8_t peek_option;
  // When peek is not enabled
  bool lr_enable;
  // When peek is not enabled
  bool zlzr_enable;
  /*
    When using Simple Pad, use the input as either a DPad or Left Analog Stick
    True: DPad
    False: LStick
  */
  bool simple_dpad;
  /*
    True:
      Down + Select: Home
      Up + Start: Capture (Switch only)
    False:
      Nothing
  */
  bool home_cap_en;

  bool in_menu;
  unsigned long current_millis;

private:
  int _status_led;
  int _eeprom_offset;
  int _flash_millis_delay;
  bool _combo_being_held;
  bool _current_select;
  bool _previous_select;
  bool _simulate_select;
  unsigned long _select_start_hold;
  unsigned long _how_long_to_hold;
  unsigned long _previous_millis;
  unsigned long _select_delay;
  unsigned long _select_time_to_sim;
  bool touched[7];
  uint8_t current_button;
  uint8_t last_button;
  uint8_t changing_button;
  typedef enum { A, B, UP, DOWN, LEFT, RIGHT, Y, NONE } button_index;
  typedef enum { PAD, HOME, LAYOUT, DPAD, LR_EN, ZLZR_EN, PEEK } option_index;
};

#endif