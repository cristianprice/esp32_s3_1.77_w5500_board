#include <s3_wifi.h>
#include <WiFi.h>
#include <s3_graphics.h>

extern S3Graphics graphics;
bool wifiConnected = false;
const char *ssid = "LaPupupu";     // Replace with your WiFi SSID
const char *password = "hidden01"; // Replace with your WiFi password

static void onWiFiEvent(WiFiEvent_t event)
{
    String ipMessage;
    switch (event)
    {
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        graphics.terminal_enqueue_message("WiFi connected to AP");
        break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        ipMessage = "IP address: " + WiFi.localIP().toString();
        graphics.terminal_enqueue_message(ipMessage.c_str());
        wifiConnected = true;
        break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        graphics.terminal_enqueue_message("WiFi disconnected");
        wifiConnected = false;
        WiFi.reconnect();
        break;

    default:
        break;
    }
}

void setup_wifi()
{
    WiFi.mode(WIFI_STA);
    WiFi.onEvent(onWiFiEvent);
    WiFi.begin(ssid, password);
}
