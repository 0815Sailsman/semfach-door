#include <Wire.h>
#include <Keypad.h>
#include <Servo.h>

// Servos
Servo servo1;
Servo servo2;

// Abstandssensoren
int trigPin = 10;
int echoPin = 11;
// Variablen für die Ultraschallsensoren
long duration;
int distance;

//Hier kommt das Signal vom ESP für die Servos an
int comPin = 12;
// Servo Variablen 
boolean turauf = false;
boolean turzu = false;
boolean checkSchranke = false;
long myTimer = 0;
long myTimeout = 2000;

// Speicherung der Eingaben
char lastCode[6] = {'#', '#', '#', '#', '#', 'n'};

// Variablen für das Zahlenfeld
const byte ROWS = 4;               //4x4-Keypad
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5}; 
byte colPins[COLS] = {6,7,8,9}; 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

char eingabe[5] = {'#', '#', '#', '#', '#'};  //array für Code eingabe (# = unbelegt)
boolean arrayvoll = false;                    //boolean für check, ob array voll ist

char validation_char = 'n';

void setup() {
    // I2C Bus als Slave mit Adresse 2 beitreten
  Wire.begin(2);
  Wire.onRequest(requestEvent);
  pinMode(trigPin, OUTPUT);  //Abstandssensor
  pinMode(echoPin, INPUT);   //Abstandssensor
  pinMode(comPin, INPUT);   // Verbindung zum ESP
    // Noch beide Servos im Code, nur einer wird benutzt
  servo1.attach(A2);
  servo2.attach(A3);
  // Sicherstellen, dass die zu sind
  servo1.write(170);
  servo2.write(20);
}

void loop() {
  // Letzte Tastenfeld-Eingabe auslesen
  char key = keypad.getKey();
  if (key){
    // Wenn Buchstabentasten, ignorieren
    if (key =='A' || key =='B' || key =='C' || key=='D'){   //Unbelegte Keys
      ;   
    } else if(key =='#') {                   //wenn Eingabe key(#) gedrückt
      //Eingabe wird überprüft:
      lastCode[5] = 'y'; 
    } else if(key =='*'){           //wenn löschen-key(*) gedrückt wird
      //löscht letzte Eingabe
      for(int i = 0; i<5; i++) {
        if(eingabe[i] == '#' && i!=0) {  //löscht die letzte eingegebene Zahl
          eingabe[i-1] = '#';
          break;
        }
        if(i == 4){                 //wenn alles belegt ist, löscht es die letzte zahl
          eingabe[i] = '#';
          arrayvoll =false;
        }
      }
      
    } else {                       //wenn Zahl eingegeben wurde:
      if(arrayvoll){               //löscht das komplette array wenn es voll war
        cleararray();
      }
      
      for(int i = 0; i<5; i++) {    //setzt die zahl an nächste freie Stelle
        arrayvoll = true;
        if(eingabe[i] =='#'){
          eingabe[i] = key;
          if(i != 4){
            arrayvoll = false; // array ist noch nicht komplett gefüllt
          } else{
            arrayvoll = true;  //array ist nun komplett gefüllt
          }
          break;
        }
      }
    }
  }
  for(int i =0; i<5; i++) {
    lastCode[i] = eingabe[i];
  }
  delay(50);
  // Gucken ob der ESP will dass die Schrank aufgemacht wird
  if(digitalRead(comPin)){
    turauf =true;
  }
  if(turauf){
    opendoor();
  }
}

// Diese Methode öffnet die Schranke mit dem Servo
// und lässt sie so lange offen wie die globale
// Boolean dazu true ist
void opendoor(){
  servo1.write(50);
  servo2.write(120);
  if(millis() > myTimeout + myTimer && !checkSchranke){
    myTimer = millis();
    checkSchranke = true;
  }
  // US-Sensor ablesen
  if(checkSchranke){
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration*0.034/2;
    if(distance < 30){
      turzu = true;
      myTimer = millis();
    }
  }
  // Durchgehen detected und kurz gewartet 
  if(millis() > myTimeout + myTimer && turzu){
    checkSchranke = false;
    turauf = false;
    turzu = false;
    servo1.write(170);
    servo2.write(20);
  }
}

// Event wenn der ESP nach Code fragt
void requestEvent(){
  for (int i = 0; i<6; i++){
    Serial.println(lastCode[i]);
    Wire.write(lastCode[i]);
  }
  if (lastCode[5] == 'y') {
    cleararray();
    lastCode[5] = 'n';
  }
}

// Diese Funktion resettet das Eingaben-Array
void cleararray() {        //löscht alle eingegebenen Werte
  for(int i = 0; i<5; i++) {
    eingabe[i] = '#';
  }
}
