//#ifndef WIDGETBASE
//#define WIDGETBASE
#pragma once

#include <Arduino.h>
#include <esp32-hal.h>
#include <stdint.h>

#include "../user_setup.h"
#include "../extras/baseTypes.h"
#include "../extras/wutils.h"
#include "widgetsetup.h"

#if defined(DISP_DEFAULT)
#include <Arduino_GFX_Library.h>
#elif defined(DISP_PCD8544)
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#elif defined(DISP_SSD1306)
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#elif defined(DISP_U8G2)
#include <Arduino.h>
#include <U8g2lib.h>
#else
#error "Unsupported display type"
#endif

#if defined(FONT_ROBOTO)
#include "../fonts/RobotoRegular/RobotoRegular2pt7b.h"
#include "../fonts/RobotoRegular/RobotoRegular3pt7b.h"
#include "../fonts/RobotoRegular/RobotoRegular4pt7b.h"
#include "../fonts/RobotoRegular/RobotoRegular5pt7b.h"
#include "../fonts/RobotoRegular/RobotoRegular6pt7b.h"
#include "../fonts/RobotoRegular/RobotoRegular7pt7b.h"
#include "../fonts/RobotoRegular/RobotoRegular8pt7b.h"
#include "../fonts/RobotoRegular/RobotoRegular9pt7b.h"
#include "../fonts/RobotoRegular/RobotoRegular10pt7b.h"
#include "../fonts/RobotoRegular/RobotoRegular11pt7b.h"
#include "../fonts/RobotoRegular/RobotoRegular12pt7b.h"
#include "../fonts/RobotoRegular/RobotoRegular13pt7b.h"
#include "../fonts/RobotoRegular/RobotoRegular14pt7b.h"
#include "../fonts/RobotoRegular/RobotoRegular15pt7b.h"
#include "../fonts/RobotoRegular/RobotoRegular20pt7b.h"
#include "../fonts/RobotoRegular/RobotoRegular25pt7b.h"
#include "../fonts/RobotoRegular/RobotoRegular30pt7b.h"
#include "../fonts/RobotoRegular/RobotoRegular40pt7b.h"
#include "../fonts/RobotoRegular/RobotoRegular50pt7b.h"

#include "../fonts/RobotoBold/RobotoBold2pt7b.h"
#include "../fonts/RobotoBold/RobotoBold3pt7b.h"
#include "../fonts/RobotoBold/RobotoBold4pt7b.h"
#include "../fonts/RobotoBold/RobotoBold5pt7b.h"
#include "../fonts/RobotoBold/RobotoBold6pt7b.h"
#include "../fonts/RobotoBold/RobotoBold7pt7b.h"
#include "../fonts/RobotoBold/RobotoBold8pt7b.h"
#include "../fonts/RobotoBold/RobotoBold9pt7b.h"
#include "../fonts/RobotoBold/RobotoBold10pt7b.h"
#include "../fonts/RobotoBold/RobotoBold11pt7b.h"
#include "../fonts/RobotoBold/RobotoBold12pt7b.h"
#include "../fonts/RobotoBold/RobotoBold13pt7b.h"
#include "../fonts/RobotoBold/RobotoBold14pt7b.h"
#include "../fonts/RobotoBold/RobotoBold15pt7b.h"
#include "../fonts/RobotoBold/RobotoBold20pt7b.h"
#include "../fonts/RobotoBold/RobotoBold25pt7b.h"
#include "../fonts/RobotoBold/RobotoBold30pt7b.h"
#include "../fonts/RobotoBold/RobotoBold40pt7b.h"
#include "../fonts/RobotoBold/RobotoBold50pt7b.h"

#endif

