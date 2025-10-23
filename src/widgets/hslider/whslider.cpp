#include "whslider.h"
#include <Arduino.h>
#include <esp_log.h>

const char* HSlider::TAG = "HSlider";

// if defined detect touch only when on handler circle, else, detect on rect total area and calculate handler position
// #define DETECT_ON_HANDLER

/**
 * @brief Constructor for the HSlider class.
 * @param _x X-coordinate for the HSlider position.
 * @param _y Y-coordinate for the HSlider position.
 * @param _screen Screen identifier where the HSlider will be displayed.
 */
HSlider::HSlider(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen), m_currentPos(0), m_lastPos(0),
      m_height(0), m_contentRadius(0), m_centerV(0), m_minX(0), m_maxX(0),
      m_value(0)
{
  // Initialize with default config
  m_config = {.width = 0, .pressedColor = 0, .backgroundColor = 0, .minValue = 0, .maxValue = 0, .radius = 0, .callback = nullptr};
  
  ESP_LOGD(TAG, "HSlider created at (%d, %d) on screen %d", _x, _y, _screen);
}

/**
 * @brief Destructor for the HSlider class.
 */
HSlider::~HSlider() {
  ESP_LOGD(TAG, "HSlider destroyed at (%d, %d)", m_xPos, m_yPos);
  
  // Clear callback
  if (m_callback != nullptr) {
    m_callback = nullptr;
  }
}

/**
 * @brief Detects if the slider has been touched and processes the touch input.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is within the slider handle area, otherwise false.
 *
 * Updates the slider position and value based on the touch position.
 */
bool HSlider::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  // Early validation checks using macros
  CHECK_VISIBLE_BOOL
  CHECK_INITIALIZED_BOOL
  CHECK_LOADED_BOOL
  CHECK_USINGKEYBOARD_BOOL
  CHECK_CURRENTSCREEN_BOOL
  CHECK_ENABLED_BOOL
  CHECK_LOCKED_BOOL
  CHECK_POINTER_TOUCH_NULL_BOOL

  bool detected = false;

  #if defined(DETECT_ON_HANDLER)
  int32_t offsetRadius = 10; // Offset to account for the radius of the slider handle
  int32_t radiusToDetect = m_config.radius + offsetRadius; // Add a small margin for touch detection
  int32_t deltaX = (*_xTouch - m_currentPos) * (*_xTouch - m_currentPos);
  int32_t deltaY = (*_yTouch - (m_yPos + radiusToDetect)) *
                   (*_yTouch - (m_yPos + radiusToDetect));
  int32_t radiusQ = radiusToDetect * radiusToDetect;

  if ((deltaX < radiusQ) && (deltaY < radiusQ)) {
    detected = true;
    m_currentPos = (*_xTouch);
    m_currentPos = constrain(m_currentPos, m_minX, m_maxX);
    updateValue();
    m_shouldRedraw = true;
  }
  #else
  if(POINT_IN_RECT(*_xTouch, *_yTouch, m_minX, m_yPos, (m_config.width), m_height)) {
    m_myTime = millis();
    detected = true;
    m_currentPos = (*_xTouch);
    m_currentPos = constrain(m_currentPos, m_minX, m_maxX);
    updateValue();
    m_shouldRedraw = true;
  }
  #endif

  if (detected) {
    ESP_LOGD(TAG, "HSlider touched at (%d, %d), value: %d", *_xTouch, *_yTouch, m_value);
  }

  return detected;
}

/**
 * @brief Retrieves the callback function associated with the slider.
 * @return Pointer to the callback function.
 */
functionCB_t HSlider::getCallbackFunc() { return m_callback; }


/**
 * @brief Validates the configuration structure.
 * @param config Configuration to validate.
 * @return True if valid, false otherwise.
 */
bool HSlider::validateConfig(const HSliderConfig& config) {
  // Validate basic parameters
  if (config.width <= 0) {
    ESP_LOGE(TAG, "Invalid width: %d", config.width);
    return false;
  }
  
  if (config.minValue >= config.maxValue) {
    ESP_LOGE(TAG, "Invalid range: min=%d, max=%d", config.minValue, config.maxValue);
    return false;
  }
  
  if (config.radius <= 0) {
    ESP_LOGE(TAG, "Invalid radius: %d", config.radius);
    return false;
  }
  
  return true;
}

/**
 * @brief Updates the slider value based on current position.
 */
void HSlider::updateValue() {
  m_value = map(m_currentPos, m_minX, m_maxX, m_config.minValue, m_config.maxValue);
}

/**
 * @brief Updates the slider dimensions and constraints.
 */
void HSlider::updateDimensions() {
  m_height = m_config.radius * 2;
  m_centerV = m_yPos + m_config.radius;
  m_minX = m_xPos + m_config.radius;
  m_maxX = m_xPos + m_config.width - m_config.radius;
  m_contentRadius = m_config.radius - 4; // Inner radius for content
}

