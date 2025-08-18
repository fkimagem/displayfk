#include "whslider.h"

/**
 * @brief Constructor for the HSlider class.
 * @param _x X-coordinate for the HSlider position.
 * @param _y Y-coordinate for the HSlider position.
 * @param _screen Screen identifier where the HSlider will be displayed.
 */
HSlider::HSlider(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen),
m_shouldRedraw(true)
{
}

/**
 * @brief Destructor for the HSlider class.
 */
HSlider::~HSlider()
{
}

/**
 * @brief Detects if the slider has been touched and processes the touch input.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is within the slider handle area, otherwise false.
 * 
 * Updates the slider position and value based on the touch position.
 */
bool HSlider::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
#if defined(HAS_TOUCH)
  if (WidgetBase::usingKeyboard || WidgetBase::currentScreen != screen || !loaded)
  {
    return false;
  }
  /*if (millis() - m_myTime < 10)
  {
    return false;
  }*/

  if (!m_enabled)
  {
    log_d("HSlider is disabled");
    return false;
  }

  m_myTime = millis();
  // Serial.println("Calculating slider");
  bool detected = false;
  int32_t offsetRadius = 10; // Offset to account for the radius of the slider handle
  int32_t radiusToDetect = m_radius + offsetRadius; // Add a small margin for touch detection
  int32_t deltaX = (*_xTouch - m_currentPos) * (*_xTouch - m_currentPos);
  int32_t deltaY = (*_yTouch - (yPos + radiusToDetect)) * (*_yTouch - (yPos + radiusToDetect));
  int32_t radiusQ = radiusToDetect * radiusToDetect;

  if ((deltaX < radiusQ) && (deltaY < radiusQ))
  {
    detected = true;

    m_currentPos = (*_xTouch);
    m_currentPos = constrain(m_currentPos, m_minX, m_maxX);

    /*if (currentPos < xPos + (height / 2))
    {
      currentPos = xPos + (height / 2);
      //Serial.println("Primeira pos");
    }
    if (currentPos > xPos + width - (height / 2))
    {
      currentPos = xPos + width - (height / 2);
      //Serial.println("Ultima po");
    }*/

    m_value = map(m_currentPos, m_minX, m_maxX, m_vmin, m_vmax);
    /*float l = width - height;
    float percent = (currentPos - (xPos + (height / 2))) / (l);
    value = (((vmax - vmin) * percent) + vmin);*/

    m_shouldRedraw = true;
  }
  // Serial.println("Calculated slider");
  return detected;
  #else
  return false;
  #endif
}

/**
 * @brief Retrieves the callback function associated with the slider.
 * @return Pointer to the callback function.
 */
functionCB_t HSlider::getCallbackFunc()
{
    return cb;
}

/**
 * @brief Retrieves the current enabled state of the slider.
 * @return True if the slider is enabled, otherwise false.
 */
bool HSlider::getEnabled()
{
  return m_enabled;
}

/**
 * @brief Sets the enabled state of the slider.
 * @param newState True to enable the slider, false to disable it.
 */
void HSlider::setEnabled(bool newState)
{
  m_enabled = newState;
}

/**
 * @brief Redraws the slider handle and filled portion on the screen.
 * 
 * Updates the visual appearance of the slider based on its current value.
 * Only redraws if the slider is on the current screen and needs updating.
 */