#if defined(FONT_SEGMENT7)
#include "../fonts/Segment/G7_Segment7_S52pt7b.h"
#include "../fonts/Segment/G7_Segment7_S53pt7b.h"
#include "../fonts/Segment/G7_Segment7_S54pt7b.h"
#include "../fonts/Segment/G7_Segment7_S55pt7b.h"
#include "../fonts/Segment/G7_Segment7_S56pt7b.h"
#include "../fonts/Segment/G7_Segment7_S57pt7b.h"
#include "../fonts/Segment/G7_Segment7_S58pt7b.h"
#include "../fonts/Segment/G7_Segment7_S59pt7b.h"
#include "../fonts/Segment/G7_Segment7_S510pt7b.h"
#include "../fonts/Segment/G7_Segment7_S511pt7b.h"
#include "../fonts/Segment/G7_Segment7_S512pt7b.h"
#include "../fonts/Segment/G7_Segment7_S513pt7b.h"
#include "../fonts/Segment/G7_Segment7_S514pt7b.h"
#include "../fonts/Segment/G7_Segment7_S515pt7b.h"
#include "../fonts/Segment/G7_Segment7_S520pt7b.h"
#include "../fonts/Segment/G7_Segment7_S525pt7b.h"
#include "../fonts/Segment/G7_Segment7_S530pt7b.h"
#include "../fonts/Segment/G7_Segment7_S540pt7b.h"
#include "../fonts/Segment/G7_Segment7_S550pt7b.h"
#endif

#if defined(FONT_LED)
#include "../fonts/Led/LedReal2pt7b.h"
#include "../fonts/Led/LedReal3pt7b.h"
#include "../fonts/Led/LedReal4pt7b.h"
#include "../fonts/Led/LedReal5pt7b.h"
#include "../fonts/Led/LedReal6pt7b.h"
#include "../fonts/Led/LedReal7pt7b.h"
#include "../fonts/Led/LedReal8pt7b.h"
#include "../fonts/Led/LedReal9pt7b.h"
#include "../fonts/Led/LedReal10pt7b.h"
#include "../fonts/Led/LedReal11pt7b.h"
#include "../fonts/Led/LedReal12pt7b.h"
#include "../fonts/Led/LedReal13pt7b.h"
#include "../fonts/Led/LedReal14pt7b.h"
#include "../fonts/Led/LedReal15pt7b.h"
#include "../fonts/Led/LedReal20pt7b.h"
#include "../fonts/Led/LedReal25pt7b.h"
#include "../fonts/Led/LedReal30pt7b.h"
#include "../fonts/Led/LedReal40pt7b.h"
#include "../fonts/Led/LedReal50pt7b.h"
#endif

#if defined(FONT_MUSIC)
#include "../fonts/MusicNet/MUSICNET2pt7b.h"
#include "../fonts/MusicNet/MUSICNET3pt7b.h"
#include "../fonts/MusicNet/MUSICNET4pt7b.h"
#include "../fonts/MusicNet/MUSICNET5pt7b.h"
#include "../fonts/MusicNet/MUSICNET6pt7b.h"
#include "../fonts/MusicNet/MUSICNET7pt7b.h"
#include "../fonts/MusicNet/MUSICNET8pt7b.h"
#include "../fonts/MusicNet/MUSICNET9pt7b.h"
#include "../fonts/MusicNet/MUSICNET10pt7b.h"
#include "../fonts/MusicNet/MUSICNET11pt7b.h"
#include "../fonts/MusicNet/MUSICNET12pt7b.h"
#include "../fonts/MusicNet/MUSICNET13pt7b.h"
#include "../fonts/MusicNet/MUSICNET14pt7b.h"
#include "../fonts/MusicNet/MUSICNET15pt7b.h"
#include "../fonts/MusicNet/MUSICNET20pt7b.h"
#include "../fonts/MusicNet/MUSICNET25pt7b.h"
#include "../fonts/MusicNet/MUSICNET30pt7b.h"
#include "../fonts/MusicNet/MUSICNET40pt7b.h"
#include "../fonts/MusicNet/MUSICNET50pt7b.h"

#endif

