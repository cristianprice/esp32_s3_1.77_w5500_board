#include "s3_ethernet.h"
#include "s3_graphics.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <SPI.h>
#include <HTTPClient.h>

S3Ethernet ethernet;
S3Graphics graphics;

void setup()
{
  Serial.begin(115200);
  graphics.begin();
  ethernet.begin();
}

void make_http_request_manual()
{
  if (!ethernet.hasIP())
  {
    Serial.println("No IP address assigned.");
    return;
  }

  EthernetClient client;

  const char *host = "example.com";
  const uint16_t port = 80;
  const char *path = "/";

  Serial.println("Connecting to server...");

  if (!client.connect(host, port))
  {
    Serial.println("Connection failed");
    return;
  }

  Serial.println("Connected, sending request...");

  // ---- HTTP REQUEST ----
  client.print(String("GET ") + path + " HTTP/1.1\r\n");
  client.print(String("Host: ") + host + "\r\n");
  client.print("User-Agent: ESP32-W5500\r\n");
  client.print("Connection: close\r\n");
  client.print("\r\n");

  // ---- READ RESPONSE ----
  unsigned long timeout = millis();
  while (client.connected() && millis() - timeout < 10000)
  {
    while (client.available())
    {
      String line = client.readStringUntil('\n');
      Serial.println(line);
      timeout = millis(); // reset timeout on data
    }
  }

  client.stop();
  Serial.println("Connection closed");
}

void loop()
{
  graphics.loop();
  ethernet.hasIP();

  make_http_request_manual();

  vTaskDelay(pdMS_TO_TICKS(5));
}
