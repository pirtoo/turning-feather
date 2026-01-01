# turning-feather Arduino setup

## Screen config

To use the TFT_eSPI library config data for the display needs to be installed into the library, not in your sketch directory. [The file](libraries-TFT_eSPI-User_Setup-featherwing-2.4.h) needs to be installed as `TFT_eSPI_Setups/setup_featherwing_2.4.h` in your Arduino libraries directory, alongside the TFT_eSPI library directory.

See [instructions](https://github.com/Bodmer/TFT_eSPI).

## GUIslice

To use GUIslice for the testing UI you also need to install the config header file. [The file](libraries-guislice-featherwing-2.4.h) needs to be installed as `GUIslice/configs/libraries-guislice-featherwing-2.4.h` in your Arduino libraries directory. The file `GUIslice/src/GUIslice_config.h` needs to be edited to inclide the line `#include "../configs/libraries-guislice-featherwing-2.4.h"`.

This needs to be redone if the library is updated.

## Internal filesystem

The boot splash image and the backup configuration live on internal flash, this can be SPIFFS or LittleFS. The configuration is normally loaded from the sdcard and uses the internal filesystem as a backup in case there is no sdcard, the boot image is normally loaded from the internal filesystem.

If you use LittleFS the first time the HUZZAH32 is powered on it will format the onboard filesystem then fail to load files from it since it is empty. You can then transfer the files in [data](turning/data/) with [the Arduino IDE uploader](https://randomnerdtutorials.com/arduino-ide-2-install-esp32-littlefs/). There is no SPIFFS uploader for V2 of the Ardino IDE, which is partly why I have migrated to LittleFS, but command line tools exist.
