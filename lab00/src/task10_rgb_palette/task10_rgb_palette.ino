const byte RED_PIN = 9;
const byte GREEN_PIN = 10;
const byte BLUE_PIN = 11;
const byte POT_PIN = A0;

int lastPrintedPot = -10;

void setColor(byte red, byte green, byte blue) {
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}

void setHue(byte hue) {
  if (hue < 85) {
    setColor(255 - hue * 3, hue * 3, 0);
  } else if (hue < 170) {
    hue -= 85;
    setColor(0, 255 - hue * 3, hue * 3);
  } else {
    hue -= 170;
    setColor(hue * 3, 0, 255 - hue * 3);
  }
}

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.println(F("T10:BOOT"));
}

void loop() {
  const int pot = analogRead(POT_PIN);
  const byte hue = map(pot, 0, 1023, 0, 255);
  setHue(hue);

  if (abs(pot - lastPrintedPot) >= 4) {
    lastPrintedPot = pot;
    Serial.print(F("T10:POT="));
    Serial.print(pot);
    Serial.print(F(" HUE="));
    Serial.println(map(hue, 0, 255, 0, 360));
    Serial.println(F("T10:UPDATED"));
  }

  delay(20);
}
