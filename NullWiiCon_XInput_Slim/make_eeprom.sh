#!/bin/bash

rm NullWiiCon_XInput_Slim.ino.with_bootloader.micro.hex
rm nwc_xinput_slim.eeprom
/home/tony/scripts/eeprom_conversion.py -f NullWiiCon_XInput_Slim.ino.micro.hex -o nwc_xinput_slim.eeprom -a NWC_X_SLIM
rm NullWiiCon_XInput_Slim.ino.micro.bin
rm NullWiiCon_XInput_Slim.ino.micro.hex
