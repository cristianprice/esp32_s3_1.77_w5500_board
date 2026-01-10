#include "s3_fs.h"
#include "SD_MMC.h"

bool setup_fs()
{
    // Initialize SD card in SDMMC 4-bit mode
    SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0, SD_D1, SD_D2, SD_D3);
    if (!SD_MMC.begin("/sdcard"))
    {
        Serial.println("SDMMC Mount Failed");
        return false;
    }
    Serial.println("SDMMC Initialized");

    // Optional: print card info
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %llu MB\n", cardSize);
    return true;
}