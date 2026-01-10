#include <s3_wifi.h>
#include <WiFi.h>

bool wifiConnected = false;
const char *ssid = "LaPupupu";     // Replace with your WiFi SSID
const char *password = "hidden01"; // Replace with your WiFi password

static void onWiFiEvent(WiFiEvent_t event)
{
    switch (event)
    {
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.println("WiFi connected to AP");
        break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        wifiConnected = true;
        break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        Serial.println("WiFi disconnected");
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
