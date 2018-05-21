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

char PostData[] = "{\"GPS\": \"334484N1120740WT1526864344\", \"TEMP\": \"48.2C\"}"; // your JSON payload
StaticJsonBuffer<200> jsonBuffer;

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

    wifiMulti.addAP("your-wifi", "your-wifi-password");

}

void loop() {
    // wait for WiFi connection
    if((wifiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        
        // configure traged server and url
        http.begin("https://<your tenant id>.SANDBOX.verygoodproxy.com/post");

        //set any needed headers
        http.addHeader("Content-type", "application/json"); // <<Allows us to filter on just the desired data structures vs all streams

        USE_SERIAL.print("[HTTP] GET...\n");

        //post some data
        int httpCode = http.POST(PostData);// the payload

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
              
                String payload = http.getString(); //get HTTPPost response
                
                USE_SERIAL.println(payload); //how does the payload look?

                //convert POST Response payload to char[]   
                char charPayload[payload.length() + 1];
                payload.toCharArray(charPayload, payload.length());
                
                //load the json
                JsonObject& obJson = jsonBuffer.parseObject(charPayload);

//                // Test if parsing succeeds.
//                if (!obJson.success()) {
//                  Serial.println("parseObject() failed");
//                  return;
//                }

                //get our tokenized sensor value
                const char* token = obJson["data"]; //<< Get the Tokenized GPS position
                String sensor_token = String(token);
                Serial.println("Sensor Token: " + sensor_token);
  
                //text display tests
                display.display();
                display.setTextSize(1);
                display.setTextColor(WHITE);
                display.setCursor(0,0);
                display.println(httpCode);
                //display.println(sensor_token); << Fix this, json parser not working
                display.display();
                delay(2000);
                display.clearDisplay();
                
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(2000);
}

