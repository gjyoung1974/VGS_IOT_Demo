/**
 * vgs_iot_demo.ino
 *  gjyoung1974@gmail.com
 *  Created on: 09.12.2015
 *  Protect and Securely operate on sentive 'sensor network' data
 *  Works on an ESP8266 and ESP32-WROOM
 */

#include <Arduino.h>
// #include <WiFiMulti.h> //esp32WiFi
#include <ESP8266WiFi.h> //esp8266WiFi
#include <ESP8266WiFiMulti.h>

//#include <HTTPClient.h> //ESP32HttpClient
#include <ESP8266HTTPClient.h>//esp8266 HttpClient

//SDA SSD1306 Display
#include "SSD1306Wire.h"
#include "SSD1306.h"

#define USE_SERIAL Serial //Serial for Debugging

#define SDA_PIN D3//SDA: GPIO21 -> for Esp32
#define SCL_PIN D5//SCL: GPIO22 -> for Esp32
#define SSD_ADDRESS 0x3c

SSD1306  display(SSD_ADDRESS, SDA_PIN, SCL_PIN);

//WiFiMulti wifiMulti; //get a wifi client (esp32)
ESP8266WiFiMulti wifiMulti; //get a wifi client (esp8266)

char PostData[] = "{\"GPS\": \"334484N1120740WT1526864344\", \"TEMP\": \"48.2C\"}"; // your JSON payload

void setup() {

      display.init();
      display.flipScreenVertically();
      display.drawString(0, 0, "VGS IOT Demo");
      display.display();

      USE_SERIAL.begin(115200);
      USE_SERIAL.println();
      USE_SERIAL.println();
      USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    wifiMulti.addAP("shrutefarms2", "Password1!Password1!");
}

void loop() {
    // wait for WiFi connection
    if((wifiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");

        //set any needed headers
        http.addHeader("Content-Type", "application/json");
        
        // configure traged server and url
        http.begin("https://tntsicixns3.SANDBOX.verygoodproxy.com/post","4ad6efaec82ca1d295bf93017e7bde5ac8881eb4"); //<< Http client requires "cert pinning"
        http.addHeader("Content-type", "application/json"); // <<Allows us to filter on just the desired data structures vs all streams

        //for proxy authentication:
        //http.begin("http://US2dihmmMZD8BGsQj2yKgjZk:6e478e95-52ed-4c3b-9493-3aefa7f9137a@tntlvnzzqsz.SANDBOX.verygoodproxy.com:8080");
        /*
          // or
          http.begin("http://tntlvnzzqsz.SANDBOX.verygoodproxy.com:8080");
          http.setAuthorization("<some user>", "<some password>");

        */

        USE_SERIAL.print("[HTTP] POST...\n");
        // start connection and send HTTP header
        int httpCode = http.POST(PostData);

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] POST... code: %d\n", httpCode);
                 
                 display.clear();
                //text display tests
                display.flipScreenVertically();
                display.setFont(ArialMT_Plain_16);
                display.drawString(0, 0, "Run HTTP POST");
                display.display();


            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);

                //text display tests
                display.flipScreenVertically();
                display.setFont(ArialMT_Plain_16);
                display.drawString(0, 20, "Result was: " + String(httpCode));
                display.display();
            
            }
            
        } else {
            USE_SERIAL.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(5000);
}
