const byte RED_PIN = 3;
const byte YELLOW_PIN = 4;
const byte GREEN_PIN = 5;
const byte BUTTON_PIN = 2;

enum State { WAIT_GREEN, REQUEST_DELAY, YELLOW, RED, RED_YELLOW };
State state = WAIT_GREEN;
unsigned long stateSince = 0;

bool candidateButton = HIGH;
bool stableButton = HIGH;
unsigned long candidateSince = 0;

void setLights(bool red, bool yellow, bool green) {
  digitalWrite(RED_PIN, red);
  digitalWrite(YELLOW_PIN, yellow);
  digitalWrite(GREEN_PIN, green);
}

void enterState(State next) {
  state = next;
  stateSince = millis();

  if (state == WAIT_GREEN) {
    setLights(false, false, true);
    Serial.println(F("T08:WAIT"));
  } else if (state == REQUEST_DELAY) {
    setLights(false, false, true);
    Serial.println(F("T08:REQUEST WAIT_5S"));
  } else if (state == YELLOW) {
    setLights(false, true, false);
    Serial.println(F("T08:YELLOW"));
  } else if (state == RED) {
    setLights(true, false, false);
    Serial.println(F("T08:RED"));
  } else {
    setLights(true, true, false);
    Serial.println(F("T08:RED_YELLOW"));
  }
}

void updateButton() {
  const bool raw = digitalRead(BUTTON_PIN);
  if (raw != candidateButton) {
    candidateButton = raw;
    candidateSince = millis();
  }

  if (candidateButton != stableButton && millis() - candidateSince >= 40) {
    stableButton = candidateButton;
    if (stableButton == LOW && state == WAIT_GREEN) {
      enterState(REQUEST_DELAY);
    }
  }
}

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  enterState(WAIT_GREEN);
}

void loop() {
  updateButton();
  const unsigned long elapsed = millis() - stateSince;

  if (state == REQUEST_DELAY && elapsed >= 5000) enterState(YELLOW);
  else if (state == YELLOW && elapsed >= 1000) enterState(RED);
  else if (state == RED && elapsed >= 3000) enterState(RED_YELLOW);
  else if (state == RED_YELLOW && elapsed >= 1000) enterState(WAIT_GREEN);
}
