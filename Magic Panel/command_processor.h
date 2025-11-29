/*
 * Command Processor
 * Handles both JawaLite and simple serial commands
 * VERSION 2.2 - Added T<pattern>:<seconds> syntax for custom duration
 */

#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include "config.h"

class CommandProcessor {
public:
  Command parseCommand(String input) {
    Command cmd;
    cmd.type = CMD_NONE;
    cmd.hasRGB = false;
    cmd.textData = "";
    cmd.textSlot = 0;
    cmd.duration = 0;  // 0 means use default duration
    
    input.trim();
    if (input.length() == 0) return cmd;
    
    if (input == "HELP" || input == "?") { cmd.type = CMD_HELP; return cmd; }
    if (input.startsWith("HELP ")) { cmd.type = CMD_HELP; cmd.textData = input.substring(5); return cmd; }
    if (input == "STATUS") { cmd.type = CMD_STATUS; return cmd; }
    if (input == "LIST") { cmd.type = CMD_LIST_PATTERNS; return cmd; }
    if (input == "SAVE") { cmd.type = CMD_SAVE; return cmd; }
    if (input == "LOAD") { cmd.type = CMD_LOAD; return cmd; }
    if (input == "ON") { cmd.type = CMD_ALL_ON; return cmd; }
    if (input == "OFF") { cmd.type = CMD_ALL_OFF; return cmd; }
    if (input == "DEMO") { cmd.type = CMD_DEMO; return cmd; }

    if (input.startsWith("TEXT_BOUNCE:")) { cmd.type = CMD_TEXT_BOUNCE; cmd.textData = input.substring(12); return cmd; }
    if (input.startsWith("TEXT:")) { cmd.type = CMD_TEXT_SCROLL; cmd.textData = input.substring(5); return cmd; }
    if (input.startsWith("TEXTSAVE")) {
      cmd.type = CMD_TEXT_SAVE;
      int colonPos = input.indexOf(':');
      if (colonPos > 8) { cmd.textSlot = input.substring(8, colonPos).toInt(); cmd.textData = input.substring(colonPos + 1); }
      return cmd;
    }
    if (input.startsWith("TEXTLOAD")) { cmd.type = CMD_TEXT_LOAD; cmd.textSlot = input.substring(8).toInt(); return cmd; }
    if (input.startsWith("FONT")) { cmd.type = CMD_SET_FONT; cmd.value = input.substring(4).toInt(); return cmd; }
    if (input.startsWith("START")) { cmd.type = CMD_SET_START; cmd.value = input.substring(5).toInt(); return cmd; }
    if (input.startsWith("TRANSITION")) { cmd.type = CMD_SET_TRANSITION; cmd.value = input.substring(10).toInt(); return cmd; }
    if (input.startsWith("PLAYLIST_RUN:")) { cmd.type = CMD_PLAYLIST_RUN; cmd.textData = input.substring(13); return cmd; }
    
    char firstChar = input.charAt(0);
    String value = input.substring(1);
    
    switch (firstChar) {
      case 'T': 
      case 'S': {
        cmd.type = CMD_PATTERN;
        // Check for duration syntax: T21:30 (pattern 21 for 30 seconds)
        int colonPos = value.indexOf(':');
        if (colonPos > 0) {
          cmd.value = value.substring(0, colonPos).toInt();
          cmd.duration = value.substring(colonPos + 1).toInt() * 1000;  // Convert seconds to milliseconds
        } else {
          cmd.value = value.toInt();
          cmd.duration = 0;  // Use default duration
        }
        break;
      }
      case 'A': cmd.type = CMD_ALL_ON; break;
      case 'D': cmd.type = CMD_ALL_OFF; break;
      case 'P': cmd.type = CMD_MODE; cmd.value = value.toInt(); break;
      case 'C': cmd.type = CMD_COLOR; if (value.indexOf(',') > 0) { parseRGB(value, cmd); } else { cmd.value = value.toInt(); } break;
      case 'B': cmd.type = CMD_BRIGHTNESS; cmd.value = value.toInt(); break;
      case 'V': cmd.type = CMD_SPEED; cmd.value = value.toInt(); break;
    }
    
    if (cmd.type == CMD_NONE && input.startsWith("SP")) { cmd.type = CMD_SPEED; cmd.value = input.substring(2).toInt(); }
    
    return cmd;
  }
  
private:
  void parseRGB(String value, Command &cmd) {
    int comma1 = value.indexOf(',');
    int comma2 = value.indexOf(',', comma1 + 1);
    if (comma1 > 0 && comma2 > comma1) {
      cmd.r = value.substring(0, comma1).toInt();
      cmd.g = value.substring(comma1 + 1, comma2).toInt();
      cmd.b = value.substring(comma2 + 1).toInt();
      cmd.hasRGB = true;
    }
  }
};

#endif // COMMAND_PROCESSOR_H