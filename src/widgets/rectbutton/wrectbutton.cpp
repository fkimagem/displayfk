#include "wrectbutton.h"

const char* RectButton::TAG = "RectButton";

/**
 * @brief Constructor for the RectButton class.
 * @param _x X-coordinate for the RectButton position.
 * @param _y Y-coordinate for the RectButton position.
 * @param _screen Screen identifier where the RectButton will be displayed.
 */
RectButton::RectButton(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen), m_status(false), m_enabled(true) {
      m_config = {.width = 0, .height = 0, .pressedColor = 0, .callback = nullptr};
      ESP_LOGD(TAG, "RectButton created at (%d, %d) on screen %d", _x, _y, _screen);
    }

/**
 * @brief Destructor for the RectButton class.
 */
RectButton::~RectButton() {
    cleanupMemory();
}

void RectButton::cleanupMemory() {
    // RectButton doesn't use dynamic memory allocation
    ESP_LOGD(TAG, "RectButton memory cleanup completed");
}

/**
 * @brief Detects if the RectButton has been touched.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is within the RectButton area, otherwise false.
 *
 * Changes the button state if touched and sets the redraw flag.
 */
bool RectButton::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  CHECK_VISIBLE_BOOL
  CHECK_USINGKEYBOARD_BOOL
  CHECK_CURRENTSCREEN_BOOL
  CHECK_LOADED_BOOL
  CHECK_DEBOUNCE_CLICK_BOOL

  if (!m_enabled) {
    ESP_LOGD(TAG, "RectButton is disabled");
    return false;
  }

  

  bool inBounds = POINT_IN_RECT(*_xTouch, *_yTouch, m_xPos, m_yPos, m_config.width, m_config.height);
  if(inBounds) {
    m_myTime = millis();
    changeState();
    m_shouldRedraw = true;
    return true;
  }
  return false;
}

/**
 * @brief Retrieves the callback function associated with the RectButton.
 * @return Pointer to the callback function.
 */
functionCB_t RectButton::getCallbackFunc() { return m_callback; }

/**
 * @brief Retrieves the current enabled state of the button.
 * @return True if the button is enabled, otherwise false.
 */
bool RectButton::getEnabled() { return m_enabled; }

/**
 * @brief Sets the enabled state of the button.
 * @param newState True to enable the button, false to disable it.
 */
void RectButton::setEnabled(bool newState) { m_enabled = newState; }

/**
 * @brief Changes the current state of the button (pressed or released).
 *
 * Inverts the current state of the button.
 */
void RectButton::changeState() { m_status = !m_status; }

/**
 * @brief Forces the button to redraw.
 *
 * Sets the flag to redraw the button on the next redraw cycle.
 */
void RectButton::forceUpdate() { m_shouldRedraw = true; }

/**
 * @brief Redraws the button on the screen, updating its appearance.
 *
 * Displays the button with different appearance based on its current state.
 * Only redraws if the button is on the current screen and needs updating.
 */
void RectButton::redraw() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_LOADED_VOID
  CHECK_SHOULDREDRAW_VOID

  m_shouldRedraw = false;

  // uint16_t darkBg = WidgetBase::lightMode ? CFK_GREY3 : CFK_GREY11;
  uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
  uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

  WidgetBase::objTFT->fillRoundRect(m_xPos + 1, m_yPos + 1, m_config.width - 2, m_config.height - 2,
                                    5, lightBg); // Botao
  WidgetBase::objTFT->drawRoundRect(m_xPos, m_yPos, m_config.width, m_config.height, 5,
                                    baseBorder); // borda Botao
  uint16_t bgColor = m_status ? m_config.pressedColor : lightBg;
  WidgetBase::objTFT->fillRoundRect(m_xPos + 6, m_yPos + 6, m_config.width - 12, m_config.height - 12,
                                    5, bgColor); // top botao
  WidgetBase::objTFT->drawRoundRect(m_xPos + 5, m_yPos + 5, m_config.width - 10, m_config.height - 10,
                                    5, baseBorder); // borda top botao
}

/**
 * @brief Initializes the button settings.
 *
 * Ensures button dimensions are within valid ranges based on the display size.
 * Marks the widget as loaded when complete.
 */
void RectButton::start() {
#if defined(DISP_DEFAULT)
  m_config.width = constrain(m_config.width, 5, WidgetBase::objTFT->width());
  m_config.height = constrain(m_config.height, 5, WidgetBase::objTFT->height());
#endif

  m_loaded = true;
}

/**
 * @brief Configures the RectButton with parameters defined in a configuration
 * structure.
 * @param config Structure containing the button configuration parameters.
 */
void RectButton::setup(const RectButtonConfig &config) {
  CHECK_TFT_VOID
  if (m_loaded) {
    ESP_LOGW(TAG, "RectButton already initialized");
    return;
  }

  // Clean up any existing memory before setting new config
  cleanupMemory();
  
  // Deep copy configuration
  m_config = config;
  
  // Set member variables from config
  m_callback = config.callback;
  
  m_loaded = true;
  
  ESP_LOGD(TAG, "RectButton configured: %dx%d, pressedColor=0x%04X", 
           m_config.width, m_config.height, m_config.pressedColor);
}

/**
 * @brief Retrieves the current status of the button.
 * @return True if the button is pressed, otherwise false.
 */
bool RectButton::getStatus() { return m_status; }

/**
 * @brief Sets the status of the button.
 * @param _status New status of the button (true for pressed, false for
 * released).
 *
 * Updates the button state, marks it for redraw, and triggers the callback if
 * provided.
 */
void RectButton::setStatus(bool _status) {
  if (!m_loaded) {
    ESP_LOGE(TAG, "RectButton widget not loaded");
    return;
  }

  m_status = _status;
  m_shouldRedraw = true;
  if (m_callback != nullptr) {
    WidgetBase::addCallback(m_callback, WidgetBase::CallbackOrigin::SELF);
  }
}

void RectButton::show() {
  m_visible = true;
  m_shouldRedraw = true;
}

void RectButton::hide() {
  m_visible = false;
  m_shouldRedraw = true;
}
