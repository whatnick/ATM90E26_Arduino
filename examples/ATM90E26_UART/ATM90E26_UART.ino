#include <energyic_UART.h>

void setup() {
  Serial.begin(115200);
  Serial.println("\nATM90E26 UART Test Started");
  InitEnergyIC();
  delay(1000);
}

void loop() {
  /*Repeatedly fetch some values from the ATM90E26 */
  Serial.print("Sys Status:");
  unsigned short s_status = GetSysStatus();
  if(s_status == 0xFFFF)
  {
	#if defined(ESP8266)
    //Read failed reset ESP, this is heavy
    ESP.restart();
	#endif
  }
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
