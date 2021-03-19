#include <LiquidCrystal_I2C.h> 
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include "RTClib.h"

// Variablen für die Displays
LiquidCrystal_I2C lcd1(0x26, 16, 2);
LiquidCrystal_I2C lcd2(0x27, 16, 2);
RTC_DS1307 rtc;

// Pins für die Kommunikation mit den Nanos (für die Servos)
const byte digComAusgang = 26;
const byte digComEingang = 27;

// Hier wird der empfangene Code von den Nanos drin gespeichert
String number_to_compare;
String received_number = "#####n";

// Name des Wlan-Hotspots
const char* ssid     = "ESP32-Access-Point";

// Initialisierung des WebServers
AsyncWebServer server(80);

// Hier wird die http Anfrage drin gespeichert
String header;

// Anhand dieser Konstanten werden später die einzelnen Eingaben der Nutzer voneinander unterschieden
const char* PARAM_INPUT_1 = "name";
const char* PARAM_INPUT_2 = "nname";
const char* PARAM_INPUT_3 = "email";

// In diesen Variablen werden die Daten bei der Registrierung dann gespeichert
String inputvName;
String inputnName;
String inputEmail;

String current_msg;

// Hier werden die aktuellen Personen gezählt.
int person_counter = 0;

// Das ist die "Stellschraube für die maximale Personenanzahl"
const byte MAX_PERSONS = 20;

// Variablen für die Turingmaschine
char band[] = {
        '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
        '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'
        };
short position = 1;

int flag = 0;
boolean result;

// HTML-Code der Website
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


// Was soll gesendet werden, wenn eine falsche URL eingegeben wird
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}


void setup() {
  // I2C Bus als Master beitreten
  Wire.begin();
  
  // Initiieren der LCDs
  lcd1.init();
  delay(20);   
  lcd2.init();                   
  delay(20);
  lcd1.backlight();
  delay(20);                    
  lcd2.backlight();

  // Starten des Accesspoints
  WiFi.softAP(ssid);
  
  // Initialisiern der RTC
  rtc.begin();

  // Initialisierung der SD-Karte
  SD.begin(5);

  // Die Pins für die Nano Signale für die Servos
  pinMode(digComAusgang, OUTPUT);
  pinMode(digComEingang, OUTPUT);

  // Erster Code (für die nächste Registrierung) wird schonmal generiert
  randomSeed(analogRead(0));
  do {
     number_to_compare = (String)genCode();
  } while(code_already_exists(number_to_compare));
  current_msg = "" + number_to_compare + "|";  

  // Bereitstellen der Website
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });


  // Was passiert wenn Daten vom ESP ankommen
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputParam;
    // Eingegebene Daten werden ausgelesen
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
    // Kompletten Datenbank-String zusammenbauen
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
    // Auf SD-Karte schreiben
    write_string_to_EOF(current_msg);
    // Bestätigung an Nutzer senden
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputvName + ", " + inputnName + ", " + inputEmail + 
                                     "<br>Dein Code fuer das G.A.T.E. lautet " + number_to_compare + 
                                     "<br><a href=\"/\">Return to Home Page</a>");
    // Zurücksetzen und auf nächsten Nutzer vorbereiten
    do {
       number_to_compare = (String)genCode();
    } while(code_already_exists(number_to_compare));
    current_msg = "" + number_to_compare + "|";  
  });
  server.onNotFound(notFound);
  
  server.begin();
}

