#ifndef WNBOX
#define WNBOX

#include "../widgetbase.h"
#include "../../extras/charstring.h"

/// @brief Configuration structure for NumberBox
struct NumberBoxConfig {
  uint16_t width;                ///< Width of the NumberBox
  uint16_t height;               ///< Height of the NumberBox
  uint16_t letterColor;          ///< Color for the text displaying the value
  uint16_t backgroundColor;      ///< Background color of the NumberBox
  float startValue;              ///< Initial numeric value to display
  #if defined(USING_GRAPHIC_LIB)
  const GFXfont* font;           ///< Font to use for the text
  #endif
  functionLoadScreen_t funcPtr;  ///< Function pointer to the parent screen's load function
  functionCB_t callback;         ///< Callback function to execute on interaction
};

/// @brief Represents a number box widget for displaying and editing numeric values.
class NumberBox : public WidgetBase
{
private:
    unsigned long m_myTime; ///< Timestamp for timing-related functions.
    uint16_t m_width; ///< Width of the NumberBox.
    uint16_t m_height; ///< Height of the NumberBox.
    uint16_t m_backgroundColor; ///< Background color of the NumberBox.
    uint16_t m_letterColor; ///< Text color for displaying the value.
    CharString m_value; ///< String representing the current numeric value.
    #if defined(USING_GRAPHIC_LIB)
    const GFXfont* m_font; ///< Font to use for the text.
    #endif
    uint8_t m_padding; ///< Padding of the NumberBox.
    bool m_shouldRedraw = false; ///< Flag to indicate if the NumberBox should be redrawn.

    #if defined(USING_GRAPHIC_LIB)
    void setup(uint16_t _width, uint16_t _height, uint16_t _letterColor, uint16_t _backgroundColor, float _startValue, const GFXfont* _font, functionLoadScreen_t _funcPtr, functionCB_t _cb);
    #endif
public:
    functionLoadScreen_t parentScreen; ///< Pointer to the function for loading the parent screen.

    NumberBox(uint16_t _x, uint16_t _y, uint8_t _screen);
    NumberBox();
    ~NumberBox();
    bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
    functionCB_t getCallbackFunc() override;
    void redraw();
    void forceUpdate();
    void setup(const NumberBoxConfig& config);
    void setValue(float str);
    float getValue();
    const char* getValueChar();
    const char* convertoToChar(float f);
    void show() override;
  void hide() override;
};
#endif