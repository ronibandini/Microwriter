// Microwriter 1.0
// Roni Bandini, November 2025
// MIT License
// ESP32P4 Dev Module + DFRobot LCD 1602 v2 + 6 buttons
// Ide Settings: CDC on boot, Flash 16 128, Partition 16 3, PSRAM enabled, USB Mode Hardware CDC and Jtag

#include "DFRobot_RGBLCD1602.h"
#include <WiFi.h>
#include <WebServer.h>
#include <SD_MMC.h>

char dtaUart[15];
char dtaLen = 0;

int led = 3;

int pinThumb = 31;
int pinIndex = 34;
int pinMiddle = 20;
int pinRing = 36;
int pinPinky = 35;
int pinDelete = 21;

String lastCombination = "";
String myText = "";
int displayOffset = 0;
unsigned long lastActivityTime = 0;
bool apStarted = false;
bool sdCardAvailable = false;

// Access Point credentials
const char* ap_ssid = "Microwriter";
const char* ap_password = "12345678";
const int bounceDelay=300;
const int demoMode=0;

unsigned long lastDeleteTime = 0;
const unsigned long doubleClickTime = 400;
int deletePressCount = 0;

WebServer server(80);

DFRobot_RGBLCD1602 lcd(/*RGBAddr*/0x2D ,/*lcdCols*/16,/*lcdRows*/2); 

char getCharFromCombination(String combo) {
  if (combo == "01100") return 'A';
  if (combo == "00111") return 'B';
  if (combo == "10100") return 'C';
  if (combo == "11100") return 'D';
  if (combo == "01000") return 'E';
  if (combo == "11110") return 'F';
  if (combo == "00011") return 'G';
  if (combo == "10001") return 'H';
  if (combo == "11000") return 'I';
  if (combo == "10011") return 'J';
  if (combo == "10010") return 'K';
  if (combo == "11001") return 'L';
  if (combo == "01111") return 'M';
  if (combo == "00110") return 'N';
  if (combo == "00100") return 'O';
  if (combo == "11111") return 'P';
  if (combo == "00101") return 'Q'; 
  if (combo == "11010") return 'R';
  if (combo == "00010") return 'S';
  if (combo == "01010") return 'T';
  if (combo == "00001") return 'U';
  if (combo == "01001") return 'V';
  if (combo == "11011") return 'W';
  if (combo == "10111") return 'X';
  if (combo == "10110") return 'Y';
  if (combo == "10101") return 'Z';
  if (combo == "10000") return ' ';
  if (combo == "01110") return '.';
  if (combo == "01101") return '-';
  if (combo == "01011") return ',';
  if (combo == "11101") return '\'';
  
  return '?';
}

void updateDisplay() {
  lcd.clear();
  
  int textLen = myText.length();
  
  if (textLen <= 32) {
    lcd.setCursor(0, 0);
    lcd.print(myText.substring(0, min(16, textLen)));
    
    if (textLen > 16) {
      lcd.setCursor(0, 1);
      lcd.print(myText.substring(16, textLen));
    }
    
    displayOffset = 0;
  } else {
    displayOffset = textLen - 32;
    
    lcd.setCursor(0, 0);
    lcd.print(myText.substring(displayOffset, displayOffset + 16));
    
    lcd.setCursor(0, 1);
    lcd.print(myText.substring(displayOffset + 16, textLen));
  }
  
  int cursorCol = (textLen - displayOffset) % 16;
  int cursorRow = ((textLen - displayOffset) >= 16) ? 1 : 0;
  lcd.setCursor(cursorCol, cursorRow);
}

void handleSave() {
  String response = "";
  bool success = false;
  
  if (!sdCardAvailable) {
    response = "{\"success\":false,\"message\":\"SD card not available\"}";
  } else if (myText.length() == 0) {
    response = "{\"success\":false,\"message\":\"No text to save\"}";
  } else {
    // Generate text filename with timestamp
    String filename = "/microwriter_" + String(millis()) + ".txt";
    
    File file = SD_MMC.open(filename, FILE_WRITE);
    if (file) {
      file.println(myText);
      file.close();
      success = true;
      response = "{\"success\":true,\"message\":\"Text saved to " + filename + "\"}";
      
      Serial.println("Text saved to SD card: " + filename);
      
      lcd.clear();
      lcd.print("Saved to SD!");
      lcd.setCursor(0, 1);
      lcd.print(filename.substring(0, 16));
      delay(2000);
      updateDisplay();
    } else {
      response = "{\"success\":false,\"message\":\"Failed to open file\"}";
      Serial.println("Failed to open file on SD card");
    }
  }
  
  server.send(200, "application/json", response);
}

