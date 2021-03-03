#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include "RTClib.h"

LiquidCrystal_I2C lcd1(0x26, 16, 2);
LiquidCrystal_I2C lcd2(0x27, 16, 2);
RTC_DS1307 rtc;

String number_to_compare;
String received_number;

// Replace with your network credentials
const char* ssid     = "ESP32-Access-Point";

// Set web server port number to 80
AsyncWebServer server(80);

// Variable to store the HTTP request
String header;

const char* PARAM_INPUT_1 = "name";
const char* PARAM_INPUT_2 = "nname";
const char* PARAM_INPUT_3 = "email";

String inputvName;
String inputnName;
String inputEmail;

String current_msg;

byte person_counter = 0;
const byte MAX_PERSONS = 20;

// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    Vorame: <input type="text" name="name"><br>
    Nachname: <input type="text" name="nname"><br>
    EMail: <input type="text" name="email"><br>
    <input type="submit" value="Submit">
  </form><br>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}


void setup() {
  Serial.begin(115200);
  Wire.begin();
  lcd1.init();
  delay(20);   
  lcd2.init();                   
  delay(20);
  lcd1.backlight();
  delay(20);                    
  lcd2.backlight();
  WiFi.softAP(ssid);
  rtc.begin();
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  SD.begin(5);

  randomSeed(analogRead(0));
  do {
     number_to_compare = (String)genCode();
  } while(code_already_exists(number_to_compare));
  current_msg = "" + number_to_compare + "|";  

    // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });


  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2) && request->hasParam(PARAM_INPUT_3)) {
      inputvName = request->getParam(PARAM_INPUT_1)->value();
      inputnName = request->getParam(PARAM_INPUT_2)->value();
      inputEmail = request->getParam(PARAM_INPUT_3)->value();
      inputParam = PARAM_INPUT_1;
    }
    else {
      inputvName = "No message sent";
      inputParam = "none";
    }
    lcd1.clear();
    lcd1.setCursor(0, 0);
    lcd1.print("New user");
    delay(20);
    lcd2.clear();
    lcd2.setCursor(0, 0);
    lcd2.print("New user");
    delay(3000);
    lcd1.clear();
    delay(20);
    lcd2.clear();
    
    long zeit = get_unixtime();
    // Build complete string
    current_msg += inputvName;
    current_msg += '|';
    current_msg += inputnName;
    current_msg += '|';
    current_msg += inputEmail;
    current_msg += '|';
    current_msg += (String)zeit;
    current_msg += '|';
    current_msg += (String)zeit;
    current_msg += "|0";
    // write the user
    write_string_to_EOF(current_msg);
    // Reset everything back to start
    do {
       number_to_compare = (String)genCode();
    } while(code_already_exists(number_to_compare));
    current_msg = "" + number_to_compare + "|";  
    
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputvName + ", " + inputnName + ", " + inputEmail + 
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });
  server.onNotFound(notFound);
  
  server.begin();
}

void loop(){
  // ask nano 1 for new code
  received_number = "";
  Wire.requestFrom(8, 6);    // request 6 bytes from slave device #8
  while (Wire.available()) { // slave may send less than requested
    received_number += Wire.read(); // receive a byte as character
            
  }
  Serial.print(received_number);
  if (received_number != "00000") {
    // Someone wants to get in
    // Check db if user is registered
    if (code_already_exists(received_number)) {
      // Check db if user is already in
      if (!is_inside(received_number)) {
        // Check if person_counter is low enough for one more person to enter the building
        if (person_counter < MAX_PERSONS) {
          // Display confirmation
          lcd1.clear();
          lcd1.setCursor(0, 0);
          lcd1.print("***Akzeptiert***");
          lcd1.setCursor(0, 1);
          lcd1.print("Tür wird geöffnet");
          // Open the gates

          // Wait for person to enter

          // Update display

          // Update db

          // Change counter
          person_counter++;
        }
      }
    }
  }
  // ask nano 2 for new code  
  
}

void write_string_to_EOF(String to_write) {
  File myFile = SD.open("/data.txt", FILE_WRITE);
  if (myFile) {
    
    myFile.seek(myFile.size());
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
  File myFile = SD.open("/data.txt");

  while (myFile.available()) {
    next_zeichen = (byte)myFile.read();
    if (next_zeichen == 124) {
      if (!skip_flag) {
        if (to_compare == current_code) {
          myFile.close();
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
  myFile.close();
  return false;  
}

boolean is_inside(String user_string) {
  byte next_zeichen;
  boolean skip_flag = false;
  String current_code;
  File myFile = SD.open("/data.txt");

  while (myFile.available()) {
    next_zeichen = (byte)myFile.read();
    if (next_zeichen == 124) {
      if (!skip_flag) {
        if (user_string == current_code) {
          // Correct user found
          short counter = 0;
          // Continue reading, until you read 5 more "|"
          do {
            next_zeichen = (byte)myFile.read();
            if (next_zeichen == 124) {
              counter++;
            }
          } while (counter != 5);
          boolean result = (boolean)myFile.read();
          myFile.close();
          return result;
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
  myFile.close();
  return false;  
}

void toggle_inside(String user_string) {
  byte next_zeichen;
  boolean skip_flag = false;
  String current_code;
  File myFile = SD.open("/data.txt");

  while (myFile.available()) {
    next_zeichen = (byte)myFile.read();
    if (next_zeichen == 124) {
      if (!skip_flag) {
        if (user_string == current_code) {
          // Correct user found
          short counter = 0;
          // Continue reading, until you read 5 more "|"
          do {
            next_zeichen = (byte)myFile.read();
            if (next_zeichen == 124) {
              counter++;
            }
          } while (counter != 5);
          boolean result = (boolean)myFile.read();
          myFile.close();
          return result;
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
  myFile.close();
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

long get_unixtime() {
  rtc.isrunning();
  DateTime now = rtc.now();
  return now.unixtime();
}
