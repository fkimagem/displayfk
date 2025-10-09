#include "wexternalkeyboard.h"

/**
 * @brief Constructor for the ExternalKeyboard class.
 * @param _x X-coordinate for the keyboard position.
 * @param _y Y-coordinate for the keyboard position.
 * @param _screen Screen identifier where the keyboard will be displayed.
 */
ExternalKeyboard::ExternalKeyboard(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen)
{
    
}

/**
 * @brief Destructor for the ExternalKeyboard class.
 */
ExternalKeyboard::~ExternalKeyboard()
{
}

void ExternalKeyboard::show()
{
    visible = true;
    m_shouldRedraw = true;
}

void ExternalKeyboard::hide()
{
    visible = false;
    m_shouldRedraw = true;
}


