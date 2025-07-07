
#define DISP_DEFAULT

#define DISP_CS	10 //CS, SS, nSS
#define DISP_MISO	-1 //MISO, SDO, SOMI
#define DISP_MOSI	11 //MOSI, SDI, SIMO
#define DISP_SCLK	12 //SCLK, SCK, CLK
#define DISP_RST	9 //RST, RES, RESET
#define DISP_DC	8 //DC, RS, A0
#define DISP_FREQUENCY	27000000

#define HAS_TOUCH true
#define TOUCH_FT6236U
#define TOUCH_FT6X36_SCL 14 //21
#define TOUCH_FT6X36_SDA 13 //19
#define TOUCH_MAP_X1 0
#define TOUCH_MAP_X2 480
#define TOUCH_MAP_Y1 320
#define TOUCH_MAP_Y2 0
#define TOUCH_SWAP_XY

#pragma message("Compiling for board P6")