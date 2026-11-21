#include <Adafruit_FT6206.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <Wire.h>

constexpr uint8_t TFT_CS = 10;
constexpr uint8_t TFT_DC = 9;
constexpr int16_t SCREEN_WIDTH = 320;
constexpr int16_t SCREEN_HEIGHT = 240;
constexpr int16_t TOOLBAR_HEIGHT = 42;

Adafruit_ILI9341 tft(TFT_CS, TFT_DC);
Adafruit_FT6206 touch;

const uint16_t palette[] = {ILI9341_RED, ILI9341_GREEN, ILI9341_BLUE};
uint16_t currentColor = ILI9341_RED;
unsigned long lastTouchMs = 0;

void drawToolbar() {
  tft.fillScreen(ILI9341_BLACK);
  tft.fillRect(0, 0, SCREEN_WIDTH, TOOLBAR_HEIGHT, ILI9341_DARKGREY);

  for (uint8_t i = 0; i < 3; i++) {
    const int16_t x = 8 + i * 54;
    tft.fillRoundRect(x, 6, 46, 30, 4, palette[i]);
  }

  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(182, 13);
  tft.print(F("DRAW AREA"));
  tft.drawRect(0, TOOLBAR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - TOOLBAR_HEIGHT, ILI9341_WHITE);
}

bool readTouch(int16_t &screenX, int16_t &screenY) {
  if (!touch.touched()) {
    return false;
  }

  TS_Point point = touch.getPoint();
  // FT6206 uses a bottom-right origin. Rotate that raw portrait coordinate
  // into the same landscape orientation as tft.setRotation(1).
  screenX = map(point.y, 0, 319, 0, SCREEN_WIDTH - 1);
  screenY = map(point.x, 0, 239, SCREEN_HEIGHT - 1, 0);
  screenX = constrain(screenX, 0, SCREEN_WIDTH - 1);
  screenY = constrain(screenY, 0, SCREEN_HEIGHT - 1);
  return true;
}

void handleTouch(int16_t x, int16_t y) {
  if (y < TOOLBAR_HEIGHT && x < 170) {
    const uint8_t selected = constrain((x - 8) / 54, 0, 2);
    currentColor = palette[selected];
    Serial.print(F("L04:TFT_COLOR="));
    Serial.println(selected);
    return;
  }

  if (y >= TOOLBAR_HEIGHT + 5) {
    tft.fillCircle(x, y, 5, currentColor);
    Serial.print(F("L04:TFT_TOUCH x="));
    Serial.print(x);
    Serial.print(F(" y="));
    Serial.println(y);
  }
}

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(1);

  if (!touch.begin(40)) {
    Serial.println(F("L04:TFT_TOUCH_ERROR"));
    while (true) {}
  }

  drawToolbar();
  Serial.println(F("L04:TFT_READY 320x240 FT6206"));
}

void loop() {
  int16_t x;
  int16_t y;
  if (millis() - lastTouchMs >= 70 && readTouch(x, y)) {
    lastTouchMs = millis();
    handleTouch(x, y);
  }
}
