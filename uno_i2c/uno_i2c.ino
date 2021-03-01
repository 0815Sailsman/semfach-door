#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"

// TODO MAKE RECEIVE EVENT SMALLER

RTC_DS1307 rtc;
String current_msg;
String number_to_compare;
int rtcground = 8;
boolean has_received = false;

String msg;

void setup() {

  Serial.begin(9600);
  SD.begin(10);
  randomSeed(analogRead(0));
  do {
     number_to_compare = (String)genCode();
  } while(code_already_exists(number_to_compare));
  current_msg = "" + number_to_compare + "|";
  Serial.println(current_msg);
  Wire.begin(9);
  Wire.onReceive(receiveEvent);

}
void receiveEvent(int bytes) {
  int dat = "";
  int flag = 0;
  int counter = 0;
  while (Wire.available()) {
    dat = Wire.read();
    char temp = dat;
    if (((temp == "i") || (temp == "o") && counter == 0) || flag == 1) {
      msg += (char)dat;
      flag = 1;
    }
    counter++;
  }
  flag = 0;
  if (msg == "") {
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
}
void loop() {
  if (has_received) {
    long zeit = get_unixtime();

    current_msg += (String)zeit + "|";
    current_msg += (String)zeit + "|";
    current_msg += "0";
    Serial.println(current_msg);
    write_string_to_EOF(current_msg);
    do {
     number_to_compare = (String)genCode();
    } while(code_already_exists(number_to_compare));
    current_msg = "" + number_to_compare + "|";
    has_received = false;
  }
  if (msg != "") {
    if (msg.c_str()[0] == "i") {
      msg = msg.substring(1);
      if (code_already_exists(msg)) {
        TWCR = 0;
        Wire.begin();
        Wire.beginTransmission(2);
        Wire.write(0);
        Serial.println("Gesendet");
        Wire.endTransmission();
        TWCR = 0;
        Wire.begin(9);
        msg = "";
      }
      else {
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
    else if (msg.c_str()[0] == "o") {
      ;
    }
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

void write_string_to_EOF(String to_write) {
  File myFile = SD.open("data.txt", FILE_WRITE);
  if (myFile) {
    
    myFile.seek(myFile.size());
    Serial.print("Writing to test.txt...");
    myFile.println(to_write);
    myFile.close();
    Serial.println("done.");
  } else {
    Serial.println("error opening test.txt");
  }
}

boolean code_already_exists(String to_compare) {
  byte next_zeichen;
  boolean skip_flag = false;
  String current_code;

  SD.begin(10);
  File myFile = SD.open("data.txt");

  while (myFile.available()) {
    next_zeichen = (byte)myFile.read();
    if (next_zeichen == 124) {
      if (!skip_flag) {
        if (to_compare == current_code) {
          return true;
        }
        Serial.println(current_code);
        current_code = "";
      }
      skip_flag = true;
    }
    if (!skip_flag) {
      if (isDigit((char)next_zeichen)) {
        current_code += (char)next_zeichen;
      }
    }
    if (next_zeichen == 13) {
      skip_flag = false;
    }
  }
  return false;
  myFile.close();
}
