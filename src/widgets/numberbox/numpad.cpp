#include "numpad.h"

/**
 * @brief 2D array defining the characters displayed on the Numpad keys
 * Layout: 
 * 7 8 9 +/-
 * 4 5 6 Del
 * 1 2 3 .
 * -- 0 ++ OK
 */
const char *Numpad::m_pad[Numpad::aRows][Numpad::aCols] = {
    {"7", "8", "9", "+/-"},
    {"4", "5", "6", "Del"},
    {"1", "2", "3", "."},
    {"--", "0", "++", "OK"}};

/**
 * @brief Constructs a Numpad widget at a specified position on a specified screen
 * @param _x X-coordinate for the Numpad position
 * @param _y Y-coordinate for the Numpad position
 * @param _screen Screen identifier where the Numpad will be displayed
 */
Numpad::Numpad(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen)
{
}

/**
 * @brief Default constructor for the Numpad class
 * Initializes a Numpad at position (0,0) on screen 0
 */
Numpad::Numpad() : WidgetBase(0, 0, 0)
{
}

/**
 * @brief Destructor for the Numpad class
 * Cleans up any resources used by the Numpad
 */
Numpad::~Numpad()
{
}

/**
 * @brief Detects if the Numpad has been touched
 * @param _xTouch Pointer to the X-coordinate of the touch
 * @param _yTouch Pointer to the Y-coordinate of the touch
 * @return True if the touch is within the Numpad area, otherwise false
 */
bool Numpad::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
    return false;
}

/**
 * @brief Retrieves the callback function associated with the Numpad
 * @return Pointer to the callback function
 */
functionCB_t Numpad::getCallbackFunc()
{
    return cb;
}

/**
 * @brief Detects if a specific key on the Numpad has been touched, and returns the type of key
 * @param _xTouch Pointer to the X-coordinate of the touch
 * @param _yTouch Pointer to the Y-coordinate of the touch
 * @param pressedKey Pointer to store the type of key that was pressed
 * @return True if a key is detected, otherwise false
 */
