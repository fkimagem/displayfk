#include "wlabel.h"
#include "wlabel.h"

/**
 * @brief Constructor for the Label class.
 * @param _x X-coordinate for the Label position.
 * @param _y Y-coordinate for the Label position.
 * @param _screen Screen identifier where the Label will be displayed.
 */
Label::Label(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen),
m_fontSize(1),
m_decimalPlaces(1)
{
}

/**
 * @brief Destructor for the Label class.
 * 
 * Frees allocated memory for text, previous text, and clears font pointer.
 */
Label::~Label()
{
  // Libera memória do texto atual se existir
  if (m_text != nullptr) {
    delete[] m_text;
    m_text = nullptr;
  }
  
  // Libera memória do texto anterior se existir
  if (m_previousText != nullptr) {
    delete[] m_previousText;
    m_previousText = nullptr;
  }
  
    #if defined(USING_GRAPHIC_LIB)
  // A fonte não é liberada aqui porque é apenas uma referência
  m_fontFamily = nullptr;
  #endif
}

/**
 * @brief Detects if the Label has been touched.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return Always returns false as labels don't respond to touch events.
 */
bool Label::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
  return false;
}

/**
 * @brief Retrieves the callback function associated with the Label.
 * @return Pointer to the callback function.
 */
functionCB_t Label::getCallbackFunc()
{
    return cb;
}

void Label::setPrefix(const char* str){ 
  if(m_prefix){
    delete[] m_prefix;
    m_prefix = nullptr;
  }

   // Aloca nova memória para prefixo
    m_prefix = new char[strlen(str) + 1];
    if (m_prefix) {
        strcpy(m_prefix, str);
    }
}

void Label::setSuffix(const char* str){
  if(m_suffix){
    delete[] m_suffix;
    m_suffix = nullptr;
  }

   // Aloca nova memória para sufixo
    m_suffix = new char[strlen(str) + 1];
    if (m_suffix) {
        strcpy(m_suffix, str);
    }
}

/**
 * @brief Sets the text for the label.
 * @param str String to set as the label text.
 * 
 * Applies prefix and suffix to the provided string and updates the label.
 */
void Label::setText(const char* str)
{
  if(!loaded || str == nullptr){
    return;
  }

  uint16_t prefixLength = strlen(m_prefix);
  uint16_t suffixLength = strlen(m_suffix);
  uint16_t textLength = strlen(str);

  // Libera a memória anterior se existir
  if(m_text){
    delete[] m_text;
    m_text = nullptr;
  }
  
  // Aloca nova memória
  m_text = new char[textLength + prefixLength + suffixLength + 1];
  if(m_text){
    strcpy(m_text, m_prefix);
    strcat(m_text, str);
    strcat(m_text, m_suffix);
    m_update = true;
  } else {
    log_e("Failed to allocate memory for label text");
  }
}

/**
 * @brief Sets the text for the label.
 * @param str String object to set as the label text.
 * 
 * Converts String object to C-string and calls the char* version of setText.
 */
void Label::setText(const String &str)
{
  setText(str.c_str());
}

/**
 * @brief Sets the text for the label as a formatted float value.
 * @param value Float value to set as the label text.
 * @param decimalPlaces Amount of decimal places to show.
 * 
 * Converts the float to a string with specified decimal places and updates the label.
 */
void Label::setTextFloat(float value, uint8_t decimalPlaces)
{
  /*if(decimalPlaces == 0){
    setTextInt(int(s));
  }else{
    char convertido[16];
    sprintf(convertido, "%.*f", s, decimalPlaces);
    setText(convertido);
  }*/

  setDecimalPlaces(decimalPlaces);

  char convertido[20];
  dtostrf(value, 2, m_decimalPlaces, convertido);
  //sprintf(convertido, "%.2f", s);
  //log_d("Novo float: ", convertido);
  setText(convertido);
}

/**
 * @brief Sets the text for the label as an integer value.
 * @param value Integer value to set as the label text.
 * 
 * Converts the integer to a string and updates the label.
 */
void Label::setTextInt(int value)
{
  char convertido[16];
  sprintf(convertido, "%d", value);
  //log_d("setTextInt %d => %s", s, convertido);
  setText(convertido);
}

/**
 * @brief Redraws the label on the screen, updating its appearance.
 * 
 * Renders the current text with specified font and colors.
 * Updates display based on the current screen mode.
 */
void Label::redraw()
{
  #if defined(USING_GRAPHIC_LIB)
  if (WidgetBase::currentScreen != screen || !m_update || !loaded)
  {
    return;
  }

  log_d("Redraw label to value %s", m_text);
  WidgetBase::objTFT->setTextColor(m_letterColor);
  WidgetBase::objTFT->setFont(m_fontFamily);
  WidgetBase::objTFT->setTextSize(m_fontSize);
  printText(m_text, xPos, yPos, m_datum, m_lastArea, m_backgroundColor);
  WidgetBase::objTFT->setTextSize(1);
  WidgetBase::objTFT->setFont((GFXfont *)0);
  //WidgetBase::objTFT->setCursor(xPos, yPos);
  //WidgetBase::objTFT->print(text);

  
  //previousText = text;
  m_update = false;
  #endif
}

