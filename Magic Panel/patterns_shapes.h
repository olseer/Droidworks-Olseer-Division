/*
 * Shape Patterns
 * Static shapes and symbols including digits
 */

#ifndef PATTERNS_SHAPES_H
#define PATTERNS_SHAPES_H

#include "led_control.h"

// Forward declarations for global variables and functions defined in the main .ino file
extern void setPatternEndTime(unsigned long duration);
extern PanelState panelState;
extern bool patternActive;

namespace Shapes {
  
  // Sprites für Gaming-Animationen
  const uint8_t pacman_open[] = { B00111100, B01111110, B11000111, B11000011, B11000111, B01111110, B00111100, B00000000 };
  const uint8_t pacman_closed[] = { B00111100, B01111110, B11111111, B11111111, B11111111, B01111110, B00111100, B00000000 };
  const uint8_t ghost[] = { B00111100, B01111110, B10111011, B11111111, B11111111, B10101010, B10101010, B00000000 };
  const uint8_t invader[] = { B00011000, B00111100, B01111110, B11011011, B11111111, B01011010, B00100100, B00000000 };

  // Shape definitions
  const uint8_t CROSS_PATTERN[8] = { B00011000, B00111100, B01111110, B11111111, B11111111, B01111110, B00111100, B00011000 };
  const uint8_t AI_LOGO[8] = { B00111100, B01000010, B10111011, B10111011, B10000001, B10000001, B01111110, B00000000 };
  const uint8_t HEART_PATTERN[8] = { B00000000, B01100110, B11111111, B11111111, B01111110, B00111100, B00011000, B00000000 };
  const uint8_t HEART_SMALL[8] = { B00000000, B00000000, B00100100, B01111110, B01111110, B00111100, B00011000, B00000000 };
  const uint8_t SMILEY_PATTERN[8] = { B00111100, B01000010, B10100101, B10000001, B10011001, B10100101, B01000010, B00111100 };
  const uint8_t SAD_PATTERN[8] = { B00111100, B01000010, B10100101, B10000001, B10100101, B10011001, B01000010, B00111100 };
  
  // Digit patterns (Centered 5x7)
  const uint8_t DIGITS[10][8] = {
    {B00000000,B00111000,B01000100,B01000100,B01000100,B01000100,B00111000,B00000000}, // 0
    {B00000000,B00010000,B00110000,B00010000,B00010000,B00010000,B00111000,B00000000}, // 1
    {B00000000,B00111000,B01000100,B00001000,B00010000,B00100000,B01111100,B00000000}, // 2
    {B00000000,B00111000,B01000100,B00110000,B00000100,B01000100,B00111000,B00000000}, // 3
    {B00000000,B00001000,B00011000,B00101000,B01111100,B00001000,B00001000,B00000000}, // 4
    {B00000000,B01111100,B01000000,B01111000,B00000100,B01000100,B00111000,B00000000}, // 5
    {B00000000,B00111000,B01000000,B01111000,B01000100,B01000100,B00111000,B00000000}, // 6
    {B00000000,B01111100,B00000100,B00001000,B00010000,B00100000,B00100000,B00000000}, // 7
    {B00000000,B00111000,B01000100,B00111000,B01000100,B01000100,B00111000,B00000000}, // 8
    {B00000000,B00111000,B01000100,B01000100,B00111100,B00000100,B00111000,B00000000}  // 9
  };

  void drawSprite(int8_t x, int8_t y, const uint8_t sprite[8], CRGB color) {
    if (x > 7 || x < -7 || y > 7 || y < -7) return;
    for (int sy = 0; sy < 8; sy++) {
      int8_t py = y + sy;
      if (py >= 0 && py < 8) {
        for (int sx = 0; sx < 8; sx++) {
          int8_t px = x + sx;
          if (px >= 0 && px < 8) {
            if ((sprite[sy] >> (7 - sx)) & 1) {
              LedControl::setPixel(px, py, color);
            }
          }
        }
      }
    }
  }

  void drawShape(const uint8_t shape[8]) {
    CRGB color = LedControl::getCurrentColor();
    for (int i = 0; i < 8; i++) {
      LedControl::setRow(i, shape[i], color);
    }
    LedControl::updatePanel();
  }
  
  void showDigit(int digit) {
    if (digit < 0 || digit > 9) return;
    drawShape(DIGITS[digit]);
  }

  void countdown(int start) {
    for (int i = start; i >= 0; i--) {
      showDigit(i);
      delay(1000);
    }
  }

  void twoGWDLogo() {
    showDigit(2); delay(1000);
    const uint8_t G[] = {0, 0x3E, 0x41, 0x41, 0x49, 0x49, 0x3E, 0}; drawShape(G); delay(1000);
    const uint8_t W[] = {0, 0x81, 0x81, 0x99, 0x99, 0x5A, 0x3C, 0}; drawShape(W); delay(1000);
    const uint8_t D[] = {0, 0x7C, 0x82, 0x81, 0x81, 0x82, 0x7C, 0}; drawShape(D);
  }

