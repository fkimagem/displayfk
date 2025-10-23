#include "numpad.h"

const char* Numpad::TAG = "Numpad";

uint16_t Numpad::m_backgroundColor = CFK_GREY3;
uint16_t Numpad::m_letterColor = CFK_BLACK;
uint16_t Numpad::m_keyColor = CFK_GREY13;

/**
 * @brief 2D array defining the characters displayed on the Numpad keys
 * Layout:
 * 7 8 9 +/-
 * 4 5 6 Del
 * 1 2 3 .
 * -- 0 ++ OK
 */
const Key_t Numpad::m_pad[NCOLS][NROWS] = {
    // Linha 0 - Números e +/-
    {{"7", PressedKeyType::NUMBER},
     {"8", PressedKeyType::NUMBER},
     {"9", PressedKeyType::NUMBER},
     {"+/-", PressedKeyType::INVERT_VALUE}},

    // Linha 1 - Números e Del
    {{"4", PressedKeyType::NUMBER},
     {"5", PressedKeyType::NUMBER},
     {"6", PressedKeyType::NUMBER},
     {"Del", PressedKeyType::DEL}},

    // Linha 2 - Números e ponto decimal
    {{"1", PressedKeyType::NUMBER},
     {"2", PressedKeyType::NUMBER},
     {"3", PressedKeyType::NUMBER},
     {".", PressedKeyType::POINT}},

    // Linha 3 - Controles especiais
    {{"--", PressedKeyType::DECREMENT},
     {"0", PressedKeyType::NUMBER},
     {"++", PressedKeyType::INCREMENT},
     {"OK", PressedKeyType::RETURN}}};

/**
 * @brief Constructs a Numpad widget at a specified position on a specified
 * screen
 * @param _x X-coordinate for the Numpad position
 * @param _y Y-coordinate for the Numpad position
 * @param _screen Screen identifier where the Numpad will be displayed
 */
Numpad::Numpad(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen), m_config{} {}

/**
 * @brief Default constructor for the Numpad class
 * Initializes a Numpad at position (0,0) on screen 0
 */
Numpad::Numpad() : WidgetBase(0, 0, 0), m_config{} {}

/**
 * @brief Destructor for the Numpad class
 * Cleans up any resources used by the Numpad
 */
Numpad::~Numpad() {
    cleanupMemory();
}

void Numpad::cleanupMemory() {
    // Numpad doesn't use dynamic memory allocation
    // CharString handles its own memory management
    ESP_LOGD(TAG, "Numpad memory cleanup completed");
}

/**
 * @brief Detects if the Numpad has been touched
 * @param _xTouch Pointer to the X-coordinate of the touch
 * @param _yTouch Pointer to the Y-coordinate of the touch
 * @return True if the touch is within the Numpad area, otherwise false
 */
bool Numpad::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) { return false; }

/**
 * @brief Retrieves the callback function associated with the Numpad
 * @return Pointer to the callback function
 */
functionCB_t Numpad::getCallbackFunc() { return m_callback; }

/**
 * @brief Detects if a specific key on the Numpad has been touched, and returns
 * the type of key
 * @param _xTouch Pointer to the X-coordinate of the touch
 * @param _yTouch Pointer to the Y-coordinate of the touch
 * @param pressedKey Pointer to store the type of key that was pressed
 * @return True if a key is detected, otherwise false
 */
