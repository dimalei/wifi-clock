#include <Arduino.h>
#include <WiFiManager.h>

String getID(){
  String  mac = String(ESP.getEfuseMac());
  mac = mac.substring(mac.length() - 4);
  return mac;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  WiFiManager wm;

  bool res;

  String macStr = "WiFi Clock " + getID();

  res = wm.autoConnect(macStr.c_str());

  if(!res) {
    Serial.println("Failed to connect to WiFi");
    ESP.restart();
  } else {
    Serial.println("Connected to WiFi");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}
