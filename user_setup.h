//#define PLACA_P1  // ESP32S3 4.3" + XPT2046
//#define PLACA_P6    // ESP32S3 + ILI9488 + FT6236U base azul
//#define PLACA_P13 // ESP32S3 7.0" + GT911
//#define PLACA_P14 // ESP32S3 4.3" + GT911
//#define PLACA_P15 // ESP32S3 4.3" + XPT2046
//#define PLACA_P9  // Display vermelho tft 4", base azul
//#define PLACA_P10 // Com display amarelo ILI9488 e FT6336
#define PLACA_CUSTOM //Defina seus proprios pinos

//#define PLACA_ST7735 1
/*
#if defined(PLACA_P1)
#include "examples/Geral/P1/board_p1.h"

#elif defined(PLACA_P6)
#include "examples/Geral/P6/board_p6.h"

#elif defined(PLACA_P10)
#include "examples/Geral/P10/board_p10.h"

#elif defined(PLACA_P13)
#include "examples/Geral/P13/board_p13.h"

#elif defined(PLACA_P14)
#include "examples/Geral/P14/board_p14.h"

#elif defined(PLACA_P15)
#include "examples/Geral/P15/board_p15.h"

#elif defined(PLACA_P9)
#include "examples/Geral/P9/board_p9.h"

#elif defined(PLACA_ST7735)
#include "setup_boards/board_st7735.h"

#elif defined(PLACA_CUSTOM)
// Paste here your defines
#define DISP_DEFAULT
#define TOUCH_XPT2046
#define INVERT_COLOR false

#else 	
#error "Defina a placa no arquivo user_setup.h"

#endif
*/

#define DISP_DEFAULT	
//#define DISP_PCD8544
//#define DISP_SSD1306
//#define DISP_U8G2
//#define TOUCH_FT6236U
//#define TOUCH_FT6336	
#define TOUCH_XPT2046
//#define TOUCH_GT911	
#define HAS_TOUCH
//#define INVERT_COLOR_BITS
//#define IS_BGR