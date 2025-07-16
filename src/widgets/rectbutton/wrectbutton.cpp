#include "wrectbutton.h"

/**
 * @brief Constructor for the RectButton class.
 * @param _x X-coordinate for the RectButton position.
 * @param _y Y-coordinate for the RectButton position.
 * @param _screen Screen identifier where the RectButton will be displayed.
 */
RectButton::RectButton(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen),
m_shouldRedraw(true)
{
}

/**
 * @brief Destructor for the RectButton class.
 */
RectButton::~RectButton()
{
}

/**
 * @brief Detects if the RectButton has been touched.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is within the RectButton area, otherwise false.
 * 
 * Changes the button state if touched and sets the redraw flag.
 */
bool RectButton::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
  if (WidgetBase::usingKeyboard || WidgetBase::currentScreen != screen || !loaded)
  {
    return false;
  }

  if (millis() - _myTime < TIMEOUT_CLICK)
  {
    return false;
  }
  
  if (!m_enabled)
  {
    log_d("RectButton is disabled");
    return false;
  }
  
  _myTime = millis();
  bool detectado = false;
  uint16_t xMax = xPos + width;
  uint16_t yMax = yPos + height;

  if ((*_xTouch > xPos) && (*_xTouch < xMax) && (*_yTouch > yPos) && (*_yTouch < yMax))
  {
    changeState();
    m_shouldRedraw = true;
    detectado = true;
  }
  return detectado;
}

/**
 * @brief Retrieves the callback function associated with the RectButton.
 * @return Pointer to the callback function.
 */
functionCB_t RectButton::getCallbackFunc()
{
  return cb;
}

/**
 * @brief Retrieves the current enabled state of the button.
 * @return True if the button is enabled, otherwise false.
 */
bool RectButton::getEnabled()
{
  return m_enabled;
}

/**
 * @brief Sets the enabled state of the button.
 * @param newState True to enable the button, false to disable it.
 */
void RectButton::setEnabled(bool newState)
{
  m_enabled = newState;
}

/**
 * @brief Changes the current state of the button (pressed or released).
 * 
 * Inverts the current state of the button.
 */
void RectButton::changeState()
{
  status = !status;
}

/**
 * @brief Forces the button to redraw.
 * 
 * Sets the flag to redraw the button on the next redraw cycle.
 */
void RectButton::forceUpdate(){
  m_shouldRedraw = true;
}

/**
 * @brief Redraws the button on the screen, updating its appearance.
 * 
 * Displays the button with different appearance based on its current state.
 * Only redraws if the button is on the current screen and needs updating.
 */
void RectButton::redraw()
{
  if (WidgetBase::currentScreen != screen || WidgetBase::usingKeyboard || !loaded || !m_shouldRedraw)
  {
    return;
  }

  m_shouldRedraw = false;

  // uint16_t darkBg = WidgetBase::lightMode ? CFK_GREY3 : CFK_GREY11;
  uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
  uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

  WidgetBase::objTFT->fillRoundRect(xPos + 1, yPos + 1, width - 2, height - 2, 5, lightBg);      // Botao
  WidgetBase::objTFT->drawRoundRect(xPos, yPos, width, height, 5, baseBorder);                   // borda Botao
  uint16_t bgColor = status ? pressedColor : lightBg;
  WidgetBase::objTFT->fillRoundRect(xPos + 6, yPos + 6, width - 12, height - 12, 5, bgColor);    // top botao
  WidgetBase::objTFT->drawRoundRect(xPos + 5, yPos + 5, width - 10, height - 10, 5, baseBorder); // borda top botao
}

/**
 * @brief Initializes the button settings.
 * 
 * Ensures button dimensions are within valid ranges based on the display size.
 * Marks the widget as loaded when complete.
 */
void RectButton::start()
{
  #if defined(DISP_DEFAULT)
  width = constrain(width, 5, WidgetBase::objTFT->width());
  height = constrain(height, 5, WidgetBase::objTFT->height());
  #endif

  
  loaded = true;
}

/**
 * @brief Configures the RectButton with specific dimensions, color, and callback function.
 * @param _width Width of the button.
 * @param _height Height of the button.
 * @param _pressedColor Color of the button when it is pressed.
 * @param _cb Callback function to execute when the button is interacted with.
 * 
 * Initializes the button properties and marks it as loaded when complete.
 */
void RectButton::setup(uint16_t _width, uint16_t _height, uint16_t _pressedColor, functionCB_t _cb)
{
  if (!WidgetBase::objTFT)
  {
    log_e("TFT not defined on WidgetBase");
    return;
  }
  if (loaded)
  {
    log_d("RectButton widget already configured");
    return;
  }
  width = _width;
  height = _height;
  pressedColor = _pressedColor;
  cb = _cb;

  loaded = true;
}

/**
 * @brief Configures the RectButton with parameters defined in a configuration structure.
 * @param config Structure containing the button configuration parameters.
 */
void RectButton::setup(const RectButtonConfig& config)
{
  setup(config.width, config.height, config.pressedColor, config.callback);
}

/**
 * @brief Retrieves the current status of the button.
 * @return True if the button is pressed, otherwise false.
 */
bool RectButton::getStatus()
{
  return status;
}

/**
 * @brief Sets the status of the button.
 * @param _status New status of the button (true for pressed, false for released).
 * 
 * Updates the button state, marks it for redraw, and triggers the callback if provided.
 */
void RectButton::setStatus(bool _status){
  if(!loaded)
  {
    log_e("RectButton widget not loaded");
    return;
  }

  status = _status;
  m_shouldRedraw = true;
  if(cb != nullptr){
    WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::SELF);
  }
  
}

