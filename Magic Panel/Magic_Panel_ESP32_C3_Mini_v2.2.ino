/*
 * Magic Panel ESP32-C3
 * by Printed-Droid.com
 * VERSION 2.2
 *
 * CHANGES IN V2.2:
 * - Added T<pattern>:<seconds> syntax for custom duration (e.g., T57:30 for 30 seconds)
 * - Duration applies only to the specific pattern execution
 * - Works with all time-based and animation patterns
 *
 * CHANGES IN V2.1:
 * - Fixed serial command processing for external controllers via RX/TX pins
 *
 * COMMAND REFERENCE:
 * Pattern Control:
 *   T<n> or S<n>         - Run pattern number n (0-100)
 *   T<n>:<seconds>       - Run pattern n for specified seconds (e.g., T57:30)
 *   DEMO                 - Run curated demo show
 *   
 * Display Control:
 *   A or ON              - All LEDs on
 *   D or OFF             - All LEDs off
 *   C<n>                 - Set color (0-8, 9=rainbow)
 *   C<r>,<g>,<b>         - Set RGB color directly
 *   B<n>                 - Set brightness (0-255)
 *   V<n> or SP<n>        - Set animation speed (1-100)
 *   P<n>                 - Set mode (0=timed, 1=always on)
 *   
 * Text Commands:
 *   TEXT:<string>        - Scroll text
 *   TEXT_BOUNCE:<string> - Bouncing text animation
 *   TEXTSAVE<n>:<string> - Save text to slot n (0-9)
 *   TEXTLOAD<n>          - Load and scroll text from slot n
 *   FONT<n>              - Set font (0=standard, 1=aurebesh)
 *   
 * System Commands:
 *   SAVE                 - Save settings to EEPROM
 *   LOAD                 - Load settings from EEPROM
 *   STATUS               - Show current settings
 *   LIST                 - List all patterns
 *   HELP                 - Show help
 *   START<n>             - Set startup pattern
 *   TRANSITION<n>        - Enable/disable smooth transitions (0/1)
 *
 * BOARD & PINS:
 * - Board: Lolin C3 Mini
 * - LED Pin: GPIO 6
 * - I2C Address: 20 (configurable)
 */

#include <FastLED.h>
#include <Wire.h>
#include <EEPROM.h>

// Include all configuration and library headers
#include "config.h"
#include "led_control.h"
#include "command_processor.h"
#include "patterns_basic.h"
#include "patterns_trace.h"
#include "patterns_effects.h"
#include "patterns_shapes.h"
#include "patterns_animations.h"
#include "patterns_text.h"

// Global variables
CRGB leds[NUM_LEDS];
PanelState panelState;
CommandProcessor cmdProcessor;

// Timing variables
unsigned long lastUpdate = 0;
unsigned long patternEndTime = 0;
bool patternActive = false;

// Duration control
unsigned long customPatternDuration = 0;  // 0 = use default duration

// Forward declarations
void runStartupAnimation();
void runPattern(int patternId, unsigned long duration = 0);
void runAllPatterns();
void runSmartDemo();
String getPatternName(int patternId);
void showHelp(String topic);
void listPatterns();
void showStatus();
void saveSettings();
void loadSettings();
void confirmCommand();
void processSerialCommand();
void executeCommand(Command cmd);
void setPatternEndTime(unsigned long duration);

void setup() {
  Serial.begin(SERIAL_BAUD);
  while (!Serial) { ; }

  // Initialize the second serial port (Hardware UART)
  // Baudrate: 115200 (as defined in config.h)
  // RX Pin: GPIO 20
  // TX Pin: GPIO 21
  Serial1.begin(SERIAL_BAUD, SERIAL_8N1, 20, 21);

  Serial.println(F("\n========================================"));
  Serial.println(F("  Magic Panel ESP32-C3 v2.2"));
  Serial.println(F("========================================"));
  Serial.println(F("Type 'HELP' for commands or 'DEMO' for a show."));
  
  EEPROM.begin(EEPROM_SIZE);
  loadSettings();
  
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(panelState.brightness);
  
  Wire.begin(panelState.i2cAddress);
  Wire.onReceive(receiveEvent);
  
  LedControl::clearPanel();
  runStartupAnimation();
}

