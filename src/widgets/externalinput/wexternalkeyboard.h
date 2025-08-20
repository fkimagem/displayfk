#ifndef WEXTERNALKEYBOARD_H
#define WEXTERNALKEYBOARD_H

#include "../widgetbase.h"
#include "winputexternal.h"
#include "../../extras/charstring.h"

#if defined(USING_GRAPHIC_LIB)
#include "../../fonts/RobotoRegular/RobotoRegular10pt7b.h"
#endif


/// @brief Represents an external keyboard widget for handling external keypad input.
class ExternalKeyboard : public WidgetBase
{
public:
    /**
     * @brief Constructor for the ExternalKeyboard class.
     * @param _x X-coordinate for the keyboard position.
     * @param _y Y-coordinate for the keyboard position.
     * @param _screen Screen identifier where the keyboard will be displayed.
     */
    ExternalKeyboard(uint16_t _x, uint16_t _y, uint8_t _screen);
    
    /**
     * @brief Destructor for the ExternalKeyboard class.
     */
    ~ExternalKeyboard();
    

private:
    // Private members will be added as needed
    
    
};
#endif