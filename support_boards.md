# Supported Boards

## Display Controller Drivers

| Driver Name | Connection Type | Library Model |
| --- | --- | --- |
| ILI9488 | SPI | ArduinoGFX |
| ESP32-8048S043 | PARALLEL | ArduinoGFX |
| ESP32-4827S043 | PARALLEL | ArduinoGFX |
| ESP32-8048S070 | PARALLEL | ArduinoGFX |
| GC9A01 | SPI | ArduinoGFX |
| ST7735 | SPI | ArduinoGFX |
| ST7789 - SPI | SPI | ArduinoGFX |
| ILI9225 | SPI | ArduinoGFX |
| GM009605 | SPI | GM009605 |
| PDC8544 | I2C | PCD8544 |
| SD1306 | I2C | SD1306 |
| ST7796 | SPI | ArduinoGFX |
| ST7789 - 8bit | PARALLEL | ArduinoGFX |
| ST7565 - U8G2 | SPI | U8G2 |
| ST7262 - Waveshare | SPI | ArduinoGFX |
| JD9365 - DSI | DSI | ArduinoGFX |
| JD9165 - DSI | DSI | ArduinoGFX |
| ST7701 - DSI | DSI | ArduinoGFX |
| ST7789 - EMBED | SPI | ArduinoGFX |
| ST7796 - EMBED | SPI | ArduinoGFX |

## Touch Controller Drivers

| Driver Name | Connection Type | Needs Calibration |
| --- | --- | --- |
| GT911 | I2C | No |
| XPT2046 | SPI | Yes |
| FT6236U | I2C | No |
| CST816 | I2C | No |
| FT6336U | I2C | No |
| GSL3680 | I2C | No |
| CST820 | I2C | No |

## Supported Devboards

| Board Name | Display Driver | Touch Driver |
| --- | --- | --- |
| ESP32S3_8048s043 GT911 | ESP32-8048S043 | GT911 |
| ESP32S3 4.3in XPT2046 | ESP32-4827S043 | XPT2046 |
| ES32S3 7in GT911 | ESP32-8048S070 | GT911 |
| TFT GC9A01 | GC9A01 | NONE |
| TFT 1.44in ST7735 | ST7735 | NONE |
| ST7789 | ST7789 - SPI | NONE |
| ST7735 1.8in | ST7735 | NONE |
| ILI9225 TFT 2.2in | ILI9225 | NONE |
| TFT ILI9488 2.8in | ILI9488 | XPT2046 |
| ST7789 2in BLUE BOARD | ST7789 - SPI | NONE |
| ILI9488 3.5in - FT6236 | ILI9488 | FT6236U |
| ILI9488 3.5in - XPT2046 | ILI9488 | XPT2046 |
| ILI9488 4in | ILI9488 | FT6336U |
| TFT ILI9488 4in | ILI9488 | XPT2046 |
| ST7789 - 8bit | ST7789 - 8bit | NONE |
| PCD8544 | PDC8544 | NONE |
| SSD1306 | SD1306 | NONE |
| ST7565 | ST7565 - U8G2 | NONE |
| Waveshare 4.3in Touch capacitive ST7262 | ST7262 - Waveshare | GT911 |
| Waveshare 10.1in Touch capacitive DSI | JD9365 - DSI | GSL3680 |
| 3.5in capacitive blackboard st7796 | ST7796 | FT6336U |
| Guition 7in Touch capacitive DSI | JD9165 - DSI | GT911 |
| Guition 2.8in Touch capacitive | ST7789 - EMBED | CST820 |
| Guition 4in Touch capacitive DSI | ST7701 - DSI | GT911 |
| 3.5in resistive yellow st7796 esp32 | ST7796 - EMBED | XPT2046 |
