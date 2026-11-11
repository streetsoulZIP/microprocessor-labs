const byte RED_PIN = 9;
const byte GREEN_PIN = 10;
const byte BLUE_PIN = 11;

void setColor(byte red, byte green, byte blue) {
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.println(F("T09:BOOT"));
}

void loop() {
  setColor(0, 255, 0);
  Serial.println(F("T09:GREEN"));
  delay(1000);

  setColor(255, 160, 0);
  Serial.println(F("T09:YELLOW=RED+GREEN"));
  delay(1000);

  setColor(255, 0, 0);
  Serial.println(F("T09:RED"));
  delay(1000);
}
