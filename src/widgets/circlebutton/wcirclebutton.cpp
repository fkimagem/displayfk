#include "wcirclebutton.h"

/**
 * @brief Constructor for the CircleButton class.
 * @param _x X position of the button.
 * @param _y Y position of the button.
 * @param _screen Screen number.
 */
CircleButton::CircleButton(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen), m_shouldRedraw(true) {}

/**
 * @brief Destructor for the CircleButton class.
 */
CircleButton::~CircleButton() {}

/**
 * @brief Detects a touch on the button.
 * @param _xTouch X position of the touch.
 * @param _yTouch Y position of the touch.
 * @return True if the touch is detected, false otherwise.
 */
bool CircleButton::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  if (!visible) {
    return false;
  }
#if defined(HAS_TOUCH)
  if (WidgetBase::usingKeyboard || WidgetBase::currentScreen != screen ||
      !loaded) {
    return false;
  }

  if (millis() - m_myTime < TIMEOUT_CLICK) {
    return false;
  }

  if (!m_enabled) {
    log_d("CircleButton is disabled");
    return false;
  }

  m_myTime = millis();
  bool detected = false;
  int32_t deltaX = (*_xTouch - xPos) * (*_xTouch - xPos);
  int32_t deltaY = (*_yTouch - yPos) * (*_yTouch - yPos);
  int32_t radiusQ = m_radius * m_radius;

  if ((deltaX < radiusQ) && (deltaY < radiusQ)) {
    changeState();
    m_shouldRedraw = true;
    detected = true;
  }
  return detected;
#else
  return false;
#endif
}

/**
 * @brief Gets the callback function for the button.
 * @return Pointer to the callback function.
 */
functionCB_t CircleButton::getCallbackFunc() { return cb; }

/**
 * @brief Forces an immediate update of the CircleButton.
 *
 * Sets the flag to redraw the button on the next redraw cycle.
 */
void CircleButton::forceUpdate() { m_shouldRedraw = true; }

/**
 * @brief Retrieves the current enabled state of the button.
 * @return True if the button is enabled, otherwise false.
 */
bool CircleButton::getEnabled() { return m_enabled; }

/**
 * @brief Sets the enabled state of the button.
 * @param newState True to enable the button, false to disable it.
 */
void CircleButton::setEnabled(bool newState) { m_enabled = newState; }

/**
 * @brief Changes the current state of the CircleButton (pressed or not
 * pressed).
 *
 * Inverts the current state of the button.
 */
void CircleButton::changeState() { m_status = !m_status; }

/**
 * @brief Redraws the CircleButton on the screen, updating its appearance based
 * on its state.
 *
 * Displays the circular button with different appearance depending on whether
 * it's pressed or not. Only redraws if the button is on the current screen and
 * needs updating.
 */
void CircleButton::redraw() {
  CHECK_TFT_VOID
  if (!visible) {
    return;
  }
#if defined(DISP_DEFAULT)
  if (WidgetBase::currentScreen != screen || !loaded || !m_shouldRedraw) {
    return;
  }
  m_shouldRedraw = false;
  uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
  uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

  log_d("Redrawing circlebutton");

  WidgetBase::objTFT->fillCircle(xPos, yPos, m_radius,
                                 lightBg); // Button background
  WidgetBase::objTFT->drawCircle(xPos, yPos, m_radius,
                                 baseBorder); // Button border
  uint16_t bgColor = m_status ? m_pressedColor : lightBg;
  WidgetBase::objTFT->fillCircle(xPos, yPos, m_radius * 0.75,
                                 bgColor); // Inner background
  WidgetBase::objTFT->drawCircle(xPos, yPos, m_radius * 0.75,
                                 baseBorder); // Inner border
#endif
}

/**
 * @brief Initializes the CircleButton with default constraints.
 *
 * Constrains the button radius between 5 and 200 pixels.
 */
void CircleButton::start() {
  m_radius =
      constrain(m_radius, 5, 200); // Limits the button radius between 5 and 200
}

/**
 * @brief Configures the CircleButton widget with specific radius, color, and
 * callback function.
 * @param _radius Radius of the circular button.
 * @param _pressedColor Color displayed when the button is pressed.
 * @param _cb Callback function to execute when the button state changes.
 *
 * Initializes the button properties and marks it as loaded when complete.
 */
void CircleButton::setup(uint16_t _radius, uint16_t _pressedColor,
                         functionCB_t _cb) {
  if (!WidgetBase::objTFT) {
    log_e("TFT not defined on WidgetBase");
    return;
  }
  if (loaded) {
    log_d("CircleButton widget already configured");
    return;
  }
  m_radius = _radius;
  m_pressedColor = _pressedColor;
  cb = _cb;
  start();
  // redraw();

  loaded = true;
}

/**
 * @brief Configures the CircleButton with parameters defined in a configuration
 * structure.
 * @param config Structure containing the button configuration parameters.
 */
void CircleButton::setup(const CircleButtonConfig &config) {
  setup(config.radius, config.pressedColor, config.callback);
}

/**
 * @brief Retrieves the current status of the CircleButton.
 * @return True if the button is pressed, otherwise false.
 */
bool CircleButton::getStatus() { return m_status; }

/**
 * @brief Sets the current state of the CircleButton.
 * @param _status True for pressed, false for not pressed.
 *
 * Updates the button state, marks it for redraw, and triggers the callback if
 * provided.
 */
void CircleButton::setStatus(bool _status) {
  if (!loaded) {
    log_e("CircleButton widget not loaded");
    return;
  }
  if (m_status == _status) {
    log_d("CircleButton widget already set to this status");
    return;
  }

  m_status = _status;
  m_shouldRedraw = true;

  if (cb != nullptr) {
    WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::SELF);
  }
}

void CircleButton::show() {
  visible = true;
  m_shouldRedraw = true;
}

void CircleButton::hide() {
  visible = false;
  m_shouldRedraw = true;
}