void loop() {
  unsigned long currentTime = millis();
  if (Serial.available()) { processSerialCommand(); }
// Listen for commands from the external controller via RX/TX pins
  if (Serial1.available()) {
// Read the incoming command as a String until a newline is received
    String commandFromController = Serial1.readStringUntil('\n');
    commandFromController.trim(); // Remove leading/trailing whitespace

    if (commandFromController.length() > 0) {
      // Convert the command to upper case
      commandFromController.toUpperCase();
      
      // Use the existing parser and execution logic
      Command cmd = cmdProcessor.parseCommand(commandFromController); // 
      executeCommand(cmd); // 
    }
  }
  if (currentTime - lastUpdate > PATTERN_UPDATE_INTERVAL) {
    lastUpdate = currentTime;
    if (patternActive && currentTime >= patternEndTime) {
      if(panelState.smoothTransitions) LedControl::fadeOutEffect();
      else LedControl::clearPanel();
      patternActive = false;
    }
    if (panelState.alwaysOn && panelState.lastPattern > 4 && !patternActive && panelState.lastPattern != PATTERN_SMART_DEMO) {
      runPattern(panelState.lastPattern);
    }
  }
}

void runPattern(int patternId, unsigned long duration) {
  // Set custom duration for this pattern execution
  customPatternDuration = duration;
  
  if (panelState.lastPattern != patternId && panelState.smoothTransitions) {
    LedControl::fadeOutEffect(2);
  } else {
    LedControl::clearPanel();
  }
  if (patternId == panelState.lastPattern && !panelState.alwaysOn && patternActive) { return; }

  if (patternId == PATTERN_SMART_DEMO) {
      runSmartDemo();
      return; 
  }
  
  patternActive = false;
  panelState.lastPattern = patternId;

  switch (patternId) {
    case 0 ... 7: runBasicPattern(patternId); break;
    case 8 ... 15: runTracePattern(patternId); break;
    case 48 ... 51: runTracePattern(patternId); break; 
    case 16 ... 19: case 24 ... 28: case 57 ... 60: case 64 ... 66: 
      runEffectPattern(patternId); break;
    case 20: case 31 ... 47: case 56:
      runShapePattern(patternId); break;
    case 21 ... 23: case 29 ... 30: case 52 ... 55: case 61 ... 63: case 67 ... 68: 
      runAnimationPattern(patternId); break;
    case 80: case 97 ... 98: 
      runTextPattern(patternId); break;
    case 99: runAllPatterns(); break;
  }
}

void processSerialCommand() {
  static String inputBuffer = "";
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\r' || c == '\n') {
      if (inputBuffer.length() > 0) {
        inputBuffer.toUpperCase();
        Command cmd = cmdProcessor.parseCommand(inputBuffer);
        executeCommand(cmd);
        inputBuffer = "";
      }
    } else {
      if (inputBuffer.length() < MAX_COMMAND_LENGTH) {
        inputBuffer += c;
      }
    }
  }
}

