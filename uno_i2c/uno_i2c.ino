#include <Wire.h>

String current_msg;

void setup() {
  Serial.begin(9600);
  current_msg = "12341|";
  Wire.begin(9);
  Wire.onReceive(receiveEvent);
}
void receiveEvent(int bytes) {
  int dat = "";;
  while (Wire.available()) {
    dat = Wire.read();
  }
  if (dat != 124 && dat != 167) {
    dat = (char)dat;
    Serial.println(dat);
    current_msg = current_msg + (char)dat;
    dat = "";
  }
  else if (dat == 167) {
    Serial.println("Wort fertig");
    current_msg = current_msg + "|";
    dat = "";
    Serial.println(current_msg);
  }
  else if (dat == 124){
    Serial.println("yeah, we lit");
    current_msg = current_msg + "|";
    dat = "";
    Serial.println(current_msg);
    //append time in|
    //append time out|
    //append false
    //save to db
   // get new ode that is not already in db
   // start new current_msg 
  }
}
void loop() {

}
