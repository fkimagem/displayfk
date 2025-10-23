#include "wspinbox.h"

const char* SpinBox::TAG = "SpinBox";



/**
 * @brief Decreases the current value by the step amount.
 *
 * Decrements the current value while ensuring it remains within the defined
 * range.
 */
void SpinBox::decreaseValue() {
  int temp = m_currentValue - m_config.step;
  m_currentValue = constrain(temp, m_config.minValue, m_config.maxValue);
}

/**
 * @brief Increases the current value by the step amount.
 *
 * Increments the current value while ensuring it remains within the defined
 * range.
 */
void SpinBox::increaseValue() {
  int temp = m_currentValue + m_config.step;
  m_currentValue = constrain(temp, m_config.minValue, m_config.maxValue);
}

/**
 * @brief Constructor for the SpinBox class.
 * @param _x X-coordinate for the SpinBox position.
 * @param _y Y-coordinate for the SpinBox position.
 * @param _screen Screen identifier where the SpinBox will be displayed.
 */
SpinBox::SpinBox(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen){

      m_config = {.width = 0, .height = 0, .step = 0, .minValue = 0, .maxValue = 0, .startValue = 0, .color = 0, .textColor = 0, .callback = nullptr};
      ESP_LOGD(TAG, "SpinBox created at (%d, %d) on screen %d", _x, _y, _screen);
    }

/**
 * @brief Destructor for the SpinBox class.
 */
SpinBox::~SpinBox() {
    cleanupMemory();
}

void SpinBox::cleanupMemory() {
    // SpinBox doesn't use dynamic memory allocation
    ESP_LOGD(TAG, "SpinBox memory cleanup completed");
}

/**
 * @brief Detects if the SpinBox has been touched and handles
 * increment/decrement actions.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is within the SpinBox area, otherwise false.
 *
 * Detects touches in either the decrement (left) or increment (right) button
 * areas, and processes the value change accordingly.
 */
bool SpinBox::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  CHECK_VISIBLE_BOOL
  CHECK_USINGKEYBOARD_BOOL
  CHECK_CURRENTSCREEN_BOOL
  CHECK_LOADED_BOOL
  CHECK_DEBOUNCE_CLICK_BOOL
  bool detectado = false;

  uint16_t topY = m_yPos;
  // uint16_t bottomY = m_yPos + m_height;

  Rect_t boundsAreaDecrement = {.x = m_xPos, .y = topY, .width = (uint16_t)(m_config.width / 2), .height = m_config.height};
  Rect_t boundsAreaIncrement = {.x = (uint16_t)(m_xPos + (m_config.width / 2)), .y = topY, .width = (uint16_t)(m_config.width / 2),
                                .height = m_config.height};

  /*uint16_t d_x = m_xPos;
  uint16_t d_x_max = m_xPos + m_width;

  uint16_t y_min = m_yPos;
  uint16_t y_max = m_yPos + m_height;*/

  // uint16_t i_x = m_xPos + m_width - m_height;
  // uint16_t i_x_max = m_xPos + m_width;

  // Detect decrement
  if ((*_xTouch > boundsAreaDecrement.x) &&
      (*_xTouch < boundsAreaDecrement.x + boundsAreaDecrement.width) &&
      (*_yTouch > boundsAreaDecrement.y) &&
      (*_yTouch < boundsAreaDecrement.y + boundsAreaDecrement.height)) {
    decreaseValue();
    m_shouldRedraw = true;
    m_myTime = millis();
    detectado = true;
  }

  if (detectado) {
    return true;
  }

  // Detect increment
  if ((*_xTouch > boundsAreaIncrement.x) &&
      (*_xTouch < boundsAreaIncrement.x + boundsAreaIncrement.width) &&
      (*_yTouch > boundsAreaIncrement.y) &&
      (*_yTouch < boundsAreaIncrement.y + boundsAreaIncrement.height)) {
    increaseValue();
    m_shouldRedraw = true;
    m_myTime = millis();
    detectado = true;
  }

  return detectado;
}

/**
 * @brief Retrieves the callback function associated with the SpinBox.
 * @return Pointer to the callback function.
 */
functionCB_t SpinBox::getCallbackFunc() { return m_callback; }

/**
 * @brief Redraws the SpinBox on the screen, updating its appearance.
 *
 * Updates the display of the current value in the SpinBox,
 * only redrawing if the SpinBox is on the current screen and needs updating.
 */
void SpinBox::redraw() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_LOADED_VOID
  CHECK_SHOULDREDRAW_VOID

  m_shouldRedraw = false;

  uint16_t btnW = m_config.height - (2 * m_offset);
  uint16_t btnH = m_config.height - (2 * m_offset);

  uint16_t availableW = m_config.width - (2 * m_offset);
  uint16_t availableH = m_config.height - (2 * m_offset);

  WidgetBase::objTFT->setTextColor(m_config.textColor);

  WidgetBase::objTFT->fillRoundRect(
      m_xPos + (2 * m_offset) + btnW, m_yPos + m_offset,
      m_config.width - (4 * m_offset + 2 * btnW), btnH, m_radius, m_config.color);
  WidgetBase::objTFT->setFont(getBestRobotoBold(
      availableW, availableH, String(m_currentValue).c_str()));
  printText(String(m_currentValue).c_str(), m_xPos + m_config.width / 2,
            m_yPos + (m_config.height / 2) - 3, MC_DATUM, m_lastArea, m_config.color);
  updateFont(FontType::UNLOAD);

}

