#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <SPI.h>


// ============= PIN DEFINITIONS =============
#define RST_PIN 9
#define SS_PIN 10

// ============= COMPONENTS =============
MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ============= AUTHORIZED CARD ============
byte authorizedCards[][4] = {{0x2B, 0x84, 0x19, 0x06}};

int authorizedCount = sizeof(authorizedCards) / sizeof(authorizedCards[0]);

// ============= SETUP =============
void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print(" RFID SYSTEM ");
  lcd.setCursor(0, 1);
  lcd.print("   READY   ");
  delay(2000);

  displayIdle();
}

// ============= LOOP =============
void loop() {
  if (!rfid.PICC_IsNewCardPresent())
    return;
  if (!rfid.PICC_ReadCardSerial())
    return;

  String uid = getUID();
  Serial.println(uid);

  if (isAuthorized(rfid.uid.uidByte)) {
    showAccessGranted();
  } else {
    showAccessDenied();
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(1000);
  displayIdle();
}

// ============= FUNCTIONS =============

String getUID() {
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10)
      uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1)
      uid += ":";
  }
  return uid;
}

bool isAuthorized(byte *cardUID) {
  for (int i = 0; i < authorizedCount; i++) {
    if (memcmp(cardUID, authorizedCards[i], 4) == 0) {
      return true;
    }
  }
  return false;
}

void showAccessGranted() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACCESS GRANTED");
  lcd.setCursor(0, 1);
  lcd.print(" Welcome 🙂 ");
}

void showAccessDenied() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACCESS DENIED");
  lcd.setCursor(0, 1);
  lcd.print(" Try Again ");
}

void displayIdle() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan your card");
  lcd.setCursor(0, 1);
  lcd.print(" Waiting... ");
}