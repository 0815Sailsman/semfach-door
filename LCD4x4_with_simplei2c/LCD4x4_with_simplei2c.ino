#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>

Servo servo;
int trigPin = 10;  //Abstandssensor
int echoPin = 11;  //Abstandssensor
long duration;     //Abstandssensor
int distance;      //Abstandssensor

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6,7,8,9}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

char eingabe[5] = {'#', '#', '#', '#', '#'};  //array für Code eingabe (# = unbelegt)
boolean arrayvoll = false;                    //boolean für check, ob array voll ist

long timer = 0;                               //für LCD-Refresh-rate
long timeout = 20; //refresh rate             //für LCD-Refresh-rate


char band[] = {
        '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
        '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'
        };
short position = 1;

int flag = 0;
boolean result;


void setup(){                                 //initialized LCD und seriellen monitor
  lcd.init();                    
  lcd.backlight();
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);  //Abstandssensor
  pinMode(echoPin, INPUT);   //Abstandssensor
  servo.attach(12);
  servo.write(0);  
}
  
void loop(){
 
  char key = keypad.getKey();           //eingegebener Key wird in variable key gespeichert
  if (key){                             
    Serial.println(key);               //wenn key eingegebn wurde, wird dieser in die Konsole geschrieben
  } 

  if (key){                                                //wenn Eingabe erfolgt ist:
    if (key =='A' || key =='B' || key =='C' || key=='D'){   //Unbelegte Keys
      Serial.println("falsche Eingabe");
      
    } else if(key =='#') {                   //wenn Eingabe key(#) gedrückt
      //Eingabe wird überprüft:
      
      if(arrayvoll) {          //wenn das array mit Zahlen voll ist, werden diese übermittelt (in Konsole geschrieben)
        bool is_valid_code = tm(eingabe);
        
        for(int i = 0; i<5; i++) {
          Serial.print(eingabe[i]);
        }
        Serial.println();
        lcd.clear();
        lcd.setCursor(0, 0);
        if (is_valid_code) {
          // Send code to uno
          
          TWCR = 0;
          Wire.begin();
          Wire.beginTransmission(9);
          for (short i=0; i < 5; i++) {
            Wire.write(eingabe[i]);
          }
          Wire.endTransmission();
          // Uno checks if everythingh is alright (person_counter, schin_drin...)
          // Returns true/false
          TWCR = 0;
          Wire.begin(2);
          Wire.onReceive(receiveEvent);
          while (flag == 0) {
            Serial.println(flag);
            continue;
          }
          Serial.println(result);
          if (result) {
            Serial.println("Correct");
          }
          else {
            Serial.println("Incorrect");
          }
          TWCR = 0;
          lcd.init();                    
          lcd.backlight();
          
          
          lcd.print("Code angenommen");  //Code angenommen erscheint für 2sec auf Bildschirm
          //opendoor();
        } else {
          lcd.print("Code abgelehnt");  //Code angenommen erscheint für 2sec auf Bildschirm
        }
        delay(2000);
        lcd.clear();
        
      } else {                     //wenn das array nicht gefüllt ist erscheint eine Fehlernachricht
        lcd.clear();
        lcd.setCursor(0, 0); 
        lcd.print("Code abgelehnt");  //Code ungueltig erscheint für 2sec auf Bildschirm
        delay(2000);
        lcd.clear();
      }
      cleararray();         //array wird gecleared
      
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
    
  //print auf LCD display:
  if(millis()> timer +timeout) {  //refreshed ~ alle 20ms
    
    lcd.setCursor(0, 0); // "Überschrift"
    lcd.print("Ihr Code:");
    lcd.setCursor(0, 1); // erste Stelle:
    lcd.print(eingabe[0]);
    lcd.setCursor(1, 1); // zweite Stelle:
    lcd.print(eingabe[1]);
    lcd.setCursor(2, 1); // dritte Stelle:
    lcd.print(eingabe[2]);
    lcd.setCursor(3, 1); // vierte Stelle:
    lcd.print(eingabe[3]);
    lcd.setCursor(4, 1); // fünfte Stelle:
    lcd.print(eingabe[4]);
  
    timer = millis(); //für refresh rate
  }
  
}

void opendoor() {
  servo.write(90);
  boolean passed = false;
  while(!passed) { 
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration*0.034/2;
    Serial.print("Distance: ") ;
    Serial.println(distance);
    if(distance < 30 && distance > 1) {
      passed = true;
      delay(3000);
    }
  }
  servo.write(0);
}


void receiveEvent(int howMany) {
  result = Wire.read();
  flag = 1;
  Serial.println(flag);  
}


void cleararray() {        //löscht alle eingegebenen Werte
  for(int i = 0; i<5; i++) {
    eingabe[i] = '#';
  }
}

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