#if defined(FONT_NOKIAN)
#include "../fonts/Nokian/Nokian2pt7b.h"
#include "../fonts/Nokian/Nokian3pt7b.h"
#include "../fonts/Nokian/Nokian4pt7b.h"
#include "../fonts/Nokian/Nokian5pt7b.h"
#include "../fonts/Nokian/Nokian6pt7b.h"
#include "../fonts/Nokian/Nokian7pt7b.h"
#include "../fonts/Nokian/Nokian8pt7b.h"
#include "../fonts/Nokian/Nokian9pt7b.h"
#include "../fonts/Nokian/Nokian10pt7b.h"
#include "../fonts/Nokian/Nokian11pt7b.h"
#include "../fonts/Nokian/Nokian12pt7b.h"
#include "../fonts/Nokian/Nokian13pt7b.h"
#include "../fonts/Nokian/Nokian14pt7b.h"
#include "../fonts/Nokian/Nokian15pt7b.h"
#include "../fonts/Nokian/Nokian20pt7b.h"
#include "../fonts/Nokian/Nokian25pt7b.h"
#include "../fonts/Nokian/Nokian30pt7b.h"
#include "../fonts/Nokian/Nokian40pt7b.h"
#include "../fonts/Nokian/Nokian50pt7b.h"
#endif

/*
#define FONT_PIXEL
#define FONT_SCORED
#define FONT_SCREENMATRIX
#define FONT_URBAN
#define FONT_VACATION

*/

#if defined(FONT_PIXEL)
#include "../fonts/Pixel/Pixel3D2pt7b.h"
#include "../fonts/Pixel/Pixel3D3pt7b.h"
#include "../fonts/Pixel/Pixel3D4pt7b.h"
#include "../fonts/Pixel/Pixel3D5pt7b.h"
#include "../fonts/Pixel/Pixel3D6pt7b.h"
#include "../fonts/Pixel/Pixel3D7pt7b.h"
#include "../fonts/Pixel/Pixel3D8pt7b.h"
#include "../fonts/Pixel/Pixel3D9pt7b.h"
#include "../fonts/Pixel/Pixel3D10pt7b.h"
#include "../fonts/Pixel/Pixel3D11pt7b.h"
#include "../fonts/Pixel/Pixel3D12pt7b.h"
#include "../fonts/Pixel/Pixel3D13pt7b.h"
#include "../fonts/Pixel/Pixel3D14pt7b.h"
#include "../fonts/Pixel/Pixel3D15pt7b.h"
#include "../fonts/Pixel/Pixel3D20pt7b.h"
#include "../fonts/Pixel/Pixel3D25pt7b.h"
#include "../fonts/Pixel/Pixel3D30pt7b.h"
#include "../fonts/Pixel/Pixel3D40pt7b.h"
#include "../fonts/Pixel/Pixel3D50pt7b.h"
#endif

#if defined(FONT_SCORED)
#include "../fonts/Scored/Scoreboard2pt7b.h"
#include "../fonts/Scored/Scoreboard3pt7b.h"
#include "../fonts/Scored/Scoreboard4pt7b.h"
#include "../fonts/Scored/Scoreboard5pt7b.h"
#include "../fonts/Scored/Scoreboard6pt7b.h"
#include "../fonts/Scored/Scoreboard7pt7b.h"
#include "../fonts/Scored/Scoreboard8pt7b.h"
#include "../fonts/Scored/Scoreboard9pt7b.h"
#include "../fonts/Scored/Scoreboard10pt7b.h"
#include "../fonts/Scored/Scoreboard11pt7b.h"
#include "../fonts/Scored/Scoreboard12pt7b.h"
#include "../fonts/Scored/Scoreboard13pt7b.h"
#include "../fonts/Scored/Scoreboard14pt7b.h"
#include "../fonts/Scored/Scoreboard15pt7b.h"
#include "../fonts/Scored/Scoreboard20pt7b.h"
#include "../fonts/Scored/Scoreboard25pt7b.h"
#include "../fonts/Scored/Scoreboard30pt7b.h"
#include "../fonts/Scored/Scoreboard40pt7b.h"
#include "../fonts/Scored/Scoreboard50pt7b.h"

#endif

