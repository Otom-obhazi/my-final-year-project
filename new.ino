#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
SoftwareSerial mySerial(3, 2);

#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 5
String UID = "3D 5C FF 30";
byte lock = 0;
bool fingerprintVerified = false;
bool rfidVerified = false;

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
MFRC522 rfid(SS_PIN, RST_PIN);
#define RELAY_PIN       4

#define ACCESS_DELAY    5000 

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.home ();
  SPI.begin();
  rfid.PCD_Init();
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
  } else {
    while (1) { delay(1); }
  }
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  
}

void loop() {
  lcd.setCursor(4, 0);
  lcd.print("Welcome!");
  if (!fingerprintVerified) {
    lcd.setCursor(1, 1);
    lcd.print("Place Finger      ");
    if (getFingerPrint() != -1) {
      fingerprintVerified = true;
    }  
  } else if (!rfidVerified) {
    lcd.setCursor(1, 1);
    lcd.print("Place Card        ");
    if (getRFID() != "") {
      rfidVerified = true;
    }
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Granted");
    lcd.setCursor(0, 1);
    lcd.print(" Tobi Akande ");
    delay(2000);
    lcd.clear();
    digitalWrite(RELAY_PIN, LOW);
    lcd.setCursor(0, 0);
    lcd.print("Door is open");
    delay(2000);
    lcd.clear();
    delay(ACCESS_DELAY);
    digitalWrite(RELAY_PIN, HIGH);  
    fingerprintVerified = false;
    rfidVerified = false;
  }
}

// returns -1 if failed, otherwise returns ID #
int getFingerPrint() {
  int p = finger.getImage();
  if (p != FINGERPRINT_OK)  
    return -1;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  
    return -1;
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  
    return -1;
  return finger.fingerID;
}

String getRFID() {
  if (!rfid.PICC_IsNewCardPresent())
    return "";
  if (!rfid.PICC_ReadCardSerial())
    return "";
  String ID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    ID.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
    ID.concat(String(rfid.uid.uidByte[i], HEX));
    delay(300);
  }
  ID.toUpperCase();
  if (ID.substring(1) == UID) {
    return ID;
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wrong card!");
    delay(1500);
    lcd.clear();
    return "";
  }
}
