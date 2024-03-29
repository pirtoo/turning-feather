/*
 * Config definition used for turning target
 * courses of fire and the stages within.
 */
 
#pragma once

#include <ArduinoJson.h>
#include "utils.h"

// maximum stages per program
#ifndef TURN_MAX_STAGES
#define TURN_MAX_STAGES 12
#endif // TURN_MAX_STAGES

// maximum number of programs
#ifndef TURN_MAX_PROGRAMS
#define TURN_MAX_PROGRAMS 55
#endif // TURN_MAX_PROGRAMS

#define TURN_NAME_LENGTH 20

#define NUM_MULT 10.0

#include <ArduinoJson.h>

// File format:
// Numbers in the file for times are in seconds, can be floats but
// will be cut off at a tenth of a second.
// Repeat minimum is 1.
// autonext (bool) and nextaway (time) not listed here yet.
//[
//  {
//    "name": "T&P1 (PP1)",
//    "stages": [
//      {"beep": 7, "flash": 0, "flashaway": 0, "face": 120, "away": 0, "repeat": 0},
//      {"beep": 7, "flash": 0, "flash": 0, "face": 2.1, "away": 5, "repeat": 6},
//      {"beep": 7, "flash": 0, "flash": 0, "face": 3, "away": 5, "repeat": 3}
//    ]
//  },
//  {
//     "name": "Granet",
//     "stages": [
//       {"beep": 6.5, "flash": 0, "flash": 0, "face": 3, "away": 7, "repeat": 5}
//     ]
//  },
//  {
//     "name": "Silhouettes",
//     "stages": [
//       {"beep": 7, "flash": 0, "flash": 0, "face": 12, "away": 0, "repeat": 0},
//       {"beep": 7, "flash": 0, "flash": 0, "face": 10, "away": 0, "repeat": 0},
//       {"beep": 7, "flash": 0, "flash": 0, "face": 8, "away": 0, "repeat": 0},
//       {"beep": 7, "flash": 0, "flash": 0, "face": 6, "away": 0, "repeat": 0}
//     ]
//   }
//]

// The config for a single stage
// Numbers are in tenths of a second
struct StageConfig {
  uint8_t beep=7;
  uint16_t face=5;
  uint16_t away=0;
  uint8_t repeat=0;
  bool autonext=false;
  uint16_t nextaway=0;
  uint16_t flash=0;
  uint16_t flashaway=0;

  void clearnext();
  void load(JsonObjectConst);
  void save(JsonObject) const;
};

// The config for a program/course of fire
struct ProgramConfig {
  static const uint8_t maxstages=TURN_MAX_STAGES;
  // text length plus null at the end
  char longname[TURN_NAME_LENGTH +1];
  //char longname[17];
  StageConfig stage[maxstages];
  uint8_t stages=0;

  void load(JsonObjectConst);
  void save(JsonObject) const;
};

// Stores the complete configuration
struct TurnConfig {
  static const uint8_t maxprograms=TURN_MAX_PROGRAMS;
  ProgramConfig program[maxprograms];
  uint8_t programs=0;

  void load(JsonArrayConst);
  void save(JsonArray) const;
};

bool serializeTurnConfig(const TurnConfig &config, Print &dst);
bool deserializeTurnConfig(Stream &src, TurnConfig &config);
