#pragma once
#include <TFT_eSPI.h>
#include <queue>
#include <mutex>

extern "C"
{
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_log.h"
#include <lvgl.h>
#include <esp_heap_caps.h>
}

#ifdef __GRAPHICS_ENABLED__

// ---------------------------
// Graphics class declaration
// ---------------------------
class S3Graphics
{
public:
    S3Graphics(); // Constructor
    void begin(); // Initialize TFT, LVGL, draw buffer, buttons
    void loop();  // Call periodically in main loop
    void create_terminal();
    void terminal_add_message(const char *msg);

    void terminal_enqueue_message(const char *msg)
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        terminalQueue.push(String(msg));
    }

    void terminal_process_queue()
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        Serial.print("Queue Size: ");
        Serial.println(terminalQueue.size());

        while (!terminalQueue.empty())
        {
            terminal_add_message(terminalQueue.front().c_str());
            terminalQueue.pop();
        }
    }

private:
    TFT_eSPI tft;
    lv_disp_draw_buf_t draw_buf;
    lv_color_t *buf1;

    lv_obj_t *terminal_txtarea = nullptr;
    char *terminal_buffer;
    size_t buffer_len = 0;

    std::queue<String> terminalQueue;
    std::mutex queueMutex;

    static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
};

#endif // __GRAPHICS_ENABLED__