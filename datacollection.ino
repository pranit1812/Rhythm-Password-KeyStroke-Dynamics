#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int pinLength = 5; // Length of each pin
const int maxPins = 5; // Maximum number of different pins
char enteredPins[maxPins][pinLength + 1]; // Store up to maxPins pins
unsigned long keyTimes[maxPins][pinLength]; // Time intervals between keypresses
int currentPin = 0;
int keyCount = 0;
unsigned long lastKeyPressTime = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    unsigned long currentTime = millis();
    if (keyCount > 0) { // Only record time if it's not the first keypress
      unsigned long interval = currentTime - lastKeyPressTime;
      keyTimes[currentPin][keyCount - 1] = interval;
    }
    lastKeyPressTime = currentTime;

    enteredPins[currentPin][keyCount] = key;
    keyCount++;

    if (keyCount == pinLength) {
      enteredPins[currentPin][pinLength] = '\0'; // Null-terminate the string
      printPinData(currentPin);
      keyCount = 0;
      currentPin = (currentPin + 1) % maxPins;
    }
  }
}

void printPinData(int pinIndex) {
  
  Serial.print(enteredPins[pinIndex]);
  Serial.print(", ");
  for (int i = 0; i < pinLength - 1; i++) {
    Serial.print(keyTimes[pinIndex][i]);
    if (i < pinLength - 2) {
      Serial.print(", ");
    }
  }
  Serial.println();
}
