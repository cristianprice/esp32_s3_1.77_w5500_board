#pragma once
#include <Arduino.h>
#include <Ethernet.h>

// Silly MAC address for Ethernet shield

class S3Ethernet
{
public:
    bool begin();
    bool hasIP();
};