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

const byte digComAusgang = 26; //communication via digital-Pin to nano
const byte digComEingang = 27;

String number_to_compare;
String received_number = "#####n";

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

int person_counter = 0;
const byte MAX_PERSONS = 20;

char band[] = {
        '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
        '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'
        };
short position = 1;

int flag = 0;
boolean result;

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

  pinMode(digComAusgang, OUTPUT);
  pinMode(digComEingang, OUTPUT);

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
    // Tell user his data
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputvName + ", " + inputnName + ", " + inputEmail + 
                                     "<br>Dein Code fuer das G.A.T.E. lautet " + number_to_compare + 
                                     "<br><a href=\"/\">Return to Home Page</a>");
    // Reset everything back to start
    do {
       number_to_compare = (String)genCode();
    } while(code_already_exists(number_to_compare));
    current_msg = "" + number_to_compare + "|";  
  });
  server.onNotFound(notFound);
  
  server.begin();
}

void loop(){
  // ask nano 1 for new code
  received_number = "";
  Wire.requestFrom(1, 6);    // request 7 bytes from slave device #1
  while (Wire.available()) { // slave may send less than requested
    byte temp = Wire.read();
    Serial.println((char)temp);
    received_number += (char)temp; // receive a byte as character   
  }
  Serial.println(received_number);

  lcd1.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("Code eingeben:");
  lcd1.setCursor(0, 1);
  lcd1.print(received_number.substring(0, 5));
  
  if (received_number.substring(5) == "y") {
    // Someone wants to get in
    char temp[5];
    for (int i = 0; i<5; i++){
      temp[i] = received_number.charAt(i);
    }
    // Check if tm validates the code
    if (tm(temp)) {
      // Check db if user is registered
      if (code_already_exists(received_number.substring(0, 5))) {
        // Check db if user is already in
        if (!is_inside(received_number.substring(0, 5))) {
          // Check if person_counter is low enough for one more person to enter the building
          if (person_counter < MAX_PERSONS) {
            // Display confirmation
            lcd1.clear();
            lcd1.setCursor(0, 0);
            lcd1.print("***Akzeptiert***");
            lcd1.setCursor(0, 1);
            lcd1.print("Oeffne Tuer");
            //open doors
            digitalWrite(digComEingang, HIGH);
            delay(1000);
            digitalWrite(digComEingang, LOW);
            delay(1000);
            // Wait for person to enter
  
            // Update display
            lcd1.clear();
            lcd1.setCursor(0, 0);
            lcd1.print("***Drinnen***");
            lcd1.setCursor(0, 1);
            lcd1.print("Schliesse Tuer");
            // Update db
            toggle_user(received_number.substring(0, 5));
            // Change counter
            person_counter++;
          }
          else {
            lcd1.clear();
            lcd1.setCursor(0, 0);
            lcd1.print("Restaurant ist voll");
          }
        }
        else {
          lcd1.clear();
          lcd1.setCursor(0, 0);
          lcd1.print("Code abgelehnt");
        }
      }
      else {
        lcd1.clear();
        lcd1.setCursor(0, 0);
        lcd1.print("Code falsch");
      }
    }
    else {
      lcd1.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("Unmoeglich");
    }
  }
  delay(500);
  // ask nano 2 for new code
  received_number = "";
  Wire.requestFrom(2, 6);    // request 7 bytes from slave device #1
  while (Wire.available()) { // slave may send less than requested
    byte temp = Wire.read();
    Serial.println((char)temp);
    received_number += (char)temp; // receive a byte as character   
  }
  Serial.println(received_number);

  lcd2.clear();
  lcd2.setCursor(0, 0);
  lcd2.print("Code eingeben:");
  lcd2.setCursor(0, 1);
  lcd2.print(received_number.substring(0, 5));
  
  if (received_number.substring(5) == "y") {
    // Someone wants to get out
    char temp[5];
    for (int i = 0; i<5; i++){
      temp[i] = received_number.charAt(i);
    }
    // Check if tm validates the code
    if (tm(temp)) {
      // Check db if user is registered
      if (code_already_exists(received_number.substring(0, 5))) {
        // Check db if user is actually in
        if (is_inside(received_number.substring(0, 5))) {
          // Display confirmation
          lcd2.clear();
          lcd2.setCursor(0, 0);
          lcd2.print("***Akzeptiert***");
          lcd2.setCursor(0, 1);
          lcd2.print("Oeffne Tuer");
          // Open the gates
          digitalWrite(digComAusgang, HIGH);
          delay(1000);
          digitalWrite(digComAusgang, LOW);
          delay(1000);
          // Wait for person to enter

          // Update display
          lcd2.clear();
          lcd2.setCursor(0, 0);
          lcd2.print("***Draußen***");
          lcd2.setCursor(0, 1);
          lcd2.print("Schliesse Tuer");
          // Update db
          toggle_user(received_number.substring(0, 5));
          // Change counter
          person_counter--;
        }
        else {
          lcd2.clear();
          lcd2.setCursor(0, 0);
          lcd2.print("Code abgelehnt");
        }
      }
      else {
        lcd2.clear();
        lcd2.setCursor(0, 0);
        lcd2.print("Code falsch");
      }
    }
    else {
      lcd2.clear();
      lcd2.setCursor(0, 0);
      lcd2.print("Unmoeglich");
    }
  }
  delay(500);
}

