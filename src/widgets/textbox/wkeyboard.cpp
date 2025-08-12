#include "wkeyboard.h"
/**
 * @brief Lowercase alphabet layout for the keyboard.
 * 
 * The layout is organized in a grid with numbers on top and controls in strategic positions:
 * - Row 1: Numbers 1-0
 * - Row 2-4: Lowercase letters
 * - Row 5: Special characters and controls (DEL, CAP, CLR, OK)
 */
/*const char *WKeyboard::m_alphabet[WKeyboard::aRows][WKeyboard::aCols] = {
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
    {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p"},
    {"a", "s", "d", "f", "g", "h", "j", "k", "l", "DEL"},
    {"z", "x", "c", "v", "b", "n", "m", " ", "\0", "CAP"},
    {":", ",", "+", "-", "*", "/", "=", ".", "CLR", "OK"}};*/

uint16_t WKeyboard::m_backgroundColor = CFK_GREY3;
uint16_t WKeyboard::m_letterColor = CFK_BLACK;
uint16_t WKeyboard::m_keyColor = CFK_GREY13;

    const Key_t WKeyboard::m_alphabet[AROWS][ACOLS] = {
    // Linha 0 - Números
    {{"1", PressedKeyType::NUMBER}, {"2", PressedKeyType::NUMBER}, {"3", PressedKeyType::NUMBER}, {"4", PressedKeyType::NUMBER}, {"5", PressedKeyType::NUMBER},
     {"6", PressedKeyType::NUMBER}, {"7", PressedKeyType::NUMBER}, {"8", PressedKeyType::NUMBER}, {"9", PressedKeyType::NUMBER}, {"0", PressedKeyType::NUMBER}},

    // Linha 1 - QWERTY topo
    {{"q", PressedKeyType::LETTER}, {"w", PressedKeyType::LETTER}, {"e", PressedKeyType::LETTER}, {"r", PressedKeyType::LETTER}, {"t", PressedKeyType::LETTER},
     {"y", PressedKeyType::LETTER}, {"u", PressedKeyType::LETTER}, {"i", PressedKeyType::LETTER}, {"o", PressedKeyType::LETTER}, {"p", PressedKeyType::LETTER}},

    // Linha 2 - QWERTY meio + DEL
    {{"a", PressedKeyType::LETTER}, {"s", PressedKeyType::LETTER}, {"d", PressedKeyType::LETTER}, {"f", PressedKeyType::LETTER}, {"g", PressedKeyType::LETTER},
     {"h", PressedKeyType::LETTER}, {"j", PressedKeyType::LETTER}, {"k", PressedKeyType::LETTER}, {"l", PressedKeyType::LETTER}, {"DEL", PressedKeyType::DEL}},

    // Linha 3 - QWERTY base + espaço, nulo, caps
    {{"z", PressedKeyType::LETTER}, {"x", PressedKeyType::LETTER}, {"c", PressedKeyType::LETTER}, {"v", PressedKeyType::LETTER}, {"b", PressedKeyType::LETTER},
     {"n", PressedKeyType::LETTER}, {"m", PressedKeyType::LETTER}, {" ", PressedKeyType::CONTROL}, {"\0", PressedKeyType::EMPTY}, {"CAP", PressedKeyType::CAPS}},

    // Linha 4 - Símbolos, limpar, ok
    {{":", PressedKeyType::ANOTHER}, {",", PressedKeyType::ANOTHER}, {"+", PressedKeyType::SIGN}, {"-", PressedKeyType::SIGN}, {"*", PressedKeyType::SIGN},
     {"/", PressedKeyType::SIGN}, {"=", PressedKeyType::SIGN}, {".", PressedKeyType::ANOTHER}, {"CLR", PressedKeyType::CLR}, {"OK", PressedKeyType::RETURN}}
};

/**
 * @brief Uppercase alphabet layout for the keyboard.
 * 
 * The layout is organized in a grid with special characters on top and controls in strategic positions:
 * - Row 1: Special characters
 * - Row 2-4: Uppercase letters
 * - Row 5: Special characters and controls (DEL, CAP, CLR, OK)
 */
