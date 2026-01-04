#include "s3_ethernet.h"
#include <SPI.h>

// Define the MAC address as a static member variable
uint8_t S3Ethernet::mac[] = {0xAB, 0xA0, 0xCD, 0xC0, 0xDE, 0xD0};

bool S3Ethernet::begin()
{
    // Reset W5500 Ethernet module
    pinMode(W550_RST, OUTPUT);
    digitalWrite(W550_RST, LOW);
    delay(10);
    digitalWrite(W550_RST, HIGH);
    delay(150);

    // Start Ethernet with the defined MAC address
    // SPI.begin(W550_SCLK, W550_MISO, W550_MOSI, W550_CS);

    /* Set CS pin for W5500 */
    Ethernet.init(W550_CS);

    if (Ethernet.begin(mac, 10000, 10000) == 0)
    {
        Serial.println("DHCP failed");
        return false;
    }

    /* Check DHCP result */
    unsigned long startTime = millis();
    while (Ethernet.localIP() == INADDR_NONE)
    {
        if (millis() - startTime > 10000) // Check for 10 seconds
        {
            Serial.println("DHCP failed");
            return false;
        }
        delay(1000);
    }

    return true;
}

bool S3Ethernet::hasIP()
{
    return (Ethernet.localIP() != INADDR_NONE);
}