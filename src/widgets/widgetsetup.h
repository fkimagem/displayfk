#ifndef WIDGETSETUP_H
#define WIDGETSETUP_H

#include <cstdint>
#include "../../user_setup.h"
#define INVERTE_BITS_16(x) ((uint16_t)(~(x)))

#ifndef UNUSED
#define UNUSED(x) (void)x;
#endif

#define POINT_IN_RECT(px, py, rx, ry, rw, rh) \
    ((px) >= (rx) && (px) <= ((rx) + (rw)) && (py) >= (ry) && (py) <= ((ry) + (rh)))

#define POINT_IN_CIRCLE(px, py, cx, cy, r) \
    (((px) - (cx)) * ((px) - (cx)) + ((py) - (cy)) * ((py) - (cy)) <= (r) * (r))

constexpr uint16_t invert_bits(uint16_t val) {
	
#ifdef INVERT_COLORS_BITS
    return ~val;
#else
    return val;
#endif
}

#if defined(IS_BGR)
constexpr uint16_t rgb2brg(uint16_t val) {
    // troca R e B no formato RGB565
    return ((val & 0x001F) << 11) |     // B -> R
           (val & 0x07E0) |            // G mantido
           ((val & 0xF800) >> 11);     // R -> B
}
#else
constexpr uint16_t rgb2brg(uint16_t val) {
    return val;
}
#endif


#if defined(DISP_DEFAULT)
constexpr uint16_t process_color(uint16_t val) {
    return rgb2brg(invert_bits(val));
}
#elif defined(DISP_PCD8544) || defined(DISP_SSD1306)
constexpr uint16_t process_color(uint16_t val) {
    return (val == 0x0000) ? 0x1 : 0x0;
}
#elif defined(DISP_U8G2)
constexpr uint16_t process_color(uint16_t val) {
    return (val != 0x0000) ? 0x1 : 0x0;
}
#endif

#define DFK_TOUCHAREA 1
#define DFK_CIRCLEBTN 1
#define DFK_RECTBTN 1
#define DFK_LED 1
#define DFK_VBAR 1
#define DFK_TOGGLE 1
#define DFK_HSLIDER 1
#define DFK_TEXTBOX 1
#define DFK_CHECKBOX 1
#define DFK_RADIO 1
#define DFK_NUMBERBOX 1
#define DFK_VANALOG 1
#define DFK_GAUGE 1
#define DFK_LINECHART 1
#define DFK_LABEL 1
#define DFK_IMAGE 1
#define DFK_SD 1
#define DFK_SPINBOX 1
#define DFK_TEXTBUTTON 1
#define DFK_CIRCULARBAR 1
#define DFK_THERMOMETER 1
//#define DFK_EXTERNALINPUT 1

#define USE_SPIFFS 1
#define USE_FATFS 1

#if defined(DISP_PCD8544) || defined(DISP_SSD1306) || defined(DISP_U8G2)
#undef DFK_TOUCHAREA
#undef DFK_CIRCLEBTN
#undef DFK_RECTBTN
#undef DFK_TOGGLE
#undef DFK_HSLIDER
#undef DFK_TEXTBOX
#undef DFK_CHECKBOX
#undef DFK_RADIO
#undef DFK_NUMBERBOX
#undef DFK_VANALOG
#undef DFK_GAUGE
#undef DFK_LINECHART
#undef DFK_SPINBOX
#undef DFK_TEXTBUTTON
#undef DFK_CIRCULARBAR
#endif

#if defined(DISP_U8G2)
#undef DFK_LABEL
#undef DFK_THERMOMETER
#undef DFK_VBAR
#undef DFK_LED
#endif

#if defined(DISP_DEFAULT) || defined(DISP_PCD8544) || defined(DISP_SSD1306)
#define USING_GRAPHIC_LIB
#endif

//These enumerate the text plotting alignment (reference datum point)
#define TL_DATUM 0 // Top left (default)
#define TC_DATUM 1 // Top centre
#define TR_DATUM 2 // Top right
#define ML_DATUM 3 // Middle left
#define CL_DATUM 3 // Centre left, same as above
#define MC_DATUM 4 // Middle centre
#define CC_DATUM 4 // Centre centre, same as above
#define MR_DATUM 5 // Middle right
#define CR_DATUM 5 // Centre right, same as above
#define BL_DATUM 6 // Bottom left
#define BC_DATUM 7 // Bottom centre
#define BR_DATUM 8 // Bottom right
#define L_BASELINE  9 // Left character baseline (Line the 'A' character would sit on)
#define C_BASELINE 10 // Centre character baseline
#define R_BASELINE 11 // Right character baseline

// Default color definitions