void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Microwriter</title>";
  html += "<style>";
  html += "* { margin: 0; padding: 0; box-sizing: border-box; }";
  html += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; ";
  html += "background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); ";
  html += "min-height: 100vh; padding: 20px; }";
  html += ".container { max-width: 900px; margin: 0 auto; }";
  html += ".header { text-align: center; color: white; margin-bottom: 30px; }";
  html += ".header h1 { font-size: 2.5em; margin-bottom: 10px; text-shadow: 2px 2px 4px rgba(0,0,0,0.3); }";
  html += ".header p { font-size: 1.1em; opacity: 0.9; }";
  html += ".card { background: white; border-radius: 15px; padding: 30px; ";
  html += "box-shadow: 0 10px 30px rgba(0,0,0,0.3); margin-bottom: 20px; }";
  html += ".text-display { background: #f8f9fa; border: 2px solid #e9ecef; ";
  html += "border-radius: 10px; padding: 20px; min-height: 150px; ";
  html += "font-size: 18px; line-height: 1.8; word-wrap: break-word; ";
  html += "font-family: 'Courier New', monospace; color: #333; }";
  html += ".text-display.empty { color: #adb5bd; font-style: italic; text-align: center; ";
  html += "display: flex; align-items: center; justify-content: center; }";
  html += ".stats { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); ";
  html += "gap: 15px; margin-top: 20px; }";
  html += ".stat-box { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); ";
  html += "color: white; padding: 15px; border-radius: 10px; text-align: center; }";
  html += ".stat-box .number { font-size: 2em; font-weight: bold; margin-bottom: 5px; }";
  html += ".stat-box .label { font-size: 0.9em; opacity: 0.9; }";
  html += ".actions { display: flex; gap: 15px; margin-top: 20px; flex-wrap: wrap; }";
  html += ".btn { flex: 1; min-width: 150px; padding: 15px 30px; border: none; ";
  html += "border-radius: 10px; font-size: 16px; font-weight: bold; cursor: pointer; ";
  html += "transition: all 0.3s ease; text-transform: uppercase; letter-spacing: 1px; }";
  html += ".btn:hover { transform: translateY(-2px); box-shadow: 0 5px 15px rgba(0,0,0,0.3); }";
  html += ".btn:active { transform: translateY(0); }";
  html += ".btn-primary { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; }";
  html += ".btn-success { background: linear-gradient(135deg, #56ab2f 0%, #a8e063 100%); color: white; }";
  html += ".btn-danger { background: linear-gradient(135deg, #eb3349 0%, #f45c43 100%); color: white; }";
  html += ".btn:disabled { opacity: 0.5; cursor: not-allowed; }";
  html += ".btn:disabled:hover { transform: none; box-shadow: none; }";
  html += ".alert { padding: 15px 20px; border-radius: 10px; margin-top: 20px; ";
  html += "font-weight: 500; display: none; animation: slideIn 0.3s ease; }";
  html += ".alert-success { background: #d4edda; color: #155724; border: 2px solid #c3e6cb; }";
  html += ".alert-error { background: #f8d7da; color: #721c24; border: 2px solid #f5c6cb; }";
  html += "@keyframes slideIn { from { opacity: 0; transform: translateY(-10px); } ";
  html += "to { opacity: 1; transform: translateY(0); } }";
  html += ".footer { text-align: center; color: white; margin-top: 30px; opacity: 0.8; }";
  html += ".status-badge { display: inline-block; padding: 5px 15px; border-radius: 20px; ";
  html += "font-size: 0.85em; font-weight: bold; margin-top: 10px; }";
  html += ".status-ok { background: #d4edda; color: #155724; }";
  html += ".status-error { background: #f8d7da; color: #721c24; }";
  html += "</style>";
  html += "</head><body>";
  html += "<div class='container'>";
  html += "<div class='header'>";
  html += "<h1>📝 Microwriter</h1>";
  html += "<p>Chord Keyboard Text Interface</p>";
  html += "</div>";
  
  html += "<div class='card'>";
  html += "<h2 style='margin-bottom: 20px; color: #333;'>Current Text</h2>";
  html += "<div class='text-display";
  if (myText.length() == 0) html += " empty";
  html += "'>";
  
  if (demoMode==1){
    myText="I’m one of those who believe that some forgotten, and sometimes deliberately quieted,  technologies trace the outline of an alternative society, one more willing to climb learning curves and to challenge the role of today’s average user";
  }
  if (myText.length() > 0) {
    html += myText;
  } else {
    html += "No text typed yet... Start typing with your Microwriter!";
  }
  
  html += "</div>";
  
  html += "<div class='stats'>";
  html += "<div class='stat-box'>";
  html += "<div class='number'>" + String(myText.length()) + "</div>";
  html += "<div class='label'>Characters</div>";
  html += "</div>";
  html += "<div class='stat-box'>";
  int wordCount = 0;
  if (myText.length() > 0) {
    wordCount = 1;
    for (int i = 0; i < myText.length(); i++) {
      if (myText[i] == ' ') wordCount++;
    }
  }
  html += "<div class='number'>" + String(wordCount) + "</div>";
  html += "<div class='label'>Words</div>";
  html += "</div>";
  html += "<div class='stat-box'>";
  html += "<div class='number'>";
  html += sdCardAvailable ? "✓" : "✗";
  html += "</div>";
  html += "<div class='label'>SD Card</div>";
  html += "</div>";
  html += "</div>";
  
  html += "<div class='actions'>";
  html += "<button class='btn btn-primary' onclick='location.reload()'>🔄 Refresh</button>";
  html += "<button class='btn btn-success' onclick='saveText()' ";
  if (!sdCardAvailable || myText.length() == 0) html += "disabled";
  html += ">💾 Save to SD</button>";
  html += "</div>";
  
  html += "<div id='alert' class='alert'></div>";
  html += "</div>";
  
  html += "<div class='footer'>";
  html += "<p>Roni Bandini © November 2025 | MIT License</p>";
  html += "<span class='status-badge ";
  html += sdCardAvailable ? "status-ok" : "status-error";
  html += "'>";
  html += sdCardAvailable ? "SD Card Ready" : "SD Card Not Available";
  html += "</span>";
  html += "</div>";
  html += "</div>";
  
  html += "<script>";
  html += "function saveText() {";
  html += "  fetch('/save', { method: 'POST' })";
  html += "    .then(response => response.json())";
  html += "    .then(data => {";
  html += "      const alert = document.getElementById('alert');";
  html += "      alert.style.display = 'block';";
  html += "      if (data.success) {";
  html += "        alert.className = 'alert alert-success';";
  html += "        alert.textContent = '✓ ' + data.message;";
  html += "      } else {";
  html += "        alert.className = 'alert alert-error';";
  html += "        alert.textContent = '✗ ' + data.message;";
  html += "      }";
  html += "      setTimeout(() => { alert.style.display = 'none'; }, 5000);";
  html += "    })";
  html += "    .catch(error => {";
  html += "      const alert = document.getElementById('alert');";
  html += "      alert.style.display = 'block';";
  html += "      alert.className = 'alert alert-error';";
  html += "      alert.textContent = '✗ Error: ' + error.message;";
  html += "    });";
  html += "}";
  html += "</script>";
  
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void startAccessPoint() {
  if (!apStarted) {
    Serial.println("Starting Access Point...");
    lcd.clear();
    lcd.print("Starting AP...");
    
    WiFi.softAP(ap_ssid, ap_password);
    IPAddress IP = WiFi.softAPIP();
    
    Serial.print("AP IP address: ");
    Serial.println(IP);
    
    server.on("/", handleRoot);
    server.on("/save", HTTP_POST, handleSave);
    server.begin();
    
    lcd.clear();
    lcd.print("AP: Microwriter");
    lcd.setCursor(0, 1);
    lcd.print(IP.toString());
    
    apStarted = true;
    delay(3000);
    updateDisplay();
  }
}

