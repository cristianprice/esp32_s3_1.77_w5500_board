#include "s3_graphics.h"

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
    const char *btn_labels[] = {LV_SYMBOL_OK " OK", LV_SYMBOL_LEFT " Left", LV_SYMBOL_RIGHT " Right"};

    int btn_width = TFT_WIDTH / 2;
    int btn_height = TFT_HEIGHT / 5;

    for (int i = 0; i < 3; i++)
    {
        lv_obj_t *btn = lv_btn_create(lv_scr_act());
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_CENTER, 0, (i - 1) * (btn_height + 10));

        lv_obj_t *btn_label = lv_label_create(btn);
        lv_label_set_text(btn_label, btn_labels[i]);

        lv_obj_set_style_text_font(btn_label, &lv_font_montserrat_12, 0);
        lv_obj_center(btn_label);
    }
}

// ---------------------------
// Initialize graphics
// ---------------------------
void S3Graphics::begin()
{
    // Initialize TFT
    tft.init();
    tft.setRotation(3);
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