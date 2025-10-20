#include "wcheckbox.h"

/**
 * @brief Constructor for the CheckBox class.
 * @param _x X-coordinate for the CheckBox position.
 * @param _y Y-coordinate for the CheckBox position.
 * @param _screen Screen identifier where the CheckBox will be displayed.
 */
CheckBox::CheckBox(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen), m_shouldRedraw(true) {}

/**
 * @brief Destructor for the CheckBox class.
 *
 * Clears the callback function pointer.
 */
CheckBox::~CheckBox() { cb = nullptr; }

/**
 * @brief Forces an immediate update of the CheckBox.
 *
 * Sets the flag to redraw the checkbox on the next redraw cycle.
 */
void CheckBox::forceUpdate() { m_shouldRedraw = true; }

/**
 * @brief Detects if the CheckBox has been touched.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is within the CheckBox area, otherwise false.
 *
 * Changes the checkbox state if touched and sets the redraw flag.
 */
bool CheckBox::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  if (!visible) {
    return false;
  }
  if (WidgetBase::usingKeyboard || WidgetBase::currentScreen != screen ||
      !loaded) {
    return false;
  }

  if (millis() - m_myTime < TIMEOUT_CLICK) {
    return false;
  }

  if (!m_enabled) {
    log_d("CheckBox is disabled");
    return false;
  }

  m_myTime = millis();
  bool detected = false;
  uint16_t xMax = xPos + m_width;
  uint16_t yMax = yPos + m_height;

  if ((*_xTouch > xPos) && (*_xTouch < xMax) && (*_yTouch > yPos) &&
      (*_yTouch < yMax)) {
    changeState();
    m_shouldRedraw = true;
    detected = true;
  }
  return detected;
}

/**
 * @brief Retrieves the callback function associated with the CheckBox.
 * @return Pointer to the callback function.
 */
functionCB_t CheckBox::getCallbackFunc() { return cb; }

/**
 * @brief Retrieves the current enabled state of the checkbox.
 * @return True if the checkbox is enabled, otherwise false.
 */
bool CheckBox::getEnabled() { return m_enabled; }

/**
 * @brief Sets the enabled state of the checkbox.
 * @param newState True to enable the checkbox, false to disable it.
 */
void CheckBox::setEnabled(bool newState) { m_enabled = newState; }

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
  if (!visible) {
    return;
  }
#if defined(DISP_DEFAULT)
  if (WidgetBase::currentScreen != screen || !loaded || !m_shouldRedraw ||
      !visible) {
    return;
  }
  m_shouldRedraw = false;

  log_d("Redrawing checkbox");

  uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;

  WidgetBase::objTFT->fillRoundRect(xPos, yPos, m_width, m_height, 5,
                                    m_checkedColor);
  WidgetBase::objTFT->fillRoundRect(xPos + 2, yPos + 2, m_width - 4,
                                    m_height - 4, 5,
                                    m_status ? m_checkedColor : lightBg);

  // uint16_t bgColor = m_status ? m_checkedColor : lightBg;
  if (m_status) {
    CoordPoint_t p1 = {static_cast<uint16_t>(xPos + 4),
                       static_cast<uint16_t>(yPos + 9)};
    CoordPoint_t p2 = {static_cast<uint16_t>(xPos + 8),
                       static_cast<uint16_t>(yPos + 13)};
    CoordPoint_t p3 = {static_cast<uint16_t>(xPos + 16),
                       static_cast<uint16_t>(yPos + 6)};

    WidgetBase::objTFT->drawLine(p1.x + 1, p1.y, p2.x, p2.y - 1, CFK_WHITE);
    WidgetBase::objTFT->drawLine(p3.x - 2, p3.y, p2.x, p2.y - 1, CFK_WHITE);

    WidgetBase::objTFT->drawLine(p1.x, p1.y, p2.x, p2.y, CFK_WHITE);
    WidgetBase::objTFT->drawLine(p3.x, p3.y, p2.x, p2.y, CFK_WHITE);

    WidgetBase::objTFT->drawLine(p1.x, p1.y + 1, p2.x, p2.y + 1, CFK_WHITE);
    WidgetBase::objTFT->drawLine(p3.x, p3.y - 1, p2.x, p2.y + 1, CFK_WHITE);
  }
#endif
}

/**
 * @brief Configures the CheckBox widget with specific dimensions, color, and
 * callback function.
 * @param _size Size of the CheckBox.
 * @param _checkedColor Color displayed when the CheckBox is checked.
 * @param _cb Callback function to execute when the CheckBox state changes.
 *
 * Initializes the checkbox properties and marks it as loaded when complete.
 */
void CheckBox::setup(uint16_t _size, uint16_t _checkedColor, functionCB_t _cb) {
  if (!WidgetBase::objTFT) {
    log_e("TFT not defined on WidgetBase");
    return;
  }
  if (loaded) {
    log_d("Checkbox widget already configured");
    return;
  }

  m_width = _size;
  m_height = _size;
  m_checkedColor = _checkedColor;
  cb = _cb;

  loaded = true;
}

/**
 * @brief Configures the CheckBox with parameters defined in a configuration
 * structure.
 * @param config Structure containing the checkbox configuration parameters.
 */
void CheckBox::setup(const CheckBoxConfig &config) {
  setup(config.size, config.checkedColor, config.callback);
}

/**
 * @brief Retrieves the current status of the CheckBox.
 * @return True if the CheckBox is checked, otherwise false.
 */
bool CheckBox::getStatus() { return m_status; }

/**
 * @brief Sets the current state of the checkbox.
 * @param status True for checked, false for unchecked.
 *
 * Updates the checkbox state, marks it for redraw, and triggers the callback if
 * provided.
 */
void CheckBox::setStatus(bool status) {
  if (!loaded) {
    log_e("Checkbox widget not loaded");
    return;
  }
  m_status = status;
  m_shouldRedraw = true;
  if (cb != nullptr) {
    WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::SELF);
  }
}

void CheckBox::show() {
  visible = true;
  m_shouldRedraw = true;
}

void CheckBox::hide() {
  visible = false;
  m_shouldRedraw = true;
}