#ifndef WNUMPAD
#define WNUMPAD

#include "../widgetbase.h"
#include "wnumberbox.h"
#include "../../extras/charstring.h"
#if defined(USING_GRAPHIC_LIB)
#include "../../fonts/RobotoRegular/RobotoRegular10pt7b.h"
#endif


/// @brief Number of rows in the numpad grid
#define NROWS 4

/// @brief Number of columns in the numpad grid
#define NCOLS 4

/// @brief Represents a numeric keypad widget, allowing number input through an on-screen pad.
class Numpad : public WidgetBase 
{
private:
    /// @brief Y-coordinate starting point for the Numpad layout
    uint32_t m_yStart = 0;
    
    /// @brief Width and height of individual keys on the Numpad
    uint32_t m_keyW, m_keyH;
    
    /// @brief Timestamp for handling timing-related functions
    unsigned long m_myTime;
    
    /// @brief Number of rows on the Numpad
    static const uint16_t aRows = NROWS;
    
    /// @brief Number of columns on the Numpad
    static const uint16_t aCols = NCOLS;
    
    /// @brief 2D array defining the characters displayed on the Numpad keys
    static const Key_t m_pad[NROWS][NCOLS];

    

    /// @brief Screen width available for the Numpad
    int32_t m_screenW;
    
    /// @brief Screen height available for the Numpad
    int32_t m_screenH;
    
    /// @brief Width available for drawing the Numpad
    uint16_t m_availableWidth;
    
    /// @brief Height available for drawing the Numpad
    uint16_t m_availableHeight;
    
    /// @brief Holds the current content being input on the Numpad
    CharString m_content;

    /// @brief Preview area for displaying the input
    Rect_t m_pontoPreview;

    /// @brief Last calculated area for the label
    TextBound_t m_lastArea = {0, 0, 0, 0};

      #if defined(USING_GRAPHIC_LIB)
    GFXfont *m_fontKeys = nullptr;
    GFXfont *m_fontPreview = nullptr;
    #endif


    /// @brief Adds a character to the current Numpad input
    /// @param c Character to add
    void addLetter(char c);

    /// @brief Removes the last character from the current Numpad input
    void removeLetter();

public:
    static uint16_t m_backgroundColor;
    static uint16_t m_letterColor;
    static uint16_t m_keyColor;

    /// @brief Pointer to the NumberBox associated with the Numpad
    NumberBox *m_field = nullptr;

    Numpad(uint16_t _x, uint16_t _y, uint8_t _screen);

    Numpad();

    ~Numpad();

    bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;

    functionCB_t getCallbackFunc() override;
    
    bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch, PressedKeyType *pressedKey);

    void redraw(bool fullScreen, bool onlyContent);

    void setup();

    void open(NumberBox *_field);

    void close();

    void insertChar(char c);
};

#endif