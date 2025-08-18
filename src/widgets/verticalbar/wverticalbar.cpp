#include "wverticalbar.h"

/**
 * @brief Constructor for the VBar class.
 * @param _x X position of the widget.
 * @param _y Y position of the widget.
 * @param _screen Screen number.
 */
VBar::VBar(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen)
{
}

/**
 * @brief Destructor for the VBar class.
 */
VBar::~VBar()
{
}

/**
 * @brief Detects a touch on the VBar widget.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is detected, false otherwise.
 */
bool VBar::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
  return false;
}

/**
 * @brief Retrieves the callback function for the VBar widget.
 * @return Pointer to the callback function.
 */
functionCB_t VBar::getCallbackFunc()
{
  return cb;
}

/**
 * @brief Sets the value of the VBar widget.
 * @param newValue New value to set.
 */
void VBar::setValue(uint32_t newValue)
{
  m_currentValue = constrain(newValue, m_vmin, m_vmax);
  // Serial.println("ajusta currentValue: " + String(currentValue));
  m_update = true;
  // redraw();
}

/**
 * @brief Redraws the VBar widget.
 */
void VBar::redraw()
{
  #if defined(DISP_DEFAULT) || defined(DISP_PCD8544)
  if (WidgetBase::currentScreen != screen || m_lastValue == m_currentValue || WidgetBase::usingKeyboard == true || !m_update || !loaded)
  {
    return;
  }
  
  int innerX = xPos + 1;
  int innerY = yPos + 1;
  int innerHeight = m_height - 2;
  int innerWidth = m_width - 2;
  int minHeight = m_round;
  int minWidth = m_round;
  int innerRound = m_round > 0 ? m_round - 1 : m_round;

  if(m_orientation == Orientation::VERTICAL){
	
	uint32_t proportionalHeight = map(m_currentValue, m_vmin, m_vmax, minHeight, innerHeight);
	
	if(m_currentValue < m_lastValue){
		uint32_t clearArea = map(m_vmax - m_currentValue, m_vmin, m_vmax, minHeight, innerHeight);
		WidgetBase::objTFT->fillRoundRect(innerX, innerY, innerWidth, clearArea, innerRound, CFK_GREY11); // fundo total
		
	}

    
    WidgetBase::objTFT->fillRoundRect(innerX, innerY + (innerHeight - proportionalHeight), innerWidth, proportionalHeight, innerRound, m_filledColor); // cor fill
    //WidgetBase::objTFT->drawRoundRect(innerX, innerY + (innerHeight - proportionalHeight), innerWidth, proportionalHeight, innerRound, CFK_BLACK);   // borda fill

  }else if(m_orientation == Orientation::HORIZONTAL){
	  
	  if(m_currentValue < m_lastValue){
		uint32_t clearArea = map(m_vmax - m_currentValue, m_vmin, m_vmax, minWidth, innerWidth);
		uint32_t xValue = map(m_currentValue, m_vmin, m_vmax, innerX, innerX + innerWidth);
		
		WidgetBase::objTFT->fillRoundRect(xValue, innerY, clearArea, innerHeight, innerRound, CFK_GREY11); // fundo total
		
	}

    uint32_t proportionalWidth = map(m_currentValue, m_vmin, m_vmax, minWidth, innerWidth); // O +1 é para tirar a borda da contagem
    WidgetBase::objTFT->fillRoundRect(innerX, innerY, proportionalWidth, innerHeight, innerRound, m_filledColor); // cor fill
	
    //WidgetBase::objTFT->drawRoundRect(xPos, yPos, m_width, m_height, m_round,CFK_BLACK);   // borda fill

  }

  
  m_lastValue = m_currentValue;
  m_update = false;

  #endif
}

/**
 * @brief Starts the VBar widget.
 */
void VBar::start()
{
#if defined(DISP_DEFAULT) || defined(DISP_PCD8544)
  m_height = constrain(m_height, 20, WidgetBase::objTFT->height());
  m_width = constrain(m_width, 20, WidgetBase::objTFT->width());
#endif
}

/**
 * @brief Forces the VBar widget to update.
 */
void VBar::forceUpdate()
{
  m_update = true;
}

/**
 * @brief Draws the background of the VBar widget.
 */
void VBar::drawBackground()
{
  #if defined(DISP_DEFAULT) || defined(DISP_PCD8544)
  if (WidgetBase::currentScreen != screen || WidgetBase::usingKeyboard == true || !m_update || !loaded)
  {
    return;
  }
  WidgetBase::objTFT->fillRoundRect(xPos, yPos, m_width, m_height, m_round, CFK_GREY11); // fundo total
  WidgetBase::objTFT->drawRoundRect(xPos, yPos, m_width, m_height, m_round, CFK_BLACK);     // borda total
  #endif
}

/**
 * @brief Sets up the VBar widget.
 * @param _width Width of the widget.
 * @param _height Height of the widget.
 * @param _filledColor Color for the filled portion of the bar.
 * @param _vmin Minimum value of the range.
 * @param _vmax Maximum value of the range.
 * @param _orientation Orientation of the bar (vertical or horizontal).
 */
void VBar::setup(uint16_t _width, uint16_t _height, uint16_t _filledColor, int _vmin, int _vmax, int _round, Orientation _orientation)
{
  if (!WidgetBase::objTFT)
  {
    log_e("TFT not defined on WidgetBase");
    return;
  }
  if (loaded)
  {
    log_d("VBar widget already configured");
    return;
  }
  m_width = _width;
  m_height = _height;
  m_filledColor = _filledColor;
  m_vmin = _vmin;
  m_vmax = _vmax;
  m_currentValue = m_vmin;
  m_orientation = _orientation;
  m_update = true;
  m_round = _round;
  start();
  loaded = true;
}

/**
 * @brief Configures the VBar with parameters defined in a configuration structure.
 * @param config Structure containing the VBar configuration parameters.
 */
void VBar::setup(const VerticalBarConfig& config)
{
  setup(config.width, config.height, config.filledColor, config.minValue, config.maxValue, config.round,config.orientation);
}
