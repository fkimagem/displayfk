#include "wcheckbox.h"
#include <Arduino.h>
#include <esp_log.h>

const char *CheckBox::TAG = "CheckBox";

/**
 * @brief Constructor for the CheckBox class.
 * @param _x X-coordinate for the CheckBox position.
 * @param _y Y-coordinate for the CheckBox position.
 * @param _screen Screen identifier where the CheckBox will be displayed.
 */
CheckBox::CheckBox(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen), m_status(false), m_borderWidth(2), m_borderRadius(5){
  
  // Initialize with default config
  m_config = {.size = 0, .checkedColor = 0, .uncheckedColor = 0, .weight = CheckBoxWeight::LIGHT, .callback = nullptr};

  m_topRightPoint = {.x = 0, .y = 0};
  m_bottomCenterPoint = {.x = 0, .y = 0};
  m_middleLeftPoint = {.x = 0, .y = 0};

  ESP_LOGD(TAG, "CheckBox created at (%d, %d) on screen %d", _x, _y, _screen);
}

/**
 * @brief Destructor for the CheckBox class.
 *
 * Clears the callback function pointer and resets state.
 */
CheckBox::~CheckBox() { 
  m_callback = nullptr; 
  ESP_LOGD(TAG, "CheckBox destroyed at (%d, %d)", m_xPos, m_yPos);
}

/**
 * @brief Forces an immediate update of the CheckBox.
 *
 * Sets the flag to redraw the checkbox on the next redraw cycle.
 */
void CheckBox::forceUpdate() { 
  m_shouldRedraw = true; 
  ESP_LOGD(TAG, "CheckBox force update requested");
}

/**
 * @brief Detects if the CheckBox has been touched.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is within the CheckBox area, otherwise false.
 *
 * Changes the checkbox state if touched and sets the redraw flag.
 */
bool CheckBox::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
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

  // Check if touch is within bounds (with some tolerance for better UX)
  const uint8_t touchTolerance = 2;
  int xDetect = m_xPos - touchTolerance;
  int yDetect = m_yPos - touchTolerance;
  int widthDetect = m_config.size + touchTolerance;
  int heightDetect = m_config.size + touchTolerance;

  bool inBouds = POINT_IN_RECT(*_xTouch, *_yTouch, xDetect, yDetect, widthDetect, heightDetect);

  if (inBouds) {
    m_myTime = millis();
    changeState();
    m_shouldRedraw = true;
    ESP_LOGD(TAG, "CheckBox touched at (%d, %d), new status: %s", 
                  *_xTouch, *_yTouch, m_status ? "checked" : "unchecked");
    return true;
  }
  
  return false;
}

/**
 * @brief Retrieves the callback function associated with the CheckBox.
 * @return Pointer to the callback function.
 */
functionCB_t CheckBox::getCallbackFunc() { return m_callback; }

// getEnabled() and setEnabled() are inherited from WidgetBase

/**
 * @brief Changes the current state of the CheckBox (checked or unchecked).
 *
 * Inverts the current state of the checkbox.
 */
void CheckBox::changeState() { m_status = !m_status; }

/**
 * @brief Redraws the CheckBox on the screen, updating its appearance based on
 * its state.
 *
 * Displays the checkbox with a different appearance depending on whether it's
 * checked or unchecked. Only redraws if the checkbox is on the current screen
 * and needs updating.
 */
void CheckBox::redraw() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_INITIALIZED_VOID
  CHECK_LOADED_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_DEBOUNCE_REDRAW_VOID
  CHECK_SHOULDREDRAW_VOID

#if defined(DISP_DEFAULT)
  m_shouldRedraw = false;

  ESP_LOGD(TAG, "Redrawing checkbox at (%d,%d) size %d, status: %s", 
               m_xPos, m_yPos, m_config.size, m_status ? "checked" : "unchecked");

  // Get appropriate colors
  uint16_t bgColor = m_status ? m_config.checkedColor : getUncheckedColor();
  
  // Draw main checkbox area
  WidgetBase::objTFT->fillRoundRect(m_xPos, m_yPos, m_config.size, m_config.size, m_borderRadius, bgColor);
  
  // Draw border if configured
  if (m_borderWidth > 0) {
    drawBorder();
  }

  // Draw checkmark if checked
  if (m_status) {
    drawCheckmark();
  }
#endif
}


/**
 * @brief Configures the CheckBox with parameters defined in a configuration
 * structure.
 * @param config Structure containing the checkbox configuration parameters.
 */
void CheckBox::setup(const CheckBoxConfig &config) {
  // Validate TFT object
  CHECK_TFT_VOID

  // Validate configuration parameters
  if (config.size < 10 || config.size > 100) {
    ESP_LOGW(TAG, "CheckBox size %d is outside recommended range (10-100), using default", config.size);
    m_config.size = (config.size < 10) ? 10 : 100;
  } else {
    m_config.size = config.size;
  }
  
  // Copy configuration (no deep copy needed as struct has no pointers)
  m_config.checkedColor = config.checkedColor;
  m_config.uncheckedColor = config.uncheckedColor;
  m_config.callback = config.callback;
  m_config.weight = config.weight;

  uint8_t offsetCheckMark = m_borderRadius;
  m_topRightPoint = {.x = (uint16_t)(m_xPos + m_config.size - offsetCheckMark), .y = (uint16_t)(m_yPos + offsetCheckMark)};
  m_bottomCenterPoint = {.x = (uint16_t)(m_xPos + m_config.size / 2), .y = (uint16_t)(m_yPos + (m_config.size * 0.666 ))};
  m_middleLeftPoint = {.x = (uint16_t)(m_xPos + offsetCheckMark), .y = (uint16_t)(m_yPos + m_config.size / 2)};


  
  // Set callback and mark as initialized
  m_callback = m_config.callback;
  m_initialized = true;
  m_loaded = true;
  
  ESP_LOGD(TAG, "CheckBox configured: size=%d, checkedColor=0x%04X, callback=%s", 
               m_config.size, m_config.checkedColor, (m_callback != nullptr) ? "set" : "null");
}