bool Numpad::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch,
                         PressedKeyType *pressedKey) {

  CHECK_VISIBLE_BOOL
  CHECK_INITIALIZED_BOOL
  CHECK_LOADED_BOOL
  CHECK_DEBOUNCE_CLICK_BOOL
  CHECK_ENABLED_BOOL
  CHECK_LOCKED_BOOL
  CHECK_POINTER_TOUCH_NULL_BOOL
  bool retorno = false;
  uint16_t xMax = m_xPos + m_availableWidth;
  uint16_t yMax = m_yPos + m_availableHeight;
  (*pressedKey) = PressedKeyType::NONE;

  
  m_myTime = millis();

  bool inBounds = POINT_IN_RECT(*_xTouch, *_yTouch, m_xPos, m_yPos, m_availableWidth, m_availableHeight);
  if(inBounds) {
    m_myTime = millis();
  }

  if ((*_xTouch > m_xPos) && (*_xTouch < xMax) && (*_yTouch > m_yPos) &&
      (*_yTouch < yMax)) {
    (*pressedKey) = PressedKeyType::NUMBER;

    int16_t aux_xIndexClick = ((*_xTouch) - m_xPos) / (m_keyW + 2);
    int16_t aux_yIndexClick = ((*_yTouch) - m_yPos) / (m_keyH + 2);

    uint16_t xIndexClick = constrain(aux_xIndexClick, 0, Numpad::aCols - 1);
    uint16_t yIndexClick = constrain(aux_yIndexClick, 0, Numpad::aRows - 1);

    const Key_t letter = m_pad[yIndexClick][xIndexClick];

    if (letter.type == PressedKeyType::INVERT_VALUE) {
      ESP_LOGD(TAG, "Invert value");

      float v = m_content.toFloat() * -1;
      m_content.setString(v);

      drawKeys(false, true);
      (*pressedKey) = PressedKeyType::INVERT_VALUE;
      return true;
    }
    if (letter.type == PressedKeyType::DECREMENT) {
      ESP_LOGD(TAG, "Decrement value");

      float v = m_content.toFloat();
      v--;
      m_content.setString(v);

      drawKeys(false, true);
      (*pressedKey) = PressedKeyType::DECREMENT;
      return true;
    }
    if (letter.type == PressedKeyType::INCREMENT) {
      ESP_LOGD(TAG, "Increment value");

      float v = m_content.toFloat();
      v++;
      m_content.setString(v);

      drawKeys(false, true);
      (*pressedKey) = PressedKeyType::INCREMENT;
      return true;
    }

    // const char *letter = m_pad[yIndexClick][xIndexClick];

    if (letter.label[0] == '\0') {
      ESP_LOGD(TAG, "Empty key. None action.");
      return false;
    }

    ESP_LOGD(TAG, "Index clicked: %d, %d = %s", xIndexClick, yIndexClick, letter);

    switch (letter.type) {
    case PressedKeyType::NUMBER:
      ESP_LOGD(TAG, "Is number");
      addLetter((char)letter.label[0]);
      break;
    case PressedKeyType::POINT:
      addLetter((char)letter.label[0]);
      break;
    case PressedKeyType::RETURN:
      (*pressedKey) = PressedKeyType::RETURN;
      break;
    case PressedKeyType::DEL:
      ESP_LOGD(TAG, "Is Delete");
      removeLetter();
      break;
    default:
      ESP_LOGD(TAG, "Another type: %d", letter.type);
      break;
    }

    retorno = true;
  }

  return retorno;
}

/**
 * @brief Redraws the Numpad on the screen, updating its appearance
 * @param fullScreen If true, redraws the entire screen
 * @param onlyContent If true, redraws only the content area
 */
