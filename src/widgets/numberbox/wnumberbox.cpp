#include "wnumberbox.h"

const char* NumberBox::TAG = "NumberBox";

/**
 * @brief Converts a float value to a string representation.
 * @param f Float value to convert.
 * @return Pointer to the string representation of the float.
 *
 * Note: This function uses a static buffer, so its return value is only valid
 * until the next call.
 */
String NumberBox::convertoToString(float f) {
  return String(f);
}

/**
 * @brief Constructs a NumberBox widget at a specified position on a specified
 * screen.
 * @param _x X-coordinate for the NumberBox position.
 * @param _y Y-coordinate for the NumberBox position.
 * @param _screen Screen identifier where the NumberBox will be displayed.
 */
NumberBox::NumberBox(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen), m_padding(3), m_shouldRedraw(true) {
      m_config = {.width = 0, .height = 0, .letterColor = 0, .backgroundColor = 0, .startValue = 0, .font = nullptr, .funcPtr = nullptr, .callback = nullptr};
    }

/**
 * @brief Default constructor for NumberBox.
 *
 * Creates a NumberBox at position (0,0) on screen 0.
 */
NumberBox::NumberBox() : WidgetBase(0, 0, 0), m_padding(3), m_shouldRedraw(true), m_config{} {}

/**
 * @brief Destructor for the NumberBox class.
 */
NumberBox::~NumberBox() {
    cleanupMemory();
}

void NumberBox::cleanupMemory() {
    // NumberBox doesn't use dynamic memory allocation
    // CharString handles its own memory management
    ESP_LOGD(TAG, "NumberBox memory cleanup completed");
}

/**
 * @brief Detects if the NumberBox has been touched.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is within the NumberBox area, otherwise false.
 *
 * When touched, activates the virtual keyboard mode for value input.
 */
bool NumberBox::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  CHECK_VISIBLE_BOOL
  CHECK_CURRENTSCREEN_BOOL
  CHECK_LOADED_BOOL
  CHECK_DEBOUNCE_CLICK_BOOL
  CHECK_USINGKEYBOARD_BOOL

  
  

  bool inBounds = POINT_IN_RECT(*_xTouch, *_yTouch, m_xPos, m_yPos, m_config.width, m_config.height);

  if (inBounds) {
    m_myTime = millis();
    WidgetBase::usingKeyboard = true;
    ESP_LOGD(TAG, "Open keyboard");
    return true;
  }

  return false;
}

/**
 * @brief Retrieves the callback function associated with the NumberBox.
 * @return Pointer to the callback function.
 */
functionCB_t NumberBox::getCallbackFunc() { return m_callback; }

/**
 * @brief Redraws the NumberBox on the screen, updating its appearance.
 *
 * Displays the current numeric value with appropriate formatting and styling.
 * Only redraws if the NumberBox is on the current screen and needs updating.
 */
void NumberBox::redraw() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_LOADED_VOID
  CHECK_SHOULDREDRAW_VOID

  m_shouldRedraw = false;

  if (m_font) {
    WidgetBase::objTFT->setFont(m_font);
  } else {
    updateFont(FontType::NORMAL);
  }

  // uint16_t darkBg = WidgetBase::lightMode ? CFK_GREY3 : CFK_GREY11;
  // uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
  // uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

  ESP_LOGD(TAG, "Redraw numberbox with value %s", m_value.getString());
  WidgetBase::objTFT->fillRect(m_xPos, m_yPos, m_config.width, m_config.height,
                               m_config.backgroundColor);
  WidgetBase::objTFT->drawRect(m_xPos, m_yPos, m_config.width, m_config.height, m_config.letterColor);
  WidgetBase::objTFT->setTextColor(m_config.letterColor);

  // TextBound_t area;
  // WidgetBase::objTFT->getTextBounds("M", m_xPos, m_yPos, &area.x, &area.y,
  // &area.width, &area.height); uint16_t qtdLetrasMax = m_width / area.width;
  // const char* conteudo = m_value.getFirstChars(qtdLetrasMax);
  // printText(conteudo, m_xPos + m_padding, m_yPos + m_height/2, ML_DATUM);
  const char *conteudo = getFirstLettersForSpace(m_value.getString(),
                                                 m_config.width * 0.9, m_config.height * 0.9);

  // log_d("Draw %d letters from %s in space %d", qtdLetrasMax, conteudo,
  // m_width);
  printText(conteudo, m_xPos + m_padding, m_yPos + m_config.height / 2, ML_DATUM);

  updateFont(FontType::UNLOAD);
}

