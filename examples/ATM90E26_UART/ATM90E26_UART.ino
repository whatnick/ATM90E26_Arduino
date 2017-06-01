#include <energyic_UART.h>
#if !defined(ARDUINO_ARCH_SAMD)
#include <SoftwareSerial.h>
#else

#endif

#if defined(ESP8266)
//NOTE: Version 1.0 and 1.1 of featherwing use pins 14,12
//version 1.2 and above using pins 13,14
//SoftwareSerial ATMSerial(14, 12, false, 256); //RX, TX v1.0-1.1
//SoftwareSerial ATMSerial(13, 14, false, 256); //RX, TX v1.2+
SoftwareSerial ATMSerial(D4, D3, false, 256); //NodeMCU v1.0
#endif 

#ifdef __AVR_ATmega32U4__ //32u4 board
SoftwareSerial ATMSerial(11, 13); //RX, TX
#endif 

#if defined(ARDUINO_ARCH_SAMD)
#include "wiring_private.h" // pinPeripheral() function
//Feather M0 
#define PIN_SerialATM_RX       12ul
#define PIN_SerialATM_TX       11ul
#define PAD_SerialATM_RX       (SERCOM_RX_PAD_3)
#define PAD_SerialATM_TX       (UART_TX_PAD_0)

// Using SERCOM1 on M0 to communicate with ATM90E26
Uart ATMSerial(&sercom1, PIN_SerialATM_RX, PIN_SerialATM_TX, PAD_SerialATM_RX, PAD_SerialATM_TX);
#endif

ATM90E26_UART eic(&ATMSerial);

void setup() {
  Serial.begin(115200);
  Serial.println("\nATM90E26 UART Test Started");
  #if defined(ARDUINO_ARCH_SAMD)
  pinPeripheral(PIN_SerialATM_RX, PIO_SERCOM);
  pinPeripheral(PIN_SerialATM_TX, PIO_SERCOM);
  #endif
  //Must begin ATMSerial before IC init
  ATMSerial.begin(9600);
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
