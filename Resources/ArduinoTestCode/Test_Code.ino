unsigned char Buffer[64];
int min1 = 1; 
int sec1 = 0;

int min2 = 0; 
int sec2 = 0;
bool countdownActive = false;   // NEW: To track whether countdown is running
unsigned long previousMillis = 0; 
const unsigned long interval = 1000;  // 1 second interval

void sendIconChangeCommand(uint8_t idHigh, uint8_t idLow, uint8_t iconId) {
  uint8_t command[] = {0x5A, 0xA5, 0x05, 0x82, idHigh, idLow, 0x00, iconId};
  Serial2.write(command, sizeof(command));
}

void sendPage(uint8_t page) {
  uint8_t command[] = {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, page};
  Serial2.write(command, sizeof(command));
}

void sendFormattedTimer(uint8_t idHigh, uint8_t idLow, int Min, int Sec) {
    char dateString[11];  
    snprintf(dateString, sizeof(dateString), "%02d:%02d", Min, Sec);
    sendTextToDWIN(dateString, idHigh, idLow);
}

void sendTextToDWIN(const char* text, uint8_t idHigh, uint8_t idLow) {
    uint8_t header[] = {0x5A, 0xA5, 0x13, 0x82};  
    uint8_t buffer[32];  
    memcpy(buffer, header, sizeof(header));

    buffer[4] = idHigh;  
    buffer[5] = idLow;   

    if (strcmp(text, "Clear@") == 0) {
        for (int i = 6; i < 22; i++) {  
            buffer[i] = 0x20;
        }
    } else {
        int textLen = strlen(text);
        for (int i = 0; i < textLen && i < 16; i++) {  
            buffer[6 + i] = (uint8_t)text[i];  
        }
        for (int i = textLen; i < 16; i++) {
            buffer[6 + i] = 0x20;
        }
    }
    
    Serial2.write(buffer, sizeof(header) + 2 + 16);  
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  
  sendPage(0);       // Switch to Starting Page
  delay(5000);
  sendPage(1);       // Switch to Second Page
  sendFormattedTimer(0x19, 0x00, min1, sec1);
  sendIconChangeCommand(0x19, 0x50, 3);
}

void loop() {
  handleDWINInput();
  handleCountdown();  // NEW: handle countdown timing
}

void handleDWINInput() {
  if (Serial2.available()) {  
        for (int i = 0; i < 32; i++) {  
            Buffer[i] = Serial2.read();         
        }
        if (Buffer[0] == 0x5A) {
          switch ((Buffer[4] << 8) | Buffer[5]) {
            case 0x1800:{
              switch (Buffer[8]) {
                case 1:
                  Serial.println("+ Button Pressed");
                  min1++;
                  if(min1 >= 60) min1 = 60;
                  sendFormattedTimer(0x19, 0x00, min1, sec1);
                break;
                case 2:
                  Serial.println("- Button Pressed");
                  min1--;
                  if(min1 <= 1) min1 = 1;
                  sendFormattedTimer(0x19, 0x00, min1, sec1);
                break; 
                case 3:
                  Serial.println("Start Button Pressed");
                  min2 = min1; 
                  sec2 = sec1;
                  countdownActive = true;    // Start countdown
                  sendIconChangeCommand(0x19, 0x50, 1);
                break; 
                case 4:
                  Serial.println("Stop Button Pressed");
                  countdownActive = false;   // Stop countdown
                  sendIconChangeCommand(0x19, 0x50, 3);
                  sendFormattedTimer(0x19, 0x00, min1, sec1);
                break;
              }
              break;
            }
            case 0x1810:{
              uint16_t receivedValue = (Buffer[7] << 8) | Buffer[8];
              Serial.print("Received Value: ");
              Serial.println(receivedValue); // For debug
              if (receivedValue == 1234) {  // 0x04D2
                sendPage(27);  // Correct page
              } else {
                sendPage(1);   // Go back to original page
              }
              break;
            }
            default:
            break;
          }
        }
  }
}

// NEW: countdown handler
void handleCountdown() {
  if (countdownActive) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      if (sec2 == 0) {
        if (min2 > 0) {
          min2--;
          sec2 = 59;
        } else {
          countdownActive = false;  // Time is over, stop countdown
          sendIconChangeCommand(0x19, 0x50, 3); // Optionally reset icon
          sendFormattedTimer(0x19, 0x00, min1, sec1);
          Serial.println("Timer completed!");
          return;
        }
      } else {
        sec2--;
      }

      sendFormattedTimer(0x19, 0x00, min2, sec2);  // Update GUI with new time
    }
  }
}