void executeCommand(Command cmd) {
  if (cmd.type == CMD_NONE) {
    Serial.println(F("Unknown command. Type HELP for a list of commands."));
    return;
  }
  switch (cmd.type) {
    case CMD_PATTERN: 
      if (cmd.duration > 0) {
        Serial.print(F("Running pattern ")); 
        Serial.print(cmd.value);
        Serial.print(F(" for "));
        Serial.print(cmd.duration / 1000);
        Serial.println(F(" seconds"));
      }
      runPattern(cmd.value, cmd.duration); 
      break;
    case CMD_DEMO: runSmartDemo(); break;
    case CMD_COLOR:
      if (cmd.hasRGB) {
        panelState.currentColor = CRGB(cmd.r, cmd.g, cmd.b);
        panelState.rainbowMode = false;
      } else {
        panelState.rainbowMode = (cmd.value == 9);
        if (!panelState.rainbowMode && cmd.value >= 0 && cmd.value < (sizeof(COLORS)/sizeof(CRGB))) {
          panelState.currentColor = COLORS[cmd.value];
        }
      }
      break;
    case CMD_BRIGHTNESS:
      panelState.brightness = constrain(cmd.value, 0, 255);
      FastLED.setBrightness(panelState.brightness);
      break;
    case CMD_SPEED: panelState.animationSpeed = constrain(cmd.value, 1, 100); break;
    case CMD_MODE: panelState.alwaysOn = (cmd.value == 1); break;
    case CMD_SAVE: saveSettings(); break;
    case CMD_LOAD: loadSettings(); break;
    case CMD_ALL_ON: LedControl::allOn(LedControl::getCurrentColor()); break;
    case CMD_ALL_OFF: LedControl::clearPanel(); break;
    case CMD_TEXT_SCROLL: Text::clearBuffer(); Text::scrollCustomText(cmd.textData, panelState.useAurebesh ? 2 : 1); LedControl::clearPanel(); break;
    case CMD_TEXT_BOUNCE: Text::bouncingText(cmd.textData, panelState.useAurebesh ? 2:1); break;
    case CMD_TEXT_SAVE: saveTextToSlot(cmd.textSlot, cmd.textData); break;
    case CMD_TEXT_LOAD: {
        String loadedText = loadTextFromSlot(cmd.textSlot);
        if (loadedText.length() > 0) {
          Text::clearBuffer(); Text::scrollCustomText(loadedText, panelState.useAurebesh ? 2 : 1); LedControl::clearPanel();
        } else { Serial.print(F("No text in slot ")); Serial.println(cmd.textSlot); }
      }
      break;
    case CMD_SET_FONT: panelState.useAurebesh = (cmd.value == 1); Serial.print(F("Font set to: ")); Serial.println(panelState.useAurebesh ? "Aurebesh" : "Standard"); break;
    case CMD_SET_START: panelState.startPattern = constrain(cmd.value, 0, 100); Serial.print(F("Startup pattern set to: ")); Serial.println(panelState.startPattern); break;
    case CMD_HELP: showHelp(cmd.textData); break;
    case CMD_STATUS: showStatus(); break;
    case CMD_LIST_PATTERNS: listPatterns(); break;
    case CMD_SET_TRANSITION:
      panelState.smoothTransitions = (cmd.value == 1);
      Serial.print(F("Smooth Transitions set to: "));
      Serial.println(panelState.smoothTransitions ? "ON" : "OFF");
      break;
    case CMD_PLAYLIST_RUN: {
        Serial.println(F("Running playlist..."));
        String playlist = cmd.textData; int commaPos;
        do {
          commaPos = playlist.indexOf(',');
          String patternStr = (commaPos == -1) ? playlist : playlist.substring(0, commaPos);
          if (commaPos != -1) playlist.remove(0, commaPos + 1);
          if (patternStr.length() > 0) {
            int patternId = patternStr.toInt();
            Serial.print(F(" > Running pattern: ")); Serial.println(patternId); runPattern(patternId);
          }
        } while (commaPos != -1);
        Serial.println(F("Playlist finished."));
      }
      break;
    case CMD_PLAYLIST_SAVE: Serial.println(F("Playlist SAVE is not implemented due to EEPROM limits.")); break;
    case CMD_PLAYLIST_LOAD: Serial.println(F("Playlist LOAD is not implemented due to EEPROM limits.")); break;
  }
}

void receiveEvent(int numBytes) {
  String i2cBuffer = "";
  while (Wire.available()) { i2cBuffer += (char)Wire.read(); }
  i2cBuffer.toUpperCase();
  executeCommand(cmdProcessor.parseCommand(i2cBuffer));
}

void saveSettings() {
  int addr = 0;
  EEPROM.write(addr++, EEPROM_MAGIC);
  EEPROM.write(addr++, panelState.brightness);
  EEPROM.write(addr++, panelState.animationSpeed);
  EEPROM.write(addr++, panelState.alwaysOn ? 1 : 0);
  EEPROM.write(addr++, panelState.currentColor.r);
  EEPROM.write(addr++, panelState.currentColor.g);
  EEPROM.write(addr++, panelState.currentColor.b);
  EEPROM.write(addr++, panelState.i2cAddress);
  EEPROM.write(addr++, panelState.startPattern);
  EEPROM.write(addr++, panelState.useAurebesh ? 1 : 0);
  EEPROM.write(addr++, panelState.smoothTransitions ? 1 : 0);
  EEPROM.commit();
  Serial.println(F("Settings saved to EEPROM"));
}

