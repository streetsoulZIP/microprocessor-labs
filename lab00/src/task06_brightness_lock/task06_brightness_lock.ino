const byte LED_PIN = 3;
const byte BUTTON_PIN = 2;
const byte POT_PIN = A0;

bool locked = false;
byte lockedPwm = 0;
int lastPrintedPot = -10;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println(F("T06:BOOT"));
}

void loop() {
  const bool buttonPressed = digitalRead(BUTTON_PIN) == LOW;
  const int pot = analogRead(POT_PIN);
  const byte livePwm = map(pot, 0, 1023, 0, 255);

  if (buttonPressed && !locked) {
    locked = true;
    lockedPwm = livePwm;
    Serial.print(F("T06:LOCKED PWM="));
    Serial.println(lockedPwm);
  } else if (!buttonPressed && locked) {
    locked = false;
    Serial.println(F("T06:UNLOCKED"));
  }

  analogWrite(LED_PIN, locked ? lockedPwm : livePwm);

  if (abs(pot - lastPrintedPot) >= 4) {
    lastPrintedPot = pot;
    if (locked) {
      Serial.println(F("T06:CHANGE_IMPOSSIBLE"));
    } else {
      Serial.print(F("T06:POT="));
      Serial.print(pot);
      Serial.print(F(" PWM="));
      Serial.println(livePwm);
      Serial.println(F("T06:UPDATED"));
    }
  }

  delay(20);
}
