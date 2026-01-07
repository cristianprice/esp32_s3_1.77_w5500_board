#pragma once
#include <TFT_eSPI.h>
#include <lvgl.h>
#include <esp_heap_caps.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_log.h"

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

private:
    TFT_eSPI tft;
    lv_disp_draw_buf_t draw_buf;
    lv_color_t *buf1;

    static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
    void create_buttons();
};

#endif // __GRAPHICS_ENABLED__