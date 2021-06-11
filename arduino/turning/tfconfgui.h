/*
 * GUI for configuration on the unit, built with GUIslice
 */

#ifndef TURNING_TFCONFGUI_H
#define TURNING_TFCONFGUI_H

#include "GUIslice.h"
#include "GUIslice_drv.h"

// Use fonts from TFT_eSPI instead of adafruit
#include <TFT_eSPI.h>

void tf_conf_gui_setup();
void tf_conf_gui_loop();

#endif //TURNING_TFCONFGUI_H
