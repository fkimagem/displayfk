#include "wcirclebutton.h"
#include <esp_log.h>

const char* CircleButton::TAG = "CircleButton";

/**
 * @brief Constructor for the CircleButton class.
 * @param _x X position of the button.
 * @param _y Y position of the button.
 * @param _screen Screen number.
 */
CircleButton::CircleButton(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen), m_status(false)
{
    m_config = {.radius = 0, .pressedColor = 0, .callback = nullptr};
    ESP_LOGD(TAG, "CircleButton created at (%d, %d) on screen %d", _x, _y, _screen);
}

/**
 * @brief Destructor for the CircleButton class.
 */
CircleButton::~CircleButton() {
    ESP_LOGD(TAG, "CircleButton destroyed at (%d, %d)", m_xPos, m_yPos);
    if (m_config.callback != nullptr) { m_config.callback = nullptr; }
}

/**
 * @brief Detects a touch on the button.
 * @param _xTouch X position of the touch.
 * @param _yTouch Y position of the touch.
 * @return True if the touch is detected, false otherwise.
 */
bool CircleButton::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  // Early validation checks using macros
  CHECK_VISIBLE_BOOL
  CHECK_INITIALIZED_BOOL
  CHECK_LOADED_BOOL
  CHECK_USINGKEYBOARD_BOOL
  CHECK_CURRENTSCREEN_BOOL
  CHECK_DEBOUNCE_CLICK_BOOL
  CHECK_ENABLED_BOOL
  CHECK_LOCKED_BOOL
  CHECK_POINTER_TOUCH_NULL_BOOL

#if defined(HAS_TOUCH)
  
  bool inBounds = POINT_IN_CIRCLE(*_xTouch, *_yTouch, m_xPos, m_yPos, m_config.radius);

  if (inBounds) {
    m_myTime = millis();
    changeState();
    m_shouldRedraw = true;
    ESP_LOGD(TAG, "CircleButton touched at (%d, %d), new status: %s", 
                  *_xTouch, *_yTouch, m_status ? "pressed" : "released");
    return true;
  }
  return false;
#else
  return false;
#endif
}

/**
 * @brief Gets the callback function for the button.
 * @return Pointer to the callback function.
 */
functionCB_t CircleButton::getCallbackFunc() { return m_config.callback; }

/**
 * @brief Forces an immediate update of the CircleButton.
 *
 * Sets the flag to redraw the button on the next redraw cycle.
 */
void CircleButton::forceUpdate() { 
  m_shouldRedraw = true; 
  ESP_LOGD(TAG, "CircleButton force update requested");
}


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
  CHECK_VISIBLE_VOID
  CHECK_INITIALIZED_VOID
  CHECK_LOADED_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_SHOULDREDRAW_VOID

#if defined(DISP_DEFAULT)
  m_shouldRedraw = false;

  uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
  uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

  ESP_LOGD(TAG, "Redrawing circlebutton at (%d,%d) radius %d, status: %s", 
                m_xPos, m_yPos, m_config.radius, m_status ? "pressed" : "released");

  WidgetBase::objTFT->fillCircle(m_xPos, m_yPos, m_config.radius,
                                 lightBg); // Button background
  WidgetBase::objTFT->drawCircle(m_xPos, m_yPos, m_config.radius,
                                 baseBorder); // Button border
  uint16_t bgColor = m_status ? m_config.pressedColor : lightBg;
  WidgetBase::objTFT->fillCircle(m_xPos, m_yPos, m_config.radius * 0.75,
                                 bgColor); // Inner background
  WidgetBase::objTFT->drawCircle(m_xPos, m_yPos, m_config.radius * 0.75,
                                 baseBorder); // Inner border
#endif
}

/**
 * @brief Initializes the CircleButton with default constraints.
 *
 * Constrains the button radius between 5 and 200 pixels.
 */
void CircleButton::start() {
  m_config.radius =
      constrain(m_config.radius, 5, 200); // Limits the button radius between 5 and 200
}


/**
 * @brief Configures the CircleButton with parameters defined in a configuration
 * structure.
 * @param config Structure containing the button configuration parameters.
 */
void CircleButton::setup(const CircleButtonConfig &config) {
  // Validate TFT object
  CHECK_TFT_VOID
  
  m_config = config;
  start();
  // redraw();

  m_loaded = true;
  m_initialized = true;
  ESP_LOGD(TAG, "CircleButton setup completed at (%d, %d) with radius %d", 
                m_xPos, m_yPos, m_config.radius);
}

/**
 * @brief Retrieves the current status of the CircleButton.
 * @return True if the button is pressed, otherwise false.
 */
bool CircleButton::getStatus() const { return m_status; }

/**
 * @brief Sets the current state of the CircleButton.
 * @param _status True for pressed, false for not pressed.
 *
 * Updates the button state, marks it for redraw, and triggers the callback if
 * provided.
 */
void CircleButton::setStatus(bool _status) {
  CHECK_LOADED_VOID
  
  if (m_status == _status) {
    ESP_LOGD(TAG, "CircleButton widget already set to this status");
    return;
  }

  m_status = _status;
  m_shouldRedraw = true;

  if (m_config.callback != nullptr) {
    WidgetBase::addCallback(m_config.callback, WidgetBase::CallbackOrigin::SELF);
  }
  ESP_LOGD(TAG, "CircleButton status changed to %s", _status ? "pressed" : "released");
}

void CircleButton::show() {
  m_visible = true;
  m_shouldRedraw = true;
  ESP_LOGD(TAG, "CircleButton shown at (%d, %d)", m_xPos, m_yPos);
}

void CircleButton::hide() {
  m_visible = false;
  m_shouldRedraw = true;
  ESP_LOGD(TAG, "CircleButton hidden at (%d, %d)", m_xPos, m_yPos);
}