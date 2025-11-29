#include <SoftwareSerial.h>

// SoftwareSerial on pins 10 (RX) and 11 (TX)
SoftwareSerial mySerial(10, 11); // RX not used, TX to other Arduino

// Input pins (1-based mapping)
const int pinCount = 12;
const int inputPins[pinCount] = {
  2, 3, 4, 5, 6, 7, 8, 9, A0, A1, A2, A3
};

// Store previous states
bool lastState[pinCount] = {false};

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  for (int i = 0; i < pinCount; i++) {
    pinMode(inputPins[i], INPUT); // Use external pull-down resistors
  }
}

void loop() {
  for (int i = 0; i < pinCount; i++) {
    bool currentState = digitalRead(inputPins[i]);

    if (currentState != lastState[i]) {
      sendCommandForPin(i + 1); // Use 1-based index for case mapping
      lastState[i] = currentState;
    }
  }

  delay(50); // Debounce delay
}

void sendCommandForPin(int pinNum) {
  String command;

  switch (pinNum) {
    case 1:  command = ";W1;clsreload";     // A1: Left, Right
             break;
    case 2:  command = ";W1;clscollet";     // A2: Left, Right, Right
             break;
    case 3:  command = ";W1;clsready";      // A3: Left, Right, Right, Right
             break;
    case 4:  /* Available for future use */  // B1: Left, Left, Right
             return;
    case 5:  command = ";W1;cfrontopen";    // B2: Left, Left, Right, Right
             break;
    case 6:  command = ";W1;cfrontclosed";  // B3: Left, Left, Right, Right, Right
             break;
    case 7:  /* Available for future use */  // C1: Left, Left, Left, Right
             return;
    case 8:  /* Available for future use */  // C2: Left, Left, Left, Right, Right
             return;
    case 9:  /* Available for future use */  // C3: Left, Left, Left, Right, Right, Right
             return;
    case 10: /* Available for future use */  // D1: Left, Left, Left, Left, Right
             return;
    case 11: /* Available for future use */  // D2: Left, Left, Left, Left, Right, Right
             return;
    case 12: /* Available for future use */  // D3: Left, Left, Left, Left, Right, Right, Right
             return;
    default: return;
  }

  mySerial.println(command);
  Serial.print("Sent: ");
  Serial.println(command);
}
