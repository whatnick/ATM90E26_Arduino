# ATM90E26 Arduino Library and hardware usage

This documentation is designed to capture basics only. For detailed information on the ASIC please look at the [ATM90E26 Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-46002-SE-M90E26-Datasheet.pdf). As with all handwritten documentation, this documentation may lag behind the code. While all effort will be made to update the documentation and code simultaneously, please refer to the source as the point of truth when in doubt or when errors arise.

- Installing the library

- Various data reading modes
    - UART Mode
    - SPI Mode
  
- Calibration and usage in practice
    - [Calibration Excel Sheet](./Energy%20setpoint%20calculator.xlsx)
    - [Using calibrations in the library](./Calibration_Software.md)
    - [Electrical Setup for Calibrations](./Calibration_Hardware.md)
    - [Sample usage video for breakout](https://youtu.be/QOhXIIofPQk)
    - [Sample usage video for featherwing](https://youtu.be/YI2-IAKHrNQ)
    - Reference loads
    - Thermal considerations
    - Voltage Sensor Considerations
    - Current Sensor Considerations
        - [CT Article](https://learn.openenergymonitor.org/electricity-monitoring/ct-sensors/introduction)
    - Commercial Calibration instruments from [Fluke](http://download.flukecal.com/pub/literature/1262701C_EN_w.pdf)