bool Numpad::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch, Numpad::PressedKeyType *pressedKey)
{
    bool retorno = false;
    uint16_t xMax = xPos + m_availableWidth;
    uint16_t yMax = yPos + m_availableHeight;
    (*pressedKey) = Numpad::PressedKeyType::NONE;

    if (millis() - m_myTime < TIMEOUT_CLICK)
    {
        return false;
    }
    m_myTime = millis();

    if ((*_xTouch > xPos) && (*_xTouch < xMax) && (*_yTouch > yPos) && (*_yTouch < yMax))
    {
        (*pressedKey) = Numpad::PressedKeyType::NUMBER;

        int16_t aux_xIndexClick = ((*_xTouch) - xPos) / (m_keyW + 2);
        int16_t aux_yIndexClick = ((*_yTouch) - yPos) / (m_keyH + 2);

        uint16_t xIndexClick = constrain(aux_xIndexClick, 0, Numpad::aCols-1);
        uint16_t yIndexClick = constrain(aux_yIndexClick, 0, Numpad::aRows-1);

        if (yIndexClick == 0 && xIndexClick == 3)
        {
            log_d("Invert value");

            float v = m_content.toFloat() * -1;
            m_content.setString(v);

            redraw(false, true);
            (*pressedKey) = Numpad::PressedKeyType::CONTROL;
            return true;
        }
        if (yIndexClick == 3 && xIndexClick == 0)
        {
            log_d("Decrement value");

            float v = m_content.toFloat();
            v--;
            m_content.setString(v);

            redraw(false, true);
            (*pressedKey) = Numpad::PressedKeyType::CONTROL;
            return true;
        }
        if (yIndexClick == 3 && xIndexClick == 2)
        {
            log_d("Increment value");

            float v = m_content.toFloat();
            v++;
            m_content.setString(v);

            redraw(false, true);
            (*pressedKey) = Numpad::PressedKeyType::CONTROL;
            return true;
        }

        const char *letter = m_pad[yIndexClick][xIndexClick];

        if (letter[0] == '\0')
        {
            log_d("Empty key. None action.");
            return false;
        }

        log_d("Index clicked: %d, %d = %s", xIndexClick, yIndexClick, letter);

        if (isdigit(letter[0]))
        {
            log_d("Is number");
            addLetter((char)letter[0]);
        }
        else if (isalpha(letter[0]))
        {
            if (strcmp(letter, "Del") == 0)
            {
                log_d("Is Delete");
                removeLetter();
            }
            else if (strcmp(letter, "OK") == 0)
            {
                log_d("Is OK");
                (*pressedKey) = Numpad::PressedKeyType::RETURN;
            }
            else
            {
                log_d("Is letter");
                addLetter((char)letter[0]);
            }
        }
        else if (ispunct(letter[0]))
        {
            log_d("Is punct");
            addLetter((char)letter[0]);
        }
        else
        {
            log_d("Another type");
            addLetter((char)letter[0]);
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
void Numpad::redraw(bool fullScreen, bool onlyContent)
{
    if(!loaded){
        Serial.println("Numpad not loaded");
        return;
    }
    if (fullScreen)
    {
        WidgetBase::objTFT->fillScreen(WidgetBase::lightMode ? CFK_WHITE : CFK_BLACK);
    }

    uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
    uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

    WidgetBase::objTFT->fillRect(m_pontoPreview.x, m_pontoPreview.y, m_pontoPreview.width, m_pontoPreview.height, CFK_WHITE);
    WidgetBase::objTFT->drawRect(m_pontoPreview.x, m_pontoPreview.y, m_pontoPreview.width, m_pontoPreview.height, baseBorder);

    if(m_keyH > 20){
        WidgetBase::objTFT->setFont(&RobotoBold10pt7b);
    }else{
        WidgetBase::objTFT->setFont(&RobotoBold5pt7b);
    }

    WidgetBase::objTFT->setTextColor(CFK_BLACK);
    TextBound_t area;
    WidgetBase::objTFT->getTextBounds("M", 50, 50, &area.x, &area.y, &area.width, &area.height);


    uint16_t qtdLetrasMax = m_pontoPreview.width / area.width;
    log_d("Box numpad %i / %i = %i", m_pontoPreview.width, area.width, qtdLetrasMax);

#if defined(DISP_BODMER)
    WidgetBase::objTFT->setTextDatum(ML_DATUM);
    WidgetBase::objTFT->setTextPadding(pontoPreview.width);
#endif

    const char* conteudo = m_content.getLastChars(qtdLetrasMax);

#if defined(DISP_DEFAULT)
        printText(conteudo, m_pontoPreview.x + 2, m_pontoPreview.y + (m_pontoPreview.height / 2), ML_DATUM, m_lastArea, CFK_WHITE);
        
#endif

    if (!onlyContent)
    {
        WidgetBase::objTFT->fillRect(xPos, yPos, m_availableWidth, m_availableHeight, WidgetBase::lightMode ? CFK_WHITE : CFK_BLACK);
        for (auto row = 0; row < Numpad::aRows; ++row)
        {
            for (auto col = 0; col < Numpad::aCols; ++col)
            {
                const char *letter = m_pad[row][col];

                if (letter[0] != '\0')
                {
                    uint16_t keyScale = 1;

                    uint16_t xCenter = xPos + (((keyScale * (m_keyW + 2))) * col) + ((keyScale * (m_keyW + 2)) / 2);
                    uint16_t yCenter = yPos + ((m_keyH + 2) * row) + (m_keyH / 2);

                    WidgetBase::objTFT->fillRoundRect(xPos + ((m_keyW + 2) * col), yPos + ((m_keyH + 2) * row), m_keyW * keyScale + (2 * (keyScale - 1)), m_keyH, 4, lightBg);
                    WidgetBase::objTFT->drawRoundRect(xPos + ((m_keyW + 2) * col), yPos + ((m_keyH + 2) * row), m_keyW * keyScale + (2 * (keyScale - 1)), m_keyH, 4, baseBorder);

#if defined(DISP_DEFAULT)
                    WidgetBase::objTFT->setTextColor(baseBorder);
                    WidgetBase::objTFT->setFont(&RobotoBold10pt7b);
                    TextBound_t area;
                    WidgetBase::objTFT->getTextBounds(letter, xCenter, yCenter, &area.x, &area.y, &area.width, &area.height);

                    if(area.width > m_keyW/2){
                        WidgetBase::objTFT->setFont(&RobotoBold5pt7b);
                    }

                    printText(letter, xCenter, yCenter, MC_DATUM);
#endif
                }
            }
        }
    }
}

/**
 * @brief Adds a character to the current Numpad input
 * @param c Character to add
 */
void Numpad::addLetter(char c)
{
    if (c == '.' && m_content.containsChar('.'))
    {
        log_e("Value of numberbox aleady has . character");
        return;
    }

    if (m_content.addChar(c))
    {
        redraw(false, true);
    }
    else
    {
        log_e("numberbox has reached maximum lenght. The max lenght is %d", MAX_LENGTH_CSTR);
    }
}

/**
 * @brief Removes the last character from the current Numpad input
 */
void Numpad::removeLetter()
{
    if(!m_content.removeLastChar()){
        log_d("Char not removed");
    }
    redraw(false, true);
}

/**
 * @brief Configures the Numpad settings
 * Initializes the Numpad layout and appearance
 */
void Numpad::setup()
{
    if (!WidgetBase::objTFT)
    {
        log_e("TFT not defined on WidgetBase");
        return;
    }
    if (loaded)
    {
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
    m_pontoPreview.width = m_screenW/2;

    xPos = (m_screenW - m_availableWidth) / 2;
    yPos = m_screenH - m_availableHeight;

    loaded = true;
}

/**
 * @brief Opens the Numpad associated with a specific NumberBox
 * @param _field Pointer to the NumberBox that will receive the Numpad input
 */
void Numpad::open(NumberBox *_field)
{
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
void Numpad::close()
{
#if defined(DISP_DEFAULT)
    WidgetBase::objTFT->setFont(nullptr);
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
void Numpad::insertChar(char c)
{
    if(WidgetBase::usingKeyboard){
        addLetter(c);
    }else{
        log_e("Cant add char. Keyboard is not open");
    }
}