void loadSettings() {
  int addr = 0;
  if (EEPROM.read(addr++) == EEPROM_MAGIC) {
    panelState.brightness = EEPROM.read(addr++);
    panelState.animationSpeed = EEPROM.read(addr++);
    panelState.alwaysOn = EEPROM.read(addr++) == 1;
    panelState.currentColor.r = EEPROM.read(addr++);
    panelState.currentColor.g = EEPROM.read(addr++);
    panelState.currentColor.b = EEPROM.read(addr++);
    panelState.i2cAddress = EEPROM.read(addr++);
    panelState.startPattern = EEPROM.read(addr++);
    panelState.useAurebesh = EEPROM.read(addr++) == 1;
    panelState.smoothTransitions = EEPROM.read(addr++) == 1;
    FastLED.setBrightness(panelState.brightness);
    Serial.println(F("Settings loaded from EEPROM"));
  } else {
    panelState.brightness = DEFAULT_BRIGHTNESS;
    panelState.animationSpeed = DEFAULT_SPEED;
    panelState.alwaysOn = true;
    panelState.currentColor = CRGB::Red;
    panelState.i2cAddress = DEFAULT_I2C_ADDRESS;
    panelState.startPattern = 0;
    panelState.useAurebesh = false;
    panelState.smoothTransitions = false;
    Serial.println(F("Using default settings"));
  }
}

void saveTextToSlot(int slot, String text) {
  if (slot < 0 || slot >= TEXT_SLOTS) return;
  int baseAddr = EEPROM_TEXT_START_ADDR + (slot * (MAX_TEXT_LENGTH + 1));
  EEPROM.write(baseAddr, text.length());
  for (unsigned int i = 0; i < text.length() && i < MAX_TEXT_LENGTH; i++) {
    EEPROM.write(baseAddr + 1 + i, text.charAt(i));
  }
  EEPROM.commit();
  Serial.print(F("Text saved to slot ")); Serial.println(slot);
}

String loadTextFromSlot(int slot) {
  if (slot < 0 || slot >= TEXT_SLOTS) return "";
  int baseAddr = EEPROM_TEXT_START_ADDR + (slot * (MAX_TEXT_LENGTH + 1));
  int textLen = EEPROM.read(baseAddr);
  if (textLen == 0 || textLen > MAX_TEXT_LENGTH) return "";
  String text = "";
  for (int i = 0; i < textLen; i++) {
    text += (char)EEPROM.read(baseAddr + 1 + i);
  }
  return text;
}

void confirmCommand() { }

void runStartupAnimation() {
  LedControl::allOn(CRGB::Red); delay(300);
  LedControl::allOn(CRGB::Green); delay(300);
  LedControl::allOn(CRGB::Blue); delay(300);
  LedControl::clearPanel();
  Shapes::showDigit(2); delay(500);
  Shapes::showDigit(2); delay(500); // Version 2.2
  LedControl::clearPanel();
  if (panelState.startPattern >= 0) {
    delay(250); runPattern(panelState.startPattern);
  }
}

void setPatternEndTime(unsigned long duration) {
  // If custom duration is set, use it instead
  if (customPatternDuration > 0) {
    patternEndTime = millis() + customPatternDuration;
    // Reset custom duration after use
    customPatternDuration = 0;
  } else {
    patternEndTime = millis() + duration;
  }
  patternActive = true;
}

