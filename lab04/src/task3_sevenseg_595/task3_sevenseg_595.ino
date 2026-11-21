constexpr uint8_t LATCH_PIN = 4;
constexpr uint8_t CLOCK_PIN = 7;
constexpr uint8_t DATA_PIN = 8;
constexpr unsigned long DIGIT_PERIOD_US = 2000;

const uint8_t segmentMasks[] = {
  0xC0, 0xF9, 0xA4, 0xB0, 0x99,
  0x92, 0x82, 0xF8, 0x80, 0x90,
};

uint16_t displayedValue = 0;
uint8_t activeDigit = 0;

void writeDigit(uint8_t position, uint8_t value) {
  const uint8_t digitMask = _BV(position);
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, digitMask);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, segmentMasks[value]);
  digitalWrite(LATCH_PIN, HIGH);
}

void refreshDisplay() {
  uint16_t divisor = 1;
  for (uint8_t i = 0; i < 3 - activeDigit; i++) {
    divisor *= 10;
  }
  writeDigit(activeDigit, (displayedValue / divisor) % 10);
  activeDigit = (activeDigit + 1) & 0x03;
}

void setup() {
  Serial.begin(115200);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  Serial.println(F("L04:595_READY DIGIT_HZ=125"));
}

void loop() {
  static unsigned long nextRefreshUs = 0;
  static unsigned long nextCountMs = 1000;
  const unsigned long nowUs = micros();

  if (static_cast<long>(nowUs - nextRefreshUs) >= 0) {
    nextRefreshUs = nowUs + DIGIT_PERIOD_US;
    refreshDisplay();
  }

  const unsigned long nowMs = millis();
  if (static_cast<long>(nowMs - nextCountMs) >= 0) {
    nextCountMs = nowMs + 1000;
    displayedValue = (displayedValue + 1) % 10000;
    Serial.print(F("L04:595 VALUE="));
    Serial.print(displayedValue);
    Serial.println(F(" DIGIT_HZ=125"));
  }
}
