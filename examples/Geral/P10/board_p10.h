
#define DISP_DEFAULT

#define DISP_CS     10 //CS, SS, nSS
#define DISP_MISO	-1 //MISO, SDO, SOMI
#define DISP_MOSI	11 //MOSI, SDI, SIMO
#define DISP_SCLK	12 //SCLK, SCK, CLK
#define DISP_RST	9 //RST, RES, RESET
#define DISP_DC     8 //DC, RS, A0

#define HAS_TOUCH true
#define TOUCH_FT6336U
#define TOUCH_FT6336_SCL    14 //21
#define TOUCH_FT6336_SDA    13 //19
#define TOUCH_FT6336_INT    18 //20
#define TOUCH_FT6336_RST    17 //22
#define TOUCH_MAP_X0 320
#define TOUCH_MAP_Y0 0
#define TOUCH_MAP_Y1 480
#define TOUCH_MAP_X1 0
//#define TOUCH_SWAP_XY
#define DISP_FREQUENCY	27000000
#define TOUCH_FREQUENCY	2500000

#pragma message("Compiling for board P10")