void setup() {
  pinMode(led,OUTPUT);

  pinMode(pinThumb, INPUT_PULLUP);
  pinMode(pinIndex, INPUT_PULLUP);
  pinMode(pinMiddle, INPUT_PULLUP);
  pinMode(pinRing, INPUT_PULLUP);
  pinMode(pinPinky, INPUT_PULLUP);
  pinMode(pinDelete, INPUT_PULLUP);

  lcd.init(); 
  lcd.setRGB(255, 0, 0); 

  Serial.begin(115200);
  delay(2000);
  Serial.println("Microwriter Started");

  lcd.print("Microwriter 1.0");
  lcd.setCursor(0, 1);
  lcd.print("Roni Bandini");
  delay(3000);
  
  // Initialize SD Card using SDIO (SD_MMC) - FireBeetle 2 ESP32-P4 has on-board TF card slot
  lcd.clear();
  lcd.print("Init SD Card...");
  Serial.println("Initializing SD card via SDIO...");
  
  // Use 1-bit mode for SD_MMC (more reliable)
  // For 4-bit mode, use: SD_MMC.begin("/sdcard", true)
  if (SD_MMC.begin("/sdcard", true)) {
    sdCardAvailable = true;
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.println("SD card initialized successfully");
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    lcd.setCursor(0, 1);
    lcd.print("SD: OK");
  } else {
    sdCardAvailable = false;
    Serial.println("SD card initialization failed");
    Serial.println("Make sure a TF card is inserted in the slot");
    lcd.setCursor(0, 1);
    lcd.print("SD: Failed");
  }
  delay(2000);
  
  lcd.clear();
  lcd.cursor();
  
  lastActivityTime = millis();
}

