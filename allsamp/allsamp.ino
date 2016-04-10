#include <ArduinoJson.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ESP8266_Simple.h>
#define ESP8266_SSID "HiHotspot"
#define ESP8266_PASS "123454321"
#define WS_IPADDR "192.168.43.182"
#define WS_PORT 8080
#define DEBUG true

ESP8266_Simple wifi(2,3);
 //===============================================================
int sprinkler = 12;
String scanWsUrl = "http://192.168.43.182:8080/sprinklrws/services/scan/get/analogsm/%1/sm/%2/temp/%3/water/%4";
int tempPin=A0;
char buffer[250]; // Remember, SRAM is very limited, think carefully about
                    // how big a buffer you really need!

float cel;
void setup() {
  Serial.begin(9600);
  Serial.println("Connecting to Wi-fi...");
  wifi.begin(9600);  
  wifi.setupAsWifiStation(ESP8266_SSID, ESP8266_PASS);
  Serial.println("Connect to Wi-fi done!");
  //EM-side ======================================================
  pinMode(12, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
//  
//  doGetTo();
//  getSensorValues();
  sendValues();
  
}
void doGetTo(){
   
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



void getSensorValues()
  {
    // analogsm/%1/sm/%2/temp/%3/water/%4"
  int tempVal = analogRead(tempPin);//
  int wtrLevel = analogRead(A1);//
  int smsAnalog = analogRead(A3);//
  int smsDigital = digitalRead(7);//
  tempVal = getTemp (tempVal);
  delay(1000);
  smsAnalog = getSoil (smsAnalog);
  delay(1000);
  wtrLevel = getWtrLevel(wtrLevel);

  
  
  }

  int getTemp (int tv){

  float mv = ( tv/1024.0)*5000; 
  float cel = mv/10;
  
  
    return Serial.println(cel);
}
 int getSoil (int sv) {
 char* soilVals [] = {"DRY","NORMAL","WET","no soil"};
   if (sv >= 391 && sv <=700)
   { return Serial.println(soilVals[0]);
   } 
   else if (sv>=342 && sv <=390){
  return Serial.println(soilVals[1]);
   
 }
 else if (sv < 340){
return Serial.println(soilVals[2]); 
 }
 else  {
return Serial.println(soilVals[3]);
 }
 }
int getWtrLevel (int wl){
     char* waterVals [] = {"NO water", "LOW" , "HIGH"} ; 
    if (wl >= 700){
 return Serial.println(waterVals[0]); 
 delay(1000); 
 }else if (wl <= 5){
  return Serial.println(waterVals[2]);
  delay(1000);
 }else  if (wl >=6  && wl < 690 ){
return Serial.println(waterVals[1]);
delay(1000);
 }  
}

void sendValues(){
  char buf[250];
  String url = String(scanWsUrl);
  url.replace("%1","600");
  url.replace("%2","dry");
  url.replace("%3","29");
  url.replace("%4","wet");
  url.toCharArray(buf,250);
  unsigned int httpResponseCode = 
    wifi.GET
    (
      F(WS_IPADDR),     // The IP address of the server you want to contact
      WS_PORT,                     // The Port to Connect to (80 is the usual "http" port)
      buf,                 // Your buffer which currently contains the path to request
      sizeof(buf),         // The size of the buffer
      NULL, // Optional hostname you are connecting to(* see below)
      0                       // Get from line 2 of the body, no headers (use 0 to get headers)
                              // responses often have a leading newline, hence starting 
                              // from line 2 here, adjust as necessary
    );

  Serial.print("Response code: ");
  Serial.println(httpResponseCode);
  if(httpResponseCode == 201 || httpResponseCode == ESP8266_OK)
  {Serial.println("Success");
    Serial.println(buf);
    }else{
      Serial.println("fail");
      Serial.println(buf);
      }
  }
 
  
