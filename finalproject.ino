#include <amo_inferencing.h>
#include <Arduino_APDS9960.h>
#include <Keypad.h>
#include <string.h>

struct User {
  String username;
  String password;
};

User users[10]; // Array to store up to 10 users
int userCount = 0;

const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const String validPins[] = {"13113", "AA2AA", "98989", "111DD", "AAABC"};
const double thresholds[] = {0.48219526, 0.39156553, 0.47024566, 0.3671266, 0.3846821};
const int numPins = 5;
unsigned long lastKeypressTime = 0;
unsigned long intervals[4];
String enteredPin = "";
bool pinVerified = false;
int intervalCount = 0;

bool inRhythmLock = false;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  if (!APDS.begin()) {
    Serial.println("Error initializing APDS-9960 sensor!");
  }

  Serial.println("Welcome to Lock System");
  showMainMenu();
}

void loop() {
  if (Serial.available()) {
    String option = Serial.readStringUntil('\n');
    
    if (option == "1") {
      signUp();
    } else if (option == "2") {
      logIn();
    } else if (option == "3") {
      inRhythmLock = true;
      Serial.println("Enter PIN (1-5) to verify rhythm:");
    } else {
      Serial.println("Invalid option. Please try again.");
      showMainMenu();
    }
  }
  
  if (inRhythmLock) {
    char key = keypad.getKey();
    if (key) {
      unsigned long currentTime = millis();
      if (enteredPin.length() > 0 && enteredPin.length() < 5) {
        intervals[intervalCount++] = currentTime - lastKeypressTime;
      }
      lastKeypressTime = currentTime;
      enteredPin += key;
      if (enteredPin.length() == 5) {
        verifyPin();
        if (pinVerified) {
          runInference();
        }
        resetVariables();
        inRhythmLock = false;
        showMainMenu();
      }
    }
  }
}

void signUp() {
  if (userCount >= 10) {
    Serial.println("User limit reached. Cannot add more users.");
    showMainMenu();
    return;
  }

  Serial.print("Enter username: ");
  while (!Serial.available()) {}
  String username = Serial.readStringUntil('\n');

  // Check for duplicate username
  for (int i = 0; i < userCount; i++) {
    if (users[i].username == username) {
      Serial.println("Duplicate username detected. Try a different username.");
      showMainMenu();
      return;
    }
  }

  Serial.println("Enter 4 gesture password (Up, Down, Left, Right): ");
  String password = readGesturePassword();

  Serial.print("You entered ");
  Serial.print(password);
  Serial.println(", is that correct? (yes/no)");
  while (!Serial.available()) {}
  String confirmation = Serial.readStringUntil('\n');

  if (confirmation == "yes") {
    users[userCount].username = username;
    users[userCount].password = password;
    userCount++;
    Serial.println("User registered successfully.");
  } else {
    Serial.println("Registration cancelled.");
  }

  showMainMenu();
}

void logIn() {
  Serial.print("Enter username: ");
  while (!Serial.available()) {}
  String username = Serial.readStringUntil('\n');

  Serial.println("Enter your gesture password: ");
  String password = readGesturePassword();

  for (int i = 0; i < userCount; i++) {
    if (users[i].username == username && users[i].password == password) {
      Serial.println("Login successful!");
      showMainMenu();
      return;
    }
  }

  Serial.println("Wrong username or password.");
  showMainMenu();
}

String readGesturePassword() {
  String password = "";
  for (int i = 0; i < 4; i++) {
    while (!APDS.gestureAvailable()) {}
    int gesture = APDS.readGesture();
    
    switch (gesture) {
      case GESTURE_UP: password += "Up "; break;
      case GESTURE_DOWN: password += "Down "; break;
      case GESTURE_LEFT: password += "Left "; break;
      case GESTURE_RIGHT: password += "Right "; break;
    }
  }
  return password;
}

void showMainMenu() {
  Serial.println("\nMain Menu:");
  Serial.println("1. Sign Up (Gesture Lock)");
  Serial.println("2. Log In (Gesture Lock)");
  Serial.println("3. Rhythm Lock");
  Serial.println("Enter option number:");
}

void verifyPin() {
  for (int i = 0; i < numPins; i++) {
    if (enteredPin == validPins[i]) {
      pinVerified = true;
      Serial.println("PIN verified");
      return;
    }
  }
  Serial.println("**************************Invalid PIN**************************");
}

void runInference() {
  Serial.println("Sending intervals to model:");
  for (int i = 0; i < 4; i++) {
    Serial.print(intervals[i]);
    if (i < 3) Serial.print(", ");
  }
  Serial.println();

  // Prepare the buffer for the model
  float buffer[4];
  for (int i = 0; i < 4; i++) {
    buffer[i] = static_cast<float>(intervals[i]);
  }

  // Turn the buffer into a signal
  signal_t signal;
  int err = numpy::signal_from_buffer(buffer, 4, &signal);
  if (err != 0) {
    Serial.println("Failed to create signal from buffer");
    return;
  }

  // Run the classifier
  ei_impulse_result_t result;
  err = run_classifier(&signal, &result, false);
  if (err != EI_IMPULSE_OK) {
    Serial.println("Failed to run classifier");
    return;
  }

  // Print the predictions
  for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
    Serial.print(result.classification[ix].label);
    Serial.print(": ");
    Serial.println(result.classification[ix].value, 6);
  }

  // Check if the PIN corresponds to one of the valid classes
  int pinClass = -1;
  for (int i = 0; i < numPins; i++) {
    if (enteredPin == validPins[i]) {
      pinClass = i;
      break;
    }
  }

  if (pinClass == -1) {
    Serial.println("Invalid PIN class");
    return;
  }

  // Check if confidence score is below threshold
  if (result.classification[pinClass].value < thresholds[pinClass]) {
    Serial.println("**************************Bad Rhythm**************************");
  } else {
    Serial.println("***************Pin and rhythm verified***************");
  }
}

void resetVariables() {
  enteredPin = "";
  intervalCount = 0;
  pinVerified = false;
  Serial.println("Enter PIN (1-5) to verify rhythm:");
}
