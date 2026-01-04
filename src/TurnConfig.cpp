/*
 * Config parsing from and to JSON
 */
#include "TurnConfig.h"

const int turn_capacity=\
  JSON_ARRAY_SIZE(TURN_MAX_PROGRAMS) + \
  TURN_MAX_PROGRAMS*JSON_ARRAY_SIZE(TURN_MAX_STAGES) + \
  TURN_MAX_PROGRAMS*JSON_OBJECT_SIZE(2) + \
  TURN_MAX_STAGES*TURN_MAX_PROGRAMS*JSON_OBJECT_SIZE(8) + \
  TURN_MAX_PROGRAMS*241;

void StageConfig::save(JsonObject obj) const {
  obj["beep"]=beep / NUM_MULT;
  obj["face"]=face / NUM_MULT;
  if (away)
    obj["away"]=away / NUM_MULT;
  if (repeat)
    obj["repeat"]=repeat;
  if (autonext)
    obj["autonext"]=autonext;
  if (nextaway)
    obj["nextaway"]=nextaway / NUM_MULT;
  if (flash)
    obj["flash"]=flash / NUM_MULT;
  if (flashaway)
    obj["flashaway"]=flashaway / NUM_MULT;
}

void StageConfig::load(JsonObjectConst obj) {
  beep=(obj["beep"] | 7.0) * NUM_MULT;
  beep=beep % 1000;
  face=(obj["face"] | 0.0) * NUM_MULT;
  face=face % 10000;
  away=(obj["away"] | 0.0) * NUM_MULT;
  away=away % 10000;
  repeat=obj["repeat"] | 1;
  if (repeat < 1) {
    repeat=1;
  }
  repeat=repeat % 100;
  autonext=obj["autonext"] | false;
  nextaway=(obj["nextaway"] | 0.0) * NUM_MULT;
  nextaway=nextaway % 10000;
  flash=(obj["flash"] | 0.0) * NUM_MULT;
  flash=flash % 10000;
  flashaway=(obj["flashaway"] | 0.0) * NUM_MULT;
  flashaway=flashaway % 10000;
}

void StageConfig::clearnext() {
  autonext=false;
  nextaway=0;
}

void ProgramConfig::save(JsonObject obj) const {
  obj["name"]=longname;

  // Add "stages" array
  JsonArray sts=obj.createNestedArray("stages");
  // Add each stage in the array
  for (uint8_t i=0; i<stages; i++)
    stage[i].save(sts.createNestedObject());
}

void ProgramConfig::load(JsonObjectConst obj) {
  strlcpy(longname, obj["name"] | "No name", sizeof(longname));

  // Get a reference to the stages array
  JsonArrayConst sts=obj["stages"];

  // Extract each stage
  stages=0;
  for (JsonObjectConst st : sts) {
    // Load the AP
    stage[stages].load(st);
    stages++;

    if (stages >= maxstages)
      break;
  }

  // The last stage should not have autonext or nextaway set
  // because there is nothing to move on to.
  stage[stages -1].clearnext();
}

void TurnConfig::load(JsonArrayConst obj) {
  // Extract each program.
  programs=0;
  
  for (JsonObjectConst prog : obj) {
    // Load program
    program[programs].load(prog);
    programs++;

    if (programs >= maxprograms)
      break;
  }
}

void TurnConfig::save(JsonArray obj) const {
  // Add each program to the array
  for (uint8_t i=0; i<programs; i++)
    program[i].save(obj.createNestedObject());
}

bool serializeTurnConfig(const TurnConfig &config, Print &dst) {
  DynamicJsonDocument doc(turn_capacity);

  // Create an array at the root
  JsonArray rootarr = doc.to<JsonArray>();
  // Fill the object
  config.save(rootarr);

  // Serialize JSON to file
  return serializeJsonPretty(doc, dst) > 0;
}

bool deserializeTurnConfig(Stream &src, TurnConfig &config) {
  DynamicJsonDocument doc(turn_capacity);

  // Parse the JSON object in the file
  DeserializationError err=deserializeJson(doc, src);
  if (err)
    return false;

  config.load(doc.as<JsonArray>());
  return true;
}