/**
 * @brief Forces the label to update, refreshing its visual state on next redraw.
 */
void Label::forceUpdate()
{
  m_update = true;
}

/**
 * @brief Defines how many decimal places the string will show.
 * @param places Amount (Must be between 0 and 5).
 */
void Label::setDecimalPlaces(uint8_t places)
{
  places = constrain(places, 0, 5);
  if(places != m_decimalPlaces){
    m_decimalPlaces = places;
    log_d("Decimal places set to %d", m_decimalPlaces);
  }
}

/**
 * @brief Defines the font size.
 * @param newSize Size to set for the font.
 */
void Label::setFontSize(uint8_t newSize)
{
  m_fontSize = newSize;
}

/**
 * @brief Configures the Label widget with specified const char array, font, alignment, and colors.
 * @param _text Initial text to display.
 * @param _fontFamily Pointer to the font used for the text.
 * @param _datum Text alignment setting.
 * @param _color Color of the text.
 * @param _bkColor Background color of the label.
 * @param _prefix Prefix text to display.
 * @param _suffix Suffix text to display.
 */
  #if defined(USING_GRAPHIC_LIB)
void Label::setup(const char *_text, const GFXfont *_fontFamily, uint16_t _datum, uint16_t _color, uint16_t _bkColor, const char* _prefix, const char* _suffix)
{
  if(!WidgetBase::objTFT){
    log_e("TFT not defined on WidgetBase");
    return;
  }
  if (loaded) {
    log_d("Label widget already configured");
    return;
  }

  // Protege contra ponteiros nulos
  _prefix = _prefix ? _prefix : "";
  _suffix = _suffix ? _suffix : "";
  _text   = _text   ? _text   : "";

  uint16_t prefixLength = strlen(_prefix);
  uint16_t suffixLength = strlen(_suffix);
  uint16_t textLength = strlen(_text);

  // Libera memória anterior se existir
  if(m_text){
    delete[] m_text;
    m_text = nullptr;
  }

  if(m_prefix){
    delete[] m_prefix;
    m_prefix = nullptr;
  }

  if(m_suffix){
    delete[] m_suffix;
    m_suffix = nullptr;
  }

   // Aloca nova memória para prefixo
    m_prefix = new char[prefixLength + 1];
    if (m_prefix) {
        strcpy(m_prefix, _prefix);
    }

    // Aloca nova memória para sufixo
    m_suffix = new char[suffixLength + 1];
    if (m_suffix) {
        strcpy(m_suffix, _suffix);
    }


  // Aloca nova memória
  m_text = new char[textLength + prefixLength + suffixLength + 1];
  #if defined(USING_GRAPHIC_LIB)
  if(m_text != nullptr) {
    strcpy(m_text, _prefix);
    strcat(m_text, _text);
    strcat(m_text, _suffix);
    m_fontFamily = const_cast<GFXfont*>(_fontFamily);
    m_datum = _datum;
    m_letterColor = _color;
    m_backgroundColor = _bkColor;
    m_lastArea.x = 0;
    m_lastArea.y = 0;
    m_lastArea.width = 0;
    m_lastArea.height = 0;
    m_update = true;
    loaded = true;
  } else {
    log_e("Failed to allocate memory for label text");
  }
  #endif
}

/**
void Label::setup(const String &_text, const GFXfont *_fontFamily, uint16_t _datum, uint16_t _color, uint16_t _bkColor, const char* _prefix, const char* _suffix)
{
  this->setup(_text.c_str(), _fontFamily, _datum, _color, _bkColor, _prefix, _suffix);
}*/

/**
 * @brief Configures the Label widget with specified float, font, alignment, and colors.
 * @param _value float value to show.
 * @param _fontFamily Pointer to the font used for the text.
 * @param _datum Text alignment setting.
 * @param _color Color of the text.
 * @param _bkColor Background color of the label.
 * @param _prefix Prefix text to display.
 * @param _suffix Suffix text to display.
 */

void Label::setup(const float _value, const GFXfont *_fontFamily, uint16_t _datum, uint16_t _color, uint16_t _bkColor, const char* _prefix, const char* _suffix)
{
  char convertido[16];
  dtostrf(_value, 2, 2, convertido);
  //sprintf(convertido, "%.*2", _value);
  this->setup(convertido, _fontFamily, _datum, _color, _bkColor, _prefix, _suffix);
}

/*void Label::setup(const int _value, const GFXfont *_fontFamily, uint16_t _datum, uint16_t _color, uint16_t _bkColor)
{
  char convertido[16];
  sprintf(convertido, "%d", _value);
  this->setup(convertido, _fontFamily, _datum, _color, _bkColor);
}*/
#endif
/**
 * @brief Configures the Label widget with parameters defined in a configuration structure.
 * @param config Structure containing the label configuration parameters.
 */
void Label::setup(const LabelConfig& config)
{
  #if defined(USING_GRAPHIC_LIB)
  setup(config.text, config.fontFamily, config.datum, config.fontColor, config.backgroundColor, config.prefix, config.suffix);
  #endif
}
