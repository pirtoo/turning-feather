/*
 * Config parsing from and to JSON
 */
#include "TurnConfig.h"
// Needed for fudge times
#include "main.h"

#ifndef FACE_FUDGE
#define FACE_FUDGE 0
#endif

#ifndef AWAY_FUDGE
#define AWAY_FUDGE 0
#endif

#ifndef INIT_PAUSE
#define INIT_PAUSE 0
#endif

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

  // Calculate total length of the stage
  totallen=0;
  if (beep > 0)
    totallen+=beep + INIT_PAUSE;
  if (face > 0)
    totallen+=face;
  if (repeat > 1) {
    if (face > 0)
      totallen+=(face + FACE_FUDGE) * (repeat -1);
    if (away > 0)
      totallen+=(away + AWAY_FUDGE) * (repeat -1);
  }
  if (flash > 0)
    totallen+=flash + FACE_FUDGE;
  if (flashaway > 0)
    totallen+=flashaway + AWAY_FUDGE;
  if (autonext && nextaway > 0)
    totallen+=nextaway + AWAY_FUDGE;
}

void StageConfig::clearnext() {
  autonext=false;
  nextaway=0;
}

bool serializeStageConfig(const StageConfig *config, char *dst, const size_t dst_len) {
  JsonDocument doc;
  bool result;
  JsonObject rootobj=doc.to<JsonObject>();
  config->save(rootobj);
  result=serializeJson(doc, dst, dst_len);
  if (! result)
    Serial.println("Failed to serialise status json");
  return result;
}

void ProgramConfig::save(JsonObject obj) const {
  obj["name"]=longname;

  // Add "stages" array
  JsonArray sts=obj["stages"].to<JsonArray>();
  // Add each stage in the array
  for (uint8_t i=0; i<stages; i++)
    stage[i].save(sts.add<JsonObject>());
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
    program[i].save(obj.add<JsonObject>());
}

bool serializeTurnConfig(const TurnConfig &config, Print &dst) {
  JsonDocument doc;

  // Create an array at the root
  JsonArray rootarr = doc.to<JsonArray>();
  // Fill the object
  config.save(rootarr);

  // Serialize JSON to file
  return serializeJsonPretty(doc, dst) > 0;
}

bool deserializeTurnConfig(Stream &src, TurnConfig &config) {
  JsonDocument doc;

  // Parse the JSON object in the file
  DeserializationError err=deserializeJson(doc, src);
  if (err)
    return false;

  config.load(doc.as<JsonArray>());
  return true;
}

void ControlStatus::save(JsonObject obj) const {
  obj["prognum"]=prognum;
  obj["stagenum"]=stagenum;
  obj["stagemax"]=stagemax;
  obj["stop"]=stop;
  obj["face"]=face;
  obj["status"]=status;
  obj["faceperc"]=faceperc;
  obj["stageperc"]=stageperc;
}

bool serializeControlStatus(ControlStatus status, char *dst, size_t dst_len) {
  JsonDocument doc;
  bool result;
  JsonObject rootobj=doc.to<JsonObject>();
  status.save(rootobj);
  result=serializeJson(doc, dst, dst_len);
  if (! result)
    Serial.println("Failed to serialise status json");
  return result;
}

bool serializeProgList(TurnConfig *tc, char *dst, const size_t dst_len) {
  JsonDocument doc;
  bool result;
  JsonArray rootobj=doc.to<JsonArray>();
  for (uint8_t i=0; i<tc->programs; i++) {
    rootobj.add(tc->program[i].longname);
  }
  result=serializeJson(doc, dst, dst_len);
  if (! result)
    Serial.println("Failed to serialise prog list json");
  return result;
}
