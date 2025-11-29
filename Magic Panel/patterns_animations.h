/*
 * Animation Patterns
 * Complex animations like Cylon, loops, VU meters, Game of Life, and more.
 * VERSION 2.2 - Added support for custom duration
 */

#ifndef PATTERNS_ANIMATIONS_H
#define PATTERNS_ANIMATIONS_H

#include "led_control.h"
#include "patterns_shapes.h" // Notwendig für die Gaming-Sprites

// Forward declarations for global variables and functions
extern PanelState panelState;
extern bool patternActive;
extern unsigned long customPatternDuration;

namespace Animations {

  // --- Conway's Game of Life ---
  void gameOfLife(int generations = 50) {
    byte world[NUM_LEDS];
    byte next_world[NUM_LEDS];

    for(int i=0; i<NUM_LEDS; i++) {
      world[i] = random(0, 2);
    }

    CRGB color = LedControl::getCurrentColor();

    for(int gen=0; gen<generations; gen++) {
      for(int i=0; i<NUM_LEDS; i++) {
        uint8_t x, y;
        LedControl::indexToXY(i, x, y);

        int neighbors = 0;
        for(int dx=-1; dx<=1; dx++) {
          for(int dy=-1; dy<=1; dy++) {
            if(dx == 0 && dy == 0) continue;
            uint8_t nx = (x + dx + MATRIX_WIDTH) % MATRIX_WIDTH;
            uint8_t ny = (y + dy + MATRIX_HEIGHT) % MATRIX_HEIGHT;
            if(world[LedControl::xyToIndex(nx, ny)] == 1) {
              neighbors++;
            }
          }
        }

        if(world[i] == 1 && (neighbors < 2 || neighbors > 3)) { next_world[i] = 0; } 
        else if (world[i] == 0 && neighbors == 3) { next_world[i] = 1; }
        else { next_world[i] = world[i]; }
      }

      int liveCells = 0;
      for(int i=0; i<NUM_LEDS; i++) {
        world[i] = next_world[i];
        if(world[i] == 1) { leds[i] = color; liveCells++; } 
        else { leds[i] = CRGB::Black; }
      }
      LedControl::updatePanel();

      if(liveCells == 0) break;
      delay(200);
    }
  }

  // --- Matrix "Digital Rain" ---
  void matrixRain(unsigned long duration) {
    unsigned long startTime = millis();
    int head[8];
    for(int i=0; i<8; i++) { head[i] = -1; }

    while(millis() - startTime < duration) {
      for(int i=0; i<8; i++) {
        if(head[i] == -1 && random(20)==0) {
          head[i] = 0;
        }
        if(head[i] != -1) {
          LedControl::setPixel(i, head[i], CRGB::White);
          head[i]++;
          if(head[i] >= 8) head[i] = -1;
        }
      }
      LedControl::updatePanel();
      
      // KORRIGIERTE ZEILE
      fadeToBlackBy(leds, NUM_LEDS, 30);
      
      delay(60);
    }
  }
  
  // --- Rotierender 3D-Würfel ---
  namespace Cube3D {
    struct Point3D { float x, y, z; };
    struct Point2D { int x, y; };

    void drawLine(int x1, int y1, int x2, int y2, CRGB color) {
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;
        while (true) {
            LedControl::setPixel(x1, y1, color);
            if (x1 == x2 && y1 == y2) break;
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x1 += sx; }
            if (e2 < dx) { err += dx; y1 += sy; }
        }
    }
    