/**
 * @brief Forces the NumberBox to redraw.
 *
 * Sets the flag to redraw the NumberBox on the next redraw cycle.
 */
void NumberBox::forceUpdate() { m_shouldRedraw = true; }

/**
 * @brief Configures the NumberBox widget with specific dimensions, colors, and
 * initial value.
 * @param _width Width of the NumberBox.
 * @param _height Height of the NumberBox.
 * @param _letterColor Color for the text displaying the value.
 * @param _backgroundColor Background color of the NumberBox.
 * @param _startValue Initial numeric value to display.
 * @param _font Font to use for the text.
 * @param _funcPtr Function pointer to the parent screen's load function.
 * @param _cb Callback function to execute on interaction.
 *
 * Initializes the NumberBox properties and marks it as loaded when complete.
 */
#if defined(USING_GRAPHIC_LIB)
void NumberBox::setup(uint16_t _width, uint16_t _height, uint16_t _letterColor,
                      uint16_t _backgroundColor, float _startValue,
                      const GFXfont *_font, functionLoadScreen_t _funcPtr,
                      functionCB_t _cb) {
  CHECK_TFT_VOID
  if (m_loaded) {
    ESP_LOGE(TAG, "Numberbox already intialized");
    return;
  }

  parentScreen = _funcPtr;
  m_callback = _cb;

  m_config.width = _width;
  m_config.height = _height;
  m_config.font = _font;

#if defined(DISP_DEFAULT)
  if (_font) {
    WidgetBase::objTFT->setFont(_font);
    TextBound_t area;
    WidgetBase::objTFT->getTextBounds("Mp", m_xPos, m_yPos, &area.x, &area.y,
                                      &area.width, &area.height);
    m_config.height = area.height + (m_padding * 2);
  }
#endif

  m_config.letterColor = _letterColor;
  m_config.backgroundColor = _backgroundColor;
  m_value.setString(_startValue);

  m_loaded = true;
}
#endif
/**
 * @brief Configures the NumberBox with parameters defined in a configuration
 * structure.
 * @param config Structure containing the NumberBox configuration parameters.
 */
void NumberBox::setup(const NumberBoxConfig &config) {
  CHECK_TFT_VOID
  if (m_loaded) {
    ESP_LOGW(TAG, "NumberBox already initialized");
    return;
  }

  // Clean up any existing memory before setting new config
  cleanupMemory();
  
  // Deep copy configuration
  m_config = config;
  
  // Copy function pointers (no deep copy needed for function pointers)
  parentScreen = config.funcPtr;
  m_callback = config.callback;
  
  // Set member variables from config
  m_config.width = config.width;
  m_config.height = config.height;
  m_config.letterColor = config.letterColor;
  m_config.backgroundColor = config.backgroundColor;
  
  #if defined(USING_GRAPHIC_LIB)
  m_config.font = config.font;
  m_font = config.font;
  #endif
  
  // Set initial value
  m_value.setString(config.startValue);
  
  m_loaded = true;
  ESP_LOGD(TAG, "NumberBox configured: %dx%d, value: %f", 
           m_config.width, m_config.height, config.startValue);
}

/**
 * @brief Sets the numeric value displayed by the NumberBox.
 * @param str Numeric value to set.
 *
 * Updates the displayed value and marks the NumberBox for redraw.
 */
void NumberBox::setValue(float str) {
  m_value.setString(str);
  ESP_LOGD(TAG, "Set value for numberbox: %s", m_value.getString());
  forceUpdate();
}

/**
 * @brief Retrieves the current numeric value of the NumberBox.
 * @return Current numeric value as a float.
 */
float NumberBox::getValue() { return m_value.toFloat(); }

/**
 * @brief Retrieves the current numeric value as a string.
 * @return Pointer to the string representation of the current value.
 */
const char *NumberBox::getValueChar() { return m_value.getString(); }

void NumberBox::show() {
  m_visible = true;
  m_shouldRedraw = true;
}

void NumberBox::hide() {
  m_visible = false;
  m_shouldRedraw = true;
}