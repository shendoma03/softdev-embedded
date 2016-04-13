#include <ArduinoJson.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ESP8266_Simple.h>
#define ESP8266_SSID "HiHotspot"
#define ESP8266_PASS "123454321"
#define WS_IPADDR "192.168.43.180"
#define WS_PORT 8080
#define DEBUG true

ESP8266_Simple wifi(2,3);
//===============================================================
int sprinkler = 12;
String scanWsUrl = "http://192.168.43.180:8080/sprinklrws/services/scan/get/analogsm/%1/sm/%2/temp/%3/water/%4";
int tempPin=A0;
char buffer[250];

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
 
 getRequest();

 sendValues();
 
}


void getRequest(){
  char buffer[250]; // Remember, SRAM is very limited, think carefully about
                    // how big a buffer you really need!
  memcpy(buffer, 0, sizeof(buffer));     // Ensure the buffer is empty first!
  strncpy_P(buffer, PSTR("/sprinklrws/services/request/latest"), sizeof(buffer)-1);

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
//    Serial.println("Response headers and body:");
//    Serial.println(buffer);

    //Serial.println("Parsing JSON...");

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(buffer);

    if(!root.success()) {
      Serial.println("parseObject() failed");
    }
    else {
      if(!root.containsKey("operation")) {
        Serial.println("ERROR: message key not found in output");
      }
      else {
        Serial.print("operation: ");
         String message = root["operation"];
        Serial.println(message);
        if (message.equals("true")){
            digitalWrite(12, HIGH); 
            
          }else{
           
                digitalWrite(12, LOW); 
            }
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
void sendValues(){
  
  int tempVal = analogRead(A0);//
  int AwtrLevel = analogRead(A1);//
  int smsAnalog = analogRead(A3);//
  
  String analogsoil = String(smsAnalog, DEC);
  tempVal = getTemp (tempVal);

  String temp = String(tempVal, DEC);
  //++
  String wtrLevel;
  String waterVals [] = {"no%20water", "low" , "high"} ; 
    if (AwtrLevel >= 700){
  wtrLevel= waterVals[0]; 
   }
   else if (AwtrLevel <= 5){
  wtrLevel =  waterVals[2];
   }
  else  if (AwtrLevel >=6  && AwtrLevel < 690 )
  {
    wtrLevel = waterVals[1]; 
  }
  Serial.println(wtrLevel);
  //++
   String soilval;
   String soilVals [] = {"dry","wet"};
   if (smsAnalog >=600){
    soilval = soilVals[0];
    }else {
      soilval = soilVals[1];
      }
 //+++++++ 
  char buf[250];
  String url = String(scanWsUrl);
  url.replace("%1",analogsoil);
  url.replace("%2",soilval);
  url.replace("%3",temp);
  url.replace("%4",wtrLevel);
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
    delay(3000);  
  }

  
  int getTemp (int tv){
  float mv = ( tv/1024.0)*5000; 
  int cel = (mv/10)-10;
    if (cel >= 35 ){
      cel = 28;
      }else { cel = cel;}
    return cel;
}


  
 
