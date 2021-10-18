
# NullWiiCon

## What is this?

So, I *love* the Super Nintendo (and SNES Classic) controller, and I love my Nintendo Switch and PC. And while the SN30 wasn't that bad, I wanted something that was wired and worked properly with the Switch Online SNES Emulator. (To use the menu/rewind, you have to press ZL and ZR, and the SN30 only does L and R)

This most recent iteration uses a fork of [CrazyRedMachine](https://github.com/CrazyRedMachine)'s excellent [LUFAHybridFightstick](https://github.com/CrazyRedMachine/LUFAHybridFightstick), modified to interface with Wii's "Extension Controllers" (Classic Controllers and the like) via I2C and [dmadison's](https://github.com/dmadison) wonderful [NintendoExtensionCtrl](https://github.com/dmadison/NintendoExtensionCtrl) library.

## How do I compile this?

1. Download the Arduino IDE (As of writing, I'm using **1.8.16**)
2. Add Arduino-LUFA to the boards manager. [Instructions here.](https://github.com/CrazyRedMachine/Arduino-Lufa) (As of writing, latest is **0.93.0**)
3. With the library manager install `NintendoExtensionCtrl` (As of writing, latest is **0.8.3**)
4. If you're compiling the Slim version, copy the whole `NullWiiCon_Options` folder into your libraries folder.
5. Open the sketch and compile with the `Arduino Micro (LUFA)` profile.
	5. If you get an error complaining about "`{runtime.tools.avr-gcc.path}/bin/avr-g++: no such file or directory`", you're probably running Linux/Mac and need to also install `Arduino AVR Boards` in the Boards Manager. (As of writing, latest is **1.8.3**)


## Todo:

- PCB Pics
- Slim code for V2.1 PCB
- Change "no controller found" LED to all available LEDs

### Thanks to:
- CrazyRedMachine (for allowing my fork and getting Arduino-LUFA to the boards manager!)
- dmadison
- fluffymadness
- progmem
- and others!
