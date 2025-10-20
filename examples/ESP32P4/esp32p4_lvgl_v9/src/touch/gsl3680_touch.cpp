#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "esp_lcd_touch.h"
#include "esp_lcd_gsl3680.h"
#include "esp32-hal-log.h"
#include "gsl3680_touch.h"

#define CONFIG_LCD_HRES 800
#define CONFIG_LCD_VRES 1280

static const char *TAG = "TOUCH";

esp_lcd_touch_handle_t tp;
esp_lcd_panel_io_handle_t tp_io_handle;

uint16_t touch_strength[1];
uint8_t touch_cnt = 0;

gsl3680_touch::gsl3680_touch(int8_t sda_pin, int8_t scl_pin, int8_t rst_pin, int8_t int_pin)
{
    _sda = sda_pin;
    _scl = scl_pin;
    _rst = rst_pin;
    _int = int_pin;
}

void gsl3680_touch::begin()
{
    log_d("Starting gsl3680 touch panel driver");

    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = (gpio_num_t)_sda,
        .scl_io_num = (gpio_num_t)_scl,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
    };
    i2c_conf.master.clk_speed = 400000; // 400kHz

    // log_d all 4 pins used
    log_d("SDA pin: %d", _sda);
    log_d("SCL pin: %d", _scl);
    log_d("RST pin: %d", _rst);
    log_d("INT pin: %d", _int);

    esp_err_t ret = i2c_param_config(I2C_NUM_0, &i2c_conf);
    if (ret != ESP_OK) {
        log_e("I2C param config failed: %s", esp_err_to_name(ret));
        return;
    }
    
    ret = i2c_driver_install(I2C_NUM_0, i2c_conf.mode, 0, 0, 0);
    if (ret != ESP_OK) {
        log_e("I2C driver install failed: %s", esp_err_to_name(ret));
        return;
    }

    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_GSL3680_CONFIG();
    log_i("Initialize touch IO (I2C)");
    esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)I2C_NUM_0, &tp_io_config, &tp_io_handle);

    esp_lcd_touch_config_t tp_cfg = {
        .x_max = CONFIG_LCD_HRES,
        .y_max = CONFIG_LCD_VRES,
        .rst_gpio_num = (gpio_num_t)_rst,
        .int_gpio_num = (gpio_num_t)_int,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 1,
        },
    };

    log_i("Initialize touch controller gsl3680");
    ret = esp_lcd_touch_new_i2c_gsl3680(tp_io_handle, &tp_cfg, &tp);
    if (ret != ESP_OK) {
        log_e("Touch controller init failed: %s", esp_err_to_name(ret));
        return;
    }
}

bool gsl3680_touch::getTouch(uint16_t *x, uint16_t *y)
{
    log_d("Reading touch data");
    esp_lcd_touch_read_data(tp);
    bool touchpad_pressed = esp_lcd_touch_get_coordinates(tp, x, y, touch_strength, &touch_cnt, 1);
    log_d("Touch data: pressed=%d, x=%d, y=%d, strength=%d", touchpad_pressed, *x, *y, touch_strength[0]);

    return touchpad_pressed;
}

void gsl3680_touch::set_rotation(uint8_t r){
switch(r){
    case 0:
        esp_lcd_touch_set_swap_xy(tp, false);   
        esp_lcd_touch_set_mirror_x(tp, false);
        esp_lcd_touch_set_mirror_y(tp, false);
        break;
    case 1:
        esp_lcd_touch_set_swap_xy(tp, false);
        esp_lcd_touch_set_mirror_x(tp, true);
        esp_lcd_touch_set_mirror_y(tp, true);
        break;
    case 2:
        esp_lcd_touch_set_swap_xy(tp, false);   
        esp_lcd_touch_set_mirror_x(tp, false);
        esp_lcd_touch_set_mirror_y(tp, false);
        break;
    case 3:
        esp_lcd_touch_set_swap_xy(tp, false);   
        esp_lcd_touch_set_mirror_x(tp, true);
        esp_lcd_touch_set_mirror_y(tp, true);
        break;
    }

}