String getPatternName(int patternId) {
    switch (patternId) {
        case 0: return F("Off");
        case 1: return F("On Indefinite");
        case 2: return F("On 2s");
        case 3: return F("On 5s");
        case 4: return F("On 10s");
        case 5: return F("Toggle");
        case 6: return F("Alert 4s");
        case 7: return F("Alert 10s");
        case 8: return F("Trace Up Fill");
        case 9: return F("Trace Up Line");
        case 10: return F("Trace Down Fill");
        case 11: return F("Trace Down Line");
        case 12: return F("Trace Right Fill");
        case 13: return F("Trace Right Line");
        case 14: return F("Trace Left Fill");
        case 15: return F("Trace Left Line");
        case 16: return F("Expand Fill");
        case 17: return F("Expand Ring");
        case 18: return F("Compress Fill");
        case 19: return F("Compress Ring");
        case 20: return F("Cross");
        case 21: return F("Cylon Column");
        case 22: return F("Cylon Row");
        case 23: return F("Eye Scan");
        case 24: return F("Fade Out/In");
        case 25: return F("Fade Out");
        case 26: return F("Flash All");
        case 27: return F("Flash Vertical");
        case 28: return F("Flash Quadrants");
        case 29: return F("Two Loop");
        case 30: return F("One Loop");
        case 31: return F("Test Fill");
        case 32: return F("Test Pixel");
        case 33: return F("AI Logo");
        case 34: return F("2GWD Logo");
        case 35: return F("Quadrant TL->TR->BR->BL");
        case 36: return F("Quadrant TR->TL->BL->BR");
        case 37: return F("Quadrant TR->BR->BL->TL");
        case 38: return F("Quadrant TL->BL->BR->TR");
        case 39: return F("Random Pixel");
        case 40: return F("Countdown 9-0");
        case 41: return F("Countdown 3-0");
        case 42: return F("Alert Random 4s");
        case 43: return F("Alert Random 8s");
        case 44: return F("Smiley Face");
        case 45: return F("Sad Face");
        case 46: return F("Heart");
        case 47: return F("Checkerboard");
        case 48: return F("Compress In Fill");
        case 49: return F("Compress In Clear");
        case 50: return F("Explode Out Fill");
        case 51: return F("Explode Out Clear");
        case 52: return F("VU Meter Columns Up");
        case 53: return F("VU Meter Rows Left");
        case 54: return F("VU Meter Columns Down");
        case 55: return F("VU Meter Rows Right");
        case 56: return F("Animated Heart");
        case 57: return F("Rainbow Cycle");
        case 58: return F("Fire Effect");
        case 59: return F("Twinkle");
        case 60: return F("Plasma");
        case 61: return F("Game of Life");
        case 62: return F("Matrix Rain");
        case 63: return F("3D Cube");
        case 64: return F("Kaleidoscope");
        case 65: return F("Raindrops");
        case 66: return F("Drip Effect");
        case 67: return F("Pac-Man");
        case 68: return F("Space Invaders");
        case 80: return F("Bouncing Text");
        case 97: return F("Scroll Text (EN)");
        case 98: return F("Scroll Text (AU)");
        case 99: return F("Test All Patterns");
        case 100: return F("Smart Demo");
        default: return F("Unknown Pattern");
    }
}

void runAllPatterns() {
  Serial.println(F("Running all patterns (full test)..."));
  for (int i = 0; i <= 68; i++) {
    Serial.print(F("Pattern ")); Serial.print(i); Serial.print(F(" - ")); Serial.println(getPatternName(i));
    runPattern(i);
    delay(2000);
  }
  Serial.print(F("Pattern 80 - ")); Serial.println(getPatternName(80));
  runPattern(80); delay(2000);
  Serial.println(F("Test complete"));
}

void runSmartDemo() {
    patternActive = false; 
    Serial.println(F("Starting Smart Demo..."));
    
    // 1. Text Scroll in Aurebesh (Red)
    panelState.currentColor = CRGB::Red;
    panelState.rainbowMode = false;
    FastLED.setBrightness(panelState.brightness);
    Text::scrollCustomText("PRINTED-DROID", 2);
    delay(500);

    // 2. Text Scroll in English (Rainbow)
    Text::scrollRainbowText("PRINTED-DROID", 1);
    delay(500);

    // 3. Curated list of patterns
    const int patterns_to_show[] = {
        1, 5, 6, 8, 9, 12, 13, 16, 17, 20, 21, 23, 24, 28, 29, 30, 33, 
        35, 40, 44, 46, 47, 52, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 80
    };
    int num_patterns = sizeof(patterns_to_show) / sizeof(patterns_to_show[0]);
    
    Serial.println(F("Now showing curated patterns..."));
    for (int i = 0; i < num_patterns; i++) {
        int patternId = patterns_to_show[i];
        Serial.print(F("Showing Pattern: ")); Serial.print(patternId); Serial.print(F(" - ")); Serial.println(getPatternName(patternId));
        runPattern(patternId);
        delay(3500);
    }
    runPattern(0);
    Serial.println(F("Smart Demo finished."));
}

