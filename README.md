# NullWiiCon

## What is this?

So, I *love* the Super Nintendo (and SNES Classic) controller, and I love my Nintendo Switch and PC. And while the SN30 wasn't that bad, I wanted something that was wired and worked properly with the Switch Online SNES Emulator. (To use the menu/rewind, you have to press ZL and ZR, and the SN30 only does L and R)

And after finding [progmem's work](https://github.com/progmem/Switch-Fightstick) on turning an Atmega32u4 into a Switch-compatible controller, I wanted to make it something I could use!

I used [fluffymadness' Arduino port](https://github.com/fluffymadness/ATMega32U4-Switch-Fightstick/) and [dmadison's wonderful NintendoExtensionCtrl](https://github.com/dmadison/NintendoExtensionCtrl), and got to work.

Later, I used [dmadison's ArduinoXInput](https://github.com/dmadison/ArduinoXInput) for a more generic PC-compatible controller.

## Todo:

- ~~Use custom USB PID/VID for Bootloader/DirectInput~~ Done. (But can't use for DirectInput/Switch since Switch ignores it with the custom PID/VID)
- Add thanks and credits to README
- Add dependencies to README
- Comments
- ~~Make RetroArch joypad profile for DirectInput~~ [Done.](https://github.com/libretro/retroarch-joypad-autoconfig/pull/623) 
- ~~Test custom PID/VID with Switch/XInput/DirectInput modes on all available OSes~~ Done. Switch didn't work. Not bothering with XInput for compatibility reasons.
- Add PCB pics to README
- ~~https://github.com/MHeironimus/ArduinoJoystickLibrary~~ Not using. Using normal Switch firmware with custom Manu/Prod descriptors.