#pragma once
#include <Ethernet.h>

// Silly MAC address for Ethernet shield

class S3Ethernet
{
    static uint8_t mac[];

public:
    bool begin();
    bool hasIP();
};