void loop() {
  if (millis() - lastActivityTime > 20000 && !apStarted) {
    startAccessPoint();
  }
  
  if (apStarted) {
    server.handleClient();
  }
  
  if (digitalRead(pinDelete) == LOW) {
    if (lastCombination != "DELETE") {
      lastCombination = "DELETE";
      
      unsigned long currentTime = millis();
      
      if (currentTime - lastDeleteTime < doubleClickTime) {
        deletePressCount++;
      } else {
        deletePressCount = 1;
      }
      lastDeleteTime = currentTime;
      
      lastActivityTime = currentTime;
      
      delay(bounceDelay); 
    }
  } else {
    if (deletePressCount > 0 && millis() - lastDeleteTime > doubleClickTime) {
      int charsToDelete = deletePressCount;
      
      Serial.print("Detected ");
      Serial.print(charsToDelete);
      Serial.println(" quick delete presses. Deleting...");
      
      for (int i = 0; i < charsToDelete; i++) {
        if (myText.length() > 0) {
          myText.remove(myText.length() - 1);
        }
      }

      updateDisplay();
      digitalWrite(led, HIGH);
      delay(100);
      digitalWrite(led, LOW);
      
      deletePressCount = 0;
      lastCombination = "00000";
    }

    int thumb = digitalRead(pinThumb);
    int index = digitalRead(pinIndex);
    int middle = digitalRead(pinMiddle);
    int ring = digitalRead(pinRing);
    int pinky = digitalRead(pinPinky);
    
    String combination = "";
    combination += (thumb == LOW) ? "1" : "0"; 
    combination += (index == LOW) ? "1" : "0"; 
    combination += (middle == LOW) ? "1" : "0";
    combination += (ring == LOW) ? "1" : "0";
    combination += (pinky == LOW) ? "1" : "0";
    
    if (combination != lastCombination && combination != "00000") {
      deletePressCount = 0;
      
      lastCombination = combination;
      
      digitalWrite(led, HIGH);
      
      char outputChar = getCharFromCombination(combination);
      
      myText += outputChar;
      
      Serial.print("Combination: ");
      Serial.print(combination);
      Serial.print(" -> ");
      Serial.println(outputChar);
      Serial.print("Full text: ");
      Serial.println(myText);
      
      updateDisplay();
      
      lastActivityTime = millis();
      delay(200);
    }
    
    if (combination == "00000") {
      digitalWrite(led, LOW);
      if (deletePressCount == 0 && lastCombination != "00000" && lastCombination != "DELETE") {
        lastCombination = "00000";
      }
    }
  }
  
  delay(50);
}