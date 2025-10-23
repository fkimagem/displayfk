#define CONFIG_I2C_ENABLE_LEGACY_DRIVER 1

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
//#include "driver/i2c.h"
#include "driver/i2c_master.h"  // Driver NOVO
#include "esp_lcd_touch.h"
#include "esp_lcd_gsl3680.h"
#include "gsl3680_touch.h"
#include <Arduino.h>

#define CONFIG_LCD_HRES 800
#define CONFIG_LCD_VRES 1280

static const char *TAG = "GSL3680";

esp_lcd_touch_handle_t tp;
esp_lcd_panel_io_handle_t tp_io_handle;
i2c_master_bus_handle_t i2c_bus_handle;  // Handle do novo driver

uint16_t touch_strength[1];
uint8_t touch_cnt = 0;

GSL3680_touch::GSL3680_touch(int8_t sda_pin, int8_t scl_pin, int8_t rst_pin, int8_t int_pin)
{
    _sda = sda_pin;
    _scl = scl_pin;
    _rst = rst_pin;
    _int = int_pin;
    ESP_LOGD(TAG, "Constructor GSL3680_touch");
    Serial.println("Constructor GSL3680_touch");
}

void GSL3680_touch::begin()
{
   
    // Configuração do novo driver I2C Master
    i2c_master_bus_config_t i2c_bus_config = {
        .i2c_port = I2C_NUM_1,
        .sda_io_num = (gpio_num_t)_sda,
        .scl_io_num = (gpio_num_t)_scl,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags = {
            .enable_internal_pullup = true,
        },
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle));

    /*
    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = (gpio_num_t)_sda,
        .scl_io_num = (gpio_num_t)_scl,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
    };
    i2c_conf.master.clk_speed = 400000; // 400kHz
    */

    // ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_1, &i2c_conf));
    // ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_1, i2c_conf.mode, 0, 0, 0));

    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_GSL3680_CONFIG();
    tp_io_config.scl_speed_hz = 400000;
    ESP_LOGD(TAG, "Initialize touch IO (I2C)");
    // esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)I2C_NUM_1, &tp_io_config, &tp_io_handle);
    esp_lcd_new_panel_io_i2c(i2c_bus_handle, &tp_io_config, &tp_io_handle);

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

    ESP_LOGD(TAG, "Initialize touch controller gsl3680");
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_gsl3680(tp_io_handle, &tp_cfg, &tp));
}

bool GSL3680_touch::getTouch(uint16_t *x, uint16_t *y)
{
    
    esp_lcd_touch_read_data(tp);
    bool touchpad_pressed = esp_lcd_touch_get_coordinates(tp, x, y, touch_strength, &touch_cnt, 1);

    return touchpad_pressed;
}

void GSL3680_touch::set_rotation(uint8_t r){
 
switch(r){
    case 0:
        esp_lcd_touch_set_swap_xy(tp, false);   
        esp_lcd_touch_set_mirror_x(tp, true);//false
        esp_lcd_touch_set_mirror_y(tp, false);//false
        break;
    case 1:
        esp_lcd_touch_set_swap_xy(tp, true);
        esp_lcd_touch_set_mirror_x(tp, false);
        esp_lcd_touch_set_mirror_y(tp, false);
        break;
    case 2:
        esp_lcd_touch_set_swap_xy(tp, false);   
        esp_lcd_touch_set_mirror_x(tp, false);
        esp_lcd_touch_set_mirror_y(tp, true);
        break;
    case 3:
        esp_lcd_touch_set_swap_xy(tp, true);   
        esp_lcd_touch_set_mirror_x(tp, true);
        esp_lcd_touch_set_mirror_y(tp, true);
        break;
    }

}