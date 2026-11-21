#include <avr/interrupt.h>

constexpr uint8_t POT_PIN = A0;
constexpr uint8_t segmentPins[] = {12, 10, 6, 8, 9, 11, 5, 7};  // A B C D E F G DP
constexpr uint8_t digitPins[] = {13, 3, 2, 4};

const uint8_t digitMasks[] = {
  0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110,
  0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111,
};

volatile uint8_t displayDigits[4] = {0, 0, 0, 0};
volatile uint8_t activeDigit = 0;

void setSegments(uint8_t value, bool decimalPoint) {
  uint8_t mask = digitMasks[value];
  if (decimalPoint) {
    mask |= 0x80;
  }
  for (uint8_t i = 0; i < 8; i++) {
    digitalWrite(segmentPins[i], bitRead(mask, i));
  }
}

ISR(TIMER1_COMPA_vect) {
  for (uint8_t pin : digitPins) {
    digitalWrite(pin, HIGH);
  }

  setSegments(displayDigits[activeDigit], activeDigit == 0);
  digitalWrite(digitPins[activeDigit], LOW);
  activeDigit = (activeDigit + 1) & 0x03;
}

void setDisplayedMillivolts(uint16_t millivolts) {
  uint8_t nextDigits[4];
  for (int8_t i = 3; i >= 0; i--) {
    nextDigits[i] = millivolts % 10;
    millivolts /= 10;
  }

  noInterrupts();
  for (uint8_t i = 0; i < 4; i++) {
    displayDigits[i] = nextDigits[i];
  }
  interrupts();
}

void setupTimer1() {
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 249;  // 16 MHz / 64 / (249 + 1) = 1000 Hz
  TCCR1B = _BV(WGM12) | _BV(CS11) | _BV(CS10);
  TIMSK1 = _BV(OCIE1A);
  interrupts();
}

void setup() {
  Serial.begin(115200);
  for (uint8_t pin : segmentPins) {
    pinMode(pin, OUTPUT);
  }
  for (uint8_t pin : digitPins) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
  }
  setupTimer1();
  Serial.println(F("L04:7SEG_READY ISR_HZ=1000 DIGIT_HZ=250"));
}

void loop() {
  static unsigned long nextReadMs = 0;
  static unsigned long nextLogMs = 0;
  static uint16_t adc = 0;
  static uint16_t millivolts = 0;

  const unsigned long now = millis();
  if (static_cast<long>(now - nextReadMs) >= 0) {
    nextReadMs = now + 100;
    adc = analogRead(POT_PIN);
    millivolts = (static_cast<uint32_t>(adc) * 5000 + 511) / 1023;
    setDisplayedMillivolts(millivolts);
  }

  if (static_cast<long>(now - nextLogMs) >= 0) {
    nextLogMs = now + 500;
    Serial.print(F("L04:7SEG ADC="));
    Serial.print(adc);
    Serial.print(F(" MV="));
    Serial.print(millivolts);
    Serial.println(F(" DIGIT_HZ=250"));
  }
}
