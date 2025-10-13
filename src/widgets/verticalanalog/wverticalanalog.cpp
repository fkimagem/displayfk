#include "wverticalanalog.h"

/**
 * @brief Constructor for the VAnalog class.
 * @param _x X-coordinate for the VAnalog position.
 * @param _y Y-coordinate for the VAnalog position.
 * @param _screen Screen identifier where the VAnalog will be displayed.
 */
VAnalog::VAnalog(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen)
{
}

/**
 * @brief Destructor for the VAnalog class.
 */
VAnalog::~VAnalog()
{
}

/**
 * @brief Detects if the VAnalog has been touched.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return Always returns false as VAnalog doesn't respond to touch events.
 */
bool VAnalog::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
  return false;
}

/**
 * @brief Retrieves the callback function associated with the VAnalog.
 * @return Pointer to the callback function.
 */
functionCB_t VAnalog::getCallbackFunc()
{
  return cb;
}

/**
 * @brief Initializes the VAnalog settings.
 * 
 * Sets dimensions, padding, and drawing area for the analog display.
 * Ensures the widget's height is within valid ranges based on the display size.
 */
void VAnalog::start()
{
  m_padding = 20;
  m_width = 40; // fixa a largura do widget
#if defined(DISP_DEFAULT)
  m_height = constrain(m_height, 40, WidgetBase::objTFT->height());
#endif

  m_paddingDraw = 5;
  m_drawArea = (m_height - m_padding) - 2 * m_paddingDraw;
}

/**
 * @brief Draws the background of the VAnalog, including static elements.
 * 
 * Creates the visual container and scale markers for the analog display.
 * Only draws if the VAnalog is on the current screen and needs updating.
 */
void VAnalog::drawBackground()
{
  if(!visible){return;}
  #if defined(DISP_DEFAULT)
  if (WidgetBase::currentScreen != screen || WidgetBase::usingKeyboard == true || !m_shouldRedraw || !loaded)
  {
    return;
  }
  log_d("Redraw background vanalog at %i,%i with %i x %i", xPos, yPos, m_width, m_height);

  //uint16_t darkBg = WidgetBase::lightMode ? CFK_GREY3 : CFK_GREY11;
  //uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
  //uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

  // WidgetBase::objTFT->fillCircle(xPos, yPos, 10, CFK_RED);
  WidgetBase::objTFT->fillRect(xPos, yPos, m_width, m_height, m_backgroundColor); // Area do grafico lightBg
  WidgetBase::objTFT->drawRect(xPos, yPos, m_width, m_height, m_borderColor);                 // contorno darkBg
  //WidgetBase::objTFT->fillRect(xPos + 1, yPos + 1, 38, height - padding, backgroundColor); // Area do grafico lightBg

  uint16_t intervaloMarca = round(m_drawArea / (m_steps));
  for (uint16_t i = 0; i < (m_steps + 1); i++)
  {
    uint16_t yLinha = intervaloMarca * i + (yPos + m_paddingDraw);
    uint16_t fimLinha = i % 5 == 0 ? 5 : 10;
    WidgetBase::objTFT->drawLine(xPos + (m_width / 2), yLinha, (xPos + m_width) - fimLinha, yLinha, m_textColor);
  }
  m_lastYArrow = yPos + m_paddingDraw + 4;
  redraw();
  log_d("Finish draw vanalog");
  #endif
}

/**
 * @brief Sets the value to be displayed on the analog scale.
 * @param newValue New value to set.
 * @param _viewValue Whether to display the numeric value text.
 * 
 * Updates the current value to display and whether to show the numeric text,
 * ensuring the value is within the defined range.
 */
void VAnalog::setValue(int newValue, bool _viewValue)
{
  m_currentValue = constrain(newValue, m_vmin, m_vmax);
  m_updateText = _viewValue;
  ////Serial.println("ajusta currentValue: " + String(currentValue));
  m_shouldRedraw = true;
  // redraw();
}

/**
 * @brief Redraws the VAnalog on the screen, updating its appearance.
 * 
 * Updates the position of the arrow marker and the numeric text if enabled.
 * Only redraws if the VAnalog is on the current screen and needs updating.
 */
