#include "s3_graphics.h"
#include <time.h>

#ifdef __GRAPHICS_ENABLED__
#define LV_BUF_SIZE (TFT_WIDTH * TFT_HEIGHT * 40) // 40 lines of display buffer

#define TERMINAL_BUFFER_SIZE 256

static void time_update_cb(lv_timer_t *timer)
{
    S3Graphics *gfx = (S3Graphics *)timer->user_data;

    time_t now;
    struct tm timeinfo;

    time(&now);
    localtime_r(&now, &timeinfo);

    char buf[16];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d",
             timeinfo.tm_hour,
             timeinfo.tm_min,
             timeinfo.tm_sec);

    lv_label_set_text(gfx->terminal_time_label, buf);
}

// ---------------------------
// Constructor
// ---------------------------
S3Graphics::S3Graphics() : tft(TFT_eSPI())
{
    buf1 = nullptr;
}

void S3Graphics::create_terminal()
{
    /* Main container */
    lv_obj_t *container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(container, TFT_WIDTH, TFT_HEIGHT);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(container, 0, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_bg_color(container, lv_color_black(), 0);

    /* Time label (top bar) */
    terminal_time_label = lv_label_create(container);
    lv_label_set_text(terminal_time_label, "00:00:00");
    lv_obj_set_style_text_font(terminal_time_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(terminal_time_label, lv_color_white(), 0);
    lv_obj_set_style_pad_left(terminal_time_label, 4, 0);
    lv_obj_set_style_pad_top(terminal_time_label, 2, 0);
    lv_obj_set_style_pad_bottom(terminal_time_label, 2, 0);
    lv_obj_set_width(terminal_time_label, TFT_WIDTH);

    /* Terminal textarea */
    terminal_txtarea = lv_textarea_create(container);
    lv_obj_set_flex_grow(terminal_txtarea, 1); // fill remaining space
    lv_obj_set_width(terminal_txtarea, TFT_WIDTH);

    lv_obj_set_style_text_font(terminal_txtarea, &lv_font_montserrat_10, 0);
    lv_obj_set_style_text_color(terminal_txtarea, lv_color_white(), 0);
    lv_obj_set_style_bg_color(terminal_txtarea, lv_color_black(), 0);

    lv_obj_set_scrollbar_mode(terminal_txtarea, LV_SCROLLBAR_MODE_AUTO);
    lv_textarea_set_one_line(terminal_txtarea, false);
    lv_textarea_set_cursor_click_pos(terminal_txtarea, false);

    /* Allocate buffer in PSRAM */
    terminal_buffer = (char *)heap_caps_malloc(TERMINAL_BUFFER_SIZE, MALLOC_CAP_SPIRAM);
    if (!terminal_buffer)
    {
        Serial.println("Failed to allocate terminal buffer!");
        return;
    }
    terminal_buffer[0] = '\0';

    lv_textarea_set_text(terminal_txtarea, terminal_buffer);

    lv_timer_create(time_update_cb, 400, this);
}

void S3Graphics::terminal_add_message(const char *msg)
{
    if (!terminal_txtarea || !terminal_buffer || !msg)
        return;

    size_t msg_len = strlen(msg);
    if (msg_len == 0)
    {
        Serial.printf("Message size: %d", msg_len);
        return;
    }

    // +1 for '\n', +1 for '\0'
    size_t required = msg_len + 2;

    // If message will not fit → clear terminal
    if (buffer_len + required >= TERMINAL_BUFFER_SIZE)
    {
        buffer_len = 0;
        terminal_buffer[0] = '\0';

        // Optional visual separator after clear
        const char *clear_msg = "--- terminal cleared ---\n";
        size_t clear_len = strlen(clear_msg);

        if (clear_len < TERMINAL_BUFFER_SIZE)
        {
            memcpy(terminal_buffer, clear_msg, clear_len);
            buffer_len = clear_len;
            terminal_buffer[buffer_len] = '\0';
        }
    }

    // Append message
    memcpy(terminal_buffer + buffer_len, msg, msg_len);
    buffer_len += msg_len;

    terminal_buffer[buffer_len++] = '\n';
    terminal_buffer[buffer_len] = '\0';
    Serial.printf("Terminal Add Message: %s\n", terminal_buffer);

    // Update LVGL
    lv_textarea_set_text(terminal_txtarea, terminal_buffer);
    lv_textarea_set_cursor_pos(terminal_txtarea, LV_TEXTAREA_CURSOR_LAST);
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
    buf2 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LV_BUF_SIZE, MALLOC_CAP_SPIRAM);
    if (!buf1 || !buf2)
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
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, LV_BUF_SIZE);

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
    create_terminal();
}

// ---------------------------
// LVGL loop handler
// ---------------------------
void S3Graphics::loop()
{
    terminal_process_queue();
    lv_timer_handler();
    lv_tick_inc(1);
}

#endif // __GRAPHICS_ENABLED__