#if defined(FONT_SCREENMATRIX)
#include "../fonts/ScreenMatrix/ScreenMatrix2pt7b.h"
#include "../fonts/ScreenMatrix/ScreenMatrix3pt7b.h"
#include "../fonts/ScreenMatrix/ScreenMatrix4pt7b.h"
#include "../fonts/ScreenMatrix/ScreenMatrix5pt7b.h"
#include "../fonts/ScreenMatrix/ScreenMatrix6pt7b.h"
#include "../fonts/ScreenMatrix/ScreenMatrix7pt7b.h"
#include "../fonts/ScreenMatrix/ScreenMatrix8pt7b.h"
#include "../fonts/ScreenMatrix/ScreenMatrix9pt7b.h"
#include "../fonts/ScreenMatrix/ScreenMatrix10pt7b.h"
#include "../fonts/ScreenMatrix/ScreenMatrix11pt7b.h"
#include "../fonts/ScreenMatrix/ScreenMatrix12pt7b.h"
#include "../fonts/ScreenMatrix/ScreenMatrix13pt7b.h"
#include "../fonts/ScreenMatrix/ScreenMatrix14pt7b.h"
#include "../fonts/ScreenMatrix/ScreenMatrix15pt7b.h"
#include "../fonts/ScreenMatrix/ScreenMatrix20pt7b.h"
#include "../fonts/ScreenMatrix/ScreenMatrix25pt7b.h"
#include "../fonts/ScreenMatrix/ScreenMatrix30pt7b.h"
#include "../fonts/ScreenMatrix/ScreenMatrix40pt7b.h"
#include "../fonts/ScreenMatrix/ScreenMatrix50pt7b.h"
#endif

#if defined(FONT_URBAN)
#include "../fonts/Urban/Urban2pt7b.h"
#include "../fonts/Urban/Urban3pt7b.h"
#include "../fonts/Urban/Urban4pt7b.h"
#include "../fonts/Urban/Urban5pt7b.h"
#include "../fonts/Urban/Urban6pt7b.h"
#include "../fonts/Urban/Urban7pt7b.h"
#include "../fonts/Urban/Urban8pt7b.h"
#include "../fonts/Urban/Urban9pt7b.h"
#include "../fonts/Urban/Urban10pt7b.h"
#include "../fonts/Urban/Urban11pt7b.h"
#include "../fonts/Urban/Urban12pt7b.h"
#include "../fonts/Urban/Urban13pt7b.h"
#include "../fonts/Urban/Urban14pt7b.h"
#include "../fonts/Urban/Urban15pt7b.h"
#include "../fonts/Urban/Urban20pt7b.h"
#include "../fonts/Urban/Urban25pt7b.h"
#include "../fonts/Urban/Urban30pt7b.h"
#include "../fonts/Urban/Urban40pt7b.h"
#include "../fonts/Urban/Urban50pt7b.h"
#endif

#if defined(FONT_VACATION)
#include "../fonts/Vacation/Vacation2pt7b.h"
#include "../fonts/Vacation/Vacation3pt7b.h"
#include "../fonts/Vacation/Vacation4pt7b.h"
#include "../fonts/Vacation/Vacation5pt7b.h"
#include "../fonts/Vacation/Vacation6pt7b.h"
#include "../fonts/Vacation/Vacation7pt7b.h"
#include "../fonts/Vacation/Vacation8pt7b.h"
#include "../fonts/Vacation/Vacation9pt7b.h"
#include "../fonts/Vacation/Vacation10pt7b.h"
#include "../fonts/Vacation/Vacation11pt7b.h"
#include "../fonts/Vacation/Vacation12pt7b.h"
#include "../fonts/Vacation/Vacation13pt7b.h"
#include "../fonts/Vacation/Vacation14pt7b.h"
#include "../fonts/Vacation/Vacation15pt7b.h"
#include "../fonts/Vacation/Vacation20pt7b.h"
#include "../fonts/Vacation/Vacation25pt7b.h"
#include "../fonts/Vacation/Vacation30pt7b.h"
#include "../fonts/Vacation/Vacation40pt7b.h"
#include "../fonts/Vacation/Vacation50pt7b.h"

#endif



#if defined(DFK_SD)
#include "SD.h"
#endif

#define HELPERS

// START SECTION - MACROS FOR VALIDATION
#define CHECK_TFT_BOOL {if(!WidgetBase::objTFT){return false;}}
#define CHECK_TFT_VOID {if(!WidgetBase::objTFT){return;}}

#define CHECK_VISIBLE_VOID {if(!m_visible){return;}}
#define CHECK_VISIBLE_BOOL {if(!m_visible){return false;}}

