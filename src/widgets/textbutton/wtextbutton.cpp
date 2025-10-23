#include "wtextbutton.h"

const char* TextButton::TAG = "TextButton";

/**
 * @brief Constructor for the TextButton class.
 * @param _x X-coordinate for the TextButton position.
 * @param _y Y-coordinate for the TextButton position.
 * @param _screen Screen identifier where the TextButton will be displayed.
 */
TextButton::TextButton(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen), m_config{} {

      m_config = {.width = 0, .height = 0, .radius = 0, .backgroundColor = 0, .textColor = 0, .text = "", .callback = nullptr};
      ESP_LOGD(TAG, "TextButton created at (%d, %d) on screen %d", _x, _y, _screen);
    }

/**
 * @brief Destructor for the TextButton class.
 */
TextButton::~TextButton() {
    cleanupMemory();
}

void TextButton::cleanupMemory() {
    // TextButton doesn't use dynamic memory allocation
    // m_text is a const pointer to external data
    ESP_LOGD(TAG, "TextButton memory cleanup completed");
}

/**
 * @brief Detects if the TextButton has been touched.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is within the TextButton area, otherwise false.
 */
bool TextButton::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  CHECK_VISIBLE_BOOL
  CHECK_USINGKEYBOARD_BOOL
  CHECK_CURRENTSCREEN_BOOL
  CHECK_LOADED_BOOL
  CHECK_DEBOUNCE_CLICK_BOOL

  if (!m_enabled) {
    ESP_LOGD(TAG, "TextButton is disabled");
    return false;
  }

  

  bool inBounds = POINT_IN_RECT(*_xTouch, *_yTouch, m_xPos, m_yPos, m_config.width, m_config.height);
  if(inBounds) {
    m_myTime = millis();
    onClick();
    m_shouldRedraw = true;
    return true;
  }

  return false;
}

/**
 * @brief Retrieves the callback function associated with the TextButton.
 * @return Pointer to the callback function.
 */
functionCB_t TextButton::getCallbackFunc() { return m_callback; }

/**
 * @brief Retrieves the current enabled state of the button.
 * @return True if the button is enabled, otherwise false.
 */
bool TextButton::getEnabled() { return m_enabled; }

/**
 * @brief Sets the enabled state of the button.
 * @param newState True to enable the button, false to disable it.
 */
void TextButton::setEnabled(bool newState) { m_enabled = newState; }

/**
 * @brief Redraws the TextButton on the screen, updating its appearance.
 *
 * Displays the button with specified colors, dimensions, and text.
 * The appearance adjusts based on the current display mode (light or dark).
 */
void TextButton::redraw() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_LOADED_VOID

  // uint16_t darkBg = WidgetBase::lightMode ? CFK_GREY3 : CFK_GREY11;
  // uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
  uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

  WidgetBase::objTFT->fillRoundRect(m_xPos + 1, m_yPos + 1, m_config.width - 2,
                                    m_config.height - 2, m_config.radius,
                                    m_config.backgroundColor); // Botao
  WidgetBase::objTFT->drawRoundRect(m_xPos, m_yPos, m_config.width, m_config.height, m_config.radius,
                                    baseBorder); // borda Botao

  WidgetBase::objTFT->setTextColor(m_config.textColor);

  // WidgetBase::objTFT->setFont(&RobotoBold10pt7b);
  WidgetBase::objTFT->setFont(getBestRobotoBold(
      m_config.width - (2 * m_offsetMargin), m_config.height - (2 * m_offsetMargin), m_config.text));
  printText(m_config.text, m_xPos + m_config.width / 2, m_yPos + (m_config.height / 2), MC_DATUM);
  // showOrigin(CFK_RED);
  // WidgetBase::objTFT->drawCircle(m_xPos + width/2, m_yPos + (height / 2), 4,
  // CFK_WHITE);

  updateFont(FontType::UNLOAD);
}

/**
 * @brief Initializes the TextButton settings.
 *
 * Ensures button dimensions are within valid ranges based on the display size.
 * Marks the widget as loaded when complete.
 */
void TextButton::start() {
#if defined(DISP_DEFAULT)
  m_config.width = constrain(m_config.width, 5, WidgetBase::objTFT->width());
  m_config.height = constrain(m_config.height, 5, WidgetBase::objTFT->height());
#endif

  m_loaded = true;
}

/**
 * @brief Simulates a click on the TextButton.
 *
 * Triggers the assigned callback function if it exists.
 * This is useful for programmatically activating the button.
 */
void TextButton::onClick() {
  if (!m_loaded) {
    ESP_LOGE(TAG, "TextButton widget not loaded");
    return;
  }
  if (m_callback != nullptr) {
    WidgetBase::addCallback(m_callback, WidgetBase::CallbackOrigin::SELF);
  }
}

/**
 * @brief Configures the TextButton with specific dimensions, colors, text, and
 * callback function.
 * @param _width Width of the button.
 * @param _height Height of the button.
 * @param _radius Radius for the button's rounded corners.
 * @param _pressedColor Color displayed when the button is pressed.
 * @param _textColor Color of the text displayed on the button.
 * @param _text Text to display on the button.
 * @param _cb Callback function to execute when the button is pressed.
 */
void TextButton::setup(uint16_t _width, uint16_t _height, uint16_t _radius,
                       uint16_t _pressedColor, uint16_t _textColor,
                       const char *_text, functionCB_t _cb) {
  CHECK_TFT_VOID
  if (m_loaded) {
    ESP_LOGD(TAG, "TextButton widget already configured");
    return;
  }
  m_text = _text;
  m_callback = _cb;

  m_loaded = true;
}

/**
 * @brief Configures the TextButton with parameters defined in a configuration
 * structure.
 * @param config Structure containing the button configuration parameters.
 */
void TextButton::setup(const TextButtonConfig &config) {
  CHECK_TFT_VOID
  if (m_loaded) {
    ESP_LOGW(TAG, "TextButton already initialized");
    return;
  }

  // Clean up any existing memory before setting new config
  cleanupMemory();
  
  // Deep copy configuration
  m_config = config;
  m_text = config.text; // Note: This is a const pointer, no deep copy needed
  m_callback = config.callback;
  
  m_loaded = true;
  
  ESP_LOGD(TAG, "TextButton configured: %dx%d, radius=%d, text='%s'", 
           m_config.width, m_config.height, m_config.radius, m_config.text);
}

void TextButton::show() {
  m_visible = true;
  m_shouldRedraw = true;
}

void TextButton::hide() {
  m_visible = false;
  m_shouldRedraw = true;
}

void TextButton::forceUpdate() { m_shouldRedraw = true; }