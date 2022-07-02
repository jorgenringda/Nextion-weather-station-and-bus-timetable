

/**
   BasicHTTPSClient.ino

    Created on: 20.08.2018

*/

#include <ArduinoJson.h>

#include "EasyNextionLibrary.h"
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClientSecureBearSSL.h>
// Fingerprint for demo URL, expires on June 2, 2021, needs to be updated well before this date
const uint8_t fingerprint[20] = {0x40, 0xaf, 0x00, 0x6b, 0xec, 0x90, 0x22, 0x41, 0x8e, 0xa3, 0xad, 0xfa, 0x1a, 0xe8, 0x25, 0x41, 0x1d, 0x1a, 0x54, 0xb3};
//String test1 = "";
ESP8266WiFiMulti WiFiMulti;
#include <SoftwareSerial.h>

SoftwareSerial Serial_test(3, 1); // RX | TX
EasyNex myNex(Serial);

//NexText tState = NexText(0, 1, "test");
void setup() {
  myNex.begin(9600);
  //Serial.begin(115200);
  // Serial.setDebugOutput(true);
//  Serial.println();
//  Serial.println();
//  Serial.println();

//  for (uint8_t t = 4; t > 0; t--) {
//    Serial.printf("[SETUP] WAIT %d...\n", t);
//    Serial.flush();
//    delay(1000);
//  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("Korsfjordens Venner", "567entup");
}

void loop() {
  
  myNex.writeStr("page page1");
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    // client->setFingerprint(fingerprint);
    // Or, if you happy to ignore the SSL certificate, then use the following line instead:
    client->setInsecure();

    HTTPClient https;

//    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, "https://api.met.no/weatherapi/locationforecast/2.0/compact?altitude=164&lat=59.66911&lon=9.65057")) {  // HTTPS

//      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
//        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, https.getStream());
//        Serial.println(doc["type"].as<String>());
        myNex.writeStr("page page0"); // Sending this command to change the page we are on Nextion using pageName
        delay(50); // Some time for Nextion to execute the command
        //test1 = doc["type"].as<String>();
        myNex.writeStr("test.txt", doc["type"].as<String>()); // The text in t0 is now this
        //tState.setText(test1);
        https.end();
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
//          Serial.println(payload);
        }
      } else {
//        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
//      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }

//  Serial.println("Wait 10s before next round...");
  delay(10000);
}
