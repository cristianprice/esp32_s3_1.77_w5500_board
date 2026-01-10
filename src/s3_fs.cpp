#include "s3_fs.h"
#include "SD_MMC.h"
#include "s3_graphics.h"

extern S3Graphics graphics;
bool setup_fs()
{
    // Initialize SD card in SDMMC 4-bit mode
    SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0, SD_D1, SD_D2, SD_D3);
    if (!SD_MMC.begin("/sdcard"))
    {
        graphics.terminal_enqueue_message("SDMMC Mount Failed");
        return false;
    }
    graphics.terminal_enqueue_message("SDMMC Initialized");

    // Optional: print card info
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    // Store card size in a buffer
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "SD Card Size: %llu MB\n", cardSize);
    graphics.terminal_enqueue_message(buffer);
    return true;
}