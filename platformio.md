# turning-feather PlatformIO and code setup

## PlatFormIO

PlatformIO (used by me with Visual Studio Code) makes managing different build targets far simpler. The project now needs multiple build targets because the original Adafruit Feather ESP32 HUZZAH32 and Featherwing 2.4" TFT display have both been superseded with V2 version. You can still buy the V1 of the ESP32 but this may not continue, I have not found V1 displays still for sale.

All combinations of V1 and V1 ESP32 boards and display boards are supported by different targets in PlatformIO. The defines needed by the code to adapt for the different versions are included in the [platformio.ini](platformio.ini) config file.

One advantage of the V2 ESP32 board is you can buy a version with a connector for a larger WiFi antenna than the small built in antenna on V1 and other versions when WiFi support is added to the code.

Before making these changes I have created a [branch of the old Arduino IDE style setup](https://github.com/pirtoo/turning-feather/tree/last_arduino_ide) for reference.

## Display libraries

The project used to use TFT_eSPI but the configuration for it was not simple and the only reason I was using it was for nicer fonts. Those fonts are now supported directly by Adafruit graphics libraries so I have switched over to those. PlatformIO will download the correct libraries via the [platformio.ini](platformio.ini) configuration.

## Internal filesystem

The boot splash image and the backup configuration live on internal flash,  LittleFS. The configuration is normally loaded from the sdcard and uses the internal filesystem as a backup in case there is no sdcard, the boot image is normally loaded from the internal filesystem.

If you use LittleFS the first time the ESP32 is powered on after the Turning Feather code is installed it will format the onboard filesystem then fail to load files from it since it is empty. You can then transfer the files in [data](data/) with [the Arduino IDE uploader](https://randomnerdtutorials.com/esp32-vs-code-platformio-littlefs/). If you change the define in the [code](include/turning.h) and the config in [platformio.ini](platformio.ini) then you can go back to SPIFFS if desired but support for SPIFFS is apparently ending.

## ArduinoJson

The project has updated to ArduinoJson V7 which is current. This no longer needs a pre-calculation of size, it will work with json files up to the size of memory. If your jso file doesn't work then it may be too large. A planned feature addition is the ability to use different files. Stage names are limited in length because the screen width is limited.
