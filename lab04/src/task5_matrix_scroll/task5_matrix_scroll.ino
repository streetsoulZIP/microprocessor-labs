#include <avr/pgmspace.h>

constexpr uint8_t DATA_PIN = 11;
constexpr uint8_t CLOCK_PIN = 13;
constexpr uint8_t CS_PIN = 10;
constexpr uint8_t MODULES = 4;
constexpr uint8_t SCREEN_COLUMNS = MODULES * 8;
constexpr uint8_t GLYPH_WIDTH = 5;
constexpr uint8_t GLYPH_STEP = 6;
constexpr uint8_t TEXT_LENGTH = 6;
constexpr int16_t TEXT_COLUMNS = TEXT_LENGTH * GLYPH_STEP;

const uint8_t font[][GLYPH_WIDTH] PROGMEM = {
  {0x26, 0x49, 0x49, 0x49, 0x32},  // S
  {0x7E, 0x09, 0x09, 0x09, 0x7E},  // A
  {0x7F, 0x02, 0x0C, 0x02, 0x7F},  // M
  {0x3E, 0x41, 0x49, 0x49, 0x7A},  // G
  {0x01, 0x01, 0x7F, 0x01, 0x01},  // T
  {0x3F, 0x40, 0x40, 0x40, 0x3F},  // U
};

uint8_t frame[MODULES][8];
int16_t scrollOffset = -SCREEN_COLUMNS;

void sendRow(uint8_t row) {
  digitalWrite(CS_PIN, LOW);
  for (int8_t module = MODULES - 1; module >= 0; module--) {
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, row + 1);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, frame[module][row]);
  }
  digitalWrite(CS_PIN, HIGH);
}

void sendAll(uint8_t address, uint8_t value) {
  digitalWrite(CS_PIN, LOW);
  for (uint8_t module = 0; module < MODULES; module++) {
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, address);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, value);
  }
  digitalWrite(CS_PIN, HIGH);
}

uint8_t textColumn(int16_t column) {
  if (column < 0 || column >= TEXT_COLUMNS) {
    return 0;
  }
  const uint8_t glyph = column / GLYPH_STEP;
  const uint8_t inside = column % GLYPH_STEP;
  return inside < GLYPH_WIDTH ? pgm_read_byte(&font[glyph][inside]) : 0;
}

void buildFrame() {
  memset(frame, 0, sizeof(frame));
  for (uint8_t x = 0; x < SCREEN_COLUMNS; x++) {
    const uint8_t columnBits = textColumn(scrollOffset + x);
    const uint8_t module = x / 8;
    const uint8_t bit = 7 - (x % 8);
    for (uint8_t row = 0; row < 8; row++) {
      if (bitRead(columnBits, row)) {
        bitSet(frame[module][row], bit);
      }
    }
  }
}

void showFrame() {
  for (uint8_t row = 0; row < 8; row++) {
    sendRow(row);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  sendAll(0x0F, 0x00);
  sendAll(0x0C, 0x01);
  sendAll(0x0B, 0x07);
  sendAll(0x09, 0x00);
  sendAll(0x0A, 0x05);
  buildFrame();
  showFrame();
  Serial.println(F("L04:SCROLL_READY TEXT=SAMGTU MODULES=4"));
}

void loop() {
  static unsigned long nextStepMs = 0;
  const unsigned long now = millis();
  if (static_cast<long>(now - nextStepMs) < 0) {
    return;
  }

  nextStepMs = now + 90;
  buildFrame();
  showFrame();
  if ((scrollOffset & 0x07) == 0) {
    Serial.print(F("L04:SCROLL OFFSET="));
    Serial.println(scrollOffset);
  }

  scrollOffset++;
  if (scrollOffset > TEXT_COLUMNS) {
    scrollOffset = -SCREEN_COLUMNS;
  }
}
