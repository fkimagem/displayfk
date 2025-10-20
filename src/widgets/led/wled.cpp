#include "wled.h"

/**
 * @brief Constructor for Led widget.
 * @param _x X-coordinate for the LED position.
 * @param _y Y-coordinate for the LED position.
 * @param _screen Screen identifier where the LED will be displayed.
 */
Led::Led(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen)
{
}

/**
 * @brief Destructor for the Led class.
 */
Led::~Led()
{
}

/**
 * @brief Detects if the LED has been touched.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return Always returns false as LEDs don't respond to touch events.
 */
bool Led::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
  return false;
}

/**
 * @brief Sets the state of the LED (on or off).
 * @param newValue New state for the LED (true for on, false for off).
 */
void Led::setState(bool newValue)
{
  m_status = newValue;
  // Serial.println("ajusta status: " + String(status));
  m_shouldRedraw = true;
  // redraw();
}

void Led::drawBackground()
{
  CHECK_TFT_VOID
  if(!visible){return;}
  #if defined(USING_GRAPHIC_LIB)
  if (WidgetBase::currentScreen != screen || WidgetBase::usingKeyboard || !loaded)
  {
    return;
  }

  WidgetBase::objTFT->drawCircle(xPos, yPos, m_radius, CFK_BLACK);
  #endif
}

/**
 * @brief Redraws the LED on the screen, updating its appearance.
 * 
 * Displays the LED with proper visual representation based on its current state.
 * The LED is drawn as a semicircle with connectors to make it look like a real LED.
 */
void Led::redraw()
{
  CHECK_TFT_VOID
  if(!visible){return;}
  #if defined(USING_GRAPHIC_LIB)
  if (WidgetBase::currentScreen != screen || WidgetBase::usingKeyboard || !m_shouldRedraw || !loaded)
  {
    return;
  }

  if(m_status)
  {
    for(uint8_t i = 0; i < m_colorLightGradientSize; i++)
    {
      uint8_t radius = m_radius - (i * 4);
      if(radius > m_radius){
        continue;//Aborta o loop se o raio for maior que o raio do LED (overflow do tipo uint8_t)
      }
      WidgetBase::objTFT->fillCircle(xPos - (2 * i), yPos - (2 * i), radius, m_colorLightGradient[i]);
    }
  }else{
    WidgetBase::objTFT->fillCircle(xPos, yPos, m_radius - 1, CFK_GREY11);
  }

  m_shouldRedraw = false;
  #endif
}

/**
 * @brief Forces the LED to update, refreshing its visual state on next redraw.
 */
void Led::forceUpdate()
{
  m_shouldRedraw = true;
}

/**
 * @brief Configures the LED widget with a specific radius and on color.
 * @param _radius Radius of the LED.
 * @param _colorOn Color of the LED when it is on.
 */
void Led::setup(uint16_t _radius, uint16_t _colorOn)
{
  if(!WidgetBase::objTFT){log_e("TFT not defined on WidgetBase");return;}
  if (loaded)
  {
    log_d("Led widget already configured");
    return;
  }

  m_radius = _radius;
  m_colorOn = _colorOn;
  for(uint8_t i = 0; i < m_colorLightGradientSize; i++)
  {
    m_colorLightGradient[i] = WidgetBase::lightenToWhite565(_colorOn, 0.08*i);
  }
  // size = constrain(_size, 1,3);
  m_shouldRedraw = true;
  loaded = true;
}

/**
 * @brief Configures the LED widget with parameters defined in a configuration structure.
 * @param config Structure containing the LED configuration parameters (radius and colorOn).
 */
void Led::setup(const LedConfig& config)
{
  setup(config.radius, config.colorOn);
}

void Led::show()
{
    visible = true;
    m_shouldRedraw = true;
}

void Led::hide()
{
    visible = false;
    m_shouldRedraw = true;
}