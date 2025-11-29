/*
 * Effect Patterns
 * Special effects like expand, compress, fade, and other visual textures.
 * VERSION 2.2 - Added support for custom duration
 */

#ifndef PATTERNS_EFFECTS_H
#define PATTERNS_EFFECTS_H

#include "led_control.h"
#include "patterns_basic.h" // Wird für einige Flash-Effekte benötigt

// Forward declarations for global variables and functions defined in the main .ino file
extern PanelState panelState;
extern bool patternActive;
extern void setPatternEndTime(unsigned long duration);
extern unsigned long customPatternDuration;

namespace Effects {
  
  // Expand from center
  void expand(int repeats = 1, int type = 1) { // Fill=1, Ring=2
    CRGB color = LedControl::getCurrentColor();
    
    for (int i = 0; i < repeats; i++) {
      if (type == 1) {
        // Fill mode
        LedControl::clearPanel();
        LedControl::setRow(3, B00011000, color); LedControl::setRow(4, B00011000, color);
        LedControl::updatePanel(); delay(150);
        LedControl::setRow(2, B00111100, color); LedControl::setRow(5, B00111100, color);
        LedControl::updatePanel(); delay(150);
        LedControl::setRow(1, B01111110, color); LedControl::setRow(6, B01111110, color);
        LedControl::updatePanel(); delay(150);
        LedControl::setRow(0, B11111111, color); LedControl::setRow(7, B11111111, color);
        LedControl::updatePanel(); delay(150);
      } else {
        // Ring mode
        LedControl::clearPanel();
        LedControl::setRow(3, B00011000, color); LedControl::setRow(4, B00011000, color);
        LedControl::updatePanel(); delay(150);
        LedControl::clearPanel();
        LedControl::setRow(2, B00111100, color); LedControl::setRow(3, B00100100, color); LedControl::setRow(4, B00100100, color); LedControl::setRow(5, B00111100, color);
        LedControl::updatePanel(); delay(150);
        LedControl::clearPanel();
        LedControl::setRow(1, B01111110, color); LedControl::setRow(6, B01111110, color); LedControl::setRow(2, B01000010, color); LedControl::setRow(5, B01000010, color);
        LedControl::updatePanel(); delay(150);
        LedControl::clearPanel();
        LedControl::setRow(0, B11111111, color); LedControl::setRow(7, B11111111, color); LedControl::setRow(1, B10000001, color); LedControl::setRow(6, B10000001, color);
        LedControl::updatePanel(); delay(150);
      }
      delay(200);
      LedControl::clearPanel();
    }
  }
  
  // Compress to center
  void compress(int repeats = 1, int type = 1) { // Fill=1, Ring=2
    CRGB color = LedControl::getCurrentColor();
    
    for (int i = 0; i < repeats; i++) {
      if (type == 1) {
        // Fill mode
        LedControl::allOn(color);
        delay(150);
        LedControl::setRow(0, B00000000); LedControl::setRow(7, B00000000);
        LedControl::updatePanel(); delay(150);
        LedControl::setRow(1, B00000000); LedControl::setRow(6, B00000000);
        LedControl::updatePanel(); delay(150);
        LedControl::setRow(2, B00000000); LedControl::setRow(5, B00000000);
        LedControl::updatePanel(); delay(150);
      } else {
        // Ring mode
        LedControl::setRow(0, B11111111, color); LedControl::setRow(7, B11111111, color); LedControl::setRow(1, B10000001, color); LedControl::setRow(6, B10000001, color);
        LedControl::updatePanel(); delay(150);
        LedControl::clearPanel();
        LedControl::setRow(1, B01111110, color); LedControl::setRow(6, B01111110, color); LedControl::setRow(2, B01000010, color); LedControl::setRow(5, B01000010, color);
        LedControl::updatePanel(); delay(150);
        LedControl::clearPanel();
        LedControl::setRow(2, B00111100, color); LedControl::setRow(3, B00100100, color); LedControl::setRow(4, B00100100, color); LedControl::setRow(5, B00111100, color);
        LedControl::updatePanel(); delay(150);
        LedControl::clearPanel();
        LedControl::setRow(3, B00011000, color); LedControl::setRow(4, B00011000, color);
        LedControl::updatePanel(); delay(150);
      }
       delay(200);
      LedControl::clearPanel();
    }
  }
  
