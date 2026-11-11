const byte LED_PIN = 13;
const byte BUTTON_PIN = 2;
const unsigned long DEBOUNCE_MS = 40;

bool ledOn = false;
bool candidate = HIGH;
bool stable = HIGH;
unsigned long candidateSince = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println(F("T04:BOOT"));
}

void loop() {
  const bool raw = digitalRead(BUTTON_PIN);

  if (raw != candidate) {
    candidate = raw;
    candidateSince = millis();
  }

  if (candidate != stable && millis() - candidateSince >= DEBOUNCE_MS) {
    stable = candidate;
    if (stable == LOW) {
      ledOn = !ledOn;
      digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
      Serial.println(ledOn ? F("T04:LED_ON") : F("T04:LED_OFF"));
    } else {
      Serial.println(F("T04:RELEASED"));
    }
  }
}
