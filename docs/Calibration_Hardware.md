# ATM90E26 Calibration hardware jig

## Equipment

The following pieces of equipment are recommended for accurate full range calibration of the ATM90E26.

- [Nichrome Wire Wound](https://eepower.com/resistor-guide/resistor-materials/wirewound-resistor/#) resistors with maximum power rating to be measured. These can be made into a resistor box for ease of use.
- [Variable Inductance](https://www.allaboutcircuits.com/textbook/experiments/chpt-4/variable-inductor/) to change circuit inductace and phase calibration.
- Reference Wattmeter for independent measurement of Current, Voltage and Power.

## Calibration procedure

### Calibrating voltage gain

This is the easiest parameter to calibrate and requires the least equipments. The steps are:

- Setup default parameters and record voltage reported by ASIC and voltage reported by a multimeter with 0.1% accuracy at the AC bus.
- Apply scaling to the **UGain** parameter to account for any discprepancy according to this formula.
  - `UGain_n = (Voltage Actual / Voltage Measured) * UGain_o` where `UGain_n` is the new Voltage gain and `UGain_o` is the old voltage gain.

### Calibrating current gain

The measurement of current in an energy monitor ASIC is the most unreliable piece since current is difficult to sample directly. We are obliged to use 1 of the 2 proxies for it.

- Magnetic field around the current carrying wire(s). Magnetic fields are typically sampled using Current Clamps or Rogowski Coils.
- Voltage drop across a thermally stable low resistance shunt in the path of the current.

The current calibration requires use of the reference resistors described in the materials section.