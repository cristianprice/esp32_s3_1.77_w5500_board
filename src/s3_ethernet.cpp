#include "s3_ethernet.h"
#include <SPI.h>
#include "esp_log.h"
#include <cstdlib>
#include <ctime>

#ifdef __ETHERNET_ENABLED__

#define LINK_SPEED_100MBPS 1
#define LINK_SPEED_10MBPS 2
#define LINK_SPEED_DOWN 0

static const char *TAG = "S3Ethernet";

// Define your MAC
uint8_t g_mac[6];
static void generateRandomMAC(uint8_t *mac)
{
    std::srand(std::time(nullptr));
    for (int i = 0; i < 6; ++i)
    {
        mac[i] = std::rand() % 256;
    }
    mac[0] &= 0xFE; // Ensure unicast and globally unique
}

bool S3Ethernet::begin()
{
    // Generate a random MAC address
    generateRandomMAC(g_mac);

// Reset the W5500
#ifndef __GRAPHICS_ENABLED__
    pinMode(W550_RST, OUTPUT);
    digitalWrite(W550_RST, LOW);
    delay(100);
    digitalWrite(W550_RST, HIGH);
    delay(150);
    SPI.begin(W550_SCLK, W550_MISO, W550_MOSI); // SCK, MISO, MOSI
#endif

    Ethernet.init(W550_CS);

    Serial.println("Starting Ethernet...");
    if (Ethernet.begin(g_mac) == 0)
    {
        Serial.println("DHCP Failed");
        return false;
    }

    Serial.print("IP Address: ");
    Serial.println(Ethernet.localIP());

    Serial.print("IP Address: ");
    Serial.println(Ethernet.localIP());
    IPAddress ip = Ethernet.localIP();
    ESP_LOGI(TAG, "DHCP succeeded, IP: %s", ip.toString().c_str());

    return true;
}

bool S3Ethernet::hasIP()
{
    if (Ethernet.linkStatus() == LINK_SPEED_100MBPS)
    {
        ESP_LOGI(TAG, "Ethernet link is up at 100Mbps");
    }
    else if (Ethernet.linkStatus() == LINK_SPEED_10MBPS)
    {
        ESP_LOGI(TAG, "Ethernet link is up at 10Mbps");
    }
    else
    {
        ESP_LOGW(TAG, "Ethernet link is down");
        return false;
    }

    IPAddress ip = Ethernet.localIP();
    ESP_LOGI(TAG, "Current IP: %s", ip.toString().c_str());
    return ip != INADDR_NONE;
}

#endif // __ETHERNET_ENABLED__