void showHelp(String topic) {
  topic.toUpperCase();
  if (topic == "" || topic == "COMMANDS") {
    Serial.println(F("\n=== Magic Panel Help Menu ==="));
    Serial.println(F("Type 'HELP FULL' for complete documentation."));
    Serial.println(F("Type 'LIST' for a detailed list of all patterns."));
    Serial.println(F("Type 'DEMO' for a curated show of patterns."));
    Serial.println(F("Type 'STATUS' to see current settings."));
    Serial.println(F("\n--- Quick Command Reference ---"));
    Serial.println(F("Pattern: T<n> or T<n>:<seconds>"));
    Serial.println(F("Example: T57:30 (Rainbow for 30 seconds)"));
    Serial.println(F("Color: C<0-9> or C<r>,<g>,<b>"));
    Serial.println(F("Brightness: B<0-255>"));
    Serial.println(F("Speed: V<1-100>"));
    return;
  }

  if (topic == "FULL") {
    Serial.println(F("\n=== Documentation: Magic Panel for ESP32-C3 Mini (v2.2) ==="));
    Serial.println(F("\n-- FAQ --"));
    Serial.println(F("Q1: Nothing on Serial Monitor?"));
    Serial.println(F("A: Check baud rate is 115200. Add 'while(!Serial);' after Serial.begin() for debugging."));
    Serial.println(F("Q2: Patterns look wrong/mirrored?"));
    Serial.println(F("A: Code expects a 'Progressive/Scanline' layout. Check your matrix wiring and 'xyToIndex' function."));
    Serial.println(F("Q3: How to change defaults permanently?"));
    Serial.println(F("A: Set values with commands (e.g., B 150), then use the 'SAVE' command."));
  }
}

void listPatterns() {
  Serial.println(F("\n--- Complete Pattern List ---"));
  for(int i=0; i<=100; i++) {
    if( (i >= 0 && i <= 68) || (i == 80) || (i >= 97 && i <= 100) ) {
      Serial.print(i);
      Serial.print(F("\t - "));
      Serial.println(getPatternName(i));
    }
  }
}

void showStatus() {
  Serial.println(F("\n=== CURRENT STATUS ==="));
  Serial.print(F("Brightness: ")); Serial.print(panelState.brightness); Serial.print(F(" (")); Serial.print((panelState.brightness * 100) / 255); Serial.println(F("%)"));
  Serial.print(F("Color: "));
  if (panelState.rainbowMode) { Serial.println(F("Rainbow Mode")); } 
  else { Serial.print(F("RGB(")); Serial.print(panelState.currentColor.r); Serial.print(F(",")); Serial.print(panelState.currentColor.g); Serial.print(F(",")); Serial.print(panelState.currentColor.b); Serial.println(F(")")); }
  Serial.print(F("Mode: ")); Serial.println(panelState.alwaysOn ? F("Always On") : F("Timed"));
  Serial.print(F("Speed: ")); Serial.println(panelState.animationSpeed);
  Serial.print(F("Font: ")); Serial.println(panelState.useAurebesh ? "Aurebesh" : "Standard");
  Serial.print(F("Startup Pattern: ")); Serial.print(panelState.startPattern); Serial.print(F(" (")); Serial.print(getPatternName(panelState.startPattern)); Serial.println(F(")"));
  Serial.print(F("I2C Address: ")); Serial.println(panelState.i2cAddress);
  Serial.print(F("Smooth Transitions: ")); Serial.println(panelState.smoothTransitions ? "ON" : "OFF");
  Serial.print(F("Last Pattern: ")); Serial.println(panelState.lastPattern);
  Serial.println(F("\nStored Texts:"));
  for (int i = 0; i < TEXT_SLOTS; i++) {
    String text = loadTextFromSlot(i);
    if (text.length() > 0) { Serial.print(F("  Slot ")); Serial.print(i); Serial.print(F(": \"")); Serial.print(text); Serial.println(F("\"")); }
  }
}