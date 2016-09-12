
#include <SoftwareSerial.h>
#include "energyic.h"

//GPIO12 = NodeMCU D6
//GPIO14 = NodeMCU D5
SoftwareSerial swSer(14, 12, false, 256); //RX, TX

void setup() {
  Serial.begin(115200);
  swSer.begin(9600);
  Serial.println("\nATM90E26 UART Test Started");
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
