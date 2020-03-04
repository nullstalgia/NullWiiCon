#!/bin/bash

rm NullWiiCon_Switch.ino.with_bootloader.micro.hex
rm nwc_switch.eeprom
/home/tony/scripts/eeprom_conversion.py -f NullWiiCon_Switch.ino.micro.hex -o nwc_switch_phat.eeprom -a NWC_S_PHAT
rm NullWiiCon_Switch.ino.micro.bin
rm NullWiiCon_Switch.ino.micro.hex