void Numpad::drawKeys(bool fullScreen, bool onlyContent) {
  CHECK_TFT_VOID
  CHECK_LOADED_VOID
  uint32_t startMillis = millis();

  if (fullScreen) {
    WidgetBase::objTFT->fillScreen(Numpad::m_backgroundColor);
  }

  WidgetBase::objTFT->fillRect(m_pontoPreview.x, m_pontoPreview.y,
                               m_pontoPreview.width, m_pontoPreview.height,
                               Numpad::m_backgroundColor);
  WidgetBase::objTFT->drawRect(m_pontoPreview.x, m_pontoPreview.y,
                               m_pontoPreview.width, m_pontoPreview.height,
                               Numpad::m_keyColor);

  WidgetBase::objTFT->setTextColor(Numpad::m_keyColor);

  WidgetBase::objTFT->setFont(m_fontPreview);
  const char *conteudo =
      getLastLettersForSpace(m_content.getString(), m_pontoPreview.width * 0.9,
                             m_pontoPreview.height * 0.9);
  printText(conteudo, m_pontoPreview.x + 2,
            m_pontoPreview.y + (m_pontoPreview.height / 2), ML_DATUM,
            m_lastArea, Numpad::m_backgroundColor);

  WidgetBase::objTFT->setFont(m_fontKeys);
  WidgetBase::objTFT->setTextColor(Numpad::m_letterColor);

  if (!onlyContent) {
    WidgetBase::objTFT->fillRect(m_xPos, m_yPos, m_availableWidth,
                                 m_availableHeight, Numpad::m_backgroundColor);
    for (auto row = 0; row < Numpad::aRows; ++row) {
      for (auto col = 0; col < Numpad::aCols; ++col) {
        const Key_t letter = m_pad[row][col];

        if (letter.type != PressedKeyType::EMPTY) {
          uint16_t keyScale = 1;

          // uint16_t xCenter = xPos + (((keyScale * (m_keyW + 2))) * col) +
          // ((keyScale * (m_keyW + 2)) / 2); uint16_t yCenter = m_yPos + ((m_keyH
          // + 2) * row) + (m_keyH / 2);
          const int key_width = m_keyW * keyScale + (2 * (keyScale - 1));
          const int key_height = m_keyH;
          const int key_x = m_xPos + ((m_keyW + 2) * col);
          const int key_y = m_yPos + ((m_keyH + 2) * row);
          const int key_round = 4;

          WidgetBase::objTFT->fillRoundRect(key_x, key_y, key_width, key_height,
                                            key_round, Numpad::m_keyColor);
          WidgetBase::objTFT->drawRoundRect(key_x, key_y, key_width, key_height,
                                            key_round, Numpad::m_letterColor);

          uint16_t xCenter = key_x + key_width / 2;
          uint16_t yCenter = key_y + key_height / 2;

          printText(letter.label, xCenter, yCenter, MC_DATUM);
        }
      }
    }
  }

  uint32_t endMillis = millis();
  ESP_LOGD(TAG, "Numpad::redraw: %i ms", endMillis - startMillis);
}

/**
 * @brief Adds a character to the current Numpad input
 * @param c Character to add
 */
void Numpad::addLetter(char c) {
  if (c == '.' && m_content.containsChar('.')) {
    ESP_LOGE(TAG, "Value of numberbox aleady has . character");
    return;
  }

  if (m_content.addChar(c)) {
    drawKeys(false, true);
  } else {
    ESP_LOGE(TAG, "numberbox has reached maximum lenght. The max lenght is %d",
          MAX_LENGTH_CSTR);
  }
}

/**
 * @brief Removes the last character from the current Numpad input
 */
void Numpad::removeLetter() {
  if (!m_content.removeLastChar()) {
    ESP_LOGD(TAG, "Char not removed");
  }
  drawKeys(false, true);
}

/**
 * @brief Configures the Numpad settings
 * Initializes the Numpad layout and appearance
 */
bool Numpad::setup() {
  CHECK_TFT_BOOL
  if (m_loaded) {
    ESP_LOGW(TAG, "Keyboard already configured");
    return false;
  }
#if defined(DISP_DEFAULT)
  m_screenW = WidgetBase::objTFT->width();
  m_screenH = WidgetBase::objTFT->height();
#endif

  m_availableWidth = m_screenW * 0.9;
  m_availableHeight = (m_screenH * 0.75);

  m_keyW = (m_availableWidth / Numpad::aCols) - 2;
  m_keyH = (m_availableHeight / Numpad::aRows) - 2;

  m_pontoPreview.x = m_screenW / 4;
  m_pontoPreview.y = 1;
  m_pontoPreview.height = (m_screenH * 0.2);
  m_pontoPreview.width = m_screenW / 2;

  m_xPos = (m_screenW - m_availableWidth) / 2;
  m_yPos = m_screenH - m_availableHeight;

  float percentUtilArea = 0.9;
#if defined(USING_GRAPHIC_LIB)
  m_fontKeys = const_cast<GFXfont *>(getBestRobotoBold(
      m_keyW * percentUtilArea, m_keyH * percentUtilArea, "CAP"));
  m_fontPreview = m_fontKeys; // const_cast<GFXfont*>(getBestRobotoBold(m_pontoPreview.width
                  // * percentUtilArea, m_pontoPreview.height *
                  // percentUtilArea, "M"));
#endif
  m_loaded = true;
  m_initialized = true;
  return true;
}

