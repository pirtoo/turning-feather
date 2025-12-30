/*
 * Utilities
 */

#ifdef ESP32
void do_restart() {
  ESP.restart();
  //esp_task_wdt_init(1,true);
  //esp_task_wdt_add(NULL);
  //while(true);
}
#endif //ESP32
