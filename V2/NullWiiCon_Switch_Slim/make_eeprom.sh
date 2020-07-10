#!/bin/bash

rm NullWiiCon_Switch_Slim.ino.with_bootloader.micro.hex
rm nwc_switch_slim.eeprom
/home/tony/scripts/eeprom_conversion.py -f NullWiiCon_Switch_Slim.ino.micro.hex -o nwc_switch_slim.eeprom -a NWC_S_SLIM
rm NullWiiCon_Switch_Slim.ino.micro.bin
rm NullWiiCon_Switch_Slim.ino.micro.hex