/*const char *WKeyboard::m_alphabetCap[WKeyboard::aRows][WKeyboard::aCols] = {
    {"!", "@", "#", "$", "%", "^", "&", "?", "(", ")"},
    {"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"},
    {"A", "S", "D", "F", "G", "H", "J", "K", "L", "DEL"},
    {"Z", "X", "C", "V", "B", "N", "M", "{", "}", "CAP"},
    {"_", "'", ":", "<", ">", ";", "~", "[", "]", "OK"}};*/

const Key_t WKeyboard::m_alphabetCap[AROWS][ACOLS] = {
    // Linha 0 - Símbolos especiais
    {{"!", PressedKeyType::SIGN}, {"@", PressedKeyType::SIGN}, {"#", PressedKeyType::SIGN}, {"$", PressedKeyType::SIGN}, {"%", PressedKeyType::SIGN},
     {"^", PressedKeyType::SIGN}, {"&", PressedKeyType::SIGN}, {"?", PressedKeyType::SIGN}, {"(", PressedKeyType::SIGN}, {")", PressedKeyType::SIGN}},

    // Linha 1 - QWERTY topo maiúsculo
    {{"Q", PressedKeyType::LETTER}, {"W", PressedKeyType::LETTER}, {"E", PressedKeyType::LETTER}, {"R", PressedKeyType::LETTER}, {"T", PressedKeyType::LETTER},
     {"Y", PressedKeyType::LETTER}, {"U", PressedKeyType::LETTER}, {"I", PressedKeyType::LETTER}, {"O", PressedKeyType::LETTER}, {"P", PressedKeyType::LETTER}},

    // Linha 2 - QWERTY meio maiúsculo + DEL
    {{"A", PressedKeyType::LETTER}, {"S", PressedKeyType::LETTER}, {"D", PressedKeyType::LETTER}, {"F", PressedKeyType::LETTER}, {"G", PressedKeyType::LETTER},
     {"H", PressedKeyType::LETTER}, {"J", PressedKeyType::LETTER}, {"K", PressedKeyType::LETTER}, {"L", PressedKeyType::LETTER}, {"DEL", PressedKeyType::DEL}},

    // Linha 3 - QWERTY base maiúsculo + chaves, caps
    {{"Z", PressedKeyType::LETTER}, {"X", PressedKeyType::LETTER}, {"C", PressedKeyType::LETTER}, {"V", PressedKeyType::LETTER}, {"B", PressedKeyType::LETTER},
     {"N", PressedKeyType::LETTER}, {"M", PressedKeyType::LETTER}, {"{", PressedKeyType::SIGN}, {"}", PressedKeyType::SIGN}, {"CAP", PressedKeyType::CAPS}},

    // Linha 4 - Símbolos diversos, ok
    {{"_", PressedKeyType::SIGN}, {"'", PressedKeyType::SIGN}, {":", PressedKeyType::SIGN}, {"<", PressedKeyType::SIGN}, {">", PressedKeyType::SIGN},
     {";", PressedKeyType::SIGN}, {"~", PressedKeyType::SIGN}, {"[", PressedKeyType::SIGN}, {"]", PressedKeyType::SIGN}, {"OK", PressedKeyType::RETURN}}
};

/**
 * @brief Constructs a WKeyboard widget at a specified position on a specified screen.
 * @param _x X-coordinate for the WKeyboard position.
 * @param _y Y-coordinate for the WKeyboard position.
 * @param _screen Screen identifier where the WKeyboard will be displayed.
 */
WKeyboard::WKeyboard(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen)
{

}

/**
 * @brief Default constructor for WKeyboard.
 * 
 * Initializes a keyboard at position (0,0) on screen 0.
 */
WKeyboard::WKeyboard() : WidgetBase(0, 0, 0)
{
}

/**
 * @brief Destructor for the WKeyboard class.
 */
WKeyboard::~WKeyboard()
{
    
}

/**
 * @brief Detects if the keyboard has been touched.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is within the keyboard area, otherwise false.
 * 
 * This base implementation always returns false as the actual detection
 * is performed by the specialized detectTouch method.
 */
bool WKeyboard::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
    return false;
}

/**
 * @brief Retrieves the callback function associated with the keyboard.
 * @return Pointer to the callback function.
 */
functionCB_t WKeyboard::getCallbackFunc()
{
    return cb;
}

