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
void update_sometimes(void *parameter);

void setup()
{
  delay(5000);

  Serial.begin(115200);
  graphics.begin();

  xTaskCreatePinnedToCore(
      update_sometimes,  /* Function to implement the task */
      "UpdateSometimes", /* Name of the task */
      4096,              /* Stack size in words */
      NULL,              /* Task input parameter */
      1,                 /* Priority of the task */
      NULL,              /* Task handle. */
      1);                /* Core where the task should run */

  setup_fs();
  setup_wifi();

  while (!wifiConnected)
  {
    delay(100);
  }

  setup_ftp();
  graphics.terminal_enqueue_message("FTP Server Started");

#ifdef __ETHERNET_ENABLED__
  ethernet.begin();
#endif
}

uint32_t loopCounter = 0;
void loop()
{
  loop_ftp();
  graphics.loop();
  delay(1);
}

void update_sometimes(void *parameter)
{
  for (;;)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - lastDelayTime >= 1000)
    {
      lastDelayTime = currentMillis;
      char buffer[50];
      snprintf(buffer, sizeof(buffer), "Millis: %ld", currentMillis);
      graphics.terminal_enqueue_message(buffer); // <- enqueue, not direct LVGL call
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
