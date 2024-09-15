#include <Arduino.h>
#include <WiFiManager.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  WiFiManager wm;

  bool res;

  res = wm.autoConnect("WiFi Clock");

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