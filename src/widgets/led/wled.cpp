#include "wled.h"

const char* Led::TAG = "Led";

/**
 * @brief Constructor for Led widget.
 * @param _x X-coordinate for the LED position.
 * @param _y Y-coordinate for the LED position.
 * @param _screen Screen identifier where the LED will be displayed.
 */
Led::Led(uint16_t _x, uint16_t _y, uint8_t _screen) 
    : WidgetBase(_x, _y, _screen),
      m_lastStatus(false),
      m_status(false),
      m_shouldRedraw(true),
      m_initialized(false)
{
  // Initialize with default config
  m_config = {.radius = 10, .colorOn = CFK_RED, .colorOff = 0};
  
  // Initialize gradient array
  for (uint8_t i = 0; i < m_colorLightGradientSize; i++) {
    m_colorLightGradient[i] = CFK_WHITE;
  }
  
  ESP_LOGD(TAG, "Led created at (%d, %d) on screen %d", _x, _y, _screen);
}

/**
 * @brief Destructor for the Led class.
 */
Led::~Led() {
  ESP_LOGD(TAG, "Led destroyed at (%d, %d)", m_xPos, m_yPos);
  
  // Use centralized cleanup method
  cleanupMemory();
  
  // Clear callback
  if (m_callback != nullptr) {
    m_callback = nullptr;
  }
  
  // Reset state
  m_loaded = false;
  m_initialized = false;
  m_shouldRedraw = false;
}

/**
 * @brief Detects if the LED has been touched.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return Always returns false as LEDs don't respond to touch events.
 */
bool Led::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  // Early validation checks using macros
  CHECK_VISIBLE_BOOL
  CHECK_INITIALIZED_BOOL
  CHECK_LOADED_BOOL
  CHECK_USINGKEYBOARD_BOOL
  CHECK_CURRENTSCREEN_BOOL
  CHECK_DEBOUNCE_CLICK_BOOL
  CHECK_ENABLED_BOOL
  CHECK_LOCKED_BOOL

  // Validate touch coordinates
  if (_xTouch == nullptr || _yTouch == nullptr) {
    ESP_LOGW(TAG, "Touch coordinates are null");
    return false;
  }

  // LEDs don't respond to touch events
  UNUSED(_xTouch);
  UNUSED(_yTouch);
  return false;
}

/**
 * @brief Retrieves the callback function associated with the LED.
 * @return Pointer to the callback function.
 */
functionCB_t Led::getCallbackFunc() { return m_callback; }

/**
 * @brief Sets the state of the LED (on or off).
 * @param newValue New state for the LED (true for on, false for off).
 */
void Led::setState(bool newValue) {
  CHECK_LOADED_VOID
  CHECK_INITIALIZED_VOID
  
  if (m_status != newValue) {
    m_status = newValue;
    m_shouldRedraw = true;
    
    // Update gradient if needed
    if (m_status) {
      updateGradient();
    }
    
    ESP_LOGD(TAG, "Led state set to %s", m_status ? "ON" : "OFF");
  }
}

/**
 * @brief Gets the current state of the LED.
 * @return Current state of the LED (true for on, false for off).
 */
bool Led::getState() const {
  return m_status;
}

/**
 * @brief Validates the configuration structure.
 * @param config Configuration to validate.
 * @return True if valid, false otherwise.
 */
bool Led::validateConfig(const LedConfig& config) {
  // Validate basic parameters
  if (config.radius <= 0) {
    ESP_LOGE(TAG, "Invalid radius: %d", config.radius);
    return false;
  }
  
  if (config.radius > 50) {
    ESP_LOGW(TAG, "Radius %d is quite large, may impact performance", config.radius);
  }
  
  return true;
}

/**
 * @brief Updates the color gradient for the light effect.
 */
void Led::updateGradient() {
  // Create gradient from bright to dim
  uint16_t baseColor = m_config.colorOn;
  for (uint8_t i = 0; i < m_colorLightGradientSize; i++) {
    // Calculate alpha-like effect by mixing with background
    uint8_t intensity = 255 - (i * 50); // Decrease intensity for each ring
    if (intensity < 50) intensity = 50; // Minimum visibility
    
    // Simple color mixing (this could be enhanced with proper color blending)
    m_colorLightGradient[i] = baseColor;
  }
}

