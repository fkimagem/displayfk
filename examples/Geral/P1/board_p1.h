//DISP FERNAnDO S3 - placa P1_ESP32_S3_TFT3.5 - base amarela
#define DISP_DEFAULT
#define DISP_CS	10 //CS, SS, nSS
#define DISP_MISO	13 //MISO, SDO, SOMI
#define DISP_MOSI	11 //MOSI, SDI, SIMO
#define DISP_SCLK	12 //SCLK, SCK, CLK
#define DISP_RST	9 //RST, RES, RESET
#define DISP_DC	8 //DC, RS, A0
#define DISP_FREQUENCY	27000000

#define HAS_TOUCH true
#define TOUCH_XPT2046
#define TC_CS	21 //CS, SS, nSS
#define TOUCH_FREQUENCY	2500000
#define TOUCH_INVERT_X false
#define TOUCH_INVERT_Y false

#pragma message("Compiling for board P1")