/**
 * @brief Retrieves the current status of the CheckBox.
 * @return True if the CheckBox is checked, otherwise false.
 */
bool CheckBox::getStatus() const { return m_status; }

/**
 * @brief Sets the current state of the checkbox.
 * @param status True for checked, false for unchecked.
 *
 * Updates the checkbox state, marks it for redraw, and triggers the callback if
 * provided.
 */
void CheckBox::setStatus(bool status) {
  CHECK_LOADED_VOID
  
  m_status = status;
  m_shouldRedraw = true;
  
  if (m_callback != nullptr) {
    WidgetBase::addCallback(m_callback, WidgetBase::CallbackOrigin::SELF);
  }
  
  ESP_LOGD(TAG, "CheckBox status set to: %s", status ? "checked" : "unchecked");
}

void CheckBox::show() {
  m_visible = true;
  m_shouldRedraw = true;
  ESP_LOGD(TAG, "CheckBox shown at (%d, %d)", m_xPos, m_yPos);
}

void CheckBox::hide() {
  m_visible = false;
  m_shouldRedraw = true;
  ESP_LOGD(TAG, "CheckBox hidden at (%d, %d)", m_xPos, m_yPos);
}


/**
 * @brief Gets the unchecked color, using auto-calculated value if not set.
 * @return The unchecked color.
 */
uint16_t CheckBox::getUncheckedColor() {
  if (m_config.uncheckedColor != 0) {
    return m_config.uncheckedColor;
  }
  // Auto-calculate based on light/dark mode
  return WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
}

/**
 * @brief Gets the border color, using auto-calculated value if not set.
 * @return The border color.
 */
uint16_t CheckBox::getBorderColor() {
  return m_config.checkedColor;
}

/**
 * @brief Draws the checkmark symbol.
 */
void CheckBox::drawCheckmark() {

  uint8_t weight = static_cast<uint8_t>(m_config.weight);

  if(weight >= (int)CheckBoxWeight::HEAVY) {
    // Draw lines +2 and -2 y position from original lines
    WidgetBase::objTFT->drawLine(m_topRightPoint.x, m_topRightPoint.y + 2, m_bottomCenterPoint.x, m_bottomCenterPoint.y + 2, CFK_WHITE);
    WidgetBase::objTFT->drawLine(m_bottomCenterPoint.x, m_bottomCenterPoint.y + 2, m_middleLeftPoint.x, m_middleLeftPoint.y + 2, CFK_WHITE);
  }

  if(weight >= (int)CheckBoxWeight::MEDIUM) {
    // Draw lines +1 and -1 y position from original lines
    WidgetBase::objTFT->drawLine(m_topRightPoint.x, m_topRightPoint.y + 1, m_bottomCenterPoint.x, m_bottomCenterPoint.y + 1, CFK_WHITE);
    WidgetBase::objTFT->drawLine(m_bottomCenterPoint.x, m_bottomCenterPoint.y + 1, m_middleLeftPoint.x, m_middleLeftPoint.y + 1, CFK_WHITE);
  }
  
  WidgetBase::objTFT->drawLine(m_topRightPoint.x, m_topRightPoint.y, m_bottomCenterPoint.x, m_bottomCenterPoint.y, CFK_WHITE);
  WidgetBase::objTFT->drawLine(m_bottomCenterPoint.x, m_bottomCenterPoint.y, m_middleLeftPoint.x, m_middleLeftPoint.y, CFK_WHITE);  
}

/**
 * @brief Draws the checkbox border.
 */
void CheckBox::drawBorder() {
  
  uint16_t borderColor = getBorderColor();
  
  // Draw border around the checkbox
  for (uint8_t i = 0; i < m_borderWidth; i++) {
    WidgetBase::objTFT->drawRoundRect(m_xPos + i, m_yPos + i, 
                                      m_config.size - 2*i, m_config.size - 2*i, 
                                      m_borderRadius, borderColor);
  }
}

// isInitialized() is inherited from WidgetBase

/**
 * @brief Sets a new size for the checkbox.
 * @param newSize The new size (width and height).
 */
void CheckBox::setSize(uint16_t newSize) {
  CHECK_LOADED_VOID
  
  if (newSize < 10 || newSize > 100) {
    ESP_LOGW(TAG, "CheckBox size %d is outside recommended range (10-100)", newSize);
    return;
  }
  
  m_config.size = newSize;
  m_shouldRedraw = true;
  
  ESP_LOGD(TAG, "CheckBox size changed to: %d", newSize);
}

/**
 * @brief Gets the current size of the checkbox.
 * @return The current size.
 */
uint16_t CheckBox::getSize() const {
  return m_config.size;
}

/**
 * @brief Sets the colors for the checkbox.
 * @param checkedColor Color when checked.
 * @param uncheckedColor Color when unchecked (0 = auto).
 */
void CheckBox::setColors(uint16_t checkedColor, uint16_t uncheckedColor) {
  CHECK_LOADED_VOID
  
  m_config.checkedColor = checkedColor;
  m_config.uncheckedColor = uncheckedColor;
  m_shouldRedraw = true;
  
  ESP_LOGD(TAG, "CheckBox colors updated: checked=0x%04X, unchecked=0x%04X", 
               checkedColor, uncheckedColor);
}