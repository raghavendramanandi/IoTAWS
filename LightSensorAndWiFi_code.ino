/*
    HTTP over TLS (HTTPS) example sketch

    This example demonstrates how to use
    WiFiClientSecure class to access HTTPS API.
    We fetch and display the status of
    esp8266/Arduino project continuous integration
    build.

    Created by Ivan Grokhotkov, 2015.
    This example is in public domain.
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "MyNetwork";
const char* password = "itsourwifi";

const int BUTTON_PIN = 4;
const int LED_PIN = 2;

String CURRENT_LIGHT_STATUS = "OFF";
int MONITOR_DURATION_SECONDS = 2;
int MONITER_INTERVAL_SECONDS = 10;
int POOLING_INTERVAL_SECONDS = 10;

/*
  const char* host = "api.github.com";
  String url = "/repos/esp8266/Arduino/commits/master/status";
*/

const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
//const char* fingerprint = "35 85 74 EF 67 35 A7 CE 40 69 50 F3 C0 F6 80 CF 80 3B 2E 19";
const char* fingerprint = "7C EC 62 55 3A B5 D1 8E 5D 33 E9 EC 4E B7 05 30 01 51 9B D8";

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  UpdateStatus();
}

void UpdateStatus() {
  const char* host = "k2l9nx7gw8.execute-api.us-west-1.amazonaws.com";
  String url;

  if(CURRENT_LIGHT_STATUS == "OFF"){
    url = "/prod/UpdateStatus?team=p4r&lightStatus=off&deviceId=1";
  }else{
    url = "/prod/UpdateStatus?team=p4r&lightStatus=on&deviceId=1";
  }
  
  String status = "";
  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }

  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line.substring(9, 11) == "200" || status != "200") {
      status = 200;
    }
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');

  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(status);
  Serial.println("==========");
  Serial.println("closing connection");
}


/*
  ON -> light on => BUTTON_PIN -> LOW
  OFF -> light off => BUTTON_PIN -> HIGH
*/

String getLightStatus() {
  String lightStatus;
  if (digitalRead(BUTTON_PIN) == LOW) {
    lightStatus = "ON";
    digitalWrite(LED_PIN, LOW);
  } else {
    lightStatus = "OFF";
    digitalWrite(LED_PIN, HIGH);
  }
  return lightStatus;
}

bool moniterForCurrentStatus() {
  int count = 0;
  String status;
  while (count <= MONITOR_DURATION_SECONDS) {

    if (getLightStatus() != CURRENT_LIGHT_STATUS) {
      return false;
    }

    count = count + MONITER_INTERVAL_SECONDS;
    delay(MONITER_INTERVAL_SECONDS * 1000); //MONITER_INTERVAL_SECONDS
  }
  return true;
}

void loop() {
  bool isLightStatusChnaged = false;
  //UpdateStatus();
  if (CURRENT_LIGHT_STATUS != getLightStatus()) {
    isLightStatusChnaged = moniterForCurrentStatus();
    if (isLightStatusChnaged) {
      CURRENT_LIGHT_STATUS = (CURRENT_LIGHT_STATUS == "ON") ? "OFF" : "ON";
      UpdateStatus();
    }
  }
  delay(POOLING_INTERVAL_SECONDS * 1000);
}