void loop(){
  // Nano 1 nach neuem Code fragen
  received_number = "";
  Wire.requestFrom(1, 6);    //7 Bytes vom "Eingangs"-Arduino abfragen
  while (Wire.available()) {
    byte temp = Wire.read();
    received_number += (char)temp; // Direkt als Zeichen casten
  }

  // Auf Eingangs-Display anzeigen
  lcd1.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("Code eingeben:");
  lcd1.setCursor(0, 1);
  lcd1.print(received_number.substring(0, 5));
  
  // Wenn jemand eine Eingabe bestätigt hat
  if (received_number.substring(5) == "y") {
    char temp[5];
	// Aufsplitten der Nachricht
    for (int i = 0; i<5; i++){
      temp[i] = received_number.charAt(i);
    }
    // Prüfung durch Turingmaschine
    if (tm(temp)) {
      // Prüfung ob der Nutzer existiert
      if (code_already_exists(received_number.substring(0, 5))) {
        // Prüfung ob der Nutzer schon im Gebäude ist
        if (!is_inside(received_number.substring(0, 5))) {
          // Prüfung ob noch Platz im Gebäude ist
          if (person_counter < MAX_PERSONS) {
            // Bestätigung anzeigen
            lcd1.clear();
            lcd1.setCursor(0, 0);
            lcd1.print("***Akzeptiert***");
            lcd1.setCursor(0, 1);
            lcd1.print("Oeffne Tuer");
            //Schranken öffnen
            digitalWrite(digComEingang, HIGH);
            delay(1000);
            digitalWrite(digComEingang, LOW);
            delay(1000);
  
            // Display aktualisieren
            lcd1.clear();
            lcd1.setCursor(0, 0);
            lcd1.print("***Drinnen***");
            lcd1.setCursor(0, 1);
            lcd1.print("Schliesse Tuer");
            // Datenbank aktualisieren
            toggle_user(received_number.substring(0, 5));
            // Zähler aktualisieren
            person_counter++;
          }
		  // Fehlermeldung wenn kein Platz im Gebäude ist
          else {
            lcd1.clear();
            lcd1.setCursor(0, 0);
            lcd1.print("Restaurant ist voll");
          }
        }
		// Fehlermeldung wenn der Nutzer eigentlich schon drin ist
        else {
          lcd1.clear();
          lcd1.setCursor(0, 0);
          lcd1.print("Code abgelehnt");
        }
      }
	  // Fehlermeldung wenn der Nutzer nicht im System existiert
      else {
        lcd1.clear();
        lcd1.setCursor(0, 0);
        lcd1.print("Code falsch");
      }
    }
	// Fehlermeldung wenn die Turingmaschine nicht bestätigt
    else {
      lcd1.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("Unmoeglich");
    }
  }
  delay(500);
  
  // Nano 2 nach neuem Code fragen
  received_number = "";
  Wire.requestFrom(2, 6);    //7 Bytes vom "Ausgangs"-Arduino abfragen
  while (Wire.available()) { // slave may send less than requested
    byte temp = Wire.read();
    received_number += (char)temp; // Direkt als Zeichen casten
  }

  // Auf Ausgangs-Display anzeigen
  lcd2.clear();
  lcd2.setCursor(0, 0);
  lcd2.print("Code eingeben:");
  lcd2.setCursor(0, 1);
  lcd2.print(received_number.substring(0, 5));
  
  // Wenn jemand eine Eingabe bestätigt hat
  if (received_number.substring(5) == "y") {
    char temp[5];
	// Aufsplitten der Nachricht
    for (int i = 0; i<5; i++){
      temp[i] = received_number.charAt(i);
    }
    // Prüfung durch Turingmaschine
    if (tm(temp)) {
      // Prüfung ob der Nutzer existiert
      if (code_already_exists(received_number.substring(0, 5))) {
        // Prüfung ob der Nutzer tatsächlich im Gebäude ist
        if (is_inside(received_number.substring(0, 5))) {
          // Bestätigung anzeigen
          lcd2.clear();
          lcd2.setCursor(0, 0);
          lcd2.print("***Akzeptiert***");
          lcd2.setCursor(0, 1);
          lcd2.print("Oeffne Tuer");
          //Schranken öffnen
          digitalWrite(digComAusgang, HIGH);
          delay(1000);
          digitalWrite(digComAusgang, LOW);
          delay(1000);
          // Wait for person to enter

          // Display aktualisieren
          lcd2.clear();
          lcd2.setCursor(0, 0);
          lcd2.print("***Draußen***");
          lcd2.setCursor(0, 1);
          lcd2.print("Schliesse Tuer");
          // Datenbank aktualisieren
          toggle_user(received_number.substring(0, 5));
          // Zähler aktualisieren
          person_counter--;
        }
		// Fehlermeldung wenn der Nutzer nicht drin ist
        else {
          lcd2.clear();
          lcd2.setCursor(0, 0);
          lcd2.print("Code abgelehnt");
        }
      }
	  // Fehlermeldung wenn der Nutzer nicht im System existiert
      else {
        lcd2.clear();
        lcd2.setCursor(0, 0);
        lcd2.print("Code falsch");
      }
    }
	// Fehlermeldung wenn die Turingmaschine nicht bestätigt
    else {
      lcd2.clear();
      lcd2.setCursor(0, 0);
      lcd2.print("Unmoeglich");
    }
  }
  delay(500);
}

// Diese Funktion nimmt eine Zeichenkette als Parameter und schreibt diese
// ans Ende der aktuellen Datenbankdatei
void write_string_to_EOF(String to_write) {
  File myFile;
  // Welche der Dateien existiert gerade
  if (SD.exists("/data.txt")) {
    myFile = SD.open("/data.txt", FILE_WRITE);
  }
  else if (SD.exists("/data2.txt")) {
    myFile = SD.open("/data2.txt", FILE_WRITE);
  }
  else {
    myFile = SD.open("/data.txt", FILE_WRITE);
  }
  // Schreiben
  if (myFile) {
    myFile.seek(myFile.size());
    myFile.println(to_write);
    myFile.close();
  }
}

