const byte LED_PIN = LED_BUILTIN;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.println(F("T01:BOOT"));
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println(F("T01:LED_ON"));
  delay(500);

  digitalWrite(LED_PIN, LOW);
  Serial.println(F("T01:LED_OFF"));
  delay(500);
}
