#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"

RTC_DS1307 rtc;
String current_msg;
String number_to_compare;
int rtcground = 8;
boolean has_received = false;
long zeit;

String msg;

void setup() {
  rtc.begin();
  Serial.begin(9600);
  SD.begin(10);
  randomSeed(analogRead(0));
  do {
     number_to_compare = (String)genCode();
  } while(code_already_exists(number_to_compare));
  current_msg = "" + number_to_compare + "|";
  Serial.println(current_msg);
  Wire.begin();
}

void loop() {
  zeit = get_unixtime();
  ask_esp();
  delay(1000);
}

long get_unixtime() {
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

void ask_esp() {
  // ESP32 HAT ADRESSE 39

  // Request total length of incoming string
  int total_length = 0;
  Wire.requestFrom(39, 4);
  while (Wire.available()) {
    short l = Wire.read();
    total_length += l;
  }
  Serial.println(total_length);
}