/**
 * @brief Detects touch events and identifies the type of key pressed.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @param pressedKey Pointer to a variable to store the detected key type.
 * @return True if a key is detected and processed, otherwise false.
 * 
 * This method handles the core keyboard interaction by detecting which key 
 * was pressed and performing the appropriate action (adding/removing characters,
 * toggling caps lock, etc.).
 */
bool WKeyboard::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch, PressedKeyType *pressedKey)
{
    bool retorno = false;
    uint16_t xMax = xPos + m_availableWidth;
    uint16_t yMax = yPos + m_availableHeight;
    (*pressedKey) = PressedKeyType::NONE;

    if (millis() - m_myTime < TIMEOUT_CLICK)
    {
        return false;
    }
    m_myTime = millis();

    if ((*_xTouch > xPos) && (*_xTouch < xMax) && (*_yTouch > yPos) && (*_yTouch < yMax))
    {
        (*pressedKey) = PressedKeyType::LETTER;

        int16_t aux_xIndexClick = ((*_xTouch) - xPos) / (m_keyW + 2);
        int16_t aux_yIndexClick = ((*_yTouch) - yPos) / (m_keyH + 2);

        uint16_t xIndexClick = constrain(aux_xIndexClick, 0, WKeyboard::aCols-1);
        uint16_t yIndexClick = constrain(aux_yIndexClick, 0, WKeyboard::aRows-1);

        Key_t letter = m_capsLock ? (m_alphabetCap[yIndexClick][xIndexClick]) : (m_alphabet[yIndexClick][xIndexClick]);

        //if (yIndexClick == 3 && xIndexClick == 9)
        if (letter.type == PressedKeyType::CAPS)
        {
            m_capsLock = !m_capsLock;
            redraw(false, false);
            (*pressedKey) = PressedKeyType::CAPS;
            return true;
        }

        // const char *letter = (Keyboard::_alphabet[yIndexClick][xIndexClick]);
        
        if (letter.type == PressedKeyType::EMPTY)
        {
            log_d("Empty key. Changing to previous one.");
            xIndexClick--;
            letter = m_capsLock ? (m_alphabetCap[yIndexClick][xIndexClick]) : (m_alphabet[yIndexClick][xIndexClick]);
        }

        log_d("Index clicked: %d, %d = %s", xIndexClick, yIndexClick, letter.label);

        switch (letter.type)
        {
        case PressedKeyType::NONE:
            break;
        case PressedKeyType::LETTER:
            addLetter((char)letter.label[0]);
            break;
        case PressedKeyType::NUMBER:
            addLetter((char)letter.label[0]);
            break;
        case PressedKeyType::ANOTHER:
            addLetter((char)letter.label[0]);
            break;
        case PressedKeyType::CONTROL:
            addLetter((char)letter.label[0]);
            break;
        case PressedKeyType::SHIFT:
            break;
        case PressedKeyType::CAPS:
            m_capsLock = !m_capsLock;
            redraw(false, false);
            break;
        case PressedKeyType::DEL:
            removeLetter();
            break;
        case PressedKeyType::CLR:
            clear();
            break;
        case PressedKeyType::SIGN:
            addLetter((char)letter.label[0]);
            break;
        case PressedKeyType::RETURN:
            (*pressedKey) = PressedKeyType::RETURN;
            break;

        case PressedKeyType::INVERT_VALUE:
        case PressedKeyType::INCREMENT:
        case PressedKeyType::DECREMENT:
        default:
            break;
        }

        /*if (isdigit(letter.label))
        {
            log_d("Is number");
            addLetter((char)letter.label);
        }
        else if (isalpha(letter.label))
        {
            if (strcmp(letter, "DEL") == 0)
            {
                log_d("Is Delete");
                removeLetter();
            }
            else if (strcmp(letter, "OK") == 0)
            {
                log_d("Is OK");
                (*pressedKey) = PressedKeyType::RETURN;
            }
            else if (strcmp(letter, "CLR") == 0)
            {
                log_d("Clear Field");
                clear();
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
        }*/

        retorno = true;
    }

    //WidgetBase::objTFT->fillCircle(*_xTouch, *_yTouch, 2, CFK_BLUE);

    return retorno;
}

/**
 * @brief Redraws the keyboard on the screen.
 * @param fullScreen If true, redraws the entire screen; otherwise, only the keyboard.
 * @param onlyContent If true, redraws only the content area without borders or background.
 * 
 * This method handles drawing the keyboard layout including keys, content display area,
 * and handling the different states (caps lock on/off).
 */
