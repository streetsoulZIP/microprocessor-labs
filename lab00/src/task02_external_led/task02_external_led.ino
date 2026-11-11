const byte LED_PIN = 13;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.println(F("T02:BOOT"));
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println(F("T02:LED_ON"));
  delay(500);

  digitalWrite(LED_PIN, LOW);
  Serial.println(F("T02:LED_OFF"));
  delay(500);
}
