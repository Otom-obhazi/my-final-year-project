#include <Wire.h>
#include <Adafruit_Fingerprint.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
PN532_I2C pn532_i2c(Wire);
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows

#define LED_PIN 5 // relay is connected to pin 5
#define TIMEOUT_INTERVAL 20000 // 20000 millisecnds = 20 seconds

NfcAdapter nfc = NfcAdapter(pn532_i2c);

String tagId1 = "FA 5F 99 1A"; // RFID Tag id
String tagId2= "C6 45 22 4B"; // rfid tag id
String tagId = "None";
byte nuidPICC[4];

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)

SoftwareSerial mySerial(2, 3);
 
#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1
 
#endif
 
 
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
 
void setup()
{
  Serial.begin(9600);
  //Serial1.begin(9600);
  pinMode(LED_PIN, OUTPUT);
lcd.init(); // initialize the lcd
  lcd.backlight();
Serial.println("System initialized");

  nfc.begin();
  
 Serial.println("\n\nAdafruit finger detect test");
 
  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
// 
//  Serial.println(F("Reading sensor parameters"));
//  finger.getParameters();
//  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
//  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
//  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
//  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
//  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
//  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
//  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
// 
//  finger.getTemplateCount();
// 
//  if (finger.templateCount == 0) {
//    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
//  }
//  else {
//    Serial.println("Waiting for valid finger...");
//      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
//  }
}

void loop()
{
  lcd.clear();                 // clear display
  lcd.setCursor(0, 0);         // move cursor to   (0, 0)
  lcd.print("RFID CARD");
  delay(2000);     
    readNFC();

  
  if (tagId==tagId1)
  {
      lcd.clear();                 // clear display
  lcd.setCursor(0, 0);         // move cursor to   (0, 0)
  lcd.print("Place Finger");
  delay(2000); 
    Serial.println("RFID tag detected!");

    Serial.println("Place your finger on the sensor within 1 minute.");

    unsigned long startTime = millis();
    while (millis() - startTime < TIMEOUT_INTERVAL)
    {
      getFingerprintID();
  delay(50);            //don't ned to run this at full speed.
    }

    Serial.println("Timeout. Please try again.");
    tagId = "";
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
      finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_BLUE);
      finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
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
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    finger.LEDcontrol(FINGERPRINT_LED_FLASHING, 25, FINGERPRINT_LED_PURPLE, 10);
    delay(1000);
  if(finger.fingerID==2)
  {
       Serial.println("Fingerprint found!");
            digitalWrite(LED_PIN, HIGH);
            lcd.clear();                 // clear display
            lcd.setCursor(0, 0);         // move cursor to   (0, 0)
           lcd.print("LOCK Opened");
            delay(5000); // LED remains ON for 2 seconds
            digitalWrite(LED_PIN, LOW);
              lcd.clear();                 // clear display
            lcd.setCursor(0, 0);         // move cursor to   (0, 0)
            lcd.print("LOCK Closed");
  }
   
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    finger.LEDcontrol(FINGERPRINT_LED_FLASHING, 25, FINGERPRINT_LED_RED, 10);
  delay(1000);
  Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
 
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
 
  return finger.fingerID;
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
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}

void readNFC() {
  if (nfc.tagPresent())
  {
    NfcTag tag = nfc.read();
    tag.print();
    tagId = tag.getUidString();
    Serial.println("Tag id");
    Serial.println(tagId);
  }
  delay(1000);
}