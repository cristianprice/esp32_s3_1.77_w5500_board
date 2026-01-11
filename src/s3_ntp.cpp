#include "s3_ntp.h"
#include <Arduino.h>

void setup_ntp()
{
    const char *tz = "EET-2EEST,M3.5.0/3,M10.5.0/4";
    configTzTime(tz, "pool.ntp.org");
}
