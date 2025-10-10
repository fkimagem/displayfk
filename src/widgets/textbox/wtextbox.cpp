#include "wtextbox.h"

/**
 * @brief Constructor for the TextBox class.
 * @param _x X-coordinate for the TextBox position.
 * @param _y Y-coordinate for the TextBox position.
 * @param _screen Screen identifier where the TextBox will be displayed.
 */
TextBox::TextBox(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen), m_padding(3), m_shouldRedraw(true) {}

/**
 * @brief Default constructor for TextBox.
 *
 * Creates a TextBox at position (0,0) on screen 0.
 */
TextBox::TextBox() : WidgetBase(0, 0, 0) {}

/**
 * @brief Destructor for the TextBox class.
 *
 * Clears pointers to font, parent screen function, and callback function.
 * The CharString object manages its own memory cleanup.
 */
TextBox::~TextBox() {
// A classe CharString cuida de sua própria memória
// A fonte é apenas uma referência, não precisamos liberar
#if defined(USING_GRAPHIC_LIB)
  m_font = nullptr;
#endif
  parentScreen = nullptr;
  cb = nullptr;
}

/**
 * @brief Detects if the TextBox has been touched.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is within the TextBox area, otherwise false.
 *
 * When touched, activates the virtual keyboard mode for text input.
 */
bool TextBox::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  if (!visible) {
    return false;
  }
#if defined(HAS_TOUCH)
  if (WidgetBase::currentScreen != screen || !loaded) {
    return false;
  }

  if (millis() - m_myTime < TIMEOUT_CLICK) {
    return false;
  }
  m_myTime = millis();

  // bool detectado = false;
  uint16_t xMax = xPos + m_width;
  uint16_t yMax = yPos + m_height;

  if ((*_xTouch > xPos) && (*_xTouch < xMax) && (*_yTouch > yPos) &&
      (*_yTouch < yMax) && WidgetBase::usingKeyboard == false) {
    WidgetBase::usingKeyboard = true;
    log_d("Open keyboard");

    return true;
  }

  return false;
#else
  return false;
#endif
}

/**
 * @brief Retrieves the callback function associated with the TextBox.
 * @return Pointer to the callback function.
 */
functionCB_t TextBox::getCallbackFunc() { return cb; }

/**
 * @brief Redraws the TextBox on the screen, updating its appearance.
 *
 * Displays the current text value with appropriate font and styling.
 * Only redraws if the TextBox is on the current screen and needs updating.
 */
void TextBox::redraw() {
  if (!visible) {
    return;
  }
#if defined(DISP_DEFAULT)
  if (WidgetBase::currentScreen != screen || !loaded || !m_shouldRedraw) {
    return;
  }

  m_shouldRedraw = false;

  if (m_font) {
    WidgetBase::objTFT->setFont(m_font);
  } else {
    updateFont(FontType::NORMAL);
  }

  log_d("Redraw textbox with value %s", m_value.getString());

  // TextBound_t area;
  // WidgetBase::objTFT->getTextBounds("M", xPos, yPos, &area.x, &area.y,
  // &area.width, &area.height);

  WidgetBase::objTFT->fillRect(xPos, yPos, m_width, m_height,
                               m_backgroundColor);
  WidgetBase::objTFT->drawRect(xPos, yPos, m_width, m_height, m_letterColor);
  WidgetBase::objTFT->setTextColor(m_letterColor);

  // uint16_t qtdLetrasMax = m_width / area.width;
  // const char *conteudo = m_value.getFirstChars(qtdLetrasMax);
  const char *conteudo = getFirstLettersForSpace(m_value.getString(),
                                                 m_width * 0.9, m_height * 0.9);

  // log_d("Draw %d letters from %s in space %d", qtdLetrasMax, conteudo,
  // m_width);
  printText(conteudo, xPos + m_padding, yPos + m_height / 2, ML_DATUM);

  updateFont(FontType::UNLOAD);
#endif
}

/**
 * @brief Configures the TextBox widget with specific parameters from a
 * configuration struct.
 * @param config The configuration struct containing all setup parameters.
 */
void TextBox::setup(const TextBoxConfig &config) {
#if defined(USING_GRAPHIC_LIB)
  setup(config.width, config.height, config.letterColor, config.backgroundColor,
        config.startValue, config.font, config.funcPtr, config.cb);
#endif
}

/**
 * @brief Configures the TextBox widget with specific dimensions, colors, and
 * initial value.
 * @param _width Width of the TextBox.
 * @param _height Height of the TextBox.
 * @param _letterColor Color for the text displaying the value.
 * @param _backgroundColor Background color of the TextBox.
 * @param _startValue Initial text value to display.
 * @param _font Font to use for the text.
 * @param _funcPtr Function pointer to the parent screen's load function.
 * @param _cb Callback function to execute on interaction.
 *
 * Initializes the TextBox properties and marks it as loaded when complete.
 * Adjusts height based on font metrics if a font is specified.
 */
#if defined(USING_GRAPHIC_LIB)
void TextBox::setup(uint16_t _width, uint16_t _height, uint16_t _letterColor,
                    uint16_t _backgroundColor, const char *_startValue,
                    const GFXfont *_font, functionLoadScreen_t _funcPtr,
                    functionCB_t _cb) {
  if (!WidgetBase::objTFT) {
    log_e("TFT not defined on WidgetBase");
    return;
  }
  parentScreen = _funcPtr;
  m_width = _width;
  m_height = _height;

#if defined(DISP_DEFAULT)
  if (_font) {
    WidgetBase::objTFT->setFont(_font);
    TextBound_t area;
    WidgetBase::objTFT->getTextBounds("Mp", xPos, yPos, &area.x, &area.y,
                                      &area.width, &area.height);
    m_height = area.height + (m_padding * 2);
  }
#endif

  m_letterColor = _letterColor;
  m_backgroundColor = _backgroundColor;
  m_value = _startValue;
  m_font = _font;
  cb = _cb;

  loaded = true;
}
#endif

/**
 * @brief Sets the text value displayed by the TextBox.
 * @param str Text value to set.
 *
 * Updates the displayed text and marks the TextBox for redraw.
 */
void TextBox::setValue(const char *str) {
  m_value.setString(str, true);
  log_d("Set value for textbox: %s", str);
  forceUpdate();
}

/**
 * @brief Forces the TextBox to redraw.
 *
 * Sets the flag to redraw the TextBox on the next redraw cycle.
 */
void TextBox::forceUpdate() { m_shouldRedraw = true; }

/**
 * @brief Retrieves the current text value of the TextBox.
 * @return Pointer to the string representation of the current value.
 */
const char *TextBox::getValue() { return m_value.getString(); }

void TextBox::show() {
  visible = true;
  m_shouldRedraw = true;
}

void TextBox::hide() {
  visible = false;
  m_shouldRedraw = true;
}