/*
 * LED Control Utilities
 * Basic functions for controlling the LED matrix
 */

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <FastLED.h>
#include "config.h"

extern CRGB leds[NUM_LEDS];
extern PanelState panelState;

namespace LedControl {
  
  // Basic LED functions
  void setPixel(uint8_t x, uint8_t y, CRGB color);
  void setPixel(uint8_t index, CRGB color);
  CRGB getPixel(uint8_t x, uint8_t y);
  
  // Row and column operations
  void setRow(uint8_t row, uint8_t pattern, CRGB color = CRGB::Red);
  void setColumn(uint8_t col, uint8_t pattern, CRGB color = CRGB::Red);
  
  // Panel operations
  void clearPanel();
  void allOn(CRGB color = CRGB::Red);
  void updatePanel();
  void fadeOutEffect(uint8_t delay_ms = 5);
  
  // Utility functions
  uint8_t xyToIndex(uint8_t x, uint8_t y);
  void indexToXY(uint8_t index, uint8_t &x, uint8_t &y);
  
  // Color utilities
  CRGB getCurrentColor();
  CRGB getRandomColor();
  CRGB getRainbowColor(uint8_t position);
}

// Implementation
namespace LedControl {
  
  inline void setPixel(uint8_t x, uint8_t y, CRGB color) {
    if (x < MATRIX_WIDTH && y < MATRIX_HEIGHT) {
      leds[xyToIndex(x, y)] = color;
    }
  }
  
  inline void setPixel(uint8_t index, CRGB color) {
    if (index < NUM_LEDS) {
      leds[index] = color;
    }
  }
  
  inline CRGB getPixel(uint8_t x, uint8_t y) {
    if (x < MATRIX_WIDTH && y < MATRIX_HEIGHT) {
      return leds[xyToIndex(x, y)];
    }
    return CRGB::Black;
  }
  
  // KORRIGIERTE FUNKTION für progressives Layout
  inline uint8_t xyToIndex(uint8_t x, uint8_t y) {
    // Standard "Scanline" Layout: Zeile für Zeile von links nach rechts
    return (y * MATRIX_WIDTH) + x;
  }
  
  // KORRIGIERTE FUNKTION für progressives Layout
  inline void indexToXY(uint8_t index, uint8_t &x, uint8_t &y) {
    y = index / MATRIX_WIDTH;
    x = index % MATRIX_WIDTH;
  }
  
  inline void setRow(uint8_t row, uint8_t pattern, CRGB color) {
    if (row < MATRIX_HEIGHT) {
      for (uint8_t col = 0; col < MATRIX_WIDTH; col++) {
        // Bit 0 ist rechts, Bit 7 ist links. Wir müssen es umkehren.
        if (pattern & (1 << (7 - col))) {
          setPixel(col, row, color);
        } else {
          setPixel(col, row, CRGB::Black);
        }
      }
    }
  }
  
  inline void setColumn(uint8_t col, uint8_t pattern, CRGB color) {
    if (col < MATRIX_WIDTH) {
      for (uint8_t row = 0; row < MATRIX_HEIGHT; row++) {
        // Bit 0 ist oben, Bit 7 ist unten.
        if (pattern & (1 << row)) {
          setPixel(col, row, color);
        } else {
          setPixel(col, row, CRGB::Black);
        }
      }
    }
  }
  
  inline void clearPanel() {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
  }
  
  inline void allOn(CRGB color) {
    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();
  }
  
  inline void updatePanel() {
    FastLED.show();
  }
  
  inline void fadeOutEffect(uint8_t delay_ms) {
    uint8_t currentBrightness = FastLED.getBrightness();
    for (int i = currentBrightness; i >= 0; i--) {
      FastLED.setBrightness(i);
      FastLED.show();
      delay(delay_ms);
    }
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    FastLED.setBrightness(currentBrightness);
  }

  inline CRGB getCurrentColor() {
    if (panelState.rainbowMode) {
      static uint8_t hue = 0;
      hue += 2;
      return CHSV(hue, 255, 255);
    }
    return panelState.currentColor;
  }
  
  inline CRGB getRandomColor() {
    return CHSV(random8(), 255, 255);
  }
  
  inline CRGB getRainbowColor(uint8_t position) {
    return CHSV(position * 255 / NUM_LEDS, 255, 255);
  }
}

#endif // LED_CONTROL_H