#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <SPI.h>


// ============= PIN DEFINITIONS =============
#define RST_PIN 9
#define SS_PIN 10

// ============= COMPONENTS =============
MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ============= AUTHORIZED CARD STORAGE ============
// This will be populated by Qt via serial commands
byte authorizedCards[10][4];  // Support up to 10 cards
String authorizedCardNames[10]; // Names for each card
int authorizedCount = 0;
int currentCardIndex = -1; // Index of currently scanned card
bool rfidSystemReady = false; // Flag to indicate if RFID system is initialized

// ============= SETUP =============
void setup() {
  SPI.begin();
  rfid.PCD_Init();

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print(" RFID SYSTEM ");
  lcd.setCursor(0, 1);
  lcd.print("  LOADING...");
  delay(2000);

  displayIdle();
}

void systemReady() {
  rfidSystemReady = true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  delay(1000);
  displayIdle();
}

// ============= LOOP =============
void loop() {
  // Check for serial commands from Qt
  if (Serial.available()) {
    handleSerialCommand();
  }

  // Don't read RFID until system is ready
  if (!rfidSystemReady) {
    return;
  }

  // Check for RFID card
  if (!rfid.PICC_IsNewCardPresent())
    return;
  if (!rfid.PICC_ReadCardSerial())
    return;

  String uid = getUID();
  Serial.println(uid);  // Send UID to Qt

  // Check if card is authorized (in our list)
  int cardIndex = findCardIndex(rfid.uid.uidByte);
  if (cardIndex >= 0) {
    currentCardIndex = cardIndex;
    showAccessGranted();
  } else {
    showAccessDenied();
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(1000);
  displayIdle();
  currentCardIndex = -1;
}

// ============= FUNCTIONS =============

String getUID() {
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    String hexByte = String(rfid.uid.uidByte[i], HEX);
    if (rfid.uid.uidByte[i] < 0x10)
      hexByte = "0" + hexByte;
    hexByte.toLowerCase();
    uid += hexByte;
    if (i < rfid.uid.size - 1)
      uid += ":";
  }
  return uid;
}

// Convert UID string "2B:84:19:06" back to byte array for comparison
bool parseUID(String uidStr, byte* uidBytes) {
  int index = 0;
  int start = 0;
  for (int i = 0; i < 4; i++) {
    int colon = uidStr.indexOf(':', start);
    if (colon == -1 && i < 3) return false;
    String hexByte = (colon == -1) ? uidStr.substring(start) : uidStr.substring(start, colon);
    uidBytes[i] = strtoul(hexByte.c_str(), NULL, 16);
    start = colon + 1;
  }
  return true;
}

int findCardIndex(byte* cardUID) {
  for (int i = 0; i < authorizedCount; i++) {
    if (memcmp(cardUID, authorizedCards[i], 4) == 0) {
      return i;
    }
  }
  return -1;
}

void showAccessGranted() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACCESS GRANTED");
  lcd.setCursor(0, 1);
  lcd.print("Welcome :)");

  // If Qt provided a name, it will update via serial command
}

void showAccessDenied() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACCESS DENIED");
  lcd.setCursor(0, 1);
  lcd.print("Try Again ");
}

void displayIdle() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan your card");
  lcd.setCursor(0, 1);
  lcd.print(" Waiting... ");
}

// Handle commands from Qt over serial
void handleSerialCommand() {
  String command = Serial.readStringUntil('\n');
  command.trim();

  // Command: ADD_CARD:UID:NAME
  // Example: ADD_CARD:2B:84:19:06:Jean Dupont
  if (command.startsWith("ADD_CARD:")) {
    if (authorizedCount < 10) {
      if (command.length() >= 21) {
        String uidStr = command.substring(9, 20);
        String name = command.substring(21);
        
        byte uidBytes[4];
      if (parseUID(uidStr, uidBytes)) {
        // Copy to authorized cards
        for (int i = 0; i < 4; i++) {
          authorizedCards[authorizedCount][i] = uidBytes[i];
        }
        authorizedCardNames[authorizedCount] = name;
        authorizedCount++;
        Serial.println("CARD_ADDED");
      }
      }
    }
  }
  // Command: CLEAR_CARDS
  else if (command == "CLEAR_CARDS") {
    authorizedCount = 0;
    Serial.println("CARDS_CLEARED");
  }
  // Command: LCD:LINE1:LINE2
  // Example: LCD:Welcome:Jean Dupont
  else if (command.startsWith("LCD:")) {
    int colon1 = command.indexOf(':', 4);
    String line1 = command.substring(4, colon1);
    String line2 = command.substring(colon1 + 1);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1.substring(0, 16));  // Truncate to 16 chars
    lcd.setCursor(0, 1);
    lcd.print(line2.substring(0, 16));
  }
  // Command: WELCOME:name:mission
  // Example: WELCOME:Jean Dupont:Collecte Zone A
  else if (command.startsWith("WELCOME:")) {
    int colon1 = command.indexOf(':', 8);
    String name = command.substring(8, colon1);
    String mission = command.substring(colon1 + 1);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Welcome");
    lcd.setCursor(0, 1);
    lcd.print(name.substring(0, 16));
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(name.substring(0, 16));
    lcd.setCursor(0, 1);
    lcd.print(mission.substring(0, 16));
  }
  // Command: READY - signal that Qt has finished loading cards
  else if (command == "READY") {
    systemReady();
  }
}
