/**
 * vgs_iot_demo.ino
 *  gjyoung1974@gmail.com
 *  Created on: 09.12.2017
 *  Protect and Securely operate on sentive 'sensor network' data
 *  Works on an ESP8266 and ESP32-WROOM
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

// The serial connection to the GPS module
SoftwareSerial ss(12, 13);

//SDA SSD1306 Display
#include <SSD1306Wire.h>
#include <SSD1306.h>

// #include <SoftwareSerial.h>

#define USE_SERIAL Serial //Serial for Debugging

// setup the OLED display
#define SDA_PIN D3//SDA: GPIO21 -> for Esp32
#define SCL_PIN D5//SCL: GPIO22 -> for Esp32
#define SSD_ADDRESS 0x3c

//configure WiFi
#ifndef STASSID
#define STASSID "yourssidhere"
#define STAPSK  "yourwifipasswordhere"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

String sGPS="";

SSD1306  display(SSD_ADDRESS, SDA_PIN, SCL_PIN);

ESP8266WiFiMulti wifiMulti;

// Our JSON payload
char PostData[1024]; // "{\"GPS\": \"334484N1120740WT1526864344\", \"TEMP\": \"48.2C\"}";

StaticJsonDocument<1024> doc;

void setup() {

      //8266's serial debug port
      USE_SERIAL.begin(9600);
      USE_SERIAL.println();
      USE_SERIAL.println();
      USE_SERIAL.println();

      //Sofware Serial Bus for serial devices
      ss.begin(9600);

      display.init();
      display.flipScreenVertically();
      display.drawString(0, 0, "VGS IOT Demo");
      display.display();

      Serial.print("connecting to ");
      Serial.println(ssid);
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
  
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      display.clear();
      display.flipScreenVertically();
      display.setFont(ArialMT_Plain_10);
      display.drawString(0, 0, "Wifi Connected");
      display.drawString(0, 10, "IP address: "  + ipToString(WiFi.localIP()));
      display.display();
      
}

void loop() {
    
    while (true){

     sGPS="";
      
      if((wifiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");

        //set any needed headers
        http.addHeader("Content-Type", "application/json");
        
        // configure server and url
        http.begin("https://yourtenantid.SANDBOX.verygoodproxy.com/post","1f7f92d283e33c34cac6e716837a1dd3650e60f0"); //<< Http client requires "cert pinning"
        http.addHeader("Content-type", "application/json"); // <<Allows us to filter on just the desired data structures vs all streams

        USE_SERIAL.print("[HTTP] POST...\n");
        
          while (ss.available() > 0){
            // get the char data byte from the GPS
            char gpsData = ss.read();
            sGPS += String(gpsData);//build a string
          }

        USE_SERIAL.println("Raw gps data:");
        USE_SERIAL.println(sGPS);

        // start connection and send HTTP header
        String("{\"GPS\": \"" + sGPS + "\", \"TEMP\": \"48.2C\"}").toCharArray(PostData,1024);
        
        int httpCode = http.POST(PostData);

        // httpCode will be negative on error
        if(httpCode > 0) {
          
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] POST... code: %d\n", httpCode);
                 
            display.clear();
            //text display tests
            display.flipScreenVertically();
            display.setFont(ArialMT_Plain_10);
            display.drawString(0, 0, "Run HTTP POST");
            display.display();

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
              
                String payload = http.getString();//get result of post
                
                USE_SERIAL.println(payload + "\n");

                deserializeJson(doc, payload);
                JsonObject root = doc.as<JsonObject>();//get the json root

                String gps = root["json"]["GPS"];//get our aliased GPS sensor data
                
                USE_SERIAL.println(gps + "\n");

                //Display HTTP code
                display.flipScreenVertically();
                display.setFont(ArialMT_Plain_10);
                display.drawString(0, 20, "Result was: " + String(httpCode));
                display.display();

                //Display token results
                display.flipScreenVertically();
                display.setFont(ArialMT_Plain_10);
                display.drawString(0, 40, gps);
                display.display();
            }
            
        } else {
            USE_SERIAL.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        
        http.end();
    }
      delay(5000);
  }  
}

String ipToString(IPAddress ip){
  String s="";
  for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}
