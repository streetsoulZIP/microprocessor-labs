const byte RED_PIN = 3;
const byte YELLOW_PIN = 4;
const byte GREEN_PIN = 5;

void setLights(bool red, bool yellow, bool green) {
  digitalWrite(RED_PIN, red);
  digitalWrite(YELLOW_PIN, yellow);
  digitalWrite(GREEN_PIN, green);
}

void blinkGreenOnce() {
  setLights(false, false, true);
  delay(500);
  setLights(false, false, false);
  delay(500);
}

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.println(F("T07A:BOOT"));
}

void loop() {
  setLights(false, false, true);
  Serial.println(F("T07A:GREEN_STEADY"));
  delay(3000);

  Serial.println(F("T07A:GREEN_BLINK"));
  blinkGreenOnce();
  blinkGreenOnce();
  blinkGreenOnce();

  setLights(false, true, false);
  Serial.println(F("T07A:YELLOW"));
  delay(1000);

  setLights(true, false, false);
  Serial.println(F("T07A:RED"));
  delay(5000);

  setLights(true, true, false);
  Serial.println(F("T07A:RED_YELLOW"));
  delay(1000);
}
