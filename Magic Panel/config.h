/*
 * Configuration file for Magic Panel ESP32-C3
 * All constants and settings in one place
 * VERSION 2.2 - Added duration field to Command struct
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <FastLED.h>

// Hardware configuration
#define NUM_LEDS 64
#define LED_PIN 6
#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8

// Default settings
#define DEFAULT_BRIGHTNESS 255
#define DEFAULT_SPEED 50
#define DEFAULT_I2C_ADDRESS 20
#define PATTERN_UPDATE_INTERVAL 10

// EEPROM settings
#define EEPROM_MAGIC 0x42
#define EEPROM_SIZE 512
#define EEPROM_TEXT_START_ADDR 12
#define TEXT_SLOTS 10
#define MAX_TEXT_LENGTH 48

// Serial communication
#define SERIAL_BAUD 115200
#define MAX_COMMAND_LENGTH 60

// Pattern IDs
enum PatternID {
  PATTERN_OFF = 0,
  PATTERN_ON_INDEF = 1,
  PATTERN_ON_2S = 2,
  PATTERN_ON_5S = 3,
  PATTERN_ON_10S = 4,
  PATTERN_TOGGLE = 5,
  PATTERN_ALERT_4S = 6,
  PATTERN_ALERT_10S = 7,
  PATTERN_TRACE_UP_FILL = 8,
  PATTERN_TRACE_UP_LINE = 9,
  PATTERN_TRACE_DOWN_FILL = 10,
  PATTERN_TRACE_DOWN_LINE = 11,
  PATTERN_TRACE_RIGHT_FILL = 12,
  PATTERN_TRACE_RIGHT_LINE = 13,
  PATTERN_TRACE_LEFT_FILL = 14,
  PATTERN_TRACE_LEFT_LINE = 15,
  PATTERN_EXPAND_FILL = 16,
  PATTERN_EXPAND_RING = 17,
  PATTERN_COMPRESS_FILL = 18,
  PATTERN_COMPRESS_RING = 19,
  PATTERN_CROSS = 20,
  PATTERN_CYLON_COL = 21,
  PATTERN_CYLON_ROW = 22,
  PATTERN_EYE_SCAN = 23,
  PATTERN_FADE_OUT_IN = 24,
  PATTERN_FADE_OUT = 25,
  PATTERN_FLASH_ALL = 26,
  PATTERN_FLASH_V = 27,
  PATTERN_FLASH_Q = 28,
  PATTERN_TWO_LOOP = 29,
  PATTERN_ONE_LOOP = 30,
  PATTERN_TEST_FILL = 31,
  PATTERN_TEST_PIXEL = 32,
  PATTERN_AI_LOGO = 33,
  PATTERN_2GWD_LOGO = 34,
  PATTERN_QUAD_TL_TR_BR_BL = 35,
  PATTERN_QUAD_TR_TL_BL_BR = 36,
  PATTERN_QUAD_TR_BR_BL_TL = 37,
  PATTERN_QUAD_TL_BL_BR_TR = 38,
  PATTERN_RANDOM_PIXEL = 39,
  PATTERN_COUNTDOWN_9 = 40,
  PATTERN_COUNTDOWN_3 = 41,
  PATTERN_ALERT_RANDOM_4S = 42,
  PATTERN_ALERT_RANDOM_8S = 43,
  PATTERN_SMILEY = 44,
  PATTERN_SAD = 45,
  PATTERN_HEART = 46,
  PATTERN_CHECKERBOARD = 47,
  PATTERN_COMPRESS_IN_FILL = 48,
  PATTERN_COMPRESS_IN_CLEAR = 49,
  PATTERN_EXPLODE_OUT_FILL = 50,
  PATTERN_EXPLODE_OUT_CLEAR = 51,
  PATTERN_VU_METER_COL_UP = 52,
  PATTERN_VU_METER_ROW_LEFT = 53,
  PATTERN_VU_METER_COL_DOWN = 54,
  PATTERN_VU_METER_ROW_RIGHT = 55,
  PATTERN_HEART_BEAT = 56,
  PATTERN_RAINBOW_CYCLE = 57,
  PATTERN_FIRE_EFFECT = 58,
  PATTERN_TWINKLE = 59,
  PATTERN_PLASMA = 60,
  PATTERN_GAME_OF_LIFE = 61,
  PATTERN_MATRIX_RAIN = 62,
  PATTERN_ROTATING_CUBE = 63,
  PATTERN_KALEIDOSCOPE = 64,
  PATTERN_RAINDROPS = 65,
  PATTERN_DRIP = 66,
  PATTERN_PACMAN = 67,
  PATTERN_INVADERS = 68,

  PATTERN_BOUNCING_TEXT = 80, 

  PATTERN_SCROLL_TEXT_EN = 97,
  PATTERN_SCROLL_TEXT_AU = 98,
  PATTERN_TEST_ALL = 99,
  PATTERN_SMART_DEMO = 100
};

// Command types
enum CommandType {
  CMD_NONE,
  CMD_PATTERN,
  CMD_COLOR,
  CMD_BRIGHTNESS,
  CMD_SPEED,
  CMD_MODE,
  CMD_SAVE,
  CMD_LOAD,
  CMD_ALL_ON,
  CMD_ALL_OFF,
  CMD_TEXT_SCROLL,
  CMD_TEXT_SAVE,
  CMD_TEXT_LOAD,
  CMD_TEXT_BOUNCE,
  CMD_SET_FONT,
  CMD_SET_START,
  CMD_HELP,
  CMD_STATUS,
  CMD_LIST_PATTERNS,
  CMD_DEMO,
  CMD_SET_TRANSITION,
  CMD_PLAYLIST_RUN,
  CMD_PLAYLIST_SAVE,
  CMD_PLAYLIST_LOAD
};

// Command structure
struct Command {
  CommandType type;
  int value;
  bool hasRGB;
  uint8_t r, g, b;
  String textData;
  int textSlot;
  unsigned long duration;  // NEW in v2.2 - duration in milliseconds (0 = use default)
};

// Panel state structure
struct PanelState {
  bool alwaysOn;
  uint8_t brightness;
  uint8_t animationSpeed;
  CRGB currentColor;
  bool rainbowMode;
  int lastPattern;
  uint8_t i2cAddress;
  int startPattern;
  bool useAurebesh;
  bool smoothTransitions;
};

// Predefined colors
const CRGB COLORS[] = {
  CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White, CRGB::Yellow,
  CRGB::Cyan, CRGB::Magenta, CRGB::Orange, CRGB::Purple
};

#endif // CONFIG_H