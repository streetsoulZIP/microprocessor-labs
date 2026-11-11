const byte RED_PIN = 3;
const byte YELLOW_PIN = 4;
const byte GREEN_PIN = 5;
const int PHASE_SECONDS = 6;

void setLights(bool red, bool yellow, bool green) {
  digitalWrite(RED_PIN, red);
  digitalWrite(YELLOW_PIN, yellow);
  digitalWrite(GREEN_PIN, green);
}

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.println(F("T07B:BOOT"));
}

void loop() {
  for (int secondsLeft = PHASE_SECONDS; secondsLeft > 0; --secondsLeft) {
    Serial.print(F("T07B:GREEN_LEFT="));
    Serial.println(secondsLeft);

    setLights(false, false, true);
    if (secondsLeft <= 3) {
      delay(500);
      setLights(false, false, false);
      delay(500);
    } else {
      delay(1000);
    }
  }

  setLights(false, true, false);
  Serial.println(F("T07B:YELLOW"));
  delay(1000);

  for (int secondsLeft = PHASE_SECONDS; secondsLeft > 0; --secondsLeft) {
    Serial.print(F("T07B:RED_LEFT="));
    Serial.println(secondsLeft);
    setLights(true, secondsLeft == 1, false);
    delay(1000);
  }
}
