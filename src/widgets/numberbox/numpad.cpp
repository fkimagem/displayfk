#include "numpad.h"

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
    : WidgetBase(_x, _y, _screen) {}

/**
 * @brief Default constructor for the Numpad class
 * Initializes a Numpad at position (0,0) on screen 0
 */
Numpad::Numpad() : WidgetBase(0, 0, 0) {}

/**
 * @brief Destructor for the Numpad class
 * Cleans up any resources used by the Numpad
 */
Numpad::~Numpad() {}

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
functionCB_t Numpad::getCallbackFunc() { return cb; }

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
  bool retorno = false;
  uint16_t xMax = xPos + m_availableWidth;
  uint16_t yMax = yPos + m_availableHeight;
  (*pressedKey) = PressedKeyType::NONE;

  if (millis() - m_myTime < TIMEOUT_CLICK) {
    return false;
  }
  m_myTime = millis();

  if ((*_xTouch > xPos) && (*_xTouch < xMax) && (*_yTouch > yPos) &&
      (*_yTouch < yMax)) {
    (*pressedKey) = PressedKeyType::NUMBER;

    int16_t aux_xIndexClick = ((*_xTouch) - xPos) / (m_keyW + 2);
    int16_t aux_yIndexClick = ((*_yTouch) - yPos) / (m_keyH + 2);

    uint16_t xIndexClick = constrain(aux_xIndexClick, 0, Numpad::aCols - 1);
    uint16_t yIndexClick = constrain(aux_yIndexClick, 0, Numpad::aRows - 1);

    const Key_t letter = m_pad[yIndexClick][xIndexClick];

    if (letter.type == PressedKeyType::INVERT_VALUE) {
      log_d("Invert value");

      float v = m_content.toFloat() * -1;
      m_content.setString(v);

      redraw(false, true);
      (*pressedKey) = PressedKeyType::INVERT_VALUE;
      return true;
    }
    if (letter.type == PressedKeyType::DECREMENT) {
      log_d("Decrement value");

      float v = m_content.toFloat();
      v--;
      m_content.setString(v);

      redraw(false, true);
      (*pressedKey) = PressedKeyType::DECREMENT;
      return true;
    }
    if (letter.type == PressedKeyType::INCREMENT) {
      log_d("Increment value");

      float v = m_content.toFloat();
      v++;
      m_content.setString(v);

      redraw(false, true);
      (*pressedKey) = PressedKeyType::INCREMENT;
      return true;
    }

    // const char *letter = m_pad[yIndexClick][xIndexClick];

    if (letter.label[0] == '\0') {
      log_d("Empty key. None action.");
      return false;
    }

    log_d("Index clicked: %d, %d = %s", xIndexClick, yIndexClick, letter);

    switch (letter.type) {
    case PressedKeyType::NUMBER:
      log_d("Is number");
      addLetter((char)letter.label[0]);
      break;
    case PressedKeyType::POINT:
      addLetter((char)letter.label[0]);
      break;
    case PressedKeyType::RETURN:
      (*pressedKey) = PressedKeyType::RETURN;
      break;
    case PressedKeyType::DEL:
      log_d("Is Delete");
      removeLetter();
      break;
    default:
      log_d("Another type: %d", letter.type);
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
void Numpad::redraw(bool fullScreen, bool onlyContent) {
  CHECK_TFT_VOID
#if defined(DISP_DEFAULT)
  if (!loaded) {
    Serial.println("Numpad not loaded");
    return;
  }
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
    WidgetBase::objTFT->fillRect(xPos, yPos, m_availableWidth,
                                 m_availableHeight, Numpad::m_backgroundColor);
    for (auto row = 0; row < Numpad::aRows; ++row) {
      for (auto col = 0; col < Numpad::aCols; ++col) {
        const Key_t letter = m_pad[row][col];

        if (letter.type != PressedKeyType::EMPTY) {
          uint16_t keyScale = 1;

          // uint16_t xCenter = xPos + (((keyScale * (m_keyW + 2))) * col) +
          // ((keyScale * (m_keyW + 2)) / 2); uint16_t yCenter = yPos + ((m_keyH
          // + 2) * row) + (m_keyH / 2);
          const int key_width = m_keyW * keyScale + (2 * (keyScale - 1));
          const int key_height = m_keyH;
          const int key_x = xPos + ((m_keyW + 2) * col);
          const int key_y = yPos + ((m_keyH + 2) * row);
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
#endif

  uint32_t endMillis = millis();
  Serial.printf("Numpad::redraw: %i ms\n", endMillis - startMillis);
}

/**
 * @brief Adds a character to the current Numpad input
 * @param c Character to add
 */
void Numpad::addLetter(char c) {
  if (c == '.' && m_content.containsChar('.')) {
    log_e("Value of numberbox aleady has . character");
    return;
  }

  if (m_content.addChar(c)) {
    redraw(false, true);
  } else {
    log_e("numberbox has reached maximum lenght. The max lenght is %d",
          MAX_LENGTH_CSTR);
  }
}

/**
 * @brief Removes the last character from the current Numpad input
 */
void Numpad::removeLetter() {
  if (!m_content.removeLastChar()) {
    log_d("Char not removed");
  }
  redraw(false, true);
}

/**
 * @brief Configures the Numpad settings
 * Initializes the Numpad layout and appearance
 */
void Numpad::setup() {
  CHECK_TFT_VOID
  if (!WidgetBase::objTFT) {
    log_e("TFT not defined on WidgetBase");
    return;
  }
  if (loaded) {
    log_w("Keyboard already configured");
    return;
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

  xPos = (m_screenW - m_availableWidth) / 2;
  yPos = m_screenH - m_availableHeight;

  float percentUtilArea = 0.9;
#if defined(USING_GRAPHIC_LIB)
  m_fontKeys = const_cast<GFXfont *>(getBestRobotoBold(
      m_keyW * percentUtilArea, m_keyH * percentUtilArea, "CAP"));
  m_fontPreview = m_fontKeys; // const_cast<GFXfont*>(getBestRobotoBold(m_pontoPreview.width
                  // * percentUtilArea, m_pontoPreview.height *
                  // percentUtilArea, "M"));
#endif
  loaded = true;
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

  redraw(true, false);
}

/**
 * @brief Closes the Numpad and clears the input
 */
void Numpad::close() {
#if defined(DISP_DEFAULT)
  WidgetBase::objTFT->setFont((GFXfont *)0);
#endif
  log_d("Value of content is: %s", m_content.getString());
  log_d("Value of content is: %f", m_content.toFloat());

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
    log_e("Cant add char. Keyboard is not open");
  }
}

void Numpad::show() {
  visible = true;
  m_shouldRedraw = true;
}

void Numpad::hide() {
  visible = false;
  m_shouldRedraw = true;
}