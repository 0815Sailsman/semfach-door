#include <Wire.h>
void setup() {
  Serial.begin(115200);
    Wire.begin();
    // Open i2c connection to uno
    // Send them data
    String inputvName = "Daniel";
    String inputnName = "Gluch";
    String inputEmail = "dagluch33@gmail.com";

    String word_done = "\v";
    String done = "\?";
    
    /*char charsname[inputvName.length()];
    inputvName.toCharArray(charsname, inputvName.length());
    char charsname2[inputnName.length()];
    inputnName.toCharArray(charsname2, inputnName.length());
    char charsmail[inputEmail.length()];
    inputEmail.toCharArray(charsmail, inputEmail.length());*/
    
    for (short i = 1; i <= inputvName.length(); i++) {
      Wire.beginTransmission(9);
      Wire.write(inputvName.substring(i-1, i).c_str());
      Wire.endTransmission();
      delay(100);
    }
    Wire.write(word_done.c_str());
    
    for (short i = 0; i < inputnName.length(); i++) {
      Wire.beginTransmission(9);
      Wire.write(inputnName.substring(i-1, i).c_str());
      Wire.endTransmission();
      delay(100);
    }
    Wire.write(word_done.c_str());
    
    for (short i = 0; i < inputEmail.length(); i++) {
      Wire.beginTransmission(9);
      Wire.write(inputEmail.substring(i-1, i).c_str());
      Wire.endTransmission();
      delay(100);
    }
    Wire.write(done.c_str());
    Serial.println("sent");
}

void loop() {
  // put your main code here, to run repeatedly:

}
