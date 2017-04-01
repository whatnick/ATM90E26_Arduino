/*
 *  This sketch sends ATM90E26 Energy Monitor data via HTTP POST request to thingspeak server.
 *  It needs the following libraries to work (besides the esp8266 standard libraries supplied with the IDE):
 *
 *  - https://github.com/whatnick/ATM90E26_Arduino
 *  - https://github.com/adafruit/Adafruit_SSD1306
 *  - https://github.com/adafruit/Adafruit_GFX
 *  - https://github.com/bblanchon/ArduinoJson
 *  - https://github.com/tzapu/WiFiManager
 *
 *  designed to run directly on Adafruit Feather Huzzah, to where it can be uploaded using this marvelous piece of software:
 *
 *  https://github.com/esp8266/Arduino
 *
 *  2017 Tisham Dhar
 *  licensed under MIT
 */
#include <FS.h> //this needs to be first, or it all crashes and burns..

#include <ESP8266WiFi.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson

//flag for saving data
bool shouldSaveConfig = false;
 
#include <Wire.h>
#include <energyic_SPI.h>

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

const char* server = "api.thingspeak.com";
// Sign up on thingspeak and get WRITE API KEY.
char auth[36] = "THINGSPEAK_KEY";

WiFiClient client;

ATM90E26_SPI eic1;
ATM90E26_SPI eic2(0);


//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void readTSConfig()
{
  //clean FS, for testing
  //SPIFFS.format();

  //read configuration from FS json
  //Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    //Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      //Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        //Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          //Serial.println("\nparsed json");
          strcpy(auth, json["auth"]);

        } else {
          //Serial.println("failed to load json config");
        }
      }
    }
  } else {
    //Serial.println("failed to mount FS");
  }
  //end read
}

void saveTSConfig()
{
  //save the custom parameters to FS
  if (shouldSaveConfig) {
    //Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["auth"] = auth;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      //Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }
}

void setup() {
  Serial.begin(115200);
  
  Wire.begin();
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  
  // Clear the buffer.
  
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  //Read previous config
  readTSConfig();
  
  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_ts_token("ts", "Thingspeak Key", auth, 33);

  //Use wifi manager to get config
  WiFiManager wifiManager;
  wifiManager.setDebugOutput(false);

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //add all your parameters here
  wifiManager.addParameter(&custom_ts_token);

  display.setCursor(0,0);
  display.println("Connecting to Wifi ");
  display.display();
  //first parameter is name of access point, second is the password
  wifiManager.autoConnect("EnergyMonitor", "whatnick");

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  Serial.print("Key:");
  Serial.println(auth);
  display.println("Wifi Connected!!");
  display.display();

  //read updated parameters
  strcpy(auth, custom_ts_token.getValue());

  saveTSConfig();
  
  Serial.print("Start ATM90");
  eic1.InitEnergyIC();
  eic2.InitEnergyIC();
  
  delay(1000);
}

void loop() {
  /*Repeatedly fetch some values from the ATM90E26 */
  unsigned short s_status = eic1.GetSysStatus();
  if(s_status == 0xFFFF)
  {
  #if defined(ESP8266)
    //Read failed reset ESP, this is heavy
    ESP.restart();
  #endif
  }
  s_status = eic2.GetSysStatus();
  if(s_status == 0xFFFF)
  {
  #if defined(ESP8266)
    //Read failed reset ESP, this is heavy
    ESP.restart();
  #endif
  }
  
  float Vrms1 = eic1.GetLineVoltage();
  float Crms1 = eic1.GetLineCurrent();
  float realPower1 = eic1.GetActivePower();
  float powerFactor1 = eic1.GetPowerFactor();
  float Vrms2 = eic2.GetLineVoltage();
  float Crms2 = eic2.GetLineCurrent();
  float realPower2 = eic2.GetActivePower();
  float powerFactor2 = eic2.GetPowerFactor();

  Serial.print(realPower1);
  Serial.print(",");
  Serial.print(Crms1);
  Serial.print(",");
  Serial.print(Vrms1);
  Serial.print(",");
  Serial.print(powerFactor1);
  Serial.print(realPower2);
  Serial.print(",");
  Serial.print(Crms2);
  Serial.print(",");
  Serial.print(Vrms2);
  Serial.print(",");
  Serial.println(powerFactor2);
  
  if (client.connect(server,80)) {  //   "184.106.153.149" or api.thingspeak.com
    String postStr = String(auth);
           postStr +="&field1=";
           postStr += String(Vrms1);
           postStr +="&field2=";
           postStr += String(realPower1);
           postStr +="&field3=";
           postStr += String(Crms1);
           postStr +="&field4=";
           postStr += String(powerFactor1);
		       postStr +="&field5=";
           postStr += String(Vrms2);
           postStr +="&field6=";
           postStr += String(realPower2);
           postStr +="&field7=";
           postStr += String(Crms2);
           postStr +="&field8=";
           postStr += String(powerFactor2);
           postStr += "\r\n\r\n";
 
     client.print("POST /update HTTP/1.1\n"); 
     client.print("Host: api.thingspeak.com\n"); 
     client.print("Connection: close\n"); 
     client.print("X-THINGSPEAKAPIKEY: "+String(auth)+"\n"); 
     client.print("Content-Type: application/x-www-form-urlencoded\n"); 
     client.print("Content-Length: "); 
     client.print(postStr.length()); 
     client.print("\n\n"); 
     client.print(postStr);  
  }
  client.stop();
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Vrms: ");
  display.print(Vrms1);
  display.print(",");
  display.println(Vrms2);
  display.print("Irms: ");
  display.print(Crms1);
  display.print(",");
  display.println(Crms2);
  display.print("Power: ");
  display.print(realPower1);
  display.print(",");
  display.println(realPower2);
  display.print("p.f.: ");
  display.print(powerFactor1);
  display.print(",");
  display.println(powerFactor2);
  
  display.display();

  delay(1000);
  //Clear display to prevent burn in
  display.clearDisplay();
  display.display();
  
  delay(15000);
}