void WKeyboard::redraw(bool fullScreen, bool onlyContent)
{
    if(!loaded){
        Serial.println("Keyboard not loaded");
        return;
    }
    if (fullScreen)
    {
        WidgetBase::objTFT->fillScreen(WKeyboard::m_backgroundColor);
    }


    // uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
    //uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

    WidgetBase::objTFT->fillRect(m_pontoPreview.x, m_pontoPreview.y, m_pontoPreview.width, m_pontoPreview.height, WKeyboard::m_backgroundColor);
    WidgetBase::objTFT->drawRect(m_pontoPreview.x, m_pontoPreview.y, m_pontoPreview.width, m_pontoPreview.height, WKeyboard::m_keyColor);

    WidgetBase::objTFT->setTextColor(WKeyboard::m_keyColor);

    /*if(m_keyH > 20){
        WidgetBase::objTFT->setFont(&RobotoBold10pt7b);
    }else{
        WidgetBase::objTFT->setFont(&RobotoBold5pt7b);
    }*/


    //TextBound_t area;
    //WidgetBase::objTFT->getTextBounds("M", 10, 10, &area.x, &area.y, &area.width, &area.height);
    //uint16_t qtdLetrasMax = m_pontoPreview.width / area.width;

    //const char* conteudo = m_content.getLastChars(qtdLetrasMax);
    WidgetBase::objTFT->setFont(m_fontPreview);
    const char* conteudo = getLastLettersForSpace(m_content.getString(), m_pontoPreview.width * 0.9, m_pontoPreview.height * 0.9);
    printText(conteudo, m_pontoPreview.x + 2, m_pontoPreview.y + (m_pontoPreview.height / 2), ML_DATUM, lastArea, WKeyboard::m_backgroundColor);


    WidgetBase::objTFT->setFont(m_fontKeys);
    WidgetBase::objTFT->setTextColor(WKeyboard::m_letterColor);
    if (!onlyContent)
    {
        WidgetBase::objTFT->fillRect(xPos, yPos, m_availableWidth, m_availableHeight, WKeyboard::m_backgroundColor);
        const float percentUtilArea = 0.9;

        for (auto row = 0; row < WKeyboard::aRows; ++row)
        {
            for (auto col = 0; col < WKeyboard::aCols; ++col)
            {
                Key_t letter = m_capsLock ? (m_alphabetCap[row][col]) : (m_alphabet[row][col]);

                if (letter.type != PressedKeyType::EMPTY)
                {
                    uint16_t keyScale = 1;
                    if (col + 1 < WKeyboard::aCols)
                    { // Tem proxima coluna
                        Key_t nextLetter = m_capsLock ? (m_alphabetCap[row][col + 1]) : (m_alphabet[row][col + 1]);
                        if (nextLetter.type == PressedKeyType::EMPTY)
                        {
                            keyScale++;
                        }
                    }

                    // keyScale = (keyScale * (_keyW + 2));

                    //uint16_t xCenter = xPos + (((keyScale * (m_keyW + 2))) * col) + ((keyScale * (m_keyW + 2)) / 2);
                    //uint16_t yCenter = yPos + ((m_keyH + 2) * row) + (m_keyH / 2);

                    const int key_width = m_keyW * keyScale + (2 * (keyScale - 1));
                    const int key_height = m_keyH;
                    const int key_x = xPos + ((m_keyW + 2) * col);
                    const int key_y = yPos + ((m_keyH + 2) * row);
                    const int key_round = 4;
                    uint16_t xCenter = key_x + key_width / 2;
                    uint16_t yCenter = key_y + key_height / 2;

                    WidgetBase::objTFT->fillRoundRect(key_x, key_y,key_width , key_height, key_round, WKeyboard::m_keyColor);
                    WidgetBase::objTFT->drawRoundRect(key_x, key_y, key_width, key_height, key_round, WKeyboard::m_letterColor);

                    //const GFXfont* font = getBestRobotoBold(key_width * percentUtilArea, key_height * percentUtilArea, letter.label);
                    //WidgetBase::objTFT->setFont(font);

                    printText(letter.label, xCenter, yCenter, MC_DATUM);
                }
            }
        }
    }

    // WidgetBase::objTFT->drawCircle(210, 160, 10, CFK_FUCHSIA);
}

