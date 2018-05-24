/**
 * vgs_iot_demo.ino
 *  gjyoung1974@gmail.com
 *  Created on: 09.12.2015
 *  Protect and Securely operate on sentive 'sensor network' data
 *
 */

#include <Arduino.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial //enable debugging on serial port

ESP8266WiFiMulti wifiMulti; //get a wifi cluent

//TODO actually poll this from a real sensor :-)
char PostData[] = "{\"GPS\": \"334484N1120740WT1526864344\", \"TEMP\": \"48.2C\"}"; // your JSON payload


void setup() {

    USE_SERIAL.begin(115200);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    wifiMulti.addAP("<your wifi SSID", "<your wifi password>");

}

void loop() {
    // wait for WiFi connection
    if((wifiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        
        // configure traged server and url
        http.begin("https://<your tenant id>.SANDBOX.verygoodproxy.com/post", "4AD6EFAEC82CA1D295BF93017E7BDE5AC8881EB4");

        //set any needed headers
        http.addHeader("Content-type", "application/json"); // <<Allows us to filter on just the desired data structures vs all streams

        USE_SERIAL.print("[HTTP] GET...\n");

        //post some data
        int httpCode = http.POST(PostData);// the payload

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
              
                String payload = http.getString(); //get HTTPPost response
                
                USE_SERIAL.println(payload); //how does the payload look?

                //get our tokenized sensor value
                String sensor_token = String(payload);
                Serial.println("Sensor Token: " + sensor_token);
  
                
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(2000);
}
