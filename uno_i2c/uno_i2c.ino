#include <Wire.h>

String current_msg;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  current_msg = "" + (String)genCode() + "|";
  Serial.println(current_msg);
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
    current_msg = "" + (String)genCode() + "|";
  }
}
void loop() {

}

long genCode() {
  long tenthousand = 10000;
  long random1 = random(1, 10);
  long random2 = random(1, 10);
  long random3 = random(1, 10);
  long random4 = random(1, 10);
  long finalnumber = random1 + random2 + random3 + random4;
  while(finalnumber > 9) {
    int finalhilfe = finalnumber /10;
    finalnumber = finalnumber - (finalhilfe*10);
    finalnumber = finalnumber + finalhilfe;
  }
  
  long codehilf = random1*tenthousand + random2*1000 + random3*100 + random4*10 + finalnumber;
  return codehilf;
}
