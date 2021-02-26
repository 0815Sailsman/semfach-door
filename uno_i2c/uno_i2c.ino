#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;
String current_msg;
int rtcground = 8;
boolean has_received = false;

void setup() {

  Serial.begin(9600);

  randomSeed(analogRead(0));
  current_msg = "" + (String)genCode() + "|";
  Serial.println(current_msg);
  Wire.begin(9);
  Wire.onReceive(receiveEvent);

}
void receiveEvent(int bytes) {
  int dat = "";
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
    current_msg = current_msg + "|";
    dat = "";
    has_received = true;
  }
}
void loop() {
  if (has_received) {
    long zeit = get_unixtime();

    current_msg += (String)zeit + "|";
    current_msg += (String)zeit + "|";
    current_msg += "0";
    Serial.println(current_msg);
    //save to db
   // get new ode that is not already in db
    current_msg = "" + (String)genCode() + "|";
    has_received = false;
  }
  delay(100);
}

long get_unixtime() {
  rtc.begin();
  rtc.isrunning();
  DateTime now = rtc.now();
  return now.unixtime();
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
