#include "s3_ethernet.h"
#include "s3_graphics.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <SPI.h>
#include <HTTPClient.h>
#include <s3_fs.h>

#ifdef __ETHERNET_ENABLED__
S3Ethernet ethernet;
#endif
S3Graphics graphics;

void setup()
{

  delay(5000);
  Serial.begin(115200);

  setup_fs();
  graphics.begin();

#ifdef __ETHERNET_ENABLED__
  ethernet.begin();
#endif
}

void loop()
{
  graphics.loop();
  vTaskDelay(pdMS_TO_TICKS(5));
}
