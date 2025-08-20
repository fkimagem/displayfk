#include "wspinbox.h"

#define SPINBOX_DEBUG

#if defined(SPINBOX_DEBUG)
#define DEBUG_D(format, ...) log_d(format, ##__VA_ARGS__)
#define DEBUG_E(format, ...) log_e(format, ##__VA_ARGS__)
#define DEBUG_W(format, ...) log_w(format, ##__VA_ARGS__)
#else
#define DEBUG_D(format, ...) 
#define DEBUG_E(format, ...) 
#define DEBUG_W(format, ...) 
#endif

/**
 * @brief Decreases the current value by the step amount.
 * 
 * Decrements the current value while ensuring it remains within the defined range.
 */
void SpinBox::decreaseValue()
{
    m_currentValue -= m_step;
    m_currentValue = constrain(m_currentValue, m_vmin, m_vmax);
}

/**
 * @brief Increases the current value by the step amount.
 * 
 * Increments the current value while ensuring it remains within the defined range.
 */
void SpinBox::increaseValue()
{
    m_currentValue += m_step;
    m_currentValue = constrain(m_currentValue, m_vmin, m_vmax);
}

/**
 * @brief Constructor for the SpinBox class.
 * @param _x X-coordinate for the SpinBox position.
 * @param _y Y-coordinate for the SpinBox position.
 * @param _screen Screen identifier where the SpinBox will be displayed.
 */
SpinBox::SpinBox(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen),
m_shouldRedraw(true)
{
}

/**
 * @brief Destructor for the SpinBox class.
 */
SpinBox::~SpinBox()
{
}

/**
 * @brief Detects if the SpinBox has been touched and handles increment/decrement actions.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is within the SpinBox area, otherwise false.
 * 
 * Detects touches in either the decrement (left) or increment (right) button areas,
 * and processes the value change accordingly.
 */
bool SpinBox::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
    if (WidgetBase::usingKeyboard || WidgetBase::currentScreen != screen || !loaded)
    {
        return false;
    }

    if (millis() - m_myTime < TIMEOUT_CLICK)
    {
        return false;
    }
    m_myTime = millis();
    bool detectado = false;

    uint16_t topY = {yPos};
    uint16_t bottomY = {yPos + m_height};

    Rect_t boundsAreaDecrement = {xPos, topY, (m_width/2), m_height};
    Rect_t boundsAreaIncrement = {xPos + (m_width/2), topY, (m_width/2), m_height};

    /*uint16_t d_x = xPos;
    uint16_t d_x_max = xPos + m_width;

    uint16_t y_min = yPos;
    uint16_t y_max = yPos + m_height;*/

    //uint16_t i_x = xPos + m_width - m_height;
    //uint16_t i_x_max = xPos + m_width;

    // Detect decrement
    if ((*_xTouch > boundsAreaDecrement.x) && (*_xTouch < boundsAreaDecrement.x + boundsAreaDecrement.width) && (*_yTouch > boundsAreaDecrement.y) && (*_yTouch < boundsAreaDecrement.y + boundsAreaDecrement.height))
    {
        decreaseValue();
        m_shouldRedraw = true;
        detectado = true;
    }

    if(detectado)
    {
        return true;
    }

    // Detect increment
    if ((*_xTouch > boundsAreaIncrement.x) && (*_xTouch < boundsAreaIncrement.x + boundsAreaIncrement.width) && (*_yTouch > boundsAreaIncrement.y) && (*_yTouch < boundsAreaIncrement.y + boundsAreaIncrement.height))
    {
        increaseValue();
        m_shouldRedraw = true;
        detectado = true;
    }

    return detectado;
}

/**
 * @brief Retrieves the callback function associated with the SpinBox.
 * @return Pointer to the callback function.
 */
functionCB_t SpinBox::getCallbackFunc()
{
    return cb;
}

/**
 * @brief Redraws the SpinBox on the screen, updating its appearance.
 * 
 * Updates the display of the current value in the SpinBox,
 * only redrawing if the SpinBox is on the current screen and needs updating.
 */
void SpinBox::redraw()
{
      #if defined(DISP_DEFAULT)
    if (WidgetBase::currentScreen != screen || WidgetBase::usingKeyboard || !loaded || !m_shouldRedraw)
    {
        return;
    }

    m_shouldRedraw = false;
  
    uint16_t btnW = m_height - (2 * m_offset);
    uint16_t btnH = m_height - (2 * m_offset);

    uint16_t availableW = m_width - (2*m_offset);
    uint16_t availableH = m_height - (2*m_offset);

    
    WidgetBase::objTFT->setTextColor(m_textColor);

    WidgetBase::objTFT->fillRoundRect(xPos + (2 * m_offset) + btnW, yPos + m_offset, m_width - (4 * m_offset + 2 * btnW), btnH, m_radius, m_colorBase);
    WidgetBase::objTFT->setFont(getBestRobotoBold(availableW, availableH, String(m_currentValue).c_str()));
    printText(String(m_currentValue).c_str(), xPos + m_width / 2, yPos + (m_height / 2) - 3, MC_DATUM, m_lastArea, m_colorBase);
    updateFont(FontType::UNLOAD);
#endif

}

