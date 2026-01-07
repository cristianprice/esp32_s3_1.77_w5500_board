#include "s3_graphics.h"

#ifdef __GRAPHICS_ENABLED__
#define LV_BUF_SIZE (TFT_WIDTH * TFT_HEIGHT * 40) // 40 lines of display buffer

// ---------------------------
// Constructor
// ---------------------------
S3Graphics::S3Graphics() : tft(TFT_eSPI())
{
    buf1 = nullptr;
}

// ---------------------------
// Flush callback for LVGL
// ---------------------------
void S3Graphics::my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    S3Graphics *self = (S3Graphics *)disp->user_data; // access the instance
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;

    self->tft.startWrite();
    self->tft.setAddrWindow(area->x1, area->y1, w, h);
    self->tft.pushColors((uint16_t *)color_p, w * h, true);
    self->tft.endWrite();

    lv_disp_flush_ready(disp);
}

// ---------------------------
// Button creation
// ---------------------------
void S3Graphics::create_buttons()
{
    /*Create a chart*/
    lv_obj_t *chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, TFT_WIDTH - 10, TFT_HEIGHT - 10);
    lv_obj_center(chart);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/

    /*Add two data series*/
    lv_chart_series_t *ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t *ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_SECONDARY_Y);

    /*Set the next points on 'ser1'*/
    lv_chart_set_next_value(chart, ser1, 10);
    lv_chart_set_next_value(chart, ser1, 10);
    lv_chart_set_next_value(chart, ser1, 10);
    lv_chart_set_next_value(chart, ser1, 10);
    lv_chart_set_next_value(chart, ser1, 10);
    lv_chart_set_next_value(chart, ser1, 10);
    lv_chart_set_next_value(chart, ser1, 10);
    lv_chart_set_next_value(chart, ser1, 30);
    lv_chart_set_next_value(chart, ser1, 70);
    lv_chart_set_next_value(chart, ser1, 90);

    /*Directly set points on 'ser2'*/
    ser2->y_points[0] = 90;
    ser2->y_points[1] = 70;
    ser2->y_points[2] = 65;
    ser2->y_points[3] = 65;
    ser2->y_points[4] = 65;
    ser2->y_points[5] = 65;
    ser2->y_points[6] = 65;
    ser2->y_points[7] = 65;
    ser2->y_points[8] = 65;
    ser2->y_points[9] = 65;

    lv_chart_refresh(chart); /*Required after direct set*/
}

// ---------------------------
// Initialize graphics
// ---------------------------
void S3Graphics::begin()
{
    // Initialize TFT
    tft.init();
    tft.setRotation(3);
    tft.setSwapBytes(true); // <<< CRITICAL
    tft.fillScreen(TFT_BLACK);

    // Allocate LVGL draw buffer in PSRAM
    buf1 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LV_BUF_SIZE, MALLOC_CAP_SPIRAM);
    if (!buf1)
    {
        Serial.println("PSRAM malloc FAILED");
        while (1)
            ;
    }
    else
    {
        Serial.println("PSRAM malloc OK");
    }

    // Initialize LVGL
    lv_init();

    // Initialize LVGL draw buffer
    lv_disp_draw_buf_init(&draw_buf, buf1, nullptr, LV_BUF_SIZE);

    // Initialize display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = TFT_WIDTH;
    disp_drv.ver_res = TFT_HEIGHT;
    disp_drv.flush_cb = S3Graphics::my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.user_data = this; // Pass instance pointer
    lv_disp_drv_register(&disp_drv);

    // Create buttons
    create_buttons();
}

// ---------------------------
// LVGL loop handler
// ---------------------------
void S3Graphics::loop()
{
    lv_timer_handler();
}

#endif // __GRAPHICS_ENABLED__