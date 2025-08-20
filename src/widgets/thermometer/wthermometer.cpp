#include "wthermometer.h"

/**
 * @brief Constructor for the Thermometer class.
 * @param _x X position of the widget.
 * @param _y Y position of the widget.
 * @param _screen Screen number.
 */
Thermometer::Thermometer(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen)
{
}

/**
 * @brief Destructor for the Thermometer class.
 */
Thermometer::~Thermometer()
{
}

/**
 * @brief Detects a touch on the Thermometer widget.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is detected, false otherwise.
 */
bool Thermometer::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
  return false;
}

/**
 * @brief Retrieves the callback function for the Thermometer widget.
 * @return Pointer to the callback function.
 */
functionCB_t Thermometer::getCallbackFunc()
{
  return cb;
}

/**
 * @brief Sets the value of the Thermometer widget.
 * @param newValue New value to set.
 */
void Thermometer::setValue(float newValue)
{
  m_currentValue = constrain(newValue, m_vmin, m_vmax);
  // Serial.println("ajusta currentValue: " + String(currentValue));
  m_update = true;
  // redraw();
}

/**
 * @brief Redraws the Thermometer widget.
 */
void Thermometer::redraw()
{
 #if defined(USING_GRAPHIC_LIB)
  if (WidgetBase::currentScreen != screen || m_lastValue == m_currentValue || WidgetBase::usingKeyboard == true || !m_update || !loaded)
  {
    return;
  }
  
  

  uint32_t heightFill = map(m_currentValue, m_vmin, m_vmax, 0, m_fillArea.height);
  uint32_t diffHeight = m_fillArea.height - heightFill;
  int startY = m_fillArea.y + (m_fillArea.height - heightFill);


    if(m_currentValue < m_lastValue){
      WidgetBase::objTFT->fillRoundRect(m_fillArea.x, m_fillArea.y, m_fillArea.width, diffHeight, 0, m_config.backgroundColor);     // area do widget
    }else{
      WidgetBase::objTFT->fillRoundRect(m_fillArea.x, startY, m_fillArea.width, heightFill, 0, m_filledColor);     // area do widget
    }

  if(m_config.subtitle){
    m_config.subtitle->setTextFloat(m_lastValue);
  }

  m_lastValue = m_currentValue;
  m_update = false;
  #endif
}

/**
 * @brief Starts the Thermometer widget.
 */
void Thermometer::start()
{
#if defined(USING_GRAPHIC_LIB)
  m_height = constrain(m_height, 20, WidgetBase::objTFT->height());
#endif
}

/**
 * @brief Forces the Thermometer widget to update.
 */
void Thermometer::forceUpdate()
{
  m_update = true;
}

/**
 * @brief Draws the background of the Thermometer widget.
 */
void Thermometer::drawBackground()
{
   #if defined(USING_GRAPHIC_LIB)
  if (WidgetBase::currentScreen != screen || WidgetBase::usingKeyboard == true || !m_update || !loaded)
  {
    return;
  }
  int radius = (m_width / 2) - m_border;
  
  m_bulb.x = xPos + (m_width / 2);
  m_bulb.y = yPos + m_height - radius - m_border;
  m_bulb.radius = radius;

  int width = m_width / 3;
  

  m_glassArea.x = xPos + ((m_width - width) / 2);
  m_glassArea.y = yPos + m_border;
  m_glassArea.width = width;
  m_glassArea.height = m_height - radius;
  
  m_glassArea.x = xPos + ((m_width - width) / 2);
  m_glassArea.y = yPos + m_border;
  m_glassArea.width = width;
  m_glassArea.height = m_height - (radius + m_border);
  
  m_fillArea.x = xPos + ((m_width - width) / 2);
  m_fillArea.y = yPos + m_border + (width/2);
  m_fillArea.width = width;
  
  m_fillArea.height = m_height - (m_border * 2 + m_bulb.radius * 2 + (width/2));

  WidgetBase::objTFT->fillRoundRect(xPos, yPos, m_width, m_height, 5, m_config.backgroundColor);     // area do widget
  WidgetBase::objTFT->drawRoundRect(xPos, yPos, m_width, m_height, 5, CFK_BLACK);     // area do widget

  

  WidgetBase::objTFT->drawRoundRect(m_glassArea.x -1, m_glassArea.y-1, m_glassArea.width+2, m_glassArea.height+2, m_glassArea.width/2, CFK_BLACK);     // borda vidro
  WidgetBase::objTFT->fillRoundRect(m_glassArea.x, m_glassArea.y, m_glassArea.width, m_glassArea.height, m_glassArea.width/2, m_config.backgroundColor);     // fundo vidro

  WidgetBase::objTFT->fillRoundRect(m_glassArea.x, m_bulb.y - m_bulb.radius, m_glassArea.width, m_bulb.radius, 0, m_filledColor);// base do vidro (para nao ficar sobrando espaço em branco)

  for(uint8_t i = 0; i < m_colorLightGradientSize; i++)
    {
      uint8_t radius = m_bulb.radius - (i * 4);
      if(radius > m_bulb.radius){
        continue;//Aborta o loop se o raio for maior que o raio do LED (overflow do tipo uint8_t)
      }
      WidgetBase::objTFT->fillCircle(m_bulb.x - (2 * i), m_bulb.y - (2 * i), radius, m_colorLightGradient[i]);
    }
    WidgetBase::objTFT->drawCircle(m_bulb.x, m_bulb.y, m_bulb.radius, CFK_BLACK); // desenha a borda do bulb
	
	int amountSpace = 10;
	int space = m_fillArea.height / amountSpace;
	
	int offset = 2;
	int size = 3;
	for(auto i = 0; i <= amountSpace;i++){
		int y = m_fillArea.y + i * space;
		WidgetBase::objTFT->drawLine(m_fillArea.x - offset, y, m_fillArea.x - (offset + size), y, m_config.markColor);
	}

#endif
}

/**
 * @brief Sets up the Thermometer widget.
 * @param _width Width of the widget.
 * @param _height Height of the widget.
 * @param _filledColor Color for the filled portion of the bar.
 * @param _vmin Minimum value of the range.
 * @param _vmax Maximum value of the range.
 */
void Thermometer::setup(uint16_t _width, uint16_t _height, uint16_t _filledColor, int _vmin, int _vmax)
{
  if (!WidgetBase::objTFT)
  {
    log_e("TFT not defined on WidgetBase");
    return;
  }
  if (loaded)
  {
    log_d("Thermometer widget already configured");
    return;
  }
  m_width = _width;
  m_height = _height;
  m_filledColor = _filledColor;
  for(uint8_t i = 0; i < m_colorLightGradientSize; i++)
  {
    m_colorLightGradient[i] = WidgetBase::lightenToWhite565(m_filledColor, 0.08*i);
  }
  m_vmin = _vmin;
  m_vmax = _vmax;
  m_currentValue = m_vmin;
  m_update = true;
  start();
  loaded = true;
}

/**
 * @brief Configures the Thermometer with parameters defined in a configuration structure.
 * @param config Structure containing the Thermometer configuration parameters.
 */
void Thermometer::setup(const ThermometerConfig& config)
{
    m_config = config;
    if(m_config.subtitle){
        m_config.subtitle->setSuffix(m_config.unit);
    }
  setup(config.width, config.height, config.filledColor, config.minValue, config.maxValue);
}
