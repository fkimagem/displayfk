#include "wnumberbox.h"

/**
 * @brief Converts a float value to a string representation.
 * @param f Float value to convert.
 * @return Pointer to the string representation of the float.
 * 
 * Note: This function uses a static buffer, so its return value is only valid until the next call.
 */
const char *NumberBox::convertoToChar(float f)
{
  char str[50];
  snprintf(str, sizeof(str), "%f", f);
  log_d("Converting to const char %s", str);
  return (const char *)str;
}

/**
 * @brief Constructs a NumberBox widget at a specified position on a specified screen.
 * @param _x X-coordinate for the NumberBox position.
 * @param _y Y-coordinate for the NumberBox position.
 * @param _screen Screen identifier where the NumberBox will be displayed.
 */
NumberBox::NumberBox(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen),
m_padding(3),
m_shouldRedraw(true)
{
}

/**
 * @brief Default constructor for NumberBox.
 * 
 * Creates a NumberBox at position (0,0) on screen 0.
 */
NumberBox::NumberBox() : WidgetBase(0, 0, 0)
{
}

/**
 * @brief Destructor for the NumberBox class.
 */
NumberBox::~NumberBox()
{
}

/**
 * @brief Detects if the NumberBox has been touched.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is within the NumberBox area, otherwise false.
 * 
 * When touched, activates the virtual keyboard mode for value input.
 */
bool NumberBox::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
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

  bool detectado = false;
  uint16_t xMax = xPos + m_width;
  uint16_t yMax = yPos + m_height;

  if ((*_xTouch > xPos) && (*_xTouch < xMax) && (*_yTouch > yPos) && (*_yTouch < yMax) && WidgetBase::usingKeyboard == false)
  {
    WidgetBase::usingKeyboard = true;
    log_d("Open keyboard");

    detectado = true;
  }

  return detectado;
}

/**
 * @brief Retrieves the callback function associated with the NumberBox.
 * @return Pointer to the callback function.
 */
functionCB_t NumberBox::getCallbackFunc()
{
  return cb;
}

/**
 * @brief Redraws the NumberBox on the screen, updating its appearance.
 * 
 * Displays the current numeric value with appropriate formatting and styling.
 * Only redraws if the NumberBox is on the current screen and needs updating.
 */
void NumberBox::redraw()
{
  #if defined(DISP_DEFAULT) || defined(DISP_PCD8544)
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

  // uint16_t darkBg = WidgetBase::lightMode ? CFK_GREY3 : CFK_GREY11;
  // uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
  //uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

  log_d("Redraw textbox with value %s", m_value.getString());
  WidgetBase::objTFT->fillRect(xPos, yPos, m_width, m_height, m_backgroundColor);
  WidgetBase::objTFT->drawRect(xPos, yPos, m_width, m_height, m_letterColor);
  WidgetBase::objTFT->setTextColor(m_letterColor);

  
  //TextBound_t area;
  //WidgetBase::objTFT->getTextBounds("M", xPos, yPos, &area.x, &area.y, &area.width, &area.height);
  //uint16_t qtdLetrasMax = m_width / area.width;
  //const char* conteudo = m_value.getFirstChars(qtdLetrasMax);
  //printText(conteudo, xPos + m_padding, yPos + m_height/2, ML_DATUM);
  const char* conteudo = getFirstLettersForSpace(m_value.getString(), m_width * 0.9, m_height * 0.9);

  //log_d("Draw %d letters from %s in space %d", qtdLetrasMax, conteudo, m_width);
  printText(conteudo, xPos + m_padding, yPos + m_height / 2, ML_DATUM);


  updateFont(FontType::UNLOAD);
  #endif
}

/**
 * @brief Forces the NumberBox to redraw.
 * 
 * Sets the flag to redraw the NumberBox on the next redraw cycle.
 */
void NumberBox::forceUpdate()
{
  m_shouldRedraw = true;
}

/**
 * @brief Configures the NumberBox widget with specific dimensions, colors, and initial value.
 * @param _width Width of the NumberBox.
 * @param _height Height of the NumberBox.
 * @param _letterColor Color for the text displaying the value.
 * @param _backgroundColor Background color of the NumberBox.
 * @param _startValue Initial numeric value to display.
 * @param _font Font to use for the text.
 * @param _funcPtr Function pointer to the parent screen's load function.
 * @param _cb Callback function to execute on interaction.
 * 
 * Initializes the NumberBox properties and marks it as loaded when complete.
 */
void NumberBox::setup(uint16_t _width, uint16_t _height, uint16_t _letterColor, uint16_t _backgroundColor, float _startValue, const GFXfont* _font, functionLoadScreen_t _funcPtr, functionCB_t _cb)
{
  if (!WidgetBase::objTFT)
  {
    log_e("TFT not defined on WidgetBase");
    return;
  }
  if (loaded)
  {
    log_e("Numberbox already intialized");
    return;
  }

  parentScreen = _funcPtr;
  cb = _cb;

  m_width = _width;
  m_height = _height;
  m_font = _font;
  
  #if defined(DISP_DEFAULT) || defined(DISP_PCD8544)
  if (_font)
  {
    WidgetBase::objTFT->setFont(_font);
    TextBound_t area;
    WidgetBase::objTFT->getTextBounds("Mp", xPos, yPos, &area.x, &area.y, &area.width, &area.height);
    m_height = area.height + (m_padding * 2);
  }
  #endif

  m_letterColor = _letterColor;
  m_backgroundColor = _backgroundColor;
  m_value.setString(_startValue);
  
  loaded = true;
}

/**
 * @brief Configures the NumberBox with parameters defined in a configuration structure.
 * @param config Structure containing the NumberBox configuration parameters.
 */
void NumberBox::setup(const NumberBoxConfig& config)
{
  setup(config.width, config.height, config.letterColor, config.backgroundColor, config.startValue, config.font, config.funcPtr, config.callback);
}

/**
 * @brief Sets the numeric value displayed by the NumberBox.
 * @param str Numeric value to set.
 * 
 * Updates the displayed value and marks the NumberBox for redraw.
 */
void NumberBox::setValue(float str)
{
  m_value.setString(str);
  log_d("Set value for numberbox: %f", str);
  forceUpdate();
}

/**
 * @brief Retrieves the current numeric value of the NumberBox.
 * @return Current numeric value as a float.
 */
float NumberBox::getValue()
{
  return m_value.toFloat();
}

/**
 * @brief Retrieves the current numeric value as a string.
 * @return Pointer to the string representation of the current value.
 */
const char *NumberBox::getValueChar()
{
  return m_value.getString();
}
