#include "wtoggle.h"

#define DEBUG_TOGGLE

#if defined(DEBUG_TOGGLE)
#define DEBUG_D(format, ...) log_d(format, ##__VA_ARGS__)
#define DEBUG_E(format, ...) log_e(format, ##__VA_ARGS__)
#define DEBUG_W(format, ...) log_w(format, ##__VA_ARGS__)
#else
#define DEBUG_D(format, ...)
#define DEBUG_E(format, ...)
#define DEBUG_W(format, ...)
#endif

/**
 * @brief Constructor for the ToggleButton class.
 * @param _x X-coordinate for the ToggleButton position.
 * @param _y Y-coordinate for the ToggleButton position.
 * @param _screen Screen identifier where the ToggleButton will be displayed.
 */
ToggleButton::ToggleButton(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen), m_shouldRedraw(true), m_enabled(true)
{
}

/**
 * @brief Destructor for the ToggleButton class.
 *
 * Clears the callback function pointer.
 */
ToggleButton::~ToggleButton() { cb = nullptr; }

/**
 * @brief Detects if the ToggleButton has been touched.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is within the ToggleButton area, otherwise false.
 *
 * Changes the button state if touched and sets the redraw flag.
 */
bool ToggleButton::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
  if (!visible)
  {
    return false;
  }
#if defined(HAS_TOUCH)
  if (WidgetBase::usingKeyboard || WidgetBase::currentScreen != screen || !loaded)
  {
    return false;
  }

  if (millis() - m_myTime < TIMEOUT_CLICK)
  {
    return false;
  }

  if (!m_enabled)
  {
    DEBUG_D("ToggleButton is disabled");
    return false;
  }

  m_myTime = millis();
  bool detectado = false;
  // uint16_t xMax = xPos + m_width;
  // uint16_t yMax = yPos + m_height;

  if (POINT_IN_RECT(*_xTouch, *_yTouch, xPos, yPos, m_width, m_height))
  {
    // if ((*_xTouch > xPos) && (*_xTouch < xMax) && (*_yTouch > yPos) &&(*_yTouch < yMax)) {
    changeState();
    m_shouldRedraw = true;
    detectado = true;
  }
  return detectado;
#else
  return false;
#endif
}

/**
 * @brief Retrieves the callback function associated with the ToggleButton.
 * @return Pointer to the callback function.
 */
functionCB_t ToggleButton::getCallbackFunc() { return cb; }

/**
 * @brief Retrieves the current enabled state of the button.
 * @return True if the button is enabled, otherwise false.
 */
bool ToggleButton::getEnabled() { return m_enabled; }

/**
 * @brief Sets the enabled state of the button.
 * @param newState True to enable the button, false to disable it.
 */
void ToggleButton::setEnabled(bool newState) { m_enabled = newState; }

/**
 * @brief Changes the current state of the button (toggled on or off).
 *
 * Inverts the current state of the toggle button.
 */
void ToggleButton::changeState()
{
  DEBUG_D("Changing state from %d to %d", m_status, !m_status);
  m_status = !m_status;
}

/**
 * @brief Forces the ToggleButton to redraw.
 *
 * Sets the flag to redraw the button on the next redraw cycle.
 */
void ToggleButton::forceUpdate() { m_shouldRedraw = true; }

/**
 * @brief Redraws the ToggleButton on the screen, updating its appearance based
 * on its state.
 *
 * Displays the toggle button with a sliding knob that moves based on the toggle
 * state. The appearance changes based on the current theme (light or dark
 * mode).
 */
