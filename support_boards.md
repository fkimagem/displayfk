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

| Board Name | Display Driver | Touch Driver | Image |
| --- | --- | --- | --- |
| ESP32S3_8048s043 GT911 | ESP32-8048S043 | GT911 | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FESP32S3_TFT_4-3_GT911_AMARELO.png?alt=media&token=8794655d-22c9-4a65-a677-3bf6561552bb) |
| ESP32S3 4.3in XPT2046 | ESP32-4827S043 | XPT2046 | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FESP32S3_TFT_4-3_XPT2046_AMARELO.png?alt=media&token=6b59243c-448b-48e3-86b6-a5f41b0f2546) |
| ES32S3 7in GT911 | ESP32-8048S070 | GT911 | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FESP32S3_TFT_7_GT911_AMARELO.png?alt=media&token=6d75ef16-d6a6-4121-ba45-01aa256a825f) |
| TFT GC9A01 | GC9A01 | NONE | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FTFT_1-28_GC9A01.png?alt=media&token=dcb46a78-7c08-47d5-ab3d-ecad82359574) |
| TFT 1.44in ST7735 | ST7735 | NONE | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FTFT_1-44_ST7735.png?alt=media&token=a273f318-4aa5-4f95-b4a1-f5a1f5ef6b2e) |
| ST7789 | ST7789 - SPI | NONE | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FTFT_1-54_ST7789_Blue.png?alt=media&token=034c324e-4cd7-4256-80e4-7dba63a44aae) |
| ST7735 1.8in | ST7735 | NONE | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FTFT_1-8_ST7735.png?alt=media&token=7c7a9937-b25f-4a89-b8cb-f8236cb9aa9e) |
| ILI9225 TFT 2.2in | ILI9225 | NONE | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FTFT_2-2_ILI9225.png?alt=media&token=7fce4936-007b-4611-bd46-56502df2c451) |
| TFT ILI9488 2.8in | ILI9488 | XPT2046 | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FTFT_2-8_ILI9488_XPT2046.png?alt=media&token=f89c0c7e-ec19-4bc8-8396-8a2256650625) |
| ST7789 2in BLUE BOARD | ST7789 - SPI | NONE | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FTFT_2_ST7789_Blue.png?alt=media&token=738aed6b-c833-4b0e-a143-dcd07af2be42) |
| ILI9488 3.5in - FT6236 | ILI9488 | FT6236U | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FTFT_3-5_ILI9488_FT6236V_Capacitive.png?alt=media&token=f464d1f1-53ff-4f89-9590-f4f9ad8ce25d) |
| ILI9488 3.5in - XPT2046 | ILI9488 | XPT2046 | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FTFT_3-5_ILI9488_XPT2046.png?alt=media&token=009d89fd-e44d-4e67-8e54-4729606d1a40) |
| ILI9488 4in | ILI9488 | FT6336U | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FTFT_4_ILI9488_FT6336U_AMARELO.png?alt=media&token=2f834f18-9061-4fab-b32f-20cc041930a7) |
| TFT ILI9488 4in | ILI9488 | XPT2046 | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FTFT_4_ILI9488_XPT2046.png?alt=media&token=c0bf7779-db11-4714-a253-cccc88070db0) |
| ST7789 - 8bit | ST7789 - 8bit | NONE | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FLilygo_S3_ST7789.png?alt=media&token=b946aa57-a5ac-4bf5-836a-a29696fdb212) |
| PCD8544 | PDC8544 | NONE | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FNOKIA_PDC8544.png?alt=media&token=5f575484-8396-4ee5-bcbc-7983f5437d14) |
| SSD1306 | SD1306 | NONE | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FI2C_0-96_GM009605.png?alt=media&token=41d9d505-759d-4a3f-8717-4cd77971cd1d) |
| ST7565 | ST7565 - U8G2 | NONE | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FGMG12864.png?alt=media&token=8f194a08-1b64-4539-a07c-6a3a33178a16) |
| Waveshare 4.3in Touch capacitive ST7262 | ST7262 - Waveshare | GT911 | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FWaveshare_S3_GT911.png?alt=media&token=3a975712-a96e-45a1-be3f-4bc07b26668b) |
| Waveshare 10.1in Touch capacitive DSI | JD9365 - DSI | GSL3680 | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FEsp32P4_JD9365_GSL3680.png?alt=media&token=725d0f91-3f7c-432c-b932-70196936df89) |
| 3.5in capacitive blackboard st7796 | ST7796 | FT6336U | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FTFT_3-5_ST7796_FT6336U_BLACK.png?alt=media&token=17e3a238-8116-447d-bcb9-46ab159370cc) |
| Guition 7in Touch capacitive DSI | JD9165 - DSI | GT911 | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FESP32PT_TFT_JD9165_7_GT911_AMARELO.png?alt=media&token=7b16aeaa-f3ab-4296-86e4-25854dbddc0a) |
| Guition 2.8in Touch capacitive | ST7789 - EMBED | CST820 | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FESP32_TFT_ST7789_2-8_CST820_AMARELO.png?alt=media&token=ff9eb45e-9ef8-4857-a39e-caf2074424d1) |
| Guition 4in Touch capacitive DSI | ST7701 - DSI | GT911 | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FEsp32_P4_JC4880P443C.png?alt=media&token=9c91ad00-e7af-451b-b897-3082ba7868c5) |
| 3.5in resistive yellow st7796 esp32 | ST7796 - EMBED | XPT2046 | ![Board](https://firebasestorage.googleapis.com/v0/b/apishopify-e8511.firebasestorage.app/o/devkit_boards%2FESP32WROOM_TFT_3-5_XPT2046_AMARELO.png?alt=media&token=953c9701-e080-42ed-9801-80b7b804e9c5) |
