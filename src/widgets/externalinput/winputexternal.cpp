#include "winputexternal.h"

/**
 * @brief Constructor for the InputExternal class.
 * @param _x X-coordinate for the input field position.
 * @param _y Y-coordinate for the input field position.
 * @param _screen Screen identifier where the input field will be displayed.
 */
InputExternal::InputExternal(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen)
{
    m_width = 100;
    m_height = 30;
    m_shouldRedraw = true;
    m_loaded = false;
}

/**
 * @brief Destructor for the InputExternal class.
 */
InputExternal::~InputExternal()
{
}

/**
 * @brief Detects if the InputExternal widget has been touched.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is within the input field area, otherwise false.
 * 
 * Checks if the touch is within the input field boundaries and not already using keyboard.
 */
bool InputExternal::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
    if (WidgetBase::currentScreen != screen || !loaded)
  {
    return false;
  }

  if (millis() - m_myTime < TIMEOUT_CLICK)
  {
    return false;
  }
  m_myTime = millis();

  // bool detected = false;
  uint16_t xMax = xPos + m_width;
  uint16_t yMax = yPos + m_height;

  if ((*_xTouch > xPos) && (*_xTouch < xMax) && (*_yTouch > yPos) && (*_yTouch < yMax) && WidgetBase::usingKeyboard == false)
  {
    //WidgetBase::usingKeyboard = true;
    log_d("Open keyboard");

    return true;
  }

  return false;
}

/**
 * @brief Retrieves the callback function associated with the InputExternal widget.
 * @return Pointer to the callback function.
 */
functionCB_t InputExternal::getCallbackFunc()
{
    return cb;
}

/**
 * @brief Redraws the InputExternal widget on the screen, updating its appearance.
 * 
 * Displays the input field with the current value and updates the visual representation.
 * Only redraws if the widget is on the current screen and needs updating.
 */
void InputExternal::redraw()
{
  if(!visible){return;}
  #if defined(USING_GRAPHIC_LIB)
    if (WidgetBase::currentScreen != screen || !loaded || !m_shouldRedraw)
  {
    return;
  }

  m_shouldRedraw = false;

  
  if (m_font)
  {
    WidgetBase::objTFT->setFont(m_font);
  }
  else
  {
    updateFont(FontType::NORMAL);
  }
  

  log_d("Redraw textbox with value %s", m_value.getString());

  TextBound_t area;
  WidgetBase::objTFT->getTextBounds("M", xPos, yPos, &area.x, &area.y, &area.width, &area.height);

  WidgetBase::objTFT->fillRect(xPos, yPos, m_width, m_height, m_backgroundColor);
  WidgetBase::objTFT->drawRect(xPos, yPos, m_width, m_height, m_letterColor);
  WidgetBase::objTFT->setTextColor(m_letterColor);

  uint16_t maxLetters = m_width / area.width;
  const char *content = m_value.getLastChars(maxLetters);

  log_d("Draw %d letters from %s in space %d", maxLetters, content, m_width);

  printText(content, xPos + m_padding, yPos + m_height / 2, ML_DATUM);

  updateFont(FontType::UNLOAD);
  #endif
}

/**
 * @brief Forces an immediate update of the InputExternal widget.
 * 
 * Sets the flag to redraw the input field on the next redraw cycle.
 */
void InputExternal::forceUpdate()
{
    m_shouldRedraw = true;
}

/**
 * @brief Configures the InputExternal widget with parameters defined in a configuration structure.
 * @param config Structure containing the input field configuration parameters.
 * 
 * Initializes the input field properties and marks it as loaded when complete.
 */
void InputExternal::setup(const InputExternalConfig& config)
{
  #if defined(USING_GRAPHIC_LIB)
    if(m_loaded){
        log_d("InputExternal already loaded");
        return;
    }
    m_width = config.width;
    m_height = config.height;
    m_letterColor = config.letterColor;
    m_backgroundColor = config.backgroundColor;
    m_value = config.startValue;
    m_font = config.font;
    m_funcPtr = config.funcPtr;
    cb = config.cb;
    m_loaded = true;
    #else
    UNUSED(config);
    #endif
}

/**
 * @brief Sets the current value of the InputExternal widget.
 * @param str New string value to display in the input field.
 */
void InputExternal::setValue(const char* str)
{
    m_value = str;
}

/**
 * @brief Retrieves the current value of the InputExternal widget as a C-style string.
 * @return Pointer to the current string value.
 */
const char* InputExternal::getValueCChar()
{
    return m_value.getLastChars(m_width);
}

/**
 * @brief Retrieves the current value of the InputExternal widget as an integer.
 * @return Integer value of the current string.
 */
int InputExternal::getValueInt()
{
    return m_value.toInt();
}

/**
 * @brief Retrieves the current value of the InputExternal widget as a float.
 * @return Float value of the current string.
 */
float InputExternal::getValueFloat()
{
    return m_value.toFloat();
}


