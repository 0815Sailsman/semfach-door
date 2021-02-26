#include <Wire.h>

String current_msg;
String dat;

void setup() {
  Serial.begin(9600);
  current_msg = "12341|";
  dat = "";
  Wire.begin(9);
  Wire.onReceive(receiveEvent);
}
void receiveEvent(int bytes) {
  
 // Serial.println("Da ist was reingekommen... Warte kurzm");
  while (Wire.available()) {
    dat += (char)Wire.read();
  }
  if (dat != "\?" && dat != "\v") {
    Serial.println(dat);
    current_msg = current_msg + dat;
    dat = "";
  }
  else if (dat == "\v") {
    current_msg = current_msg + "|";
    dat = "";
  }
  else if (dat == "\?"){
   Serial.println("yeah, we lit");
   Serial.println(current_msg);
   dat = "";
    //append time in|
    //append time out|
    //append false
    //save to db
   // get new ode that is not already in db
   // start new current_msg 
  }
}
void loop() {
  // put your main code here, to run repeatedly:

}
