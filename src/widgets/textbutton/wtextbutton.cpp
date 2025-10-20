#include "wtextbutton.h"

/**
 * @brief Constructor for the TextButton class.
 * @param _x X-coordinate for the TextButton position.
 * @param _y Y-coordinate for the TextButton position.
 * @param _screen Screen identifier where the TextButton will be displayed.
 */
TextButton::TextButton(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen) {}

/**
 * @brief Destructor for the TextButton class.
 */
TextButton::~TextButton() {}

/**
 * @brief Detects if the TextButton has been touched.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is within the TextButton area, otherwise false.
 */
bool TextButton::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
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
    log_d("TextButton is disabled");
    return false;
  }

  m_myTime = millis();
  bool detectado = false;
  uint16_t xMax = xPos + m_width;
  uint16_t yMax = yPos + m_height;

  if ((*_xTouch > xPos) && (*_xTouch < xMax) && (*_yTouch > yPos) &&
      (*_yTouch < yMax)) {
    redraw();
    detectado = true;
  }
  return detectado;
}

/**
 * @brief Retrieves the callback function associated with the TextButton.
 * @return Pointer to the callback function.
 */
functionCB_t TextButton::getCallbackFunc() { return cb; }

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
  if (!visible) {
    return;
  }
#if defined(DISP_DEFAULT)
  if (WidgetBase::currentScreen != screen || WidgetBase::usingKeyboard ||
      !loaded) {
    return;
  }

  // uint16_t darkBg = WidgetBase::lightMode ? CFK_GREY3 : CFK_GREY11;
  // uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
  uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

  WidgetBase::objTFT->fillRoundRect(xPos + 1, yPos + 1, m_width - 2,
                                    m_height - 2, m_radius,
                                    m_pressedColor); // Botao
  WidgetBase::objTFT->drawRoundRect(xPos, yPos, m_width, m_height, m_radius,
                                    baseBorder); // borda Botao

  WidgetBase::objTFT->setTextColor(m_textColor);

  // WidgetBase::objTFT->setFont(&RobotoBold10pt7b);
  WidgetBase::objTFT->setFont(getBestRobotoBold(
      m_width - (2 * m_offsetMargin), m_height - (2 * m_offsetMargin), m_text));
  printText(m_text, xPos + m_width / 2, yPos + (m_height / 2), MC_DATUM);
  // showOrigin(CFK_RED);
  // WidgetBase::objTFT->drawCircle(xPos + width/2, yPos + (height / 2), 4,
  // CFK_WHITE);

  updateFont(FontType::UNLOAD);
#endif
}

/**
 * @brief Initializes the TextButton settings.
 *
 * Ensures button dimensions are within valid ranges based on the display size.
 * Marks the widget as loaded when complete.
 */
void TextButton::start() {
#if defined(DISP_DEFAULT)
  m_width = constrain(m_width, 5, WidgetBase::objTFT->width());
  m_height = constrain(m_height, 5, WidgetBase::objTFT->height());
#endif

  loaded = true;
}

/**
 * @brief Simulates a click on the TextButton.
 *
 * Triggers the assigned callback function if it exists.
 * This is useful for programmatically activating the button.
 */
void TextButton::onClick() {
  if (!loaded) {
    log_e("TextButton widget not loaded");
    return;
  }
  if (cb != nullptr) {
    WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::SELF);
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
  if (!WidgetBase::objTFT) {
    log_e("TFT not defined on WidgetBase");
    return;
  }
  if (loaded) {
    log_d("RectButton widget already configured");
    return;
  }
  m_width = _width;
  m_height = _height;
  m_pressedColor = _pressedColor;
  m_textColor = _textColor;
  m_text = _text;
  m_radius = _radius;
  cb = _cb;

  loaded = true;
}

/**
 * @brief Configures the TextButton with parameters defined in a configuration
 * structure.
 * @param config Structure containing the button configuration parameters.
 */
void TextButton::setup(const TextButtonConfig &config) {
  setup(config.width, config.height, config.radius, config.backgroundColor,
        config.textColor, config.text, config.callback);
}

void TextButton::show() {
  visible = true;
  m_shouldRedraw = true;
}

void TextButton::hide() {
  visible = false;
  m_shouldRedraw = true;
}