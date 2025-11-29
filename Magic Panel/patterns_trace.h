/*
 * Trace Patterns
 * Line and fill movements in all directions
 */

#ifndef PATTERNS_TRACE_H
#define PATTERNS_TRACE_H

#include "led_control.h"

extern PanelState panelState;
extern bool patternActive;

namespace Trace {
  
  void traceUp(int type = 1) { // Fill=1, Line=2
    CRGB color = LedControl::getCurrentColor();
    for (int row = 7; row >= 0; row--) {
      if (type == 2) LedControl::clearPanel();
      LedControl::setRow(row, 0xFF, color);
      LedControl::updatePanel();
      delay(100);
    }
  }
  
  void traceDown(int type = 1) { // Fill=1, Line=2
    CRGB color = LedControl::getCurrentColor();
    for (int row = 0; row < 8; row++) {
      if (type == 2) LedControl::clearPanel();
      LedControl::setRow(row, 0xFF, color);
      LedControl::updatePanel();
      delay(100);
    }
  }
  
  void traceRight(int type = 1) { // Fill=1, Line=2
    CRGB color = LedControl::getCurrentColor();
    for (int col = 0; col < 8; col++) {
      if (type == 2) LedControl::clearPanel();
      LedControl::setColumn(col, 0xFF, color);
      LedControl::updatePanel();
      delay(100);
    }
  }
  
  void traceLeft(int type = 1) { // Fill=1, Line=2
    CRGB color = LedControl::getCurrentColor();
    for (int col = 7; col >= 0; col--) {
      if (type == 2) LedControl::clearPanel();
      LedControl::setColumn(col, 0xFF, color);
      LedControl::updatePanel();
      delay(100);
    }
  }
  
  void compressIn(int type = 1) { // 1=Fill, 2=Fill then Clear
    CRGB color = LedControl::getCurrentColor();
    LedControl::clearPanel();
    for (int i = 0; i < 4; i++) {
      for (int x = i; x < 8 - i; x++) {
        LedControl::setPixel(x, i, color);
        LedControl::setPixel(x, 7 - i, color);
        LedControl::setPixel(i, x, color);
        LedControl::setPixel(7 - i, x, color);
      }
      LedControl::updatePanel();
      delay(100);
    }
    if (type == 2) {
      delay(500);
      for (int i = 3; i >= 0; i--) {
        for (int x = i; x < 8 - i; x++) {
          LedControl::setPixel(x, i, CRGB::Black);
          LedControl::setPixel(x, 7 - i, CRGB::Black);
          LedControl::setPixel(i, x, CRGB::Black);
          LedControl::setPixel(7 - i, x, CRGB::Black);
        }
        LedControl::updatePanel();
        delay(100);
      }
    }
  }

  void explodeOut(int type = 1) { // 1=Fill, 2=Fill then Clear
    CRGB color = LedControl::getCurrentColor();
    LedControl::clearPanel();
    for (int i = 3; i >= 0; i--) {
      for (int x = i; x < 8 - i; x++) {
        LedControl::setPixel(x, i, color);
        LedControl::setPixel(x, 7 - i, color);
        LedControl::setPixel(i, x, color);
        LedControl::setPixel(7 - i, x, color);
      }
      LedControl::updatePanel();
      delay(100);
    }
    if (type == 2) {
      delay(500);
      for (int i = 0; i < 4; i++) {
        for (int x = i; x < 8 - i; x++) {
          LedControl::setPixel(x, i, CRGB::Black);
          LedControl::setPixel(x, 7 - i, CRGB::Black);
          LedControl::setPixel(i, x, CRGB::Black);
          LedControl::setPixel(7 - i, x, CRGB::Black);
        }
        LedControl::updatePanel();
        delay(100);
      }
    }
  }
}

void runTracePattern(int patternId) {
  switch (patternId) {
    case 8: Trace::traceUp(1); break;
    case 9: Trace::traceUp(2); break;
    case 10: Trace::traceDown(1); break;
    case 11: Trace::traceDown(2); break;
    case 12: Trace::traceRight(1); break;
    case 13: Trace::traceRight(2); break;
    case 14: Trace::traceLeft(1); break;
    case 15: Trace::traceLeft(2); break;
    case 48: Trace::compressIn(1); break;
    case 49: Trace::compressIn(2); break;
    case 50: Trace::explodeOut(1); break;
    case 51: Trace::explodeOut(2); break;
  }
}
#endif // PATTERNS_TRACE_H