/**
 * @brief Draws the background of the SpinBox, including static elements.
 *
 * Creates the visual container, plus and minus buttons for the SpinBox.
 * This is typically called once during setup rather than on every redraw.
 */
void SpinBox::drawBackground() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_LOADED_VOID

  uint16_t btnW = m_config.height - (2 * m_offset);
  uint16_t btnH = m_config.height - (2 * m_offset);

  // uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;

  WidgetBase::objTFT->fillRoundRect(m_xPos, m_yPos, m_config.width, m_config.height, m_radius,
                                    m_config.color);
  WidgetBase::objTFT->drawRoundRect(m_xPos, m_yPos, m_config.width, m_config.height, m_radius,
                                    CFK_BLACK);

  WidgetBase::objTFT->fillRoundRect(
      m_xPos + m_offset, m_yPos + m_offset, btnW, btnH, m_radius,
      WidgetBase::lightenColor565(m_config.color, 4));
  WidgetBase::objTFT->fillRoundRect(
      m_xPos + m_config.width - m_offset - btnW, m_yPos + m_offset, btnW, btnH, m_radius,
      WidgetBase::lightenColor565(m_config.color, 4));

  WidgetBase::objTFT->drawRoundRect(m_xPos + m_offset, m_yPos + m_offset, btnW,
                                    btnH, m_radius, CFK_BLACK);
  WidgetBase::objTFT->drawRoundRect(m_xPos + m_config.width - m_offset - btnW,
                                    m_yPos + m_offset, btnW, btnH, m_radius,
                                    CFK_BLACK);

  CoordPoint_t btnLeft = {static_cast<uint16_t>(m_xPos + m_offset + (btnW / 2)),
                          static_cast<uint16_t>(m_yPos + m_offset + (btnH / 2))};
  CoordPoint_t btnRight = {
      static_cast<uint16_t>(m_xPos + m_config.width - (m_offset + (btnW / 2))),
      static_cast<uint16_t>(m_yPos + m_offset + (btnH / 2))};

  // WidgetBase::objTFT->fillCircle(btnLeft.x, btnLeft.y, 4, CFK_BLUE);
  const uint8_t sinalW = btnW / 2;
  const uint8_t sinalH = ceil(sinalW / 10.0);
  // Sinal menos
  WidgetBase::objTFT->fillRect(btnLeft.x - (sinalW / 2),
                               btnLeft.y - (sinalH / 2), sinalW, sinalH,
                               m_config.textColor);

  // Sinal mais
  WidgetBase::objTFT->fillRect(btnRight.x - (sinalW / 2),
                               btnRight.y - (sinalH / 2), sinalW, sinalH,
                               m_config.textColor);
  WidgetBase::objTFT->fillRect(btnRight.x - (sinalH / 2),
                               btnRight.y - (sinalW / 2), sinalH, sinalW,
                               m_config.textColor);

  m_shouldRedraw = true;


  // WidgetBase::objTFT->fillCircle(btnRight.x, btnRight.y, 4, CFK_YELLOW);
}


/**
 * @brief Configures the SpinBox with parameters defined in a configuration
 * structure.
 * @param config Structure containing the SpinBox configuration parameters.
 */
void SpinBox::setup(const SpinBoxConfig &config) {
  CHECK_TFT_VOID
  if (m_loaded) {
    ESP_LOGW(TAG, "SpinBox already initialized");
    return;
  }

  // Clean up any existing memory before setting new config
  cleanupMemory();
  
  // Deep copy configuration
  m_config = config;
  
  // Set member variables from config
  if (m_config.minValue > m_config.maxValue) {
    int temp = m_config.minValue;
    m_config.minValue = m_config.maxValue;
    m_config.maxValue = temp;
  }

  m_currentValue = constrain(m_config.startValue, m_config.minValue, m_config.maxValue);
  m_callback = config.callback;
  m_loaded = true;
}

/**
 * @brief Retrieves the current value of the SpinBox.
 * @return Current numeric value.
 */
int SpinBox::getValue() { return m_currentValue; }

/**
 * @brief Sets the current value of the SpinBox.
 * @param _value New numeric value to set.
 *
 * Updates the SpinBox's value, ensures it's within the defined range,
 * and marks the widget for redraw. Triggers the callback if provided.
 */
void SpinBox::setValue(int _value) {
  if (!m_loaded) {
    ESP_LOGE(TAG, "Spinbox widget not loaded");
    return;
  }

  m_currentValue = constrain(_value, m_config.minValue, m_config.maxValue);
  m_shouldRedraw = true;

  if (m_callback != nullptr) {
    WidgetBase::addCallback(m_callback, WidgetBase::CallbackOrigin::SELF);
  }
}

void SpinBox::show() {
  m_visible = true;
  m_shouldRedraw = true;
}

void SpinBox::hide() {
  m_visible = false;
  m_shouldRedraw = true;
}

void SpinBox::forceUpdate() { m_shouldRedraw = true; }