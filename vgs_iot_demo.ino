/**
 * vgs_iot_demo.ino
 *  gjyoung1974@gmail.com
 *  Created on: 09.12.2015
 *  Protect and Securely operate on sentive 'sensor network' data
 *
 */

#include <Arduino.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>

#define USE_SERIAL Serial

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);//enable OLED reset

WiFiMulti wifiMulti; //get a wifi cluent

void setup() {

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  
  // Start the OLED i2c display
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();

  // draw a single pixel to start the display
  display.drawPixel(10, 10, WHITE);
  display.display();
  delay(2000);
  display.clearDisplay();

  //text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("VGS IOT sensor demo");
  display.display();
  delay(2000);
  display.clearDisplay();

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
        http.begin("https://<some tenant id>.SANDBOX.verygoodproxy.com/post");

        //for proxy authentication:
        //http.begin("http://US2dihmmMZD8BGsQj2yKgjZk:6e478e95-52ed-4c3b-9493-3aefa7f9137a@tntlvnzzqsz.SANDBOX.verygoodproxy.com:8080");
        /*
          // or
          http.begin("http://tntlvnzzqsz.SANDBOX.verygoodproxy.com:8080");
          http.setAuthorization("<some user>", "<some password>");

        */

        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        //int httpCode = http.GET();
        int httpCode = http.POST("{\"SensorData\": \"1526699920:33.4484,112.0740\"}");

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

                //text display tests
                display.setTextSize(1);
                display.setTextColor(WHITE);
                display.setCursor(0,0);
                display.println("Running HTTP GET");
                display.display();
                delay(2000);
                display.clearDisplay();

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);

                //text display tests
                display.display();
                display.setTextSize(1);
                display.setTextColor(WHITE);
                display.setCursor(0,0);
                display.println(httpCode);
                display.display();
                delay(2000);
                display.clearDisplay();
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(5000);
}

