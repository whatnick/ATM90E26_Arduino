# ATM90E26 Calibration hardware jig

## Equipment

The following pieces of equipment are recommended for accurate full range calibration of the ATM90E26.

- [Nichrome Wire Wound](https://eepower.com/resistor-guide/resistor-materials/wirewound-resistor/#) resistors with 100W power rating to be measured. These can be made into a resistor box for ease of use.
- [Variable Inductance](https://www.allaboutcircuits.com/textbook/experiments/chpt-4/variable-inductor/) to change circuit inductace and phase calibration.
- Large Purely Resistive Load other than the Nichrome resistors for 240V Power calibration, preferably a 100W+ Incandescent bulb or 1kW+ Heater.
- Large Inductive/Resistive Load for Power factor calibration, preferably a high powered fan or other motorized load such as a fridge.
- Small Inductive/Resistive Load for low end power factor calibration, preferably a compact fluorescent bulb.
- Reference Multimeter/Wattmeter for independent measurement of Current, Voltage, Power and Power Factor.

A sample jig can be seen [here](
https://twitter.com/MaxClerkwell/status/1315005293328297989?s=20)

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

- Wire a 12V AC source in series with the reference resistor and an ammeter. 
- Clamp the live side of the wiring with the CT being used with the ASIC and note the current measurements on the ASIC and the ammeter.
- Repeat the measurements with a few different reference Nicrome resistors, taking care not to exceed the power dissipation ratings of the resistors with heatsinks on. There are typically [4R, 8R and 10R](https://au.banggood.com/100W-Watt-Power-Metal-Shell-Case-Wirewound-Resistor-p-89494.html?ID=49543&cur_warehouse=CN) resistors in the calibration kit. With 1R resistors use 9V AC as input. The Voltage reference signal to the ASIC and current into the calibration resistors can come from separate transformers (ideally the same one is used).
- Once a few actual vs measured current values are collected use an equation similar to the **UGain** one to compute **IGain**.
    - `IGain_n = (Current Actual / Current Measured) * IGain_o` where `IGain_n` is the new current gain and `IGain_o` is the old current gain.
    - Update Checksum as appropriate using built in [library methods](./Calibration_Software.md)

### Calibrating power

If the voltage and current multipliers are set correctly in the above steps the power for unity power factor loads should not require any additional work.

### Calibrating power factor

This requires use of the variable inductor or a well known inductive load. Use the motorized load (fan/fridge) to calibrate the higher set points of the power factor and change the `Lphi` and `Nphi` default values. The library currently does not implement runtime change to these values they are hardcoded in the header file. If the purely resistive load shows unity power factor not phase angle calibrations are necessary.