/**
 * @brief Gets the off color for the LED.
 * @return Color to use when LED is off.
 */
uint16_t Led::getOffColor() {
  if (m_config.colorOff != 0) {
    return m_config.colorOff;
  }
  // Auto-calculate based on light/dark mode
  return WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
}

/**
 * @brief Gets the background color for the LED.
 * @return Background color to use.
 */
uint16_t Led::getBackgroundColor() {
  return m_config.colorOff;
}

/**
 * @brief Centralized memory cleanup method.
 */
void Led::cleanupMemory() {
  // Led doesn't use dynamic memory, but method is here for consistency
  // Reset gradient array to default values
  for (uint8_t i = 0; i < m_colorLightGradientSize; i++) {
    m_colorLightGradient[i] = CFK_WHITE;
  }
}

void Led::drawBackground() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_INITIALIZED_VOID
  CHECK_LOADED_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_CURRENTSCREEN_VOID

  uint16_t borderColor = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;
  WidgetBase::objTFT->drawCircle(m_xPos, m_yPos, m_config.radius, borderColor);
  
  ESP_LOGD(TAG, "Led background drawn at (%d, %d)", m_xPos, m_yPos);
}

/**
 * @brief Redraws the LED on the screen, updating its appearance.
 * 
 * Displays the LED with proper visual representation based on its current state.
 * The LED is drawn with gradient effect when on and solid color when off.
 */
void Led::redraw() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_INITIALIZED_VOID
  CHECK_LOADED_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_DEBOUNCE_REDRAW_VOID

  if (!m_shouldRedraw) {
    return;
  }

  m_shouldRedraw = false;
  
  ESP_LOGD(TAG, "Redrawing Led at (%d, %d), status: %s", m_xPos, m_yPos, m_status ? "ON" : "OFF");

  if (m_status) {
    for(uint8_t i = 0; i < m_colorLightGradientSize; i++)
    {
      uint8_t radius = m_config.radius - (i * 4);
      if(radius > m_config.radius){
        continue;//Aborta o loop se o raio for maior que o raio do LED (overflow do tipo uint8_t)
      }
      WidgetBase::objTFT->fillCircle(m_xPos - (2 * i), m_yPos - (2 * i), radius, m_colorLightGradient[i]);
    }
  } else {
    // Draw off state
    WidgetBase::objTFT->fillCircle(m_xPos, m_yPos, m_config.radius - 1, getOffColor());
  }
}

/**
 * @brief Forces the LED to update, refreshing its visual state on next redraw.
 */
void Led::forceUpdate()
{
  m_shouldRedraw = true;
}

/**
 * @brief Configures the LED widget with parameters defined in a configuration structure.
 * @param config Structure containing the LED configuration parameters (radius and colorOn).
 */
void Led::setup(const LedConfig& config) {
  CHECK_TFT_VOID
  if (m_loaded) {
    ESP_LOGD(TAG, "Led widget already configured");
    return;
  }

  // Validate configuration first
  if (!validateConfig(config)) {
    ESP_LOGE(TAG, "Invalid configuration provided");
    return;
  }

  // Clean up any existing memory
  cleanupMemory();

  // Assign the configuration structure
  m_config = config;
  
  // Update gradient if needed
  updateGradient();
  
  // Initialize LED
  m_shouldRedraw = true;
  m_loaded = true;
  m_initialized = true;
  
  ESP_LOGD(TAG, "Led setup completed at (%d, %d)", m_xPos, m_yPos);
}

void Led::show() {
  m_visible = true;
  m_shouldRedraw = true;
  ESP_LOGD(TAG, "Led shown at (%d, %d)", m_xPos, m_yPos);
}

void Led::hide() {
  m_visible = false;
  m_shouldRedraw = true;
  ESP_LOGD(TAG, "Led hidden at (%d, %d)", m_xPos, m_yPos);
}