  // Fade out and optionally back in
  void fadeOutIn(int type = 1) { // 1=Out/In, 2=Out only
    CRGB color = LedControl::getCurrentColor();
    LedControl::allOn(color);
    
    for (int i = panelState.brightness; i >= 0; i--) {
      FastLED.setBrightness(i);
      FastLED.show();
      delay(5);
    }
    
    if (type == 1) {
      delay(200);
      for (int i = 0; i <= panelState.brightness; i++) {
        FastLED.setBrightness(i);
        FastLED.show();
        delay(5);
      }
    }
    FastLED.setBrightness(panelState.brightness); // Restore brightness
  }

  // Flash Quadrants
  void flashQ(int repeats = 8) {
    CRGB color = LedControl::getCurrentColor();
    for(int i=0; i<repeats; i++) {
      LedControl::clearPanel();
      // TL & BR
      for(int y=0; y<4; y++) for(int x=0; x<4; x++) LedControl::setPixel(x, y, color);
      for(int y=4; y<8; y++) for(int x=4; x<8; x++) LedControl::setPixel(x, y, color);
      LedControl::updatePanel();
      delay(200);

      LedControl::clearPanel();
      // TR & BL
      for(int y=0; y<4; y++) for(int x=4; x<8; x++) LedControl::setPixel(x, y, color);
      for(int y=4; y<8; y++) for(int x=0; x<4; x++) LedControl::setPixel(x, y, color);
      LedControl::updatePanel();
      delay(200);
    }
  }

  // Rainbow cycle effect
  void rainbowCycle(int duration = 5000) {
    unsigned long startTime = millis();
    uint8_t hue = 0;
    while (millis() - startTime < duration) {
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(hue + (i * 4), 255, 255);
      }
      LedControl::updatePanel();
      hue += 2;
      delay(20);
    }
  }
  
  // Fire effect
  void fireEffect(int duration = 5000) {
    unsigned long startTime = millis();
    byte heat[NUM_LEDS];
    while (millis() - startTime < duration) {
      for( int i = 0; i < NUM_LEDS; i++) {
        heat[i] = qsub8( heat[i],  random8(0, ((55 * 10) / NUM_LEDS) + 2));
      }
      for( int k= NUM_LEDS - MATRIX_WIDTH; k < NUM_LEDS; k++) {
        heat[k] = qadd8( heat[k], random8(70, 130) );
      }
      for(int y=0; y<MATRIX_HEIGHT; y++){
        for(int x=0; x<MATRIX_WIDTH; x++){
          int index = LedControl::xyToIndex(x,y);
          if(y>0){
            int below = LedControl::xyToIndex(x, y-1);
            heat[below] = (heat[below] + heat[index]*2)/3;
          }
        }
      }
      for( int j = 0; j < NUM_LEDS; j++) { leds[j] = HeatColor(heat[j]); }
      LedControl::updatePanel();
      delay(30);
    }
  }
  
  // Twinkle effect
  void twinkle(int duration = 5000) {
    unsigned long startTime = millis();
    LedControl::clearPanel();
    while (millis() - startTime < duration) {
      if (random(100) < 20) {
        leds[random(NUM_LEDS)] = LedControl::getRandomColor();
      }
      for (int i = 0; i < NUM_LEDS; i++) { leds[i].fadeToBlackBy(15); }
      LedControl::updatePanel();
      delay(30);
    }
  }
  
  // Plasma effect
  void plasma(int duration = 5000) {
    unsigned long startTime = millis();
    int16_t time = 0;
    while (millis() - startTime < duration) {
      for (int x = 0; x < MATRIX_WIDTH; x++) {
        for (int y = 0; y < MATRIX_HEIGHT; y++) {
          int16_t h = (sin8(x*20 + time/2) + cos8(y*15 + time/2) + sin8((x+y)*15 + time/2));
          leds[LedControl::xyToIndex(x,y)] = CHSV(h,255,255);
        }
      }
      LedControl::updatePanel();
      time += 6;
      delay(20);
    }
  }

  void kaleidoscope(unsigned long duration) {
    unsigned long startTime = millis();
    float angle1 = 0, angle2 = 0, angle3 = 0;
    float speed1 = 0.05, speed2 = 0.08, speed3 = 0.03;
    while(millis() - startTime < duration) {
      angle1 += speed1; angle2 += speed2; angle3 += speed3;
      for (int y=0; y<4; y++) {
        for (int x=0; x<4; x++) {
          uint8_t hue = (sin(x*0.5+angle1) + cos(y*0.5+angle2) + sin((x+y)*0.5+angle3) + 4) * 32;
          CRGB color = CHSV(hue, 255, 255);
          LedControl::setPixel(x, y, color);
          LedControl::setPixel(7-x, y, color);
          LedControl::setPixel(x, 7-y, color);
          LedControl::setPixel(7-x, 7-y, color);
        }
      }
      LedControl::updatePanel();
      delay(20);
    }
  }

  void raindrops(unsigned long duration) {
    unsigned long startTime = millis();
    while(millis() - startTime < duration) {
      if(random(10) == 0) {
        leds[LedControl::xyToIndex(random(8), random(8))] = CHSV(random(140, 180), 200, 255);
      }
      for(int i=0; i<NUM_LEDS; i++) leds[i].fadeToBlackBy(10);
      LedControl::updatePanel();
      delay(30);
    }
  }

  void dripEffect(unsigned long duration) {
    unsigned long startTime = millis();
    uint8_t drips[8];
    for(int i=0; i<8; i++) drips[i] = 8;

    while(millis() - startTime < duration) {
      for(int i=0; i<8; i++) {
        if(drips[i] >= 8 && random(100) == 0) {
          drips[i] = 0;
        }
        if(drips[i] < 8) {
          LedControl::setPixel(i, drips[i], LedControl::getCurrentColor());
          drips[i]++;
        }
      }
      LedControl::updatePanel();
      for(int i=0; i<NUM_LEDS; i++) leds[i].fadeToBlackBy(30);
      delay(50);
    }
  }
}

