#include <Adafruit_Fingerprint.h>
#include <MFRC522.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>

int getFingerprintIDez();
SoftwareSerial mySerial(2, 3);
MFRC522 rfid(10, 9);
LiquidCrystal_I2C lcd(0x27,16,2);

int state_rfid = 0;
int state_finger = 0;
int relay = 4;
int led_merah = 6;
int led_hijau = 5;
int buzzer = 7;

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup(){
  Serial.begin(9600);
  SPI.begin();     
  rfid.PCD_Init();
  lcd.init();
  lcd.backlight();
  pinMode(relay, OUTPUT);
  pinMode(led_merah, OUTPUT);
  pinMode(led_hijau, OUTPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(relay, LOW);
  finger.begin(57600);
//  if (finger.verifyPassword()) {
//    Serial.println("Found fingerprint sensor!");
//  } else {
//    Serial.println("Did not find fingerprint sensor :(");
//    while (1);
//  }
//  Serial.println("Waiting for valid finger…");
}

void loop(){
  lcd.setCursor(0,0);
  lcd.print(" SELAMAT DATANG ");
  lcd.setCursor(0,1);
  lcd.print("scan e-ktp anda ");
  if(state_rfid == 0 || state_finger == 0){ 
    if(!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()){
      return;
    }
    if(scanRFID() == false){ 
       state_rfid = 0;
       state_finger = 0;
       Serial.println("KARTU RFID TIDAK VALID");
       lcd.setCursor(0,1);
       lcd.print("e-ktp tidakvalid");
       digitalWrite(led_merah, HIGH);
       digitalWrite(buzzer, HIGH);
       delay(5000);  
       digitalWrite(led_merah, LOW);
       digitalWrite(buzzer, LOW);
       return; 
     }
     Serial.println("KARTU RFID VALID");
     lcd.setCursor(0,1);
     lcd.print("e-ktp anda valid");
     digitalWrite(led_hijau, HIGH);
     digitalWrite(buzzer, HIGH);
     delay(2000);  
     digitalWrite(led_hijau, LOW);
     digitalWrite(buzzer, LOW);
     state_rfid = 1;
     state_finger = 1;
  }

  if(state_rfid == 1 || state_finger == 1){
    lcd.setCursor(0,1);
    lcd.print("scan finger anda");
    getFingerprintIDez();
  }       
  
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
  case FINGERPRINT_OK:
  Serial.println("Image taken");
  break;
  case FINGERPRINT_NOFINGER:
  Serial.println("No finger detected");
  return p;
  case FINGERPRINT_PACKETRECIEVEERR:
  Serial.println("Communication error");
  return p;
  case FINGERPRINT_IMAGEFAIL:
  Serial.println("Imaging error");
  return p;
  default:
  Serial.println("Unknown error");
  return p;
}

// OK success!
p = finger.image2Tz();
switch (p) {
  case FINGERPRINT_OK:
  Serial.println("Image converted");
  break;
  case FINGERPRINT_IMAGEMESS:
  Serial.println("Image too messy");
  return p;
  case FINGERPRINT_PACKETRECIEVEERR:
  Serial.println("Communication error");
  return p;
  case FINGERPRINT_FEATUREFAIL:
  Serial.println("Could not find fingerprint features");
  return p;
  case FINGERPRINT_INVALIDIMAGE:
  Serial.println("Could not find fingerprint features");
  return p;
  default:
  Serial.println("Unknown error");
  return p;
}

// OK converted!
p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } 
  else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } 
  else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } 
  else {
    Serial.println("Unknown error");
    return p;
   }
   
// found a match!
Serial.print("Found ID #"); Serial.print(finger.fingerID);
Serial.print(" with confidence of "); Serial.println(finger.confidence);
}
// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print("with confidence of "); Serial.println(finger.confidence);
  lcd.setCursor(0,0);
  lcd.print("AKSESS DITERIMAA");
  lcd.setCursor(0,1);
  lcd.print("silahkan masuk:)");
  digitalWrite(relay, HIGH);
  digitalWrite(led_hijau, HIGH);
  digitalWrite(buzzer, HIGH);   
  delay(5000);
  digitalWrite(relay, LOW);
  digitalWrite(led_hijau, LOW);
  digitalWrite(buzzer, LOW);
  state_rfid = 0;
  state_finger = 0;
  return finger.fingerID;
}

bool scanRFID(){
    String id;
    id = String(rfid.uid.uidByte[0]) + String(rfid.uid.uidByte[1]) + String(rfid.uid.uidByte[2]) + String(rfid.uid.uidByte[3]);
    Serial.print("ID Anda = ");
    Serial.println(id);
    if(id != "514110282"){ // denied
        return false;
    }
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    // granted
    return true;
}
