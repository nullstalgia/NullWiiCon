/*
  NullWiiCon_Options.cpp - Library for a key combo "menu" for my NullWiiCon.
  Created by nullstalgia, January 2020.
*/

#include "NullWiiCon_Options.h"
#include "Arduino.h"
#include "EEPROM.h"

NullWiiCon_Options::NullWiiCon_Options(int status_led, int eeprom_offset,
                                       int flash_millis_delay,
                                       int how_long_to_hold, int select_delay,
                                       int select_time_to_sim) {
  // These indexes line up as a type of config as well :)
  pinMode(status_led, OUTPUT);
  _status_led = status_led;
  _eeprom_offset = eeprom_offset;
  _flash_millis_delay = flash_millis_delay;
  _how_long_to_hold = how_long_to_hold;
  _combo_being_held = false;
  _select_delay = select_delay;
  _select_time_to_sim = select_time_to_sim;
  current_button = NONE;
  last_button = NONE;
  changing_button = NONE;
}

void NullWiiCon_Options::begin() {
  uint8_t offset = _eeprom_offset;
  simple_pad = EEPROM.read(offset);
  offset++;
  western_layout = EEPROM.read(offset);
  offset++;
  peek_option = EEPROM.read(offset);
  offset += 8;
  lr_enable = EEPROM.read(offset);
  offset++;
  zlzr_enable = EEPROM.read(offset);
  offset++;
  simple_dpad = EEPROM.read(offset);
  offset++;
  home_cap_en = EEPROM.read(offset);

  if (peek_option >= 3) {
    peek_option = 0;
    simple_pad = true;
    simple_dpad = true;
    western_layout = false;
    lr_enable = true;
    zlzr_enable = false;
    home_cap_en = true;
  }
}

bool NullWiiCon_Options::menu_check(ClassicController &controller) {
  current_millis = millis();
  bool start = controller.buttonPlus();
  bool select = controller.buttonMinus();
  if (!in_menu) {
    if (start && select) {
      if (_combo_being_held == false) {
        _previous_millis = current_millis;
        _combo_being_held = true;
      } else {
        if (current_millis - _previous_millis > _how_long_to_hold) {
          in_menu = true;
        }
      }
    } else {
      _combo_being_held = false;
    }
  } else {
    if (start && select) {
      if (_combo_being_held) {
        digitalWrite(_status_led, in_menu);
        return in_menu;
      } else {
        current_button = NONE;
        last_button = NONE;
        changing_button = NONE;
        in_menu = false;
        uint8_t offset = _eeprom_offset;
        EEPROM.write(offset, simple_pad);
        offset++;
        EEPROM.write(offset, western_layout);
        offset++;
        EEPROM.write(offset, peek_option);
        offset += 8;
        EEPROM.write(offset, lr_enable);
        offset++;
        EEPROM.write(offset, zlzr_enable);
        offset++;
        EEPROM.write(offset, simple_dpad);
      }
    } else {
      _combo_being_held = false;
    }
  }
  digitalWrite(_status_led, in_menu);
  return in_menu;
}

void NullWiiCon_Options::fill_touched(ClassicController &controller) {
  touched[A] = controller.buttonA();
  touched[B] = controller.buttonB();
  touched[UP] = controller.dpadUp();
  touched[DOWN] = controller.dpadDown();
  touched[LEFT] = controller.dpadLeft();
  touched[RIGHT] = controller.dpadRight();
  touched[Y] = controller.buttonY();
}

#define OFF_BLINK 0
#define ON_BLINK 1

void NullWiiCon_Options::flash_amount(uint8_t amount) {
  if (amount == OFF_BLINK) {
    digitalWrite(_status_led, LOW);
    delay(_flash_millis_delay * 4);
    digitalWrite(_status_led, HIGH);
    return;
  }
  for (uint8_t i = 0; i < amount; i++) {
    digitalWrite(_status_led, LOW);
    delay(_flash_millis_delay);
    digitalWrite(_status_led, HIGH);
    delay(_flash_millis_delay);
  }
}

void NullWiiCon_Options::flash_option(uint8_t option) {
  if (option == PAD) {
    flash_amount(simple_pad ? ON_BLINK : OFF_BLINK);
  } else if (option == LAYOUT) {
    flash_amount(western_layout ? ON_BLINK : OFF_BLINK);
  } else if (option == PEEK) {
    int amount = peek_option;
    if (amount == 0) {
      amount = OFF_BLINK;
    } else if (amount == 1) {
      amount = 2;
    } else if (amount == 2) {
      amount = 5;
    }
    flash_amount(amount);
  } else if (option == DPAD) {
    flash_amount(simple_dpad ? ON_BLINK : OFF_BLINK);
  } else if (option == LR_EN) {
    flash_amount(lr_enable ? ON_BLINK : OFF_BLINK);
  } else if (option == ZLZR_EN) {
    flash_amount(zlzr_enable ? ON_BLINK : OFF_BLINK);
  } else if (option == HOME) {
    flash_amount(home_cap_en ? ON_BLINK : OFF_BLINK);
  }
}

void NullWiiCon_Options::change_option(uint8_t option) {
  if (option == PAD) {
    simple_pad = !simple_pad;
  } else if (option == LAYOUT) {
    western_layout = !western_layout;
  } else if (option == PEEK) {
    peek_option++;
    if (peek_option >= 3) {
      peek_option = 0;
    }
  } else if (option == DPAD) {
    simple_dpad = !simple_dpad;
  } else if (option == LR_EN) {
    lr_enable = !lr_enable;
  } else if (option == ZLZR_EN) {
    zlzr_enable = !zlzr_enable;
  } else if (option == HOME) {
    home_cap_en = !home_cap_en;
  }
  flash_option(option);
}

void NullWiiCon_Options::menu_work(ClassicController &controller) {
  fill_touched(controller);
  current_button = NONE;
  for (uint8_t i = 0; i < sizeof(touched); i++) {
    if (touched[i]) {
      current_button = i;
      break;
    }
  }
  if (current_button != last_button && current_button != NONE) {
    if (current_button == changing_button) {
      change_option(changing_button);
    } else {
      changing_button = current_button;
      flash_option(changing_button);
    }
  }
  last_button = current_button;
}
bool NullWiiCon_Options::peek_work(ClassicController &controller) {
  bool select_output = controller.buttonSelect();
  if (peek_option == 0 || !simple_pad) {
    //select_output = controller.buttonSelect();
  } else {
    select_output = false;
    _current_select = controller.buttonSelect();
    // && !controller.buttonL() &&
    //!controller.buttonR();
    
    if (!_simulate_select) {
      if (_current_select != _previous_select) {
        if (_current_select == true) {
          _select_start_hold = current_millis;
        } else {
          if (current_millis - _select_start_hold <= _select_delay && !_lrzlzr_was_simulated) {
            _simulate_select = true;
            _select_start_hold = current_millis;
          } else {
            select_output = false;
            _lrzlzr_was_simulated = false;
          }
        }
      } else {
        if (_current_select == true) {
          if (controller.buttonL() || controller.buttonR()) {
            _lrzlzr_was_simulated = true;
          }
          if (current_millis - _select_start_hold >= _select_delay) {
            select_output = true;
          } else {
            select_output = false;
          }
        }
      }
    } else {
      if (current_millis - _select_start_hold <= _select_time_to_sim &&
          !controller.buttonL() && !controller.buttonR()) {
        select_output = true;
      } else {
        select_output = false;
        _simulate_select = false;
      }
    }
    _previous_select = _current_select;
  }
  return select_output;
}