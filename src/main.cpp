#include <TFT_eSPI.h>
#include <lvgl.h>
#include <esp_heap_caps.h>
#include "s3_ethernet.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

S3Ethernet ethernet;       // Ethernet instance
TFT_eSPI tft = TFT_eSPI(); // TFT instance

// LVGL display buffer size (in pixels)
#define LV_BUF_SIZE (TFT_WIDTH * TFT_HEIGHT * 40) // 40 lines of display buffer

// LVGL objects
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf1;

// Display flush callback
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = area->x2 - area->x1 + 1;
  uint32_t h = area->y2 - area->y1 + 1;

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)color_p, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

// Function to run ethernet.begin() in a FreeRTOS task
void ethernetTask(void *pvParameters)
{
  ethernet.begin();
  vTaskDelete(NULL); // Delete the task after execution
}

void setup()
{
  Serial.begin(115200);

  // Initialize TFT
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  // Allocate LVGL draw buffer in PSRAM
  buf1 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LV_BUF_SIZE, MALLOC_CAP_SPIRAM);
  if (buf1)
  {
    Serial.println("PSRAM malloc OK");
  }
  else
  {
    Serial.println("PSRAM malloc FAILED");
    while (1)
      ;
  }

  // Initialize LVGL
  lv_init();

  // Initialize LVGL draw buffer
  lv_disp_draw_buf_init(&draw_buf, buf1, NULL, LV_BUF_SIZE);

  // Initialize display driver
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = TFT_WIDTH;
  disp_drv.ver_res = TFT_HEIGHT;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // ==========================
  // Create 3 buttons with symbols
  // ==========================
  const char *btn_labels[] = {LV_SYMBOL_OK " OK", LV_SYMBOL_LEFT " Left", LV_SYMBOL_RIGHT " Right"};

  int btn_width = TFT_WIDTH / 2;   // 1/3 of screen width
  int btn_height = TFT_HEIGHT / 5; // moderate height

  for (int i = 0; i < 3; i++)
  {
    lv_obj_t *btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, btn_width, btn_height);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, (i - 1) * (btn_height + 10));

    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, btn_labels[i]);

    // Set smaller font (10 px)
    lv_obj_set_style_text_font(btn_label, &lv_font_montserrat_12, 0);

    lv_obj_center(btn_label);
  }

  // Create a FreeRTOS task for ethernet.begin()
  // xTaskCreate(ethernetTask, "EthernetTask", 4096, NULL, 1, NULL);
}

void loop()
{
  lv_timer_handler();
  // Serial.println("Has ip: " + String(ethernet.hasIP() ? "Yes" : "No"));
  delay(5);
}
