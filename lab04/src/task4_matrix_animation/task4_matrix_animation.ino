#include <avr/pgmspace.h>

constexpr uint8_t DATA_PIN = 11;
constexpr uint8_t CLOCK_PIN = 13;
constexpr uint8_t CS_PIN = 10;

const uint8_t frames[][8] PROGMEM = {
  {0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81},
  {0x00, 0x18, 0x3C, 0x7E, 0x7E, 0x3C, 0x18, 0x00},
  {0x18, 0x3C, 0x7E, 0xDB, 0xFF, 0x66, 0x24, 0x00},
  {0x00, 0x24, 0x66, 0xFF, 0xDB, 0x7E, 0x3C, 0x18},
};

void sendRegister(uint8_t address, uint8_t value) {
  digitalWrite(CS_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, address);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, value);
  digitalWrite(CS_PIN, HIGH);
}

void showFrame(uint8_t frame) {
  for (uint8_t row = 0; row < 8; row++) {
    sendRegister(row + 1, pgm_read_byte(&frames[frame][row]));
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  sendRegister(0x0F, 0x00);  // display test off
  sendRegister(0x0C, 0x01);  // normal operation
  sendRegister(0x0B, 0x07);  // all eight rows
  sendRegister(0x09, 0x00);  // no BCD decoding
  sendRegister(0x0A, 0x06);  // medium intensity
  showFrame(0);
  Serial.println(F("L04:MATRIX_READY PATTERN=X"));
}

void loop() {
  static unsigned long nextFrameMs = 900;
  static uint8_t frame = 0;
  const unsigned long now = millis();

  if (static_cast<long>(now - nextFrameMs) >= 0) {
    nextFrameMs = now + 350;
    frame = (frame + 1) % 4;
    showFrame(frame);
    Serial.print(F("L04:MATRIX_FRAME="));
    Serial.println(frame);
  }
}
