#include <Wire.h>
void setup() {
  Serial.begin(115200);
    Wire.begin();
    String inputvName = "Daniel";
    String inputnName = "Gluch";
    String inputEmail = "dagluch33@gmail.com";

    short chartime = 20;
    
    transmit_word(inputvName, chartime);
    transmit_word_done();
    transmit_word(inputnName, chartime);
    transmit_word_done();
    transmit_word(inputEmail, chartime);
    transmit_done();
    Serial.println("sent");
}

void loop() {

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