void Numpad::setup(const NumpadConfig& config) {
  CHECK_TFT_VOID
  if (m_loaded) {
    ESP_LOGW(TAG, "Numpad already configured");
    return;
  }

  // Clean up any existing memory before setting new config
  cleanupMemory();
  
  // Deep copy configuration
  m_config = config;
  
  // Update static colors from config
  m_backgroundColor = config.backgroundColor;
  m_letterColor = config.letterColor;
  m_keyColor = config.keyColor;
  
  #if defined(USING_GRAPHIC_LIB)
  m_fontKeys = const_cast<GFXfont*>(config.fontKeys);
  m_fontPreview = const_cast<GFXfont*>(config.fontPreview);
  #endif
  
  // Initialize layout without calling setup() to avoid recursion
  #if defined(DISP_DEFAULT)
  m_screenW = WidgetBase::objTFT->width();
  m_screenH = WidgetBase::objTFT->height();
  #endif

  m_availableWidth = m_screenW * 0.9;
  m_availableHeight = (m_screenH * 0.75);

  m_keyW = (m_availableWidth / Numpad::aCols) - 2;
  m_keyH = (m_availableHeight / Numpad::aRows) - 2;

  m_pontoPreview.x = m_screenW / 4;
  m_pontoPreview.y = 1;
  m_pontoPreview.height = (m_screenH * 0.2);
  m_pontoPreview.width = m_screenW / 2;

  m_xPos = (m_screenW - m_availableWidth) / 2;
  m_yPos = m_screenH - m_availableHeight;

  m_loaded = true;
  m_initialized = true;
  ESP_LOGD(TAG, "Numpad configured with custom settings");
}

/**
 * @brief Opens the Numpad associated with a specific NumberBox
 * @param _field Pointer to the NumberBox that will receive the Numpad input
 */
void Numpad::open(NumberBox *_field) {
  m_myTime = millis() + (TIMEOUT_CLICK *
                         3); // Espera o tempo de 3 clicks para iniciar a
                             // detecção, evitando apertar tecla assim que abre.
  m_field = _field;
  WidgetBase::usingKeyboard = true;
  m_content = m_field->getValueChar();
#if defined(DISP_DEFAULT)
  WidgetBase::objTFT->setFont(&RobotoBold10pt7b);
#endif

  drawKeys(true, false);
}

/**
 * @brief Closes the Numpad and clears the input
 */
void Numpad::close() {
#if defined(DISP_DEFAULT)
  WidgetBase::objTFT->setFont((GFXfont *)0);
#endif
  ESP_LOGD(TAG, "Value of content is: %s", m_content.getString());
  ESP_LOGD(TAG, "Value of content is: %f", m_content.toFloat());

  m_field->setValue(m_content.toFloat());

  WidgetBase::usingKeyboard = false;
}

/**
 * @brief Inserts a character into the current input content
 * @param c Character to insert
 */
void Numpad::insertChar(char c) {
  if (WidgetBase::usingKeyboard) {
    addLetter(c);
  } else {
    ESP_LOGE(TAG, "Cant add char. Keyboard is not open");
  }
}

void Numpad::show() {
  m_visible = true;
  m_shouldRedraw = true;
}

void Numpad::hide() {
  m_visible = false;
  m_shouldRedraw = true;
}

void Numpad::forceUpdate() { m_shouldRedraw = true; }

void Numpad::redraw() {
  drawKeys(false, false);
}