    void render(unsigned long duration) {
      Point3D v[8] = {
        {-1,-1,-1}, {1,-1,-1}, {1,1,-1}, {-1,1,-1},
        {-1,-1,1}, {1,-1,1}, {1,1,1}, {-1,1,1}
      };
      unsigned long startTime = millis();
      float angle = 0;
      while(millis() - startTime < duration) {
        angle += 0.05;
        Point2D p[8];
        for(int i=0; i<8; i++) {
          float rz = v[i].z * cos(angle*0.5) - v[i].x * sin(angle*0.5);
          float rx = v[i].z * sin(angle*0.5) + v[i].x * cos(angle*0.5);
          float ry = v[i].y;

          float rz2 = rz * cos(angle) - ry * sin(angle);
          float ry2 = rz * sin(angle) + ry * cos(angle);
          
          float perspective = 2.0 / (3.0 - rz2);
          p[i].x = (int)((rx * perspective * 3) + 3.5);
          p[i].y = (int)((ry2 * perspective * 3) + 3.5);
        }
        LedControl::clearPanel();
        CRGB color = LedControl::getCurrentColor();
        for(int i=0; i<4; i++) {
          drawLine(p[i].x, p[i].y, p[(i+1)%4].x, p[(i+1)%4].y, color);
          drawLine(p[i+4].x, p[i+4].y, p[((i+1)%4)+4].x, p[((i+1)%4)+4].y, color);
          drawLine(p[i].x, p[i].y, p[i+4].x, p[i+4].y, color);
        }
        LedControl::updatePanel();
        delay(30);
      }
    }
  }

  // --- Gaming-Animationen ---
  void pacmanAnimation(unsigned long duration) {
      unsigned long startTime = millis();
      while(millis() - startTime < duration) {
        for(int x=-7; x<9; x++) {
          LedControl::clearPanel();
          Shapes::drawSprite(x, 0, (millis()/200 % 2 == 0) ? Shapes::pacman_open : Shapes::pacman_closed, CRGB::Yellow);
          Shapes::drawSprite(x-5, 0, Shapes::ghost, CRGB::Red);
          LedControl::updatePanel();
          delay(120);
        }
      }
  }

  void invadersAnimation(unsigned long duration) {
      unsigned long startTime = millis();
      int inv_x = 1, inv_y=0, dir=1;
      while(millis() - startTime < duration) {
        LedControl::clearPanel();
        Shapes::drawSprite(inv_x, inv_y, Shapes::invader, LedControl::getCurrentColor());
        LedControl::updatePanel();
        inv_x += dir;
        if(inv_x > 2 || inv_x < 0) {
          dir *= -1;
          inv_y++;
          if(inv_y > 2) inv_y=0;
        }
        delay(300);
      }
  }

  // --- Bestehende Animationen ---
  void cylonCol(int repeats = 2, int scanDelay = 80) {
    CRGB color = LedControl::getCurrentColor();
    for (int i = 0; i < repeats; i++) {
      for (int col = 0; col < 8; col++) { LedControl::clearPanel(); LedControl::setColumn(col, 0xFF, color); LedControl::updatePanel(); delay(scanDelay); }
      for (int col = 6; col > 0; col--) { LedControl::clearPanel(); LedControl::setColumn(col, 0xFF, color); LedControl::updatePanel(); delay(scanDelay); }
    }
  }
  
  void cylonRow(int repeats = 2, int scanDelay = 80) {
    CRGB color = LedControl::getCurrentColor();
    for (int i = 0; i < repeats; i++) {
      for (int row = 0; row < 8; row++) { LedControl::clearPanel(); LedControl::setRow(row, 0xFF, color); LedControl::updatePanel(); delay(scanDelay); }
      for (int row = 6; row > 0; row--) { LedControl::clearPanel(); LedControl::setRow(row, 0xFF, color); LedControl::updatePanel(); delay(scanDelay); }
    }
  }
  
  void eyeScan(int repeats = 1, int scanDelay = 70) {
    CRGB color = LedControl::getCurrentColor();
    for (int i = 0; i < repeats; i++) {
      for (int row = 0; row < 8; row++) { LedControl::setRow(row, 0xFF, color); LedControl::updatePanel(); delay(scanDelay); LedControl::setRow(row, 0x00); }
      LedControl::clearPanel(); delay(scanDelay);
      for (int col = 7; col >= 0; col--) { LedControl::setColumn(col, 0xFF, color); LedControl::updatePanel(); delay(scanDelay); LedControl::setColumn(col, 0x00); }
      LedControl::clearPanel(); delay(scanDelay);
    }
  }
  
  void oneLoop(int repeats = 2) {
    CRGB color = LedControl::getCurrentColor();
    for (int j = 0; j < repeats; j++) {
      for (int i = 0; i < 28; i++) {
        LedControl::clearPanel();
        uint8_t x, y;
        if (i < 8) { x=i; y=0; }
        else if (i < 15) { x=7; y=i-7; }
        else if (i < 22) { x=7-(i-15); y=7;}
        else { x=0; y=7-(i-22); }
        LedControl::setPixel(x, y, color);
        LedControl::updatePanel();
        delay(50);
      }
    }
  }

  void twoLoop(int repeats = 2) {
    CRGB color = LedControl::getCurrentColor();
    for (int j = 0; j < repeats; j++) {
      for (int i = 0; i < 28; i++) {
        LedControl::clearPanel();
        uint8_t x1, y1, x2, y2;
        auto path = [&](uint8_t p, uint8_t& x, uint8_t& y){
            if (p < 8) { x=p; y=0; }
            else if (p < 15) { x=7; y=p-7; }
            else if (p < 22) { x=7-(p-15); y=7;}
            else { x=0; y=7-(p-22); }
        };
        path(i % 28, x1, y1);
        path((i + 14) % 28, x2, y2);
        LedControl::setPixel(x1, y1, color);
        LedControl::setPixel(x2, y2, color);
        LedControl::updatePanel();
        delay(50);
      }
    }
  }
  
  void vuMeter(int loops = 15, int type = 1) {
    CRGB color = LedControl::getCurrentColor();
    int level[8] = {0,0,0,0,0,0,0,0};
    for(int i=0; i<8; i++) level[i] = random(0, 9);
    
    for (int count = 0; count < loops; count++) {
      for (int i = 0; i < 8; i++) {
        uint8_t bar = (1 << level[i]) - 1;
        if(type == 3 || type == 4) bar = ~bar;
        if(type % 2 != 0) LedControl::setColumn(i, bar, color);
        else LedControl::setRow(i, bar, color);
      }
      LedControl::updatePanel();
      delay(150);
      for (int i = 0; i < 8; i++) level[i] = constrain(level[i] + random(-2, 3), 0, 8);
    }
  }
}

