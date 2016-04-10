#include <ArduinoJson.h>

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ESP8266_Simple.h>

#define ESP8266_SSID "HiHotspot"
#define ESP8266_PASS "123454321"
#define WS_IPADDR "192.168.43.182"
#define WS_PORT 8080

ESP8266_Simple wifi(2,3);

#define DEBUG true

void setup() {
  Serial.begin(9600);
  Serial.println("Connecting to Wi-fi...");
  wifi.begin(9600);  
  wifi.setupAsWifiStation(ESP8266_SSID, ESP8266_PASS, &Serial);
  Serial.println("Connect to Wi-fi done!");
  //EM-side ======================================================
  pinMode(13, OUTPUT);
}

void loop() {
  char buffer[250]; // Remember, SRAM is very limited, think carefully about
                    // how big a buffer you really need!
  memcpy(buffer, 0, sizeof(buffer));     // Ensure the buffer is empty first!
  strncpy_P(buffer, PSTR("/SampleArduinoCom/hellows/hello"), sizeof(buffer)-1);

  Serial.print("Accessing web service ");
  Serial.print(buffer);
  Serial.print(": ");
  

  unsigned int httpResponseCode = 
    wifi.GET
    (
      F(WS_IPADDR),     // The IP address of the server you want to contact
      WS_PORT,                     // The Port to Connect to (80 is the usual "http" port)
      buffer,                 // Your buffer which currently contains the path to request
      sizeof(buffer),         // The size of the buffer
      NULL, // Optional hostname you are connecting to(* see below)
      0                       // Get from line 2 of the body, no headers (use 0 to get headers)
                              // responses often have a leading newline, hence starting 
                              // from line 2 here, adjust as necessary
    );

  Serial.print("Response code: ");
  Serial.println(httpResponseCode);
  if(httpResponseCode == 200 || httpResponseCode == ESP8266_OK)
  {
    Serial.println("Response headers and body:");
    Serial.println(buffer);

    Serial.println("Parsing JSON...");

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(buffer);

    if(!root.success()) {
      Serial.println("parseObject() failed");
    }
    else {
      if(!root.containsKey("message")) {
        Serial.println("ERROR: message key not found in output");
      }
      else {
        Serial.print("Message: ");
        const char* message = root["message"][0];
        Serial.println(message);
      }
    }
  }
  else {
   if(httpResponseCode < 100)
    {
      // And it's on our end, but what was it?  Well we can find out easily.      
      wifi.debugPrintError((byte)httpResponseCode, &Serial);
    }
    else
    {
      // It's probably a server problem
      Serial.print("HTTP Status ");
      Serial.println(httpResponseCode);
    }
  }

  delay(5000);
}
