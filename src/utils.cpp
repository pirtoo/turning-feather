/*
 * Utilities
 */

#include "utils.h"

#ifdef ESP32
void do_restart() {
  esp_restart();
  //esp_task_wdt_init(1,true);
  //esp_task_wdt_add(NULL);
  //while(true);
}
#endif //ESP32
