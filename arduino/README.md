# turning-feather Arduino setup

## Screen config

To use the TFT_eSPI library config data for the display needs to be installed into the libraary. [The file](libraries-TFT_eSPI-User_Setup-featherwing-2.4.h) needs to be installed as TFT_eSPI_Setups/setup_featherwing_2.4.h in your Arduino libraries directory, alongside the TFT_eSPI library directory.

See [instructions](https://github.com/Bodmer/TFT_eSPI).

## Internal filesystem

The boot image and the backup configuration live on internal flash, this can be SPIFFS or Little FS. There are tools to upload files to either internal filesystem and the files to upload are in the [data](data/) directory. The configuration is normally loaded from the sdcard and uses the internal filesystem as a backup in case there is no sdcard, the boot image is normally loaded from the internal filesystem.