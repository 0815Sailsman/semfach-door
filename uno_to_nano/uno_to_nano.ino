#include <Wire.h>
String msg;

void setup() {
  Wire.begin(9);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
}

void loop() {
  if (msg != "") {
    Serial.println(msg);
    TWCR = 0;
    Wire.begin();
    Wire.beginTransmission(2);
    Wire.write(1);
    Serial.println("Gesendet");
    Wire.endTransmission();
    TWCR = 0;
    Wire.begin(9);
    msg = "";
  }
}

void receiveEvent() {
  while (Wire.available()) {
    msg += (char)(short)Wire.read();
  }
}