#define CHECK_DEBOUNCE_CLICK_BOOL {if(millis() - m_myTime < TIMEOUT_CLICK){return false;}}
#define CHECK_DEBOUNCE_CLICK_VOID {if(millis() - m_myTime < TIMEOUT_CLICK){return;}}

#define CHECK_DEBOUNCE_REDRAW_BOOL {if(millis() - m_myTime < TIMEOUT_REDRAW){return false;}}
#define CHECK_DEBOUNCE_REDRAW_VOID {if(millis() - m_myTime < TIMEOUT_REDRAW){return;}}

#define CHECK_DEBOUNCE_FAST_REDRAW_BOOL {if(millis() - m_myTime < TIMEOUT_FAST_REDRAW){return false;}}
#define CHECK_DEBOUNCE_FAST_REDRAW_VOID {if(millis() - m_myTime < TIMEOUT_FAST_REDRAW){return;}}

#define CHECK_ENABLED_BOOL {if(!m_enabled){ESP_LOGW(TAG, "Widget is disabled"); return false;}}
#define CHECK_ENABLED_VOID {if(!m_enabled){ESP_LOGW(TAG, "Widget is disabled"); return;}}

#define CHECK_LOCKED_BOOL {if(m_locked){ESP_LOGW(TAG, "Widget is locked"); return false;}}
#define CHECK_LOCKED_VOID {if(m_locked){ESP_LOGW(TAG, "Widget is locked"); return;}}

#define CHECK_INITIALIZED_BOOL {if(!m_initialized){return false;}}
#define CHECK_INITIALIZED_VOID {if(!m_initialized){return;}}

#define CHECK_LOADED_BOOL {if(!m_loaded){return false;}}
#define CHECK_LOADED_VOID {if(!m_loaded){return;}}

#define CHECK_USINGKEYBOARD_BOOL {if(WidgetBase::usingKeyboard){return false;}}
#define CHECK_USINGKEYBOARD_VOID {if(WidgetBase::usingKeyboard){return;}}

#define CHECK_CURRENTSCREEN_BOOL {if(WidgetBase::currentScreen != m_screen){return false;}}
#define CHECK_CURRENTSCREEN_VOID {if(WidgetBase::currentScreen != m_screen){return;}}

#define CHECK_SHOULDREDRAW_BOOL {if(!m_shouldRedraw){return false;}}
#define CHECK_SHOULDREDRAW_VOID {if(!m_shouldRedraw){return;}}

#define CHECK_POINTER_TOUCH_NULL_BOOL {if(_xTouch == nullptr || _yTouch == nullptr){ ESP_LOGW(TAG, "Touch coordinates are null"); return false;}}
#define CHECK_POINTER_TOUCH_NULL_VOID {if(_xTouch == nullptr || _yTouch == nullptr){ ESP_LOGW(TAG, "Touch coordinates are null"); return;}}

// END SECTION - MACROS FOR VALIDATION


/// @brief Abstract base class for all widgets.
/// @note This class cannot be instantiated directly. It must be inherited by concrete widget classes.
class WidgetBase
{
private:
  static const char* TAG; ///< Tag for logging identification
  static uint16_t convertToRGB565(uint8_t r, uint8_t g, uint8_t b);
  static void extract565toRGB(uint16_t color565, uint8_t &r, uint8_t &g, uint8_t &b);

public:
  enum class CallbackOrigin{
    TOUCH = 1,
    SELF = 2
  };

  static QueueHandle_t xFilaCallback; ///< Queue for callbacks
  static bool usingKeyboard;    ///< True if using a virtual keyboard.
  static uint8_t currentScreen; ///< Number of the current screen.
  static bool showingLog;       ///< True if log is being displayed.
  #if defined(USING_GRAPHIC_LIB)
  static const GFXfont *fontNormal; ///< Pointer to the normal font.
  static const GFXfont *fontBold;   ///< Pointer to the bold font.
  #endif
  static void addCallback(functionCB_t callback, CallbackOrigin origin);
  static uint16_t screenWidth;             ///< Width of the screen.
  static uint16_t screenHeight;            ///< Height of the screen.
  static bool lightMode;                   ///< True for light mode, false for dark mode.
  static functionLoadScreen_t loadScreen; ///< Pointer to the function that loads the screen.
  static uint16_t backgroundColor;         ///< Background color for the widget.
  

