#ifndef WINPUTEXTERNAL_H
#define WINPUTEXTERNAL_H

#include "../widgetbase.h"
#include "../../extras/charstring.h"

struct InputExternalConfig {
    uint16_t width; ///< Width of the input field
    uint16_t height; ///< Height of the input field
    uint16_t letterColor;      ///< Text color for displaying the value
    uint16_t backgroundColor;  ///< Background color of the TextBox
    const char* startValue;    ///< Initial text value to display
    #if defined(USING_GRAPHIC_LIB)
    const GFXfont* font;       ///< Font to use for the text
    #endif
    functionLoadScreen_t funcPtr; ///< Function pointer to the parent screen's load function
    functionCB_t cb;           ///< Callback function to execute on interaction
};

class InputExternal : public WidgetBase
{
    public:
    functionLoadScreen_t parentScreen;

    InputExternal(uint16_t _x, uint16_t _y, uint8_t _screen);
    ~InputExternal();
    bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
    functionCB_t getCallbackFunc() override;
    void redraw();
    void forceUpdate();
    void setup(const InputExternalConfig& config);
    void setValue(const char* str);
    const char* getValueCChar();
    int getValueInt();
    float getValueFloat();

private:
    bool m_shouldRedraw;
    bool m_loaded;
    uint8_t m_padding;
    unsigned long m_myTime;
    uint16_t m_width;
    uint16_t m_height;
    uint16_t m_letterColor;
    uint16_t m_backgroundColor;
    CharString m_value;
    #if defined(USING_GRAPHIC_LIB)
    const GFXfont* m_font;
    #endif
    functionLoadScreen_t m_funcPtr;
    
};

#endif