void ToggleButton::redraw()
{
  if (!visible)
  {
    return;
  }
#if defined(DISP_DEFAULT)
  if (WidgetBase::currentScreen != screen || !loaded || !m_shouldRedraw)
  {
    return;
  }
  m_shouldRedraw = false;
  // uint16_t darkBg = WidgetBase::lightMode ? CFK_GREY3 : CFK_GREY11;
  uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
  uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;
  uint16_t statusBall = m_status ? CFK_WHITE : CFK_GREY3;

  WidgetBase::objTFT->fillRoundRect(xPos, yPos, m_width, m_height, m_height / 2,
                                    m_status ? m_pressedColor
                                             : lightBg); // fundo
  WidgetBase::objTFT->drawRoundRect(xPos, yPos, m_width, m_height, m_height / 2,
                                    baseBorder); // borda

  uint8_t offsetBorder = 5;

  uint8_t raioBall = (m_height - (2 * offsetBorder)) / 2;
  uint16_t posBall = xPos;

  if (m_status)
  {
    posBall = xPos + m_width - offsetBorder - raioBall;
  }
  else
  {
    posBall = xPos + offsetBorder + raioBall;
  }

  WidgetBase::objTFT->fillCircle(posBall, yPos + m_height / 2, raioBall,
                                 statusBall); // circulo
  WidgetBase::objTFT->drawCircle(posBall, yPos + m_height / 2, raioBall,
                                 baseBorder); // circulo
  /*
  if(m_status){
    WidgetBase::objTFT->fillRect(50,50, 20, 20, CFK_CHOCOLATE);
  }else{
    for (int r = 0; r < 20; r++)
    {
      blitRowFromCanvas(50, 50 + r, 20);
    }
  }*/

  if (m_canvas)
  {
    m_canvas->flush();
  }
#endif
}

void ToggleButton::blitRowFromCanvas(int x, int y, int w)
{
  if (m_canvas)
  {
    // O buffer do canvas é RGB565 contínuo, com stride = SCREEN_W
    uint16_t *buf = (uint16_t *)m_canvas->getFramebuffer();
    uint16_t *row = buf + (y * m_width + x);
    WidgetBase::objTFT->draw16bitRGBBitmap(x, y, row, w, 1);
  }
}

/**
 * @brief Initializes the ToggleButton settings.
 *
 * Ensures button width is within valid ranges based on the display size.
 */
void ToggleButton::start()
{
#if defined(DISP_DEFAULT)
  m_width = constrain(m_width, 30, WidgetBase::objTFT->width());
#endif
}

/**
 * @brief Configures the ToggleButton with specific dimensions, color, and
 * callback function.
 * @param _width Width of the button.
 * @param _height Height of the button.
 * @param _pressedColor Color displayed when the button is toggled on.
 * @param _cb Callback function to execute when the button is toggled.
 *
 * Initializes the button properties and marks it as loaded when complete.
 */
void ToggleButton::setup(uint16_t _width, uint16_t _height,
                         uint16_t _pressedColor, functionCB_t _cb)
{

  if (!WidgetBase::objTFT)
  {
    DEBUG_E("TFT not defined on WidgetBase");
    return;
  }
  if (loaded)
  {
    DEBUG_E("Toggle widget already configured");
    return;
  }

  m_width = _width;
  m_height = _height;
  m_pressedColor = _pressedColor;
  cb = _cb;
  // m_canvas = new Arduino_Canvas(m_width, m_height, WidgetBase::objTFT, 50, 50);
  // m_canvas->begin();
  // m_canvas->fillRect(0, 0, m_width, m_height, CFK_AQUA);
  // m_canvas->fillCircle(m_width / 2, m_height / 2, m_height / 2, CFK_DEEPPINK);
  // m_canvas->flush();

  start();

  loaded = true;
}

/**
 * @brief Configures the ToggleButton with parameters defined in a configuration
 * structure.
 * @param config Structure containing the button configuration parameters.
 */
void ToggleButton::setup(const ToggleButtonConfig &config)
{
  setup(config.width, config.height, config.pressedColor, config.callback);
}

/**
 * @brief Retrieves the current status of the button.
 * @return True if the button is toggled on, otherwise false.
 */
bool ToggleButton::getStatus() { return m_status; }

/**
 * @brief Sets the current state of the toggle button.
 * @param status True for on, false for off.
 *
 * Updates the button state, marks it for redraw, and triggers the callback if
 * provided.
 */
void ToggleButton::setStatus(bool status)
{
  if (!loaded)
  {
    DEBUG_E("ToggleButton widget not loaded");
    return;
  }
  DEBUG_D("Setting status to %d", status);
  m_status = status;
  m_shouldRedraw = true;
  if (cb != nullptr)
  {
    WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::SELF);
  }
}

void ToggleButton::show()
{
  visible = true;
  m_shouldRedraw = true;
}

void ToggleButton::hide()
{
  visible = false;
  m_shouldRedraw = true;
}