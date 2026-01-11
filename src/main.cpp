#include "s3_ethernet.h"
#include "s3_graphics.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <SPI.h>
#include <HTTPClient.h>
#include <s3_fs.h>
#include <s3_wifi.h>
#include "Arduino.h"
#include "s3_ftp.h"
#include "s3_ntp.h"

#ifdef __ETHERNET_ENABLED__
S3Ethernet ethernet;
#endif
S3Graphics graphics;

void setup()
{
  Serial.begin(115200);
  graphics.begin();

  setup_fs();
  setup_wifi();

  while (!wifiConnected)
  {
    delay(10);
  }

  setup_ftp();
  graphics.terminal_enqueue_message("FTP Server Started");
  setup_ntp();
  graphics.terminal_enqueue_message("NTP Clock Synchronized");

#ifdef __ETHERNET_ENABLED__
  ethernet.begin();
#endif
}

void loop()
{
  loop_ftp();
  graphics.loop();
  yield();
}