// Main dispatcher function for this category
void runEffectPattern(int patternId) {
  int repeats = panelState.alwaysOn ? 1 : 4;
  unsigned long duration = panelState.alwaysOn ? 15000 : 8000;
  
  // Check for custom duration
  if (customPatternDuration > 0) {
    duration = customPatternDuration;
    // For repeat-based patterns, calculate repeats based on duration
    repeats = duration / 1600;  // Approximate time per repeat cycle
    if (repeats < 1) repeats = 1;
    customPatternDuration = 0;  // Reset after use
  }

  switch (patternId) {
    case 16: Effects::expand(repeats, 1); break;
    case 17: Effects::expand(repeats, 2); break;
    case 18: Effects::compress(repeats, 1); break;
    case 19: Effects::compress(repeats, 2); break;
    case 24: Effects::fadeOutIn(1); break;
    case 25: Effects::fadeOutIn(2); break;
    case 26: Basic::alert(repeats * 2); break;
    case 27: { 
        CRGB color = LedControl::getCurrentColor();
        for (int i = 0; i < repeats * 2; i++) {
          for (int c = 0; c < 4; c++) LedControl::setColumn(c, 0xFF, color);
          for (int c = 4; c < 8; c++) LedControl::setColumn(c, 0x00, color);
          FastLED.show(); delay(200);
          for (int c = 0; c < 4; c++) LedControl::setColumn(c, 0x00, color);
          for (int c = 4; c < 8; c++) LedControl::setColumn(c, 0xFF, color);
          FastLED.show(); delay(200);
        }
      }
      break;
    case 28: Effects::flashQ(repeats); break;
    case 57: Effects::rainbowCycle(duration); break;
    case 58: Effects::fireEffect(duration); break;
    case 59: Effects::twinkle(duration); break;
    case 60: Effects::plasma(duration); break;
    case 64: Effects::kaleidoscope(duration); break;
    case 65: Effects::raindrops(duration); break;
    case 66: Effects::dripEffect(duration); break;
  }
  if (!panelState.alwaysOn) LedControl::clearPanel();
}
#endif // PATTERNS_EFFECTS_H