void HSlider::redraw()
{
  #if defined(DISP_DEFAULT) || defined(DISP_PCD8544)
  if (WidgetBase::currentScreen != screen || !loaded || !m_shouldRedraw)
  {
    return;
  }

  m_shouldRedraw = false;

  uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;


  //uint16_t lastPosCircleX = map(lastPos, xPos + radius, xPos + width - radius, radius, width - radius);
  WidgetBase::objTFT->fillCircle(m_lastPos, m_centerV, m_contentRadius, CFK_WHITE); // slider background


  //uint16_t posCircleX = map(currentPos, xPos + radius, xPos + width - radius, radius, width - radius);

  log_d("Redraw ballslider with value %i at %i", m_value, m_currentPos);

  uint8_t fillRadius = 2;

  WidgetBase::objTFT->fillRoundRect(m_minX, m_centerV - (fillRadius), m_currentPos - (m_minX), fillRadius * 2, fillRadius, m_pressedColor);
  WidgetBase::objTFT->fillCircle(m_currentPos, m_centerV, m_contentRadius, lightBg); // slider handle
  WidgetBase::objTFT->fillCircle(m_currentPos, m_centerV, m_contentRadius * 0.75, m_pressedColor);                                  // slider center

  //WidgetBase::objTFT->drawCircle(posCircleX, radius, radius, baseBorder);        // large outline
  //WidgetBase::objTFT->drawCircle(posCircleX, radius, radius * 0.75, baseBorder); // slider center

  m_lastPos = m_currentPos;
  #endif
}

/**
 * @brief Draws the slider's background and static elements.
 * 
 * Creates the track and border for the slider.
 * Called during initial setup and when the slider needs a complete redraw.
 */
void HSlider::drawBackground()
{
  #if defined(DISP_DEFAULT) || defined(DISP_PCD8544)
  if (WidgetBase::currentScreen != screen || !loaded)
  {
    return;
  }

  uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

  WidgetBase::objTFT->fillRoundRect(xPos, yPos, m_width, m_height, m_radius, WidgetBase::lightMode ? CFK_WHITE : CFK_BLACK);
  WidgetBase::objTFT->drawRoundRect(xPos, yPos, m_width, m_height, m_radius, baseBorder);

  redraw();
  #endif
}

/**
 * @brief Configures the slider with specific parameters.
 * @param _width Width of the slider track.
 * @param _pressedColor Color when the slider handle is pressed.
 * @param _vmin Minimum value of the slider range.
 * @param _vmax Maximum value of the slider range.
 * @param _radius Radius of the slider handle.
 * @param _cb Callback function to execute on slider value change.
 * 
 * Initializes the slider dimensions, value range, and appearance.
 */
void HSlider::setup(uint16_t _width, uint16_t _pressedColor, int _vmin, int _vmax, uint32_t _radius, functionCB_t _cb)
{
  if (loaded)
  {
    log_d("HSlider widget already configured");
    return;
  }
  m_width = _width;
  m_height = _radius * 2;
  m_pressedColor = _pressedColor;
  m_vmin = _vmin;
  m_vmax = _vmax;
  m_value = m_vmin;
  m_radius = _radius;
  m_contentRadius = m_radius - 4;
  cb = _cb;
  m_minX = xPos + m_radius;
  m_maxX = xPos + m_width - m_radius;

  m_currentPos = m_minX;
  m_lastPos = m_currentPos;
  m_centerV = yPos + (m_height / 2);
  loaded = true;
  drawBackground();
}

/**
 * @brief Configures the HSlider with parameters defined in a configuration structure.
 * @param config Structure containing the slider configuration parameters.
 */
void HSlider::setup(const HSliderConfig& config)
{
  setup(config.width, config.pressedColor, config.minValue, config.maxValue, config.radius, config.callback);
}

/**
 * @brief Retrieves the current value of the slider.
 * @return Current value of the slider.
 */
int HSlider::getValue()
{
  return m_value;
}

/**
 * @brief Sets the value of the slider.
 * @param _value New value for the slider.
 * 
 * Updates the slider position based on the provided value and triggers the callback.
 */
void HSlider::setValue(int _value){
  if(!loaded) 
  {
    log_e("HSlider widget not loaded");
    return;
  }

  m_value = constrain(_value, m_vmin, m_vmax);
  m_shouldRedraw = true;
  if(cb != nullptr){
    WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::SELF);
  }

}

/**
 * @brief Forces the slider to update, refreshing its visual state on next redraw.
 */
void HSlider::forceUpdate(){
  m_shouldRedraw = true;
}


