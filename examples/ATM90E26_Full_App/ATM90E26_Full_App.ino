#include <Wire.h>
#include <energyic_UART.h>


#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 16
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#ifdef ESP8266
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

void setup() {
  Serial.begin(115200);
  Serial.print("Start ATM90");
  InitEnergyIC();
  Wire.begin();
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  
  // Clear the buffer.
  
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  delay(1000);
}

void loop() {
  /*Repeatedly fetch some values from the ATM90E26 */
  unsigned short s_status = GetSysStatus();
  if(s_status == 0xFFFF)
  {
  #if defined(ESP8266)
    //Read failed reset ESP, this is heavy
    ESP.restart();
  #endif
  }
  float Vrms = GetLineVoltage();
  float Crms = GetLineCurrent();
  float realPower = GetActivePower();
  float powerFactor = GetPowerFactor();

  Serial.print(realPower);
  Serial.print(",");
  Serial.print(Crms);
  Serial.print(",");
  Serial.print(Vrms);
  Serial.print(",");
  Serial.println(powerFactor);
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Vrms: ");
  display.println(Vrms);
  display.print("Irms: ");
  display.println(Crms);
  display.print("Power: ");
  display.println(realPower);
  display.print("p.f.: ");
  display.println(powerFactor);
  
  display.display();
  
  delay(10);
}
