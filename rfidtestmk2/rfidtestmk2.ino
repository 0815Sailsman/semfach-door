#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

boolean currently_open = false;
int outPin = 1;

//*****************************************************************************************//
void setup() {
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card
  pinMode(outPin, OUTPUT);
}

//*****************************************************************************************//
void loop() {
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  MFRC522::StatusCode status;

  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("Karte erkannt!"));
  // Get uid
  if (mfrc522.uid.uidByte[0] == 112 &&
      mfrc522.uid.uidByte[1] == 108 &&
      mfrc522.uid.uidByte[2] == 23  &&
      mfrc522.uid.uidByte[3] == 48) {
    Serial.println("AAAAAHH DIE IST JA RICHTIG!!!");
    toggleDoor();      
  }
  else if (mfrc522.uid.uidByte[0] == 73 &&
          mfrc522.uid.uidByte[1] == 208 &&
          mfrc522.uid.uidByte[2] == 179  &&
          mfrc522.uid.uidByte[3] == 85) {
    Serial.println("AAAAAHH DIE IST JA RICHTIG!!!");
    toggleDoor();         
  }
  /*else if (mfrc522.uid.uidByte[0] == 183 &&
          mfrc522.uid.uidByte[1] == 131 &&
          mfrc522.uid.uidByte[2] == 71  &&
          mfrc522.uid.uidByte[3] == 178) {
    Serial.println("AAAAAHH DIE IST JA RICHTIG!!!");        
  }*/

  delay(1000); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
//*****************************************************************************************//

void toggleDoor() {
  if (currently_open) {
    digitalWrite(outPin, LOW);
    currently_open = false;
  }
  else {
    digitalWrite(outPin, HIGH);
    currently_open = true;
  }
}