void VAnalog::redraw()
{
  if(!visible){return;}
  #if defined(DISP_DEFAULT)
  ////Serial.println(WidgetBase::usingKeyboard);
  if (WidgetBase::currentScreen != screen || WidgetBase::usingKeyboard == true || !m_shouldRedraw || !loaded)
  {
    return;
  }

  if (m_updateText)
  {
    WidgetBase::objTFT->setFont(&RobotoRegular10pt7b);
  }
  // uint16_t darkBg = WidgetBase::lightMode ? CFK_GREY3 : CFK_GREY11;
  // uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
  // uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

  m_lastValue = m_currentValue;
  // WidgetBase::objTFT->fillRect(xPos + 1, lastYArrow - 4, 15, 8, CFK_GREY3);
  WidgetBase::objTFT->fillTriangle(xPos + 2, m_lastYArrow - 3, xPos + 15, m_lastYArrow, xPos + 2, m_lastYArrow + 3, m_backgroundColor);
  uint32_t vatual = m_currentValue - m_vmin;
  float total = m_vmax - m_vmin;
  float porcentagem = (vatual / total) * m_drawArea;
  uint32_t yMarca = yPos + m_paddingDraw + m_drawArea - porcentagem;
  m_lastYArrow = yMarca;

  WidgetBase::objTFT->fillTriangle(xPos + 2, yMarca - 3, xPos + 15, yMarca, xPos + 2, yMarca + 3, m_arrowColor);

  // WidgetBase::objTFT->drawCentreString("0", xPos, yPos - 20, 2);
  if (m_updateText)
  {
    WidgetBase::objTFT->setTextColor(m_textColor);
    WidgetBase::objTFT->setFont(&RobotoRegular5pt7b);
    TextBound_t textBg;
    textBg.x = xPos + 2;
    textBg.y = yPos + m_height - 2 - m_paddingDraw - m_padding;
    textBg.width = m_width - 4;
    textBg.height = m_paddingDraw + m_padding;
    WidgetBase::objTFT->fillRect(textBg.x, textBg.y, textBg.width, textBg.height, m_backgroundColor);
    printText(String(m_currentValue).c_str(), xPos + (m_width / 2), yPos + m_height - m_paddingDraw, BC_DATUM);
  }
  m_shouldRedraw = false;
  #endif
}

/**
 * @brief Forces the VAnalog to update, refreshing its visual state on next redraw.
 */
void VAnalog::forceUpdate()
{
  m_shouldRedraw = true;
}

/**
 * @brief Configures the VAnalog widget with specific parameters.
 * @param _width Width of the VAnalog display.
 * @param _height Height of the VAnalog display.
 * @param _vmin Minimum value of the scale.
 * @param _vmax Maximum value of the scale.
 * @param _steps Number of divisions on the scale.
 * @param _arrowColor Color of the arrow marker.
 * @param _textColor Color of the text display.
 * @param _backgroundColor Background color of the display.
 * @param _borderColor Color of the display border.
 * 
 * Initializes the VAnalog properties and marks it as loaded when complete.
 */
void VAnalog::setup(uint16_t _width, uint16_t _height, int _vmin, int _vmax, uint8_t _steps, uint16_t _arrowColor, uint16_t _textColor, uint16_t _backgroundColor, uint16_t _borderColor)
{
  if (!WidgetBase::objTFT)
  {
    log_e("TFT not defined on WidgetBase");
    return;
  }
  if (loaded)
  {
    log_d("VAnalog widget already configured");
    return;
  }
  m_width = _width;
  m_height = _height;
  m_vmin = _vmin;
  m_vmax = _vmax;
  m_steps = _steps;
  m_currentValue = m_vmin;
  m_lastValue = m_vmin;
  m_arrowColor = _arrowColor;
  m_textColor = _textColor;
  m_backgroundColor = _backgroundColor;
  m_borderColor = _borderColor;

  start();
  m_shouldRedraw = true;
  loaded = true;
}

/**
 * @brief Configures the VAnalog with parameters defined in a configuration structure.
 * @param config Structure containing the vertical analog configuration parameters.
 */
void VAnalog::setup(const VerticalAnalogConfig& config)
{
  setup(config.width, config.height, config.minValue, config.maxValue, config.steps, 
        config.arrowColor, config.textColor, config.backgroundColor, config.borderColor);
}

void VAnalog::show()
{
    visible = true;
    m_shouldRedraw = true;
}

void VAnalog::hide()
{
    visible = false;
    m_shouldRedraw = true;
}