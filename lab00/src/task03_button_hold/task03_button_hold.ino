const byte LED_PIN = 13;
const byte BUTTON_PIN = 2;

bool lastPressed = false;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println(F("T03:BOOT"));
}

void loop() {
  const bool pressed = digitalRead(BUTTON_PIN) == LOW;
  digitalWrite(LED_PIN, pressed ? HIGH : LOW);

  if (pressed != lastPressed) {
    lastPressed = pressed;
    Serial.println(pressed ? F("T03:PRESSED") : F("T03:RELEASED"));
  }

  delay(10);
}