  void checkerboard(int flashes = 4) {
    CRGB color1 = LedControl::getCurrentColor();
    CRGB color2 = panelState.rainbowMode ? LedControl::getRandomColor() : CRGB::Black;
    for (int flash = 0; flash < flashes; flash++) {
      for (int i = 0; i < NUM_LEDS; i++) leds[i] = (i % 2 == (i / 8) % 2) ? color1 : color2;
      FastLED.show(); delay(200);
      for (int i = 0; i < NUM_LEDS; i++) leds[i] = (i % 2 != (i / 8) % 2) ? color1 : color2;
      FastLED.show(); delay(200);
    }
  }

  void animatedHeart() {
    CRGB color = LedControl::getCurrentColor();
    for (int beat = 0; beat < 3; beat++) {
      drawShape(HEART_SMALL); delay(200);
      drawShape(HEART_PATTERN); delay(300);
      drawShape(HEART_SMALL); delay(150);
      LedControl::clearPanel(); delay(400);
    }
  }

  void randomAlert(int repeats = 20) {
    CRGB color = LedControl::getCurrentColor();
    for (int i = 0; i < repeats; i++) {
      LedControl::allOn(color);
      delay(random(5, 40));
      LedControl::clearPanel();
      delay(random(3, 25));
    }
  }

  void randomPixel(int duration = 40) {
    CRGB color = LedControl::getCurrentColor();
    for (int i = 0; i < duration; i++) {
      leds[random(NUM_LEDS)] = color;
      FastLED.show();
      delay(100);
      LedControl::clearPanel();
    }
  }

  void testFill(int pixelDelay = 20) {
    CRGB color = LedControl::getCurrentColor();
    for (int i = 0; i < NUM_LEDS; i++) { leds[i] = color; FastLED.show(); delay(pixelDelay); }
    for (int i = 0; i < NUM_LEDS; i++) { leds[i] = CRGB::Black; FastLED.show(); delay(pixelDelay); }
  }

  void testPixel(int pixelDelay = 20) {
    CRGB color = LedControl::getCurrentColor();
    for (int i = 0; i < NUM_LEDS; i++) {
      LedControl::clearPanel();
      leds[i] = color;
      FastLED.show();
      delay(pixelDelay);
    }
  }

  void quadrant(int type = 1, int repeats = 2) {
    CRGB color = LedControl::getCurrentColor();
    uint8_t quadrants[4][2] = {{0,0}, {4,0}, {4,4}, {0,4}}; // TL, TR, BR, BL
    uint8_t order[4];

    switch(type) {
      case 1: { uint8_t o[] = {0,1,2,3}; memcpy(order, o, 4); break; } // TL, TR, BR, BL
      case 2: { uint8_t o[] = {1,0,3,2}; memcpy(order, o, 4); break; } // TR, TL, BL, BR
      case 3: { uint8_t o[] = {1,2,3,0}; memcpy(order, o, 4); break; } // TR, BR, BL, TL
      case 4: { uint8_t o[] = {0,3,2,1}; memcpy(order, o, 4); break; } // TL, BL, BR, TR
      default: { uint8_t o[] = {0,1,2,3}; memcpy(order, o, 4); break; }
    }

    for(int r=0; r<repeats; r++) {
      for(int i=0; i<4; i++) {
        LedControl::clearPanel();
        uint8_t q_index = order[i];
        for(int y=quadrants[q_index][1]; y < quadrants[q_index][1]+4; y++) {
          for(int x=quadrants[q_index][0]; x < quadrants[q_index][0]+4; x++) {
            LedControl::setPixel(x,y,color);
          }
        }
        LedControl::updatePanel();
        delay(200);
      }
    }
  }
}

// Main dispatcher function for this category
void runShapePattern(int patternId) {
  unsigned long duration = 3000;

  switch (patternId) {
    case 20: Shapes::drawShape(Shapes::CROSS_PATTERN); setPatternEndTime(duration); break;
    case 31: Shapes::testFill(); break;
    case 32: Shapes::testPixel(); break;
    case 33: Shapes::drawShape(Shapes::AI_LOGO); setPatternEndTime(duration); break;
    case 34: Shapes::twoGWDLogo(); setPatternEndTime(4000); break;
    case 35: Shapes::quadrant(1); break;
    case 36: Shapes::quadrant(2); break;
    case 37: Shapes::quadrant(3); break;
    case 38: Shapes::quadrant(4); break;
    case 39: Shapes::randomPixel(); break;
    case 40: Shapes::countdown(9); break;
    case 41: Shapes::countdown(3); break;
    case 42: Shapes::randomAlert(20); break;
    case 43: Shapes::randomAlert(40); break;
    case 44: Shapes::drawShape(Shapes::SMILEY_PATTERN); setPatternEndTime(duration); break;
    case 45: Shapes::drawShape(Shapes::SAD_PATTERN); setPatternEndTime(duration); break;
    case 46: Shapes::drawShape(Shapes::HEART_PATTERN); setPatternEndTime(duration); break;
    case 47: Shapes::checkerboard(); break;
    case 56: Shapes::animatedHeart(); break;
  }
  if (!panelState.alwaysOn && !patternActive) LedControl::clearPanel();
}

#endif // PATTERNS_SHAPES_H