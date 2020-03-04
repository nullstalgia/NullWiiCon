#!/bin/bash

rm NullWiiCon_XInput.ino.with_bootloader.micro.hex
rm nwc_xinput.eeprom
/home/tony/scripts/eeprom_conversion.py -f NullWiiCon_XInput.ino.micro.hex -o nwc_xinput_phat.eeprom -a NWC_X_PHAT
rm NullWiiCon_XInput.ino.micro.bin
rm NullWiiCon_XInput.ino.micro.hex
