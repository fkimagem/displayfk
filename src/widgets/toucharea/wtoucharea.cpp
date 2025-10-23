#include "wtoucharea.h"

const char* TouchArea::TAG = "[TouchArea]";

/**
 * @brief Constructor for the TouchArea class.
 * @param _x X-coordinate for the TouchArea position.
 * @param _y Y-coordinate for the TouchArea position.
 * @param _screen Screen identifier where the TouchArea will be displayed.
 */
TouchArea::TouchArea(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen) {}

/**
 * @brief Destructor for the TouchArea class.
 */
TouchArea::~TouchArea() {}

/**
 * @brief Detects if the TouchArea has been touched.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is within the TouchArea, otherwise false.
 *
 * Determines if a touch event occurred within the defined area's boundaries.
 */
bool TouchArea::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  CHECK_VISIBLE_BOOL
  CHECK_USINGKEYBOARD_BOOL
  CHECK_CURRENTSCREEN_BOOL
  CHECK_LOADED_BOOL
  CHECK_DEBOUNCE_CLICK_BOOL
  CHECK_ENABLED_BOOL
  CHECK_LOCKED_BOOL
  CHECK_POINTER_TOUCH_NULL_BOOL

  bool inBounds = POINT_IN_RECT(*_xTouch, *_yTouch, m_xPos, m_yPos, m_width, m_height);
  if(inBounds) {
    m_myTime = millis();
  }

  return inBounds;
}

/**
 * @brief Retrieves the callback function associated with the TouchArea.
 * @return Pointer to the callback function.
 */
functionCB_t TouchArea::getCallbackFunc() { return m_callback; }

/**
 * @brief Redraws the TouchArea on the screen.
 *
 * Empty implementation since TouchArea is invisible by default.
 */
void TouchArea::redraw() {
  if (!m_visible) {
    return;
  }
}

/**
 * @brief Simulates a click on the TouchArea.
 *
 * Triggers the callback function as if the area was touched.
 * Useful for programmatically activating the touch area.
 */
void TouchArea::onClick() {
#if defined(DFK_TOUCHAREA)
  if (!m_loaded) {
    ESP_LOGE(TAG, "TouchArea widget not loaded");
    return;
  }
  if (m_callback != nullptr) {
    WidgetBase::addCallback(m_callback, WidgetBase::CallbackOrigin::SELF);
  }
#endif
}

/**
 * @brief Configures the TouchArea with specific dimensions and a callback
 * function.
 * @param _width Width of the TouchArea.
 * @param _height Height of the TouchArea.
 * @param _cb Callback function to execute when the area is touched.
 *
 * Initializes the TouchArea properties and marks it as loaded when complete.
 */
void TouchArea::setup(uint16_t _width, uint16_t _height, functionCB_t _cb) {
  if (!WidgetBase::objTFT) {
    ESP_LOGE(TAG, "TFT not defined on WidgetBase");
    return;
  }
  if (m_loaded) {
    ESP_LOGD(TAG, "TouchArea widget already configured");
    return;
  }
  m_width = _width;
  m_height = _height;
  m_callback = _cb;
  m_loaded = true;
}

/**
 * @brief Configures the TouchArea with parameters defined in a configuration
 * structure.
 * @param config Structure containing the touch area configuration parameters.
 */
void TouchArea::setup(const TouchAreaConfig &config) {
  setup(config.width, config.height, config.callback);
}

/**
 * @brief Changes the current state of the touch area.
 *
 * Not implemented in the current version.
 */
void TouchArea::changeState() {
  // Not implemented
}

/**
 * @brief Retrieves the current status of the TouchArea.
 * @return Current status of the touch area.
 */
bool TouchArea::getStatus() { return m_status; }

void TouchArea::show() {
  m_visible = true;
  m_shouldRedraw = true;
}

void TouchArea::hide() {
  m_visible = false;
  m_shouldRedraw = true;
}

void TouchArea::forceUpdate() { m_shouldRedraw = true; }