// Diese Funktion nimmt eine Zeichenkette als Paramter und überprüft die aktuelle
// Datenbankdatei und liefert wahr als Rückgabewert wenn diese Zeichenkette als Code existiert
boolean code_already_exists(String to_compare) {
  byte next_zeichen;
  boolean skip_flag = false;
  String current_code;
  File myFile;
  // Welche der Dateien existiert gerade
  if (SD.exists("/data.txt")) {
    myFile = SD.open("/data.txt");
  }
  else if (SD.exists("/data2.txt")) {
    myFile = SD.open("/data2.txt");
  }

  while (myFile.available()) {
    next_zeichen = (byte)myFile.read();
	// Wenn ein Trennzeichen gelesen wurde
    if (next_zeichen == 124) {
	  // Wenn wir nicht gerade mittendrin sind
      if (!skip_flag) {
		// Wenn das gelesene mit dem Parameter übereinstimmt
        if (to_compare == current_code) {
          myFile.close();
          return true;
        }
        current_code = "";
      }
      skip_flag = true;
    }
	// Wenn wir tatsächlich gerade am Anfang einer Zeile sind (Wo der Code steht)
    if (!skip_flag) {
	  // Wenn da eine Zahl steht
      if (isDigit((char)next_zeichen)) {
		// Eine Ziffer des Codes der Zeile
        current_code += (char)next_zeichen;
      }
    }
	// Wenn wir das Ende einer Zeile erreicht haben
    if (next_zeichen == 13) {
	  // Zurücksetzen der flag, dass jetzt theoretisch wieder ein Code ausgelesen werden kann
      skip_flag = false;
    }
  }
  // Standardmäßig falsch
  myFile.close();
  return false;  
}


// Diese Funktion nimmt eine Zeichenkette als Paramter und überprüft die aktuelle
// Datenbankdatei und liefert wahr als Rückgabewert wenn diese Zeichenkette als Code existiert
// und wenn der Besitzer dieser Zeichenkette sich gerade im Gebäude befindet
boolean is_inside(String user_string) {
  byte next_zeichen;
  bool skip_flag = false;
  String current_code;
  bool result;
  File myFile;
  // Welche der Dateien existiert gerade
  if (SD.exists("/data.txt")) {
    myFile = SD.open("/data.txt");
  }
  else if (SD.exists("/data2.txt")) {
    myFile = SD.open("/data2.txt");
  }

  while (myFile.available()) {
    next_zeichen = (byte)myFile.read();
	// Wenn ein Trennzeichen gelesen wurde
    if (next_zeichen == 124) {
		// Wenn wir nicht gerade mittendrin sind
      if (!skip_flag) {
		// Wenn das gelesene mit dem Parameter übereinstimmt
        if (user_string == current_code) {
          short counter = 0;
          // Weiterlesen, bis 5 weitere Trennzeichen gelesen wurden
          do {
            next_zeichen = (byte)myFile.read();
            if (next_zeichen == 124) {
              counter++;
            }
          } while (counter != 5);
          char temp = (char)myFile.read();
		  // Überprüfung ob da eine 0 oder 1 steht
          if (temp == '0') {
            result = false;
          }
          else if (temp == '1') {
            result = true;
          }
          myFile.close();
          return result;
        }
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


// Diese Funktion nimmt eine Zeichenkette als Paramter und überprüft die aktuelle
// Datenbankdatei ob diese Zeichenkette als Code existiert und ändert den "Status"
// Des Nutzers -> ob ersich im Gebäude befindet oder nicht
void toggle_user(String user_string) {
  byte next_zeichen;
  boolean skip_flag = false;
  String current_code;
  File oldFile;
  File newFile;
  // Überprüfung welche der Dateien existiert und Erstellung der anderen
  if (SD.exists("/data.txt")) {
    oldFile = SD.open("/data.txt");
    newFile = SD.open("/data2.txt", FILE_WRITE);
  }
  else if (SD.exists("/data2.txt")) {
    oldFile = SD.open("/data2.txt");
    newFile = SD.open("/data.txt", FILE_WRITE);
  }

  while (oldFile.available()) {
    next_zeichen = (byte)oldFile.read();
    newFile.print((char)next_zeichen);
    if (next_zeichen == 124) {
      if (!skip_flag) {
        if (user_string == current_code) {
          short counter = 0;
          do {
            next_zeichen = (byte)oldFile.read();
			// Alles erstmal übernehmen
            newFile.print((char)next_zeichen);
            if (next_zeichen == 124) {
              counter++;
            }
          } while (counter != 5);
		  // Ersetzen der "Status" Variable
          char old = (char)oldFile.read();
          if (old == '1') {
            newFile.print(0);
          }
          else {
            newFile.print(1);
          }
        }
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
  
  // Alte Datei löschen, neue speichern
  String oldFileName = oldFile.name();
  oldFile.close();
  newFile.close();
  if (oldFileName == "/data.txt") {
    SD.remove("/data.txt");
  }
  else if(oldFileName == "/data2.txt") {
    SD.remove("/data2.txt");
  } 
}

// Diese Funktion generiert einen theoretisch möglichen Code der eine korrekte Quersumme hat
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

// Diese Funktion gibt den aktuellen UnixZeitstempel von der RTC wieder
long get_unixtime() {
  rtc.isrunning();
  DateTime now = rtc.now();
  return now.unixtime();
}

// Implementierung der Turingmaschine
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
       read_value = band[position];
       write_value = get_write_value(state, read_value);
       if (write_value == '-1') {
           return false;
       }
       next_state = get_next_state(state, read_value);
       if (next_state == final_state) {
           return true;
       }
       else if (next_state == -1) {
           return false;
       }
        next_move = get_next_move(state, read_value);
        if (next_move == '-1') {
            return false;
        }
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