/**
 * @brief Draws the background of the SpinBox, including static elements.
 * 
 * Creates the visual container, plus and minus buttons for the SpinBox.
 * This is typically called once during setup rather than on every redraw.
 */
void SpinBox::drawBackground()
{
    #if defined(DISP_DEFAULT) 
    if (WidgetBase::currentScreen != screen || WidgetBase::usingKeyboard || !loaded)
    {
        return;
    }

    uint16_t btnW = m_height - (2 * m_offset);
    uint16_t btnH = m_height - (2 * m_offset);

    uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;

    WidgetBase::objTFT->fillRoundRect(xPos, yPos, m_width, m_height, m_radius, m_colorBase);
    WidgetBase::objTFT->drawRoundRect(xPos, yPos, m_width, m_height, m_radius, CFK_BLACK);

    WidgetBase::objTFT->fillRoundRect(xPos + m_offset, yPos + m_offset, btnW, btnH, m_radius, WidgetBase::lightenColor565(m_colorBase, 4));
    WidgetBase::objTFT->fillRoundRect(xPos + m_width - m_offset - btnW, yPos + m_offset, btnW, btnH, m_radius, WidgetBase::lightenColor565(m_colorBase, 4));

    WidgetBase::objTFT->drawRoundRect(xPos + m_offset, yPos + m_offset, btnW, btnH, m_radius, CFK_BLACK);
    WidgetBase::objTFT->drawRoundRect(xPos + m_width - m_offset - btnW, yPos + m_offset, btnW, btnH, m_radius, CFK_BLACK);

    CoordPoint_t btnLeft = {static_cast<uint16_t>(xPos + m_offset + (btnW / 2)), static_cast<uint16_t>(yPos + m_offset + (btnH / 2))};
    CoordPoint_t btnRight = {static_cast<uint16_t>(xPos + m_width - (m_offset + (btnW / 2))), static_cast<uint16_t>(yPos + m_offset + (btnH / 2))};

    //WidgetBase::objTFT->fillCircle(btnLeft.x, btnLeft.y, 4, CFK_BLUE);
    const uint8_t sinalW = btnW/2;
    const uint8_t sinalH = ceil(sinalW/10.0);
    //Sinal menos
    WidgetBase::objTFT->fillRect(btnLeft.x - (sinalW/2), btnLeft.y - (sinalH/2), sinalW, sinalH, m_textColor);

    //Sinal mais
    WidgetBase::objTFT->fillRect(btnRight.x - (sinalW/2), btnRight.y - (sinalH/2), sinalW, sinalH, m_textColor);
    WidgetBase::objTFT->fillRect(btnRight.x - (sinalH/2), btnRight.y - (sinalW/2), sinalH, sinalW, m_textColor);

    m_shouldRedraw = true;
    #endif

    //WidgetBase::objTFT->fillCircle(btnRight.x, btnRight.y, 4, CFK_YELLOW);
}

/**
 * @brief Configures the SpinBox widget with specific dimensions, range, step, and color.
 * @param _width Width of the SpinBox.
 * @param _height Height of the SpinBox.
 * @param _step Step value for the spin box increments and decrements.
 * @param _min Minimum value of the SpinBox range.
 * @param _max Maximum value of the SpinBox range.
 * @param _startValue Current value of the SpinBox.
 * @param _cor Color for the display elements.
 * @param _textCor Text color.
 * @param _cb Callback function to execute when the value changes.
 * 
 * Initializes the SpinBox properties and marks it as loaded when complete.
 */
void SpinBox::setup(uint16_t _width, uint16_t _height, uint16_t _step, int _min, int _max, int _startValue, uint16_t _cor, uint16_t _textCor, functionCB_t _cb)
{
    if (!WidgetBase::objTFT)
    {
        log_e("TFT not defined on WidgetBase");
        return;
    }
    if (loaded)
    {
        log_d("Spinbox widget already configured");
        return;
    }
    m_width = _width;
    m_height = _height;
    m_step = _step;
    m_vmin = _min;
    m_vmax = _max;
    if(m_vmin > m_vmax)
    {
        std::swap(m_vmin, m_vmax);
    }
    m_currentValue = constrain(_startValue, m_vmin, m_vmax);
    m_colorBase = _cor;
    m_textColor = _textCor;
    cb = _cb;

    loaded = true;
}

/**
 * @brief Configures the SpinBox with parameters defined in a configuration structure.
 * @param config Structure containing the SpinBox configuration parameters.
 */
void SpinBox::setup(const SpinBoxConfig& config)
{
  setup(config.width, config.height, config.step, config.minValue, config.maxValue, config.startValue, config.color, config.textColor, config.callback);
}

/**
 * @brief Retrieves the current value of the SpinBox.
 * @return Current numeric value.
 */
int SpinBox::getValue()
{
    return m_currentValue;
}

/**
 * @brief Sets the current value of the SpinBox.
 * @param _value New numeric value to set.
 * 
 * Updates the SpinBox's value, ensures it's within the defined range,
 * and marks the widget for redraw. Triggers the callback if provided.
 */
void SpinBox::setValue(int _value)
{
    if(!loaded)
    {
        log_e("Spinbox widget not loaded");
        return;
    }
    
    m_currentValue = constrain(_value, m_vmin, m_vmax);
    m_shouldRedraw = true;

    if(cb != nullptr){
        WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::SELF);
    }
}