// Main dispatcher function for this category
void runAnimationPattern(int patternId) {
  unsigned long duration = panelState.alwaysOn ? 15000 : 8000;
  int repeats = panelState.alwaysOn ? 10 : 2;
  
  // Check for custom duration
  if (customPatternDuration > 0) {
    duration = customPatternDuration;
    // Calculate repeats based on duration for repetition-based patterns
    switch(patternId) {
      case 21: case 22: repeats = duration / 1200; break;  // Cylon patterns
      case 23: repeats = duration / 2000; break;           // Eye scan
      case 29: case 30: repeats = duration / 1400; break;  // Loop patterns
      case 52: case 53: case 54: case 55: repeats = duration / 150; break; // VU meters
      case 61: repeats = duration / 200; break;            // Game of Life generations
      default: break;
    }
    if (repeats < 1) repeats = 1;
    customPatternDuration = 0;  // Reset after use
  }

  switch (patternId) {
    case 21: Animations::cylonCol(repeats); break;
    case 22: Animations::cylonRow(repeats); break;
    case 23: Animations::eyeScan(repeats); break;
    case 29: Animations::twoLoop(repeats); break;
    case 30: Animations::oneLoop(repeats); break;
    case 52: Animations::vuMeter(repeats, 1); break;
    case 53: Animations::vuMeter(repeats, 2); break;
    case 54: Animations::vuMeter(repeats, 3); break;
    case 55: Animations::vuMeter(repeats, 4); break;
    case 61: Animations::gameOfLife(repeats); break;
    case 62: Animations::matrixRain(duration); break;
    case 63: Animations::Cube3D::render(duration); break;
    case 67: Animations::pacmanAnimation(duration); break;
    case 68: Animations::invadersAnimation(duration); break;
  }
  if (!panelState.alwaysOn) LedControl::clearPanel();
}

#endif // PATTERNS_ANIMATIONS_H