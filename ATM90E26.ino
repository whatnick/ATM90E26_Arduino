/* ATM90E26 Energy Monitor Demo Application

    The MIT License (MIT)

  Copyright (c) 2016 whatnick and Ryzee

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// the sensor communicates using SPI, so include the library:
#include <SPI.h>
#include "energyic.h"

void setup() {
  /* Initialize the serial port to host */
  Serial.begin(9600);
  /*Initialise the ATM90E26 + SPI port */
  InitEnergyIC();
}

void loop() {
  /*Repeatedly fetch some values from the ATM90E26 */
  Serial.print("Sys Status:");
  Serial.println(GetSysStatus(),HEX);
  delay(10);
  Serial.print("Meter Status:");
  Serial.println(GetMeterStatus(),HEX);
  delay(10);
  Serial.print("Voltage:");
  Serial.println(GetLineVoltage());
  delay(10);
  Serial.print("Current:");
  Serial.println(GetLineCurrent());
  delay(10);
  Serial.print("Active power:");
  Serial.println(GetActivePower());
  delay(10);
  Serial.print("p.f.:");
  Serial.println(GetPowerFactor());
  delay(1000);
}
