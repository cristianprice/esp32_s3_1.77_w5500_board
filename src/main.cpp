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

#ifdef __ETHERNET_ENABLED__
S3Ethernet ethernet;
#endif
S3Graphics graphics;

unsigned long lastDelayTime = 0; // Track the last delay time
void delay_maybe();

void setup()
{
  delay(5000);
  Serial.begin(115200);
  setup_fs();

  setup_wifi();
  graphics.begin();

  while (!wifiConnected)
  {
    delay(100);
  }

  setup_ftp();

#ifdef __ETHERNET_ENABLED__
  ethernet.begin();
#endif
}

void loop()
{
  graphics.loop();
  loop_ftp();
  delay_maybe();
}

void delay_maybe()
{
  unsigned long currentTime = millis();
  if (currentTime - lastDelayTime >= 10) // Check if 10ms have passed
  {
    delay(10);
    lastDelayTime = currentTime;
  }
}
