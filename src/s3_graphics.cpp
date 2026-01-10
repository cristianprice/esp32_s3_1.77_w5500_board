#include "s3_graphics.h"

#ifdef __GRAPHICS_ENABLED__
#define LV_BUF_SIZE (TFT_WIDTH * TFT_HEIGHT * 40) // 40 lines of display buffer

#define TERMINAL_BUFFER_SIZE 128

// ---------------------------
// Constructor
// ---------------------------
S3Graphics::S3Graphics() : tft(TFT_eSPI())
{
    buf1 = nullptr;
}

void S3Graphics::create_terminal()
{
    // Create the text area to occupy full screen
    terminal_txtarea = lv_textarea_create(lv_scr_act());
    lv_obj_set_size(terminal_txtarea, TFT_WIDTH, TFT_HEIGHT);

    // Optional styling
    lv_obj_set_style_text_font(terminal_txtarea, &lv_font_montserrat_10, 0);
    lv_obj_set_style_text_color(terminal_txtarea, lv_color_white(), 0);
    lv_obj_set_style_bg_color(terminal_txtarea, lv_color_black(), 0);

    // Allocate buffer in PSRAM
    terminal_buffer = (char *)heap_caps_malloc(TERMINAL_BUFFER_SIZE, MALLOC_CAP_SPIRAM);
    if (!terminal_buffer)
    {
        Serial.println("Failed to allocate terminal buffer!");
        return;
    }
    terminal_buffer[0] = '\0'; // start empty

    lv_obj_set_scrollbar_mode(terminal_txtarea, LV_SCROLLBAR_MODE_AUTO);
    lv_textarea_set_one_line(terminal_txtarea, false);
    lv_textarea_set_cursor_click_pos(terminal_txtarea, false);

    // Assign buffer to the textarea
    lv_textarea_set_text(terminal_txtarea, terminal_buffer);
}

void S3Graphics::terminal_add_message(const char *msg)
{
    if (!terminal_txtarea || !terminal_buffer || !msg)
        return;

    size_t msg_len = strlen(msg);
    if (msg_len == 0)
        return;

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
    Serial.println(terminal_buffer);

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
    create_terminal();
}

// ---------------------------
// LVGL loop handler
// ---------------------------
void S3Graphics::loop()
{
    lv_timer_handler();
}

#endif // __GRAPHICS_ENABLED__