// Declarações:
constexpr uint16_t CFK_NO_COLOR     = process_color(0x0000);
constexpr uint16_t CFK_BLACK        = process_color(0x0000);
constexpr uint16_t CFK_WHITE        = process_color(0xffff);
constexpr uint16_t CFK_IVORY        = process_color(0xfffe);
constexpr uint16_t CFK_YELLOW       = process_color(0xffe0);
constexpr uint16_t CFK_GOLD         = process_color(0xfea0);
constexpr uint16_t CFK_ORANGE       = process_color(0xfd20);
constexpr uint16_t CFK_CORAL        = process_color(0xfbea);
constexpr uint16_t CFK_TOMATO       = process_color(0xfb08);
constexpr uint16_t CFK_RED          = process_color(0xf800);
constexpr uint16_t CFK_MAROON       = process_color(0x8000);
constexpr uint16_t CFK_LAVENDER     = process_color(0xe73f);
constexpr uint16_t CFK_PINK         = process_color(0xfe19);
constexpr uint16_t CFK_DEEPPINK     = process_color(0xf8b2);
constexpr uint16_t CFK_PLUM         = process_color(0xdd1b);
constexpr uint16_t CFK_VIOLET       = process_color(0xec1d);
constexpr uint16_t CFK_FUCHSIA      = process_color(0xf81f);
constexpr uint16_t CFK_PURPLE       = process_color(0x8010);
constexpr uint16_t CFK_INDIGO       = process_color(0x4810);
constexpr uint16_t CFK_LIME         = process_color(0x07e0);
constexpr uint16_t CFK_GREEN        = process_color(0x0400);
constexpr uint16_t CFK_OLIVE        = process_color(0x8400);
constexpr uint16_t CFK_AQUA         = process_color(0x07ff);
constexpr uint16_t CFK_SKYBLUE      = process_color(0x867d);
constexpr uint16_t CFK_TEAL         = process_color(0x0410);
constexpr uint16_t CFK_BLUE         = process_color(0x001f);
constexpr uint16_t CFK_NAVY         = process_color(0x0010);
constexpr uint16_t CFK_SILVER       = process_color(0xc618);
constexpr uint16_t CFK_GRAY         = process_color(0x8410);
constexpr uint16_t CFK_WHEAT        = process_color(0xf6f6);
constexpr uint16_t CFK_TAN          = process_color(0xd5b1);
constexpr uint16_t CFK_CHOCOLATE    = process_color(0xd343);
constexpr uint16_t CFK_SADDLEBROWN  = process_color(0x8a22);

// Defines for color palette
//#define INVERTE_BITS_16(x) ((uint16_t)(~(x)))

constexpr uint16_t CFK_COLOR01   = process_color(0xF800);
constexpr uint16_t CFK_COLOR02   = process_color(0xF980);
constexpr uint16_t CFK_COLOR03   = process_color(0xFB00);
constexpr uint16_t CFK_COLOR04   = process_color(0xFC60);
constexpr uint16_t CFK_COLOR05   = process_color(0xFDE0);
constexpr uint16_t CFK_COLOR06   = process_color(0xFF60);
constexpr uint16_t CFK_COLOR07   = process_color(0xDFE0);
constexpr uint16_t CFK_COLOR08   = process_color(0xAFE0);
constexpr uint16_t CFK_COLOR09   = process_color(0x87E0);
constexpr uint16_t CFK_COLOR10   = process_color(0x57E0);
constexpr uint16_t CFK_COLOR11   = process_color(0x27E0);
constexpr uint16_t CFK_COLOR12   = process_color(0x07E2);
constexpr uint16_t CFK_COLOR13   = process_color(0x07E8);
constexpr uint16_t CFK_COLOR14   = process_color(0x07EE);
constexpr uint16_t CFK_COLOR15   = process_color(0x07F3);
constexpr uint16_t CFK_COLOR16   = process_color(0x07F9);
constexpr uint16_t CFK_COLOR17   = process_color(0x07FF);
constexpr uint16_t CFK_COLOR18   = process_color(0x067F);
constexpr uint16_t CFK_COLOR19   = process_color(0x04FF);
constexpr uint16_t CFK_COLOR20   = process_color(0x039F);
constexpr uint16_t CFK_COLOR21   = process_color(0x021F);
constexpr uint16_t CFK_COLOR22   = process_color(0x009F);
constexpr uint16_t CFK_COLOR23   = process_color(0x201F);
constexpr uint16_t CFK_COLOR24   = process_color(0x501F);
constexpr uint16_t CFK_COLOR25   = process_color(0x801F);
constexpr uint16_t CFK_COLOR26   = process_color(0xA81F);
constexpr uint16_t CFK_COLOR27   = process_color(0xD81F);
constexpr uint16_t CFK_COLOR28   = process_color(0xF81D);
constexpr uint16_t CFK_COLOR29   = process_color(0xF817);
constexpr uint16_t CFK_COLOR30   = process_color(0xF811);
constexpr uint16_t CFK_COLOR31   = process_color(0xF80C);
constexpr uint16_t CFK_COLOR32   = process_color(0xF806);

constexpr uint16_t CFK_GREY1     = process_color(0x1082);
constexpr uint16_t CFK_GREY2     = process_color(0x2104);
constexpr uint16_t CFK_GREY3     = process_color(0x3186);
constexpr uint16_t CFK_GREY4     = process_color(0x4228);
constexpr uint16_t CFK_GREY5     = process_color(0x52AA);
constexpr uint16_t CFK_GREY6     = process_color(0x632C);
constexpr uint16_t CFK_GREY7     = process_color(0x73AE);
constexpr uint16_t CFK_GREY8     = process_color(0x8C51);
constexpr uint16_t CFK_GREY9     = process_color(0x9CD3);
constexpr uint16_t CFK_GREY10    = process_color(0xAD55);
constexpr uint16_t CFK_GREY11    = process_color(0xBDD7);
constexpr uint16_t CFK_GREY12    = process_color(0xCE79);
constexpr uint16_t CFK_GREY13    = process_color(0xDEFB);
constexpr uint16_t CFK_GREY14    = process_color(0xEF7D);


#if defined(USING_GRAPHIC_LIB)

#define FONT_ROBOTO
#define FONT_SEGMENT7
#define FONT_LED
#define FONT_MUSIC
#define FONT_NOKIAN
#define FONT_PIXEL
#define FONT_SCORED
#define FONT_SCREENMATRIX
#define FONT_URBAN
#define FONT_VACATION



#endif

#endif