#ifndef WIDGETSETUP_H
#define WIDGETSETUP_H

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
 //#define DFK_EXTERNALINPUT 1

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
#define CFK_NO_COLOR	0x0000
#define CFK_BLACK	0x0000
#define CFK_WHITE	0xffff
#define CFK_IVORY	0xfffe
#define CFK_YELLOW	0xffe0
#define CFK_GOLD	0xfea0
#define CFK_ORANGE	0xfd20
#define CFK_CORAL	0xfbea
#define CFK_TOMATO	0xfb08
#define CFK_RED	0xf800
#define CFK_MAROON	0x8000
#define CFK_LAVENDER	0xe73f
#define CFK_PINK	0xfe19
#define CFK_DEEPPINK	0xf8b2
#define CFK_PLUM	0xdd1b
#define CFK_VIOLET	0xec1d
#define CFK_FUCHSIA	0xf81f
#define CFK_PURPLE	0x8010
#define CFK_INDIGO	0x4810
#define CFK_LIME	0x07e0
#define CFK_GREEN	0x0400
#define CFK_OLIVE	0x8400
#define CFK_AQUA	0x07ff
#define CFK_SKYBLUE	0x867d
#define CFK_TEAL	0x0410
#define CFK_BLUE	0x001f
#define CFK_NAVY	0x0010
#define CFK_SILVER	0xc618
#define CFK_GRAY	0x8410
#define CFK_WHEAT	0xf6f6
#define CFK_TAN	0xd5b1
#define CFK_CHOCOLATE	0xd343
#define CFK_SADDLEBROWN	0x8a22

// Defines for color palette
#define CFK_NO_COLOR	0x0040
#define CFK_BLACK	0x0000
#define CFK_WHITE	0xFFFF
#define CFK_COLOR01	0xF800
#define CFK_COLOR02	0xF980
#define CFK_COLOR03	0xFB00
#define CFK_COLOR04	0xFC60
#define CFK_COLOR05	0xFDE0
#define CFK_COLOR06	0xFF60
#define CFK_COLOR07	0xDFE0
#define CFK_COLOR08	0xAFE0
#define CFK_COLOR09	0x87E0
#define CFK_COLOR10	0x57E0
#define CFK_COLOR11	0x27E0
#define CFK_COLOR12	0x07E2
#define CFK_COLOR13	0x07E8
#define CFK_COLOR14	0x07EE
#define CFK_COLOR15	0x07F3
#define CFK_COLOR16	0x07F9
#define CFK_COLOR17	0x07FF
#define CFK_COLOR18	0x067F
#define CFK_COLOR19	0x04FF
#define CFK_COLOR20	0x039F
#define CFK_COLOR21	0x021F
#define CFK_COLOR22	0x009F
#define CFK_COLOR23	0x201F
#define CFK_COLOR24	0x501F
#define CFK_COLOR25	0x801F
#define CFK_COLOR26	0xA81F
#define CFK_COLOR27	0xD81F
#define CFK_COLOR28	0xF81D
#define CFK_COLOR29	0xF817
#define CFK_COLOR30	0xF811
#define CFK_COLOR31	0xF80C
#define CFK_COLOR32	0xF806
#define CFK_GREY1	0x1082
#define CFK_GREY2	0x2104
#define CFK_GREY3	0x3186
#define CFK_GREY4	0x4228
#define CFK_GREY5	0x52AA
#define CFK_GREY6	0x632C
#define CFK_GREY7	0x73AE
#define CFK_GREY8	0x8C51
#define CFK_GREY9	0x9CD3
#define CFK_GREY10	0xAD55
#define CFK_GREY11	0xBDD7
#define CFK_GREY12	0xCE79
#define CFK_GREY13	0xDEFB
#define CFK_GREY14	0xEF7D


#define FONT_ROBOTO
#define FONT_DIGIT


#endif