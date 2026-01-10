#include "ESP-FTP-Server-Lib.h"
#include "FTPFilesystem.h"
#include "SD_MMC.h"
#include "s3_graphics.h"

#define FTP_USER "esp32"
#define FTP_PASSWORD "esp32"

FTPServer ftp;
fs::FS *ftpfs = &SD_MMC;
extern S3Graphics graphics;

void setup_ftp()
{
    ftp.addUser(FTP_USER, FTP_PASSWORD);
    ftp.addFilesystem("SDMMC", ftpfs);
    ftp.begin();
}

bool ftp_was_connected = false;

void loop_ftp()
{
    ftp.handle();
    bool connected_now = ftp.countConnections() > 0;

    // Only print message if state changed
    if (connected_now && !ftp_was_connected)
    {
        graphics.terminal_enqueue_message("FTP client connected");
        Serial.println("FTP client connected");
    }
    else if (!connected_now && ftp_was_connected)
    {
        graphics.terminal_enqueue_message("FTP client disconnected");
        Serial.println("FTP client disconnected");
    }

    ftp_was_connected = connected_now; // save state for next loop
}