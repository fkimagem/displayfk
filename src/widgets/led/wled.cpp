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
  m_update = true;
  // redraw();
}

/**
 * @brief Redraws the LED on the screen, updating its appearance.
 * 
 * Displays the LED with proper visual representation based on its current state.
 * The LED is drawn as a semicircle with connectors to make it look like a real LED.
 */
void Led::redraw()
{
  if (WidgetBase::currentScreen != screen || WidgetBase::usingKeyboard == true || !m_update || !loaded)
  {
    return;
  }
  //uint16_t darkBg = WidgetBase::lightMode ? CFK_GREY3 : CFK_GREY11;
//uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

  m_lastStatus = m_status;
  // uint16_t corOff = CFK_GREY3;
  //WidgetBase::objTFT->fillCircle(xPos + 1, yPos - (2 * radius) + 1, radius, baseBorder);                          // Sombra circulo
  //WidgetBase::objTFT->fillRect(xPos - (radius + 1), yPos - (2 * radius) + 1, 2 * radius, 2 * radius, baseBorder); // sombra quadrado
  uint16_t corPintar = CFK_GREY11;
  //uint16_t corBrilho = CFK_WHITE;
  if (m_status)
  {
    corPintar = m_colorOn;
    //corBrilho = CFK_BLACK;
  }

  // Serial.println("led redraw: " + String(status));
  WidgetBase::objTFT->fillCircle(xPos, yPos - (2 * m_radius), m_radius, corPintar);                                                                // circulo
  WidgetBase::objTFT->drawCircle(xPos, yPos - (2 * m_radius), m_radius, CFK_BLACK);                                                                // borda circulo
  WidgetBase::objTFT->fillRect(xPos - m_radius, yPos - (2 * m_radius), (2 * m_radius) + 1, (2 * m_radius), corPintar);                                     // quadrado
  WidgetBase::objTFT->drawRect(xPos - m_radius, yPos - (2 * m_radius), (2 * m_radius) + 1, (2 * m_radius), baseBorder); // borda quadrado
  WidgetBase::objTFT->drawFastHLine(xPos - (m_radius - 1), yPos - (2 * m_radius), 2 * m_radius - 2, corPintar);                                      // linha para esconder borda quadrado

  //WidgetBase::objTFT->drawFastVLine(xPos, yPos - (3 * radius) - 10, 8, corBrilho);            // brilho cima
  //WidgetBase::objTFT->drawFastHLine(xPos + radius + 2, yPos - (2 * radius), 8, corBrilho);    // brilho direita
  //WidgetBase::objTFT->drawFastHLine(xPos - (radius + 10), yPos - (2 * radius), 8, corBrilho); // brilho esquerda

  WidgetBase::objTFT->drawFastVLine(xPos - 4, yPos - 8, 12, baseBorder); // conector esquerda
  WidgetBase::objTFT->drawFastVLine(xPos - 5, yPos - 8, 12, baseBorder); // conector esquerda
  WidgetBase::objTFT->drawFastVLine(xPos + 4, yPos - 6, 12, baseBorder); // conector direita
  WidgetBase::objTFT->drawFastVLine(xPos + 5, yPos - 6, 12, baseBorder); // conector direita
  WidgetBase::objTFT->fillRoundRect(xPos - m_radius - 3, yPos - 2, (2 * m_radius) + 6, 4, 2, baseBorder);

  //showOrigin(CFK_RED);

  m_update = false;
}

/**
 * @brief Forces the LED to update, refreshing its visual state on next redraw.
 */
void Led::forceUpdate()
{
  m_update = true;
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
  // size = constrain(_size, 1,3);
  m_update = true;
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
