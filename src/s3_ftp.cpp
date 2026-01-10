#include "ESP-FTP-Server-Lib.h"
#include "FTPFilesystem.h"
#include "SD_MMC.h"

#define FTP_USER "esp32"
#define FTP_PASSWORD "esp32"

FTPServer ftp;
fs::FS *ftpfs = &SD_MMC;

void setup_ftp()
{
    ftp.addUser(FTP_USER, FTP_PASSWORD);
    ftp.addFilesystem("SDMMC", ftpfs);
    ftp.begin();
}

void loop_ftp()
{
    ftp.handle();
}