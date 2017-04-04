#include <energyic_UART.h>

ATM90E26_UART eic;

void setup() {
  Serial.begin(115200);
  Serial.println("\nATM90E26 UART Test Started");
  eic.InitEnergyIC();
  delay(1000);
}

void loop() {
  /*Repeatedly fetch some values from the ATM90E26 */
  Serial.print("Sys Status:");
  unsigned short s_status = eic.GetSysStatus();
  if(s_status == 0xFFFF)
  {
	#if defined(ESP8266)
    //Read failed reset ESP, this is heavy
    ESP.restart();
	#endif
  }
  Serial.println(eic.GetSysStatus(),HEX);
  delay(10);
  Serial.print("Meter Status:");
  Serial.println(eic.GetMeterStatus(),HEX);
  delay(10);
  Serial.print("Voltage:");
  Serial.println(eic.GetLineVoltage());
  delay(10);
  Serial.print("Current:");
  Serial.println(eic.GetLineCurrent());
  delay(10);
  Serial.print("Active power:");
  Serial.println(eic.GetActivePower());
  delay(10);
  Serial.print("p.f.:");
  Serial.println(eic.GetPowerFactor());
  delay(1000);
}
