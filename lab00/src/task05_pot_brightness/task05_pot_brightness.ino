const byte LED_PIN = 3;
const byte POT_PIN = A0;

int lastPot = -10;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.println(F("T05:BOOT"));
}

void loop() {
  const int pot = analogRead(POT_PIN);
  const byte pwm = map(pot, 0, 1023, 0, 255);
  analogWrite(LED_PIN, pwm);

  if (abs(pot - lastPot) >= 4) {
    lastPot = pot;
    Serial.print(F("T05:POT="));
    Serial.print(pot);
    Serial.print(F(" PWM="));
    Serial.println(pwm);
    Serial.println(F("T05:UPDATED"));
  }

  delay(20);
}
