/*
 * Basic Patterns
 * Simple on/off patterns and alerts
 */

#ifndef PATTERNS_BASIC_H
#define PATTERNS_BASIC_H

#include "led_control.h"

// Forward declaration for function in main .ino file
extern void setPatternEndTime(unsigned long duration);
extern PanelState panelState;
extern bool patternActive;

namespace Basic {
  
  // Basic on/off patterns
  void allOnTimed(unsigned long duration = 0) {
    LedControl::allOn(LedControl::getCurrentColor());
    if (duration > 0) {
      setPatternEndTime(duration);
    } else {
      setPatternEndTime(3600000); // 1 hour
    }
  }
  
  // Toggle top and bottom halves
  void toggle(int repeats = 10) {
    CRGB color = LedControl::getCurrentColor();
    
    for (int i = 0; i < repeats; i++) {
      LedControl::clearPanel();
      for (int row = 0; row < 4; row++) LedControl::setRow(row, B11111111, color);
      LedControl::updatePanel();
      delay(500);
      
      LedControl::clearPanel();
      for (int row = 4; row < 8; row++) LedControl::setRow(row, B11111111, color);
      LedControl::updatePanel();
      delay(500);
    }
  }
  
  // Alert pattern - rapid flashing
  void alert(unsigned long duration) {
    unsigned long startTime = millis();
    while(millis() - startTime < duration) {
      LedControl::allOn(LedControl::getCurrentColor());
      delay(100);
      LedControl::clearPanel();
      delay(100);
    }
  }
}

// Main dispatcher function for this category
void runBasicPattern(int patternId) {
  switch (patternId) {
    case 0: LedControl::clearPanel(); break;
    case 1: Basic::allOnTimed(0); break;
    case 2: Basic::allOnTimed(2000); break;
    case 3: Basic::allOnTimed(5000); break;
    case 4: Basic::allOnTimed(10000); break;
    case 5: Basic::toggle(panelState.alwaysOn ? 2 : 5); break;
    case 6: Basic::alert(4000); break;
    case 7: Basic::alert(10000); break;
  }
  if (!panelState.alwaysOn && !patternActive) LedControl::clearPanel();
}

#endif // PATTERNS_BASIC_H