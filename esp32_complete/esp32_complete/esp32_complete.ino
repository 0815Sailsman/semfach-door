#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include "RTClib.h"


RTC_DS1307 rtc;
// Flag if current user is already known
boolean flag = true;
String number_to_compare;

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
