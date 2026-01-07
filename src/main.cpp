#include "s3_ethernet.h"
#include "s3_graphics.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <SPI.h>
#include <HTTPClient.h>

#ifdef __ETHERNET_ENABLED__
S3Ethernet ethernet;
#endif
S3Graphics graphics;

void setup()
{
  Serial.begin(115200);
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
