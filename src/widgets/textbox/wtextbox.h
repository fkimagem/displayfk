#ifndef WTBOX
#define WTBOX

#include "../widgetbase.h"
#include "../../extras/charstring.h"

/// @brief Configuration struct for TextBox setup
struct TextBoxConfig {
    uint16_t width;            ///< Width of the TextBox
    uint16_t height;           ///< Height of the TextBox
    uint16_t letterColor;      ///< Text color for displaying the value
    uint16_t backgroundColor;  ///< Background color of the TextBox
    const char* startValue;    ///< Initial text value to display
    #if defined(USING_GRAPHIC_LIB)
    const GFXfont* font;       ///< Font to use for the text
    #endif
    functionLoadScreen_t funcPtr; ///< Function pointer to the parent screen's load function
    functionCB_t cb;           ///< Callback function to execute on interaction
};

/// @brief Represents a text box widget for displaying and editing text.
class TextBox : public WidgetBase
{
private:
    unsigned long m_myTime; ///< Timestamp for handling timing-related functions.
    uint16_t m_width; ///< Width of the TextBox.
    uint16_t m_height; ///< Height of the TextBox.
    uint16_t m_backgroundColor; ///< Background color of the TextBox.
    uint16_t m_letterColor; ///< Text color for displaying the value.
    CharString m_value; ///< String representing the current text value.
    #if defined(USING_GRAPHIC_LIB)
    const GFXfont *m_font; ///< Font to use for the text.
    #endif
    uint8_t m_padding; ///< Padding of the TextBox.
    bool m_shouldRedraw; ///< Flag to indicate if the TextBox should be redrawn.

    #if defined(USING_GRAPHIC_LIB)
    void setup(uint16_t _width, uint16_t _height, uint16_t _letterColor, uint16_t _backgroundColor, const char* _startValue, const GFXfont* _font, functionLoadScreen_t _funcPtr, functionCB_t _cb);
    #endif

public:
    functionLoadScreen_t parentScreen; ///< Pointer to the function for loading the parent screen.

    TextBox(uint16_t _x, uint16_t _y, uint8_t _screen);
    TextBox();
    ~TextBox();
    bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
    functionCB_t getCallbackFunc() override;
    void redraw();
    void forceUpdate();
    void setup(const TextBoxConfig& config);
    void setValue(const char* str);
    const char* getValue();
};

#endif