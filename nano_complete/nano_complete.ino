#include <Wire.h>
#include <Keypad.h>
#include <Servo.h>

Servo servo1; //servo
Servo servo2;

int trigPin = 10; //abstandssensor
int echoPin = 11;
long duration;
int distance;

char lastCode[6] = {'#', '#', '#', '#', '#', 'n'};

const byte ROWS =4;               //4x4-Keypad
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5}; 
byte colPins[COLS] = {6,7,8,9}; 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

char eingabe[5] = {'#', '#', '#', '#', '#'};  //array für Code eingabe (# = unbelegt)
boolean arrayvoll = false;                    //boolean für check, ob array voll ist

char band[] = {                                                     //turing
        '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
        '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'
        };
short position = 1;

int flag = 0;
boolean result;

char validation_char = 'n';

void setup() {
  // put your setup code here, to run once:
  Wire.begin(2);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);  //Abstandssensor
  pinMode(echoPin, INPUT);   //Abstandssensor
  servo1.attach(A2);
  servo2.attach(A3);
  servo1.write(0);
  servo2.write(140);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  char key = keypad.getKey();
  if (key){                                                //wenn Eingabe erfolgt ist:
    if (key =='A' || key =='B' || key =='C' || key=='D'){   //Unbelegte Keys
      Serial.println("falsche Eingabe");
      
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
}

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

void cleararray() {        //löscht alle eingegebenen Werte
  for(int i = 0; i<5; i++) {
    eingabe[i] = '#';
  }
}