  static uint16_t lightenColor565(unsigned short color, float factor);
  static uint16_t lightenToWhite565(uint16_t color, float factor);
  static uint16_t darkenColor565(unsigned short color, float factor);
  static uint16_t blendColors(uint16_t color1, uint16_t color2, float factor);

#if defined(DISP_DEFAULT)
  static Arduino_GFX *objTFT; ///< Pointer to the Arduino display object.
#elif defined(DISP_PCD8544)
  static Adafruit_PCD8544 *objTFT; ///< Pointer to the PCD8544 display object.
#elif defined(DISP_SSD1306)
  static Adafruit_SSD1306 *objTFT; ///< Pointer to the SSD1306 display object.
#elif defined(DISP_U8G2)
  static U8G2 *objTFT; ///< Pointer to the U8G2 display object.
#else
#error "Choose a display model on user_setup.h"
#endif

  

#if defined(DFK_SD)
  static fs::SDFS *mySD; ///< Pointer to the SD file system.
#endif

  WidgetBase(uint16_t _x, uint16_t _y, uint8_t _screen);

  virtual ~WidgetBase();
  virtual bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) = 0;
  virtual functionCB_t getCallbackFunc() = 0;
  virtual void show() = 0;
  virtual void hide() = 0;
  virtual void forceUpdate() = 0;
  virtual void redraw() = 0;
  
  bool showingMyScreen();
  
  // State management methods
  bool isInitialized() const;
  bool isEnabled() const;
  void setEnabled(bool enabled);
  bool isValidState() const;
  bool validateParameters() const;
  
  // Lock management methods
  void lock();
  void unlock();
  bool isLocked() const;
  
#if defined(USING_GRAPHIC_LIB)
  static void recalculateTextPosition(const char* _texto, uint16_t *_x, uint16_t *_y, uint8_t _datum);
  #endif

protected:
  bool m_visible = true;        ///< True if the widget is visible
  uint16_t m_xPos;              ///< X position of the widget
  uint16_t m_yPos;              ///< Y position of the widget
  uint8_t m_screen;             ///< Screen index for the widget
  bool m_loaded = false;        ///< True if the widget has been initialized
  bool m_enabled = true;        ///< True if the widget is enabled
  bool m_initialized = false;   ///< True if the widget is properly initialized
  bool m_shouldRedraw = true;   ///< Flag to indicate if the widget should be redrawn
  bool m_locked = false;        ///< True if the widget is locked (prevents interaction)
  unsigned long m_myTime = 0;   ///< Timestamp for handling timing-related functions (debounce, etc.)
  functionCB_t m_callback = nullptr; ///< Callback function to execute when the widget is clicked

#if defined(USING_GRAPHIC_LIB)
  const GFXfont* getBestRobotoBold(uint16_t availableWidth, uint16_t availableHeight, const char* texto);
  const GFXfont* getBestFontForArea(const char* text,uint16_t width,uint16_t height,const GFXfont* const fonts[],size_t fontCount);
  const char* getFirstLettersForSpace(const char* textoCompleto, uint16_t width, uint16_t height);
  const char* getLastLettersForSpace(const char* textoCompleto,uint16_t width,uint16_t height);
#endif

  void updateFont(FontType _f);

#if defined(USING_GRAPHIC_LIB)
  void printText(const char* _texto, uint16_t _x, uint16_t _y, uint8_t _datum, TextBound_t &lastTextBoud, uint16_t _colorPadding);
  void printText(const char* _texto, uint16_t _x, uint16_t _y, uint8_t _datum);
  TextBound_t getTextBounds(const char* str, int16_t x, int16_t y);
  void drawRotatedImageOptimized(uint16_t *image, int16_t width, int16_t height, float angle, int16_t pivotX, int16_t pivotY, int16_t drawX, int16_t drawY);
#endif

  void showOrigin(uint16_t color);
};

//#endif