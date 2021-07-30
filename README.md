# NullWiiCon

## What is this?

So, I *love* the Super Nintendo (and SNES Classic) controller, and I love my Nintendo Switch and PC. And while the SN30 wasn't that bad, I wanted something that was wired and worked properly with the Switch Online SNES Emulator. (To use the menu/rewind, you have to press ZL and ZR, and the SN30 only does L and R)

## How does it work?

I use the excellent [NintendoExtensionCtrl](https://github.com/dmadison/NintendoExtensionCtrl) library by dmadison to interface with with the connected controller via the I2C protocol. And to communicate with the PC/Switch, I use a fork of CrazyRedMachine's [LUFAHybridFightstick](https://github.com/CrazyRedMachine/LUFAHybridFightstick), which is a fork of fluffymadness' [ATMega32U4-Switch-Fightstick](https://github.com/fluffymadness/ATMega32U4-Switch-Fightstick), which itself is a fork of progmem's [Switch-Fightstick](https://github.com/progmem/Switch-Fightstick). Without these people's help, this project would not have been possible!



More V2.2 Stuff Here

