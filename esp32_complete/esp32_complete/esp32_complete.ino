#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>

// Replace with your network credentials
const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";

// Set web server port number to 80
AsyncWebServer server(80);

// Variable to store the HTTP request
String header;

const char* PARAM_INPUT_1 = "name";
const char* PARAM_INPUT_2 = "nname";
const char* PARAM_INPUT_3 = "email";

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

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

    // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });


  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputvName;
    String inputnName;
    String inputEmail;
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
    Serial.println(inputvName);
    Serial.println(inputnName);
    Serial.println(inputEmail);

    short chartime = 20;
    transmit_word(inputvName, chartime);
    transmit_word_done();
    transmit_word(inputnName, chartime);
    transmit_word_done();
    transmit_word(inputEmail, chartime);
    transmit_done();
    
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputvName + ", " + inputnName + ", " + inputEmail + 
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });
  server.onNotFound(notFound);
  
  server.begin();
}

void loop(){
}

void transmit_word(String pWord, int char_delay) {
    for (short i = 1; i <= pWord.length(); i++) {
      Serial.println(pWord.substring(i-1, i));
      Wire.beginTransmission(9);
      Wire.write(pWord.substring(i-1, i).c_str());
      Wire.endTransmission();
      delay(char_delay);
    }
}

void transmit_word_done() {
    String spacer = "§";
    Wire.beginTransmission(9);
    Wire.write(spacer.c_str());
    Wire.endTransmission();
}

void transmit_done() {
    String spacer = "|";
    Wire.beginTransmission(9);
    Wire.write(spacer.c_str());
    Wire.endTransmission();
}
