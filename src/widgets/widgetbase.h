//#ifndef WIDGETBASE
//#define WIDGETBASE
#pragma once

#include <esp32-hal.h>
#include <stdint.h>

#include "../extras/baseTypes.h"
#include "../extras/wutils.h"
#include "widgetsetup.h"
#include "../user_setup.h"

#if defined(DISP_DEFAULT)
#include <Arduino_GFX_Library.h>
#endif

#if defined(FONT_ROBOTO)
#include "../fonts/RobotoRegular/Roboto_Regular5pt7b.h"
#include "../fonts/RobotoRegular/Roboto_Regular10pt7b.h"
#include "../fonts/RobotoRegular/Roboto_Regular15pt7b.h"
#include "../fonts/RobotoRegular/Roboto_Regular20pt7b.h"
#include "../fonts/RobotoRegular/Roboto_Regular25pt7b.h"
#include "../fonts/RobotoRegular/Roboto_Regular30pt7b.h"
#include "../fonts/RobotoRegular/Roboto_Regular40pt7b.h"
#include "../fonts/RobotoRegular/Roboto_Regular50pt7b.h"

#include "../fonts/RobotoBold/Roboto_Bold5pt7b.h"
#include "../fonts/RobotoBold/Roboto_Bold10pt7b.h"
#include "../fonts/RobotoBold/Roboto_Bold15pt7b.h"
#include "../fonts/RobotoBold/Roboto_Bold20pt7b.h"
#include "../fonts/RobotoBold/Roboto_Bold25pt7b.h"
#include "../fonts/RobotoBold/Roboto_Bold30pt7b.h"
#include "../fonts/RobotoBold/Roboto_Bold40pt7b.h"
#include "../fonts/RobotoBold/Roboto_Bold50pt7b.h"

#endif

#if defined(FONT_DIGIT)
#include "../fonts/Segment/G7_Segment7_S510pt7b.h"
#endif



#if defined(DFK_SD)
#include "SD.h"
#endif

#define HELPERS
//#define DEBUG_TEXT_BOUND


/// @brief Base class for all widgets.
class WidgetBase
{
private:
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
  static const GFXfont *fontNormal; ///< Pointer to the normal font.
  static const GFXfont *fontBold;   ///< Pointer to the bold font.
  static void addCallback(functionCB_t callback, CallbackOrigin origin);
  

  static uint16_t lightenColor565(unsigned short color, float factor);
  static uint16_t lightenToWhite565(uint16_t color, float factor);
  static uint16_t darkenColor565(unsigned short color, float factor);
  static uint16_t blendColors(uint16_t color1, uint16_t color2, float factor);

#if defined(DISP_DEFAULT)
  static Arduino_GFX *objTFT; ///< Pointer to the Arduino display object.
#else
#error "Choose a display model on user_setup.h"
#endif


  static uint16_t screenWidth;             ///< Width of the screen.
  static uint16_t screenHeight;            ///< Height of the screen.
  static bool lightMode;                   ///< True for light mode, false for dark mode.
  static functionLoadScreen_t loadScreen; ///< Pointer to the function that loads the screen.
  static uint16_t backgroundColor;         ///< Background color for the widget.

#if defined(DFK_SD)
  static fs::SDFS *mySD; ///< Pointer to the SD file system.
#endif

  WidgetBase(uint16_t _x, uint16_t _y, uint8_t _screen);

  virtual ~WidgetBase();

  virtual bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch);

  virtual functionCB_t getCallbackFunc();
#if defined(DISP_DEFAULT)
  static void recalculateTextPosition(const char* _texto, uint16_t *_x, uint16_t *_y, uint8_t _datum);
  #endif

protected:
  uint16_t xPos;              ///< X position of the widget.
  uint16_t yPos;              ///< Y position of the widget.
  uint8_t screen;             ///< Screen index for the widget.
  bool loaded = false;        ///< True if the widget has been initialized.
  functionCB_t cb = nullptr; ///< Callback function to execute when the widget is clicked.


  const GFXfont* GetBestRobotoBold(uint16_t availableWidth, uint16_t availableHeight, const char* texto);


  void updateFont(FontType _f);

#if defined(DISP_DEFAULT)
  void printText(const char* _texto, uint16_t _x, uint16_t _y, uint8_t _datum, TextBound_t &lastTextBoud, uint16_t _colorPadding);
  void printText(const char* _texto, uint16_t _x, uint16_t _y, uint8_t _datum);

  

  TextBound_t getTextBounds(const char* str, int16_t x, int16_t y);

  void drawRotatedImageOptimized(uint16_t *image, int16_t width, int16_t height, float angle, int16_t pivotX, int16_t pivotY, int16_t drawX, int16_t drawY);
#endif

  void showOrigin(uint16_t color);
};

//#endif