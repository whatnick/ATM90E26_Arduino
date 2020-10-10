# Arduino Library Calibration functions

The [ATM90E26 Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-46002-SE-M90E26-Datasheet.pdf) has an extensive list of calibration registers. Of these the ones we are most interested in are:

- **Ugain** for Voltage gain
- **IgainL** for Live current line gain

These also affect the checksum2 stored in the device and have to be updated together. The library provides a `CalcCheckSum` function to help in evaluating the checksums the ASIC needs when the gain values are changed.