#pragma once
#include <Arduino.h>
#include <Ethernet.h>

// Silly MAC address for Ethernet shield

#ifdef __ETHERNET_ENABLED__

class S3Ethernet
{
public:
    bool begin();
    bool hasIP();
};

#endif // __ETHERNET_ENABLED__