void write_string_to_EOF(String to_write) {
  File myFile;
  if (SD.exists("/data.txt")) {
    myFile = SD.open("/data.txt", FILE_WRITE);
  }
  else if (SD.exists("/data2.txt")) {
    myFile = SD.open("/data2.txt", FILE_WRITE);
  }
  else {
    myFile = SD.open("/data.txt", FILE_WRITE);
  }
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
  Serial.println("yeet it");
  byte next_zeichen;
  boolean skip_flag = false;
  String current_code;
  File myFile;
  if (SD.exists("/data.txt")) {
    myFile = SD.open("/data.txt");
  }
  else if (SD.exists("/data2.txt")) {
    myFile = SD.open("/data2.txt");
  }

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
  bool skip_flag = false;
  String current_code;
  bool result;
  File myFile;
  if (SD.exists("/data.txt")) {
    myFile = SD.open("/data.txt");
  }
  else if (SD.exists("/data2.txt")) {
    myFile = SD.open("/data2.txt");
  }
  Serial.println(myFile.name());

  while (myFile.available()) {
    next_zeichen = (byte)myFile.read();
    if (next_zeichen == 124) {
      if (!skip_flag) {
        if (user_string == current_code) {
          // Correct user found
          Serial.println("Correct code found");
          short counter = 0;
          // Continue reading, until you read 5 more "|"
          do {
            next_zeichen = (byte)myFile.read();
            if (next_zeichen == 124) {
              counter++;
            }
          } while (counter != 5);
          char temp = (char)myFile.read();
          Serial.println(temp);
          if (temp == '0') {
            Serial.println("Der ist draußen");
            result = false;
          }
          else if (temp == '1') {
            Serial.println("Der ist drinnen");
            result = true;
          }
          Serial.println(result);
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

void toggle_user(String user_string) {
  byte next_zeichen;
  boolean skip_flag = false;
  String current_code;
  File oldFile;
  File newFile;
  if (SD.exists("/data.txt")) {
    Serial.println("Wir sind bei der ersten Datei");
    oldFile = SD.open("/data.txt");
    newFile = SD.open("/data2.txt", FILE_WRITE);
  }
  else if (SD.exists("/data2.txt")) {
    Serial.println("Wir sind bei der zweiten Datei");
    oldFile = SD.open("/data2.txt");
    newFile = SD.open("/data.txt", FILE_WRITE);
  }

  while (oldFile.available()) {
    next_zeichen = (byte)oldFile.read();
    newFile.print((char)next_zeichen);
    if (next_zeichen == 124) {
      if (!skip_flag) {
        if (user_string == current_code) {
          // Correct user found
          short counter = 0;
          // Continue reading, until you read 5 more "|"
          do {
            next_zeichen = (byte)oldFile.read();
            newFile.print((char)next_zeichen);
            if (next_zeichen == 124) {
              counter++;
            }
          } while (counter != 5);
          char old = (char)oldFile.read();
          if (old == '1') {
            newFile.print(0);
          }
          else {
            newFile.print(1);
          }
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
  
  String oldFileName = oldFile.name();
  Serial.println(oldFileName);
  oldFile.close();
  newFile.close();
  if (oldFileName == "/data.txt") {
    SD.remove("/data.txt");
  }
  else if(oldFileName == "/data2.txt") {
    SD.remove("/data2.txt");
  } 
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

//TM
bool tm(char input[]) {
    reset_tm();
    const char NONE = 'N';
    const char LEFT = 'L';
    const char RIGHT = 'R';

    short state = 0;
    write_input_to_band(input);
    short final_state = 42;

    char read_value;
    char write_value;
    int next_state;
    char next_move;
    while (true) {
       //Serial.println(band);
       read_value = band[position];
       //Serial.print("read");
       //Serial.println(read_value);
       write_value = get_write_value(state, read_value);
       if (write_value == '-1') {
           return false;
       }
       //Serial.print("write");
       //Serial.println(write_value);
       next_state = get_next_state(state, read_value);
       if (next_state == final_state) {
           return true;
       }
       else if (next_state == -1) {
           return false;
       }
       //Serial.print("state");
       //Serial.println(next_state);
        next_move = get_next_move(state, read_value);
        if (next_move == '-1') {
            return false;
        }
       //Serial.print("move");
       //Serial.println(next_move);
        band[position] = write_value;
        state = next_state;
        if (next_move == NONE) {
            position = position;
        }
        else if (next_move == RIGHT) {
            position++;
        }
        else if (next_move == LEFT) {
            position--;
        }
    }
}


void reset_tm() {
  for (short i = 0; i < 20; i++) {
    band[i] = '#';
  }
  position = 1;
}


void write_input_to_band(char input[]) {
    short backup = position;
    for (short i = 0; i < 5; i++) {
        band[position] = input[i];
        position++;
    }
    position = backup;
}

char get_write_value(int state, char read){
    switch (state)
    {
    case 0:
        switch (read)
        {
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 1:
        switch (read)
        {
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 2:
        switch (read)
        {
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 3:
        switch (read)
        {
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 4:
        switch (read)
        {
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 5:
        switch (read)
        {
        case '#':
            return '|';
        default:
            break;
        }
    case 6:
        switch (read)
        {
        case '#':
            return '0';
        default:
            break;
        }
    case 7:
        switch (read)
        {
        case '#':
            return '0';
        default:
            break;
        }
    case 8:
        switch (read)
        {
        case '#':
            return '0';
        default:
            break;
        }
    case 9:
        switch (read)
        {
        case '#':
            return '0';
        default:
            break;
        }
    case 10:
        switch (read)
        {
        case '#':
            return '0';
        default:
            break;
        }
    case 11:
        switch (read)
        {
        case '#':
            return '0';
        default:
            break;
        }
    case 12:
        switch (read)
        {
        case '#':
            return '|';
        default:
            break;
        }
    case 13:
        switch (read)
        {
        case '|':
            return '|';
        case '0':
            return '0';
        case '1':
            return '1';
        default:
            break;
        }
    case 14:
        switch (read)
        {
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 15:
        switch (read)
        {
        case '0':
            return '0';
        case '1':
            return '0';
        case '2':
            return '1';
        case '3':
            return '2';
        case '4':
            return '3';
        case '5':
            return '4';
        case '6':
            return '5';
        case '7':
            return '6';
        case '8':
            return '7';
        case '9':
            return '8';
        default:
            break;
        }
    case 16:
        switch (read)
        {
        case '#':
            return '#';
        case '0':
            return '0';
        case '1':
            return '0';
        case '2':
            return '1';
        case '3':
            return '2';
        case '4':
            return '3';
        case '5':
            return '4';
        case '6':
            return '5';
        case '7':
            return '6';
        case '8':
            return '7';
        case '9':
            return '8';
        default:
            break;
        }
    case 17:
        switch (read)
        {
        case '|':
            return '|';
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 18:
        switch (read)
        {
        case '0':
            return '1';
        case '1':
            return '0';
        default:
            break;
        }
    case 19:
        switch (read)
        {
        case '#':
            return '0';
        case '|':
            return '|';
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 20:
        switch (read)
        {
        case '#':
            return '0';
        default:
            break;
        }
    case 21:
        switch (read)
        {
        case '#':
            return '|';
        default:
            break;
        }
    case 22:
        switch (read)
        {
        case '|':
            return '|';
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 23:
        switch (read)
        {
        case '|':
            return '|';
        case '0':
            return '0';
        case '1':
            return '0';
        default:
            break;
        }
    case 24:
        switch (read)
        {
        case '|':
            return '|';
        case '0':
            return '1';
        default:
            break;
        }
    case 25:
        switch (read)
        {
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 26:
        switch (read)
        {
        case '0':
            return '1';
        case '1':
            return '2';
        case '2':
            return '3';
        case '3':
            return '4';
        case '4':
            return '5';
        case '5':
            return '6';
        case '6':
            return '7';
        case '7':
            return '8';
        case '8':
            return '9';
        case '9':
            return '0';
        default:
            break;
        }
    case 27:
        switch (read)
        {
        case '0':
            return '1';
        case '1':
            return '2';
        case '2':
            return '3';
        case '3':
            return '4';
        default:
            break;
        }
    case 28:
        switch (read)
        {
        case '|':
            return '|';
        case '0':
            return '0';
        default:
            break;
        }
    case 29:
        switch (read)
        {
        case '0':
            return '0';
        case '1':
            return '0';
        case '2':
            return '1';
        case '3':
            return '2';
        case '4':
            return '3';
        case '5':
            return '4';
        case '6':
            return '5';
        case '7':
            return '6';
        case '8':
            return '7';
        case '9':
            return '8';
        default:
            break;
        }   
    case 30:
        switch (read)
        {
        case '0':
            return '0';
        case '1':
            return '0';
        case '2':
            return '1';
        case '3':
            return '2';
        case '4':
            return '3';
        case '5':
            return '4';
        case '6':
            return '5';
        case '7':
            return '6';
        case '8':
            return '7';
        case '9':
            return '8';
        default:
            break;
        } 
    case 31:
        switch (read)
        {
        case '0':
            return '1';
        case '1':
            return '2';
        case '2':
            return '3';
        case '3':
            return '4';
        case '4':
            return '5';
        case '5':
            return '6';
        case '6':
            return '7';
        case '7':
            return '8';
        case '8':
            return '9';
        case '9':
            return '0';
        default:
            break;
        } 
    case 32:
        switch (read)
        {
        case '0':
            return '1';
        case '1':
            return '2';
        case '2':
            return '3';
        case '3':
            return '4';
        case '4':
            return '5';
        case '5':
            return '6';
        case '6':
            return '7';
        case '7':
            return '8';
        case '8':
            return '9';
        default:
            break;
        } 
    case 33:
        switch (read)
        {
        case '0':
            return '0';
        default:
            break;
        } 
    case 34:
        switch (read)
        {
        case '0':
            return '0';
        default:
            break;
        } 
    case 35:
        switch (read)
        {
        case '|':
            return '|';
        default:
            break;
        } 
    case 36:
        switch (read)
        {
        case '|':
            return '|';
        case '0':
            return '0';
        default:
            break;
        } 
    case 37:
        switch (read)
        {
        case '1':
            return '0';
        case '2':
            return '1';
        case '3':
            return '2';
        case '4':
            return '3';
        case '5':
            return '4';
        case '6':
            return '5';
        case '7':
            return '6';
        case '8':
            return '7';
        case '9':
            return '8';
        default:
            break;
        }
    case 38:
        switch (read)
        {
        case '#':
            return '#';
        case '|':
            return '|';
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 39:
        switch (read)
        {
        case '|':
            return '|';
        default:
            break;
        }
    case 40:
        switch (read)
        {
        case '|':
            return '|';
        case '0':
            return '0';
        default:
            break;
        }
    case 41:
        switch (read)
        {
        case '0':
            return '0';
        default:
            break;
        }
    case 42:
        break;
    case 43:
        switch (read)
        {
        case '|':
            return '|';
        default:
            break;
        }
    case 44:
        switch (read)
        {
        case '|':
            return '|';
        case '0':
            return '0';
        case '1':
            return '1';
        default:
            break;
        }

    default:
        return '-1';
    }
    return '-1';
}

int get_next_state(int state, char read){
    switch (state)
    {
    case 0:
        switch (read)
        {
        case '0':
            return 1;
        case '1':
            return 1;
        case '2':
            return 1;
        case '3':
            return 1;
        case '4':
            return 1;
        case '5':
            return 1;
        case '6':
            return 1;
        case '7':
            return 1;
        case '8':
            return 1;
        case '9':
            return 1;
        default:
            break;
        }
    case 1:
        switch (read)
        {
        case '0':
            return 2;
        case '1':
            return 2;
        case '2':
            return 2;
        case '3':
            return 2;
        case '4':
            return 2;
        case '5':
            return 2;
        case '6':
            return 2;
        case '7':
            return 2;
        case '8':
            return 2;
        case '9':
            return 2;
        default:
            break;
        }
    case 2:
        switch (read)
        {
        case '0':
            return 3;
        case '1':
            return 3;
        case '2':
            return 3;
        case '3':
            return 3;
        case '4':
            return 3;
        case '5':
            return 3;
        case '6':
            return 3;
        case '7':
            return 3;
        case '8':
            return 3;
        case '9':
            return 3;
        default:
            break;
        }
    case 3:
        switch (read)
        {
        case '0':
            return 4;
        case '1':
            return 4;
        case '2':
            return 4;
        case '3':
            return 4;
        case '4':
            return 4;
        case '5':
            return 4;
        case '6':
            return 4;
        case '7':
            return 4;
        case '8':
            return 4;
        case '9':
            return 4;
        default:
            break;
        }
    case 4:
        switch (read)
        {
        case '0':
            return 5;
        case '1':
            return 5;
        case '2':
            return 5;
        case '3':
            return 5;
        case '4':
            return 5;
        case '5':
            return 5;
        case '6':
            return 5;
        case '7':
            return 5;
        case '8':
            return 5;
        case '9':
            return 5;
        default:
            break;
        }
    case 5:
        switch (read)
        {
        case '#':
            return 6;
        default:
            break;
        }
    case 6:
        switch (read)
        {
        case '#':
            return 7;
        default:
            break;
        }
    case 7:
        switch (read)
        {
        case '#':
            return 8;
        default:
            break;
        }
    case 8:
        switch (read)
        {
        case '#':
            return 9;
        default:
            break;
        }
    case 9:
        switch (read)
        {
        case '#':
            return 10;
        default:
            break;
        }
    case 10:
        switch (read)
        {
        case '#':
            return 11;
        default:
            break;
        }
    case 11:
        switch (read)
        {
        case '#':
            return 12;
        default:
            break;
        }
    case 12:
        switch (read)
        {
        case '#':
            return 13;
        default:
            break;
        }
    case 13:
        switch (read)
        {
        case '|':
            return 14;
        case '0':
            return 13;
        case '1':
            return 13;
        default:
            break;
        }
    case 14:
        switch (read)
        {
        case '0':
            return 15;
        case '1':
            return 15;
        case '2':
            return 15;
        case '3':
            return 15;
        case '4':
            return 15;
        case '5':
            return 15;
        case '6':
            return 15;
        case '7':
            return 15;
        case '8':
            return 15;
        case '9':
            return 15;
        default:
            break;
        }
    case 15:
        switch (read)
        {
        case '0':
            return 16;
        case '1':
            return 17;
        case '2':
            return 17;
        case '3':
            return 17;
        case '4':
            return 17;
        case '5':
            return 17;
        case '6':
            return 17;
        case '7':
            return 17;
        case '8':
            return 17;
        case '9':
            return 17;
        default:
            break;
        }
    case 16:
        switch (read)
        {
        case '#':
            return 19;
        case '0':
            return 16;
        case '1':
            return 17;
        case '2':
            return 17;
        case '3':
            return 17;
        case '4':
            return 17;
        case '5':
            return 17;
        case '6':
            return 17;
        case '7':
            return 17;
        case '8':
            return 17;
        case '9':
            return 17;
        default:
            break;
        }
    case 17:
        switch (read)
        {
        case '|':
            return 44;
        case '0':
            return 17;
        case '1':
            return 17;
        case '2':
            return 17;
        case '3':
            return 17;
        case '4':
            return 17;
        case '5':
            return 17;
        case '6':
            return 17;
        case '7':
            return 17;
        case '8':
            return 17;
        case '9':
            return 17;
        default:
            break;
        }
    case 18:
        switch (read)
        {
        case '0':
            return 13;
        case '1':
            return 18;
        default:
            break;
        }
    case 19:
        switch (read)
        {
        case '#':
            return 20;
        case '|':
            return 19;
        case '0':
            return 19;
        case '1':
            return 19;
        case '2':
            return 19;
        case '3':
            return 19;
        case '4':
            return 19;
        case '5':
            return 19;
        case '6':
            return 19;
        case '7':
            return 19;
        case '8':
            return 19;
        case '9':
            return 19;
        default:
            break;
        }
    case 20:
        switch (read)
        {
        case '#':
            return 21;
        default:
            break;
        }
    case 21:
        switch (read)
        {
        case '#':
            return 22;
        default:
            break;
        }
    case 22:
        switch (read)
        {
        case '|':
            return 23;
        case '0':
            return 22;
        case '1':
            return 22;
        case '2':
            return 22;
        case '3':
            return 22;
        case '4':
            return 22;
        case '5':
            return 22;
        case '6':
            return 22;
        case '7':
            return 22;
        case '8':
            return 22;
        case '9':
            return 22;
        default:
            break;
        }
    case 23:
        switch (read)
        {
        case '|':
            return 28;
        case '0':
            return 23;
        case '1':
            return 24;
        default:
            break;
        }
    case 24:
        switch (read)
        {
        case '|':
            return 25;
        case '0':
            return 24;
        default:
            break;
        }
    case 25:
        switch (read)
        {
        case '0':
            return 26;
        case '1':
            return 26;
        case '2':
            return 26;
        case '3':
            return 26;
        case '4':
            return 26;
        case '5':
            return 26;
        case '6':
            return 26;
        case '7':
            return 26;
        case '8':
            return 26;
        case '9':
            return 26;
        default:
            break;
        }
    case 26:
        switch (read)
        {
        case '0':
            return 22;
        case '1':
            return 22;
        case '2':
            return 22;
        case '3':
            return 22;
        case '4':
            return 22;
        case '5':
            return 22;
        case '6':
            return 22;
        case '7':
            return 22;
        case '8':
            return 22;
        case '9':
            return 27;
        default:
            break;
        }
    case 27:
        switch (read)
        {
        case '0':
            return 22;
        case '1':
            return 22;
        case '2':
            return 22;
        case '3':
            return 22;
        default:
            break;
        }
    case 28:
        switch (read)
        {
        case '|':
            return 29;
        case '0':
            return 28;
        default:
            break;
        }
    case 29:
        switch (read)
        {
        case '0':
            return 30;
        case '1':
            return 31;
        case '2':
            return 31;
        case '3':
            return 31;
        case '4':
            return 31;
        case '5':
            return 31;
        case '6':
            return 31;
        case '7':
            return 31;
        case '8':
            return 31;
        case '9':
            return 31;
        default:
            break;
        }   
    case 30:
        switch (read)
        {
        case '0':
            return 34;
        case '1':
            return 33;
        case '2':
            return 33;
        case '3':
            return 33;
        case '4':
            return 33;
        case '5':
            return 33;
        case '6':
            return 33;
        case '7':
            return 33;
        case '8':
            return 33;
        case '9':
            return 33;
        default:
            break;
        } 
    case 31:
        switch (read)
        {
        case '0':
            return 29;
        case '1':
            return 29;
        case '2':
            return 29;
        case '3':
            return 29;
        case '4':
            return 29;
        case '5':
            return 29;
        case '6':
            return 29;
        case '7':
            return 29;
        case '8':
            return 29;
        case '9':
            return 32;
        default:
            break;
        } 
    case 32:
        switch (read)
        {
        case '0':
            return 29;
        case '1':
            return 29;
        case '2':
            return 29;
        case '3':
            return 29;
        case '4':
            return 29;
        case '5':
            return 29;
        case '6':
            return 29;
        case '7':
            return 29;
        case '8':
            return 29;
        default:
            break;
        } 
    case 33:
        switch (read)
        {
        case '0':
            return 35;
        default:
            break;
        } 
    case 34:
        switch (read)
        {
        case '0':
            return 39;
        default:
            break;
        } 
    case 35:
        switch (read)
        {
        case '|':
            return 36;
        default:
            break;
        } 
    case 36:
        switch (read)
        {
        case '|':
            return 37;
        case '0':
            return 36;
        default:
            break;
        } 
    case 37:
        switch (read)
        {
        case '1':
            return 38;
        case '2':
            return 38;
        case '3':
            return 38;
        case '4':
            return 38;
        case '5':
            return 38;
        case '6':
            return 38;
        case '7':
            return 38;
        case '8':
            return 38;
        case '9':
            return 38;
        default:
            return -1;
        }
    case 38:
        switch (read)
        {
        case '#':
            return 43;
        case '|':
            return 38;
        case '0':
            return 38;
        case '1':
            return 38;
        case '2':
            return 38;
        case '3':
            return 38;
        case '4':
            return 38;
        case '5':
            return 38;
        case '6':
            return 38;
        case '7':
            return 38;
        case '8':
            return 38;
        case '9':
            return 38;
        default:
            break;
        }
    case 39:
        switch (read)
        {
        case '|':
            return 40;
        default:
            break;
        }
    case 40:
        switch (read)
        {
        case '|':
            return 41;
        case '0':
            return 40;
        default:
            break;
        }
    case 41:
        switch (read)
        {
        case '0':
            return 42;
        default:
            break;
        }
    case 42:
        break;
    case 43:
        switch (read)
        {
        case '|':
            return 30;
        default:
            break;
        }
    case 44:
        switch (read)
        {
        case '|':
            return 18;
        case '0':
            return 44;
        case '1':
            return 44;
        default:
            break;
        }

    default:
        return -1;
    }
    return -1;
}

char get_next_move(int state, char read){
    switch (state)
    {
    case 0:
        switch (read)
        {
        case '0':
            return 'R';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 1:
        switch (read)
        {
        case '0':
            return 'R';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 2:
        switch (read)
        {
        case '0':
            return 'R';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 3:
        switch (read)
        {
        case '0':
            return 'R';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 4:
        switch (read)
        {
        case '0':
            return 'R';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 5:
        switch (read)
        {
        case '#':
            return 'R';
        default:
            break;
        }
    case 6:
        switch (read)
        {
        case '#':
            return 'R';
        default:
            break;
        }
    case 7:
        switch (read)
        {
        case '#':
            return 'R';
        default:
            break;
        }
    case 8:
        switch (read)
        {
        case '#':
            return 'R';
        default:
            break;
        }
    case 9:
        switch (read)
        {
        case '#':
            return 'R';
        default:
            break;
        }
    case 10:
        switch (read)
        {
        case '#':
            return 'R';
        default:
            break;
        }
    case 11:
        switch (read)
        {
        case '#':
            return 'R';
        default:
            break;
        }
    case 12:
        switch (read)
        {
        case '#':
            return 'L';
        default:
            break;
        }
    case 13:
        switch (read)
        {
        case '|':
            return 'L';
        case '0':
            return 'L';
        case '1':
            return 'L';
        default:
            break;
        }
    case 14:
        switch (read)
        {
        case '0':
            return 'L';
        case '1':
            return 'L';
        case '2':
            return 'L';
        case '3':
            return 'L';
        case '4':
            return 'L';
        case '5':
            return 'L';
        case '6':
            return 'L';
        case '7':
            return 'L';
        case '8':
            return 'L';
        case '9':
            return 'L';
        default:
            break;
        }
    case 15:
        switch (read)
        {
        case '0':
            return 'L';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 16:
        switch (read)
        {
        case '#':
            return 'R';
        case '0':
            return 'L';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 17:
        switch (read)
        {
        case '|':
            return 'R';
        case '0':
            return 'R';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 18:
        switch (read)
        {
        case '0':
            return 'L';
        case '1':
            return 'L';
        default:
            break;
        }
    case 19:
        switch (read)
        {
        case '#':
            return 'R';
        case '|':
            return 'R';
        case '0':
            return 'R';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 20:
        switch (read)
        {
        case '#':
            return 'R';
        default:
            break;
        }
    case 21:
        switch (read)
        {
        case '#':
            return 'L';
        default:
            break;
        }
    case 22:
        switch (read)
        {
        case '|':
            return 'L';
        case '0':
            return 'L';
        case '1':
            return 'L';
        case '2':
            return 'L';
        case '3':
            return 'L';
        case '4':
            return 'L';
        case '5':
            return 'L';
        case '6':
            return 'L';
        case '7':
            return 'L';
        case '8':
            return 'L';
        case '9':
            return 'L';
        default:
            break;
        }
    case 23:
        switch (read)
        {
        case '|':
            return 'R';
        case '0':
            return 'L';
        case '1':
            return 'R';
        default:
            break;
        }
    case 24:
        switch (read)
        {
        case '|':
            return 'R';
        case '0':
            return 'R';
        default:
            break;
        }
    case 25:
        switch (read)
        {
        case '0':
            return 'R';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 26:
        switch (read)
        {
        case '0':
            return 'L';
        case '1':
            return 'L';
        case '2':
            return 'L';
        case '3':
            return 'L';
        case '4':
            return 'L';
        case '5':
            return 'L';
        case '6':
            return 'L';
        case '7':
            return 'L';
        case '8':
            return 'L';
        case '9':
            return 'L';
        default:
            break;
        }
    case 27:
        switch (read)
        {
        case '0':
            return 'L';
        case '1':
            return 'L';
        case '2':
            return 'L';
        case '3':
            return 'L';
        default:
            break;
        }
    case 28:
        switch (read)
        {
        case '|':
            return 'R';
        case '0':
            return 'R';
        default:
            break;
        }
    case 29:
        switch (read)
        {
        case '0':
            return 'R';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }   
    case 30:
        switch (read)
        {
        case '0':
            return 'L';
        case '1':
            return 'L';
        case '2':
            return 'L';
        case '3':
            return 'L';
        case '4':
            return 'L';
        case '5':
            return 'L';
        case '6':
            return 'L';
        case '7':
            return 'L';
        case '8':
            return 'L';
        case '9':
            return 'L';
        default:
            break;
        } 
    case 31:
        switch (read)
        {
        case '0':
            return 'L';
        case '1':
            return 'L';
        case '2':
            return 'L';
        case '3':
            return 'L';
        case '4':
            return 'L';
        case '5':
            return 'L';
        case '6':
            return 'L';
        case '7':
            return 'L';
        case '8':
            return 'L';
        case '9':
            return 'L';
        default:
            break;
        } 
    case 32:
        switch (read)
        {
        case '0':
            return 'N';
        case '1':
            return 'N';
        case '2':
            return 'N';
        case '3':
            return 'N';
        case '4':
            return 'N';
        case '5':
            return 'N';
        case '6':
            return 'N';
        case '7':
            return 'N';
        case '8':
            return 'N';
        default:
            break;
        } 
    case 33:
        switch (read)
        {
        case '0':
            return 'L';
        default:
            break;
        } 
    case 34:
        switch (read)
        {
        case '0':
            return 'L';
        default:
            break;
        } 
    case 35:
        switch (read)
        {
        case '|':
            return 'L';
        default:
            break;
        } 
    case 36:
        switch (read)
        {
        case '|':
            return 'L';
        case '0':
            return 'L';
        default:
            break;
        } 
    case 37:
        switch (read)
        {
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 38:
        switch (read)
        {
        case '#':
            return 'L';
        case '|':
            return 'R';
        case '0':
            return 'R';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 39:
        switch (read)
        {
        case '|':
            return 'L';
        default:
            break;
        }
    case 40:
        switch (read)
        {
        case '|':
            return 'L';
        case '0':
            return 'L';
        default:
            break;
        }
    case 41:
        switch (read)
        {
        case '0':
            return 'N';
        default:
            break;
        }
    case 42:
        break;
    case 43:
        switch (read)
        {
        case '|':
            return 'L';
        default:
            break;
        }
    case 44:
        switch (read)
        {
        case '|':
            return 'L';
        case '0':
            return 'R';
        case '1':
            return 'R';
        default:
            break;
        }

    default:
        return '-1';
    }
    return '-1';
}