/**
 * @brief Adds a character to the keyboard's current input.
 * @param c Character to add.
 * 
 * Adds a character to the current content string and redraws the content area to show the update.
 */
void WKeyboard::addLetter(char c)
{

    if (m_content.addChar(c))
    {
        redraw(false, true);
    }
    else
    {
        log_e("textbox has reached maximum lenght. The max lenght is %d", MAX_LENGTH_CSTR);
    }
}

/**
 * @brief Removes the last character from the keyboard's current input.
 * 
 * Deletes the last character from the content string and redraws the content area
 * to show the update.
 */
void WKeyboard::removeLetter()
{
    // content.remove(content.length() - 1);
    m_content.removeLastChar();
    redraw(false, true);
}

/**
 * @brief Sets up the keyboard, configuring necessary parameters.
 * 
 * Initializes the keyboard layout, calculates key dimensions, and sets up
 * the display area based on screen dimensions.
 */
void WKeyboard::setup()
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

    // log_d("Screen width: %d", screenW);
    // log_d("Screen height: %d", screenH);

    m_availableWidth = m_screenW * 0.9;
    m_availableHeight = m_screenH * 0.75;

    m_keyW = (m_availableWidth / WKeyboard::aCols) - 2;
    m_keyH = (m_availableHeight / WKeyboard::aRows) - 2;

    // log_d("Available width: %d", availableWidth);
    // log_d("Available height: %d", availableHeight);

    // log_d("Key width: %d", _keyW);
    // log_d("Key height: %d", _keyH);

    log_d("Keys: %i, %i", m_keyW, m_keyH);

    m_pontoPreview.x = m_screenW / 4;
    m_pontoPreview.y = 1;
    m_pontoPreview.height = (m_screenH * 0.2);
    m_pontoPreview.width = m_screenW/2;

    xPos = (m_screenW - m_availableWidth) / 2;
    yPos = m_screenH -m_availableHeight;

    float percentUtilArea = 0.9;
    m_fontKeys = const_cast<GFXfont*>(getBestRobotoBold(m_keyW * percentUtilArea, m_keyH * percentUtilArea, "CAP"));
    m_fontPreview = m_fontKeys;// const_cast<GFXfont*>(getBestRobotoBold(m_pontoPreview.width * percentUtilArea, m_pontoPreview.height * percentUtilArea, "M"));

    m_capsLock = false;
    loaded = true;
}

/**
 * @brief Clears the current input content.
 * 
 * Resets the content string to empty and redraws the content area.
 */
void WKeyboard::clear()
{
    m_content.setString("", true);
    redraw(false, true);
}

/**
 * @brief Opens the keyboard for a specific TextBox field.
 * @param _field Pointer to the TextBox field to be edited.
 * 
 * Prepares the keyboard for input by setting the appropriate flags and initializing
 * the content with the current value from the TextBox.
 */
void WKeyboard::open(TextBox *_field)
{
    m_myTime = millis() + (TIMEOUT_CLICK * 3);// Espera o tempo de 3 clicks para iniciar a detecção, evitando apertar tecla assim que abre.
    m_field = _field;
    WidgetBase::usingKeyboard = true;
    m_content = m_field->getValue();
#if defined(DISP_DEFAULT)
    if(m_keyH > 20){
        WidgetBase::objTFT->setFont(&RobotoBold10pt7b);
    }else{
        WidgetBase::objTFT->setFont(&RobotoBold5pt7b);
    }
    
#endif

    redraw(true, false);
}

/**
 * @brief Closes the keyboard and applies the content to the associated field.
 * 
 * Updates the TextBox with the edited content and cleans up resources.
 */
void WKeyboard::close()
{

    updateFont(FontType::UNLOAD);

    m_field->setValue(m_content.getString());

    // delete field;
    // field = nullptr;
    WidgetBase::usingKeyboard = false;
}

/**
 * @brief Inserts a character into the current input content.
 * @param c Character to insert.
 * 
 * Adds a character to the content if the keyboard is currently open.
 */
void WKeyboard::insertChar(char c)
{
    if(WidgetBase::usingKeyboard){
        addLetter(c);
    }else{
        log_e("Cant add char. Keyboard is not open");
    }
}
