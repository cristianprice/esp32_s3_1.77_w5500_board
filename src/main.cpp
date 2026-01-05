#include "s3_ethernet.h"
#include "s3_graphics.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <SPI.h>

S3Ethernet ethernet;
S3Graphics graphics;

void setup()
{
  Serial.begin(115200);
  ethernet.begin();
  // graphics.begin();
}

void loop()
{
  // graphics.loop();
  ethernet.hasIP();
  vTaskDelay(pdMS_TO_TICKS(5));
}