/**
 * @brief Initializes the HSlider widget with default calculations.
 */
void HSlider::start() {
  updateDimensions();
  m_shouldRedraw = true;
  ESP_LOGD(TAG, "HSlider initialized at (%d, %d)", m_xPos, m_yPos);
}

// setEnabled method removed - use inherited methods from WidgetBase

/**
 * @brief Redraws the slider handle and filled portion on the screen.
 *
 * Updates the visual appearance of the slider based on its current value.
 * Only redraws if the slider is on the current screen and needs updating.
 */
void HSlider::redraw() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_INITIALIZED_VOID
  CHECK_LOADED_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_DEBOUNCE_FAST_REDRAW_VOID
  CHECK_SHOULDREDRAW_VOID


  m_shouldRedraw = false;

  uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;

  // Clear previous position
  WidgetBase::objTFT->fillCircle(m_lastPos, m_centerV, m_contentRadius,
                                 m_config.backgroundColor); // slider background

  ESP_LOGD(TAG, "Redraw HSlider with value %i at %i", m_value, m_currentPos);

  uint8_t fillRadius = 2;

  // Draw progress bar
  if(m_lastPos > m_currentPos){

    WidgetBase::objTFT->fillRoundRect(m_currentPos, m_centerV - (fillRadius),
                                  m_lastPos - (m_currentPos), fillRadius * 2,
                                  fillRadius, m_config.backgroundColor);
  }else{
    WidgetBase::objTFT->fillRoundRect(m_minX, m_centerV - (fillRadius),
                                  m_currentPos - (m_minX), fillRadius * 2,
                                  fillRadius, m_config.pressedColor);
  }
  
  // Draw slider handle
  WidgetBase::objTFT->fillCircle(m_currentPos, m_centerV, m_contentRadius,
                                 lightBg); // slider handle
  WidgetBase::objTFT->fillCircle(m_currentPos, m_centerV,
                                 m_contentRadius * 0.75,
                                 m_config.pressedColor); // slider center

  m_lastPos = m_currentPos;
}

/**
 * @brief Draws the slider's background and static elements.
 *
 * Creates the track and border for the slider.
 * Called during initial setup and when the slider needs a complete redraw.
 */
void HSlider::drawBackground() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_LOADED_VOID

  uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

  WidgetBase::objTFT->fillRoundRect(m_xPos, m_yPos, m_config.width, m_height, m_config.radius,
                                    m_config.backgroundColor);
  WidgetBase::objTFT->drawRoundRect(m_xPos, m_yPos, m_config.width, m_height, m_config.radius,
                                    baseBorder);

  redraw();
}

/**
 * @brief Configures the HSlider with parameters defined in a configuration
 * structure.
 * @param config Structure containing the slider configuration parameters.
 */
void HSlider::setup(const HSliderConfig &config) {
  // Validate TFT object
  CHECK_TFT_VOID

  // Validate configuration first
  if (!validateConfig(config)) {
    ESP_LOGE(TAG, "Invalid configuration provided");
    return;
  }

  // Assign the configuration structure
  m_config = config;

  start();

  m_currentPos = m_minX;
  m_lastPos = m_currentPos;

  updateValue();

  m_loaded = true;
  m_initialized = true;

  ESP_LOGD(TAG, "HSlider setup completed at (%d, %d)", m_xPos, m_yPos);
}

/**
 * @brief Retrieves the current value of the slider.
 * @return Current value of the slider.
 */
int HSlider::getValue() const { return m_value; }

/**
 * @brief Sets the value of the slider.
 * @param newValue New value for the slider.
 *
 * Updates the slider position based on the provided value and triggers the
 * callback.
 */
void HSlider::setValue(int newValue) {
  CHECK_LOADED_VOID
  CHECK_INITIALIZED_VOID
  
  m_value = constrain(newValue, m_config.minValue, m_config.maxValue);
  
  // Update position based on value
  m_currentPos = map(m_value, m_config.minValue, m_config.maxValue, m_minX, m_maxX);
  
  m_shouldRedraw = true;
  
  if (m_callback != nullptr) {
    WidgetBase::addCallback(m_callback, WidgetBase::CallbackOrigin::SELF);
  }
  
  ESP_LOGD(TAG, "HSlider value set to %d", m_value);
}

/**
 * @brief Forces the slider to update, refreshing its visual state on next
 * redraw.
 */
void HSlider::forceUpdate() { 
  m_shouldRedraw = true; 
  ESP_LOGD(TAG, "HSlider force update requested");
}

void HSlider::show() {
  m_visible = true;
  m_shouldRedraw = true;
  ESP_LOGD(TAG, "HSlider shown at (%d, %d)", m_xPos, m_yPos);
}

void HSlider::hide() {
  m_visible = false;
  m_shouldRedraw = true;
  ESP_LOGD(TAG, "HSlider hidden at (%d, %d)", m_xPos, m_yPos);
}
