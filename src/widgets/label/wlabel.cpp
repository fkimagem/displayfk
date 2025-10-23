#include "wlabel.h"
#include "wlabel.h"

const char* Label::TAG = "Label";

/**
 * @brief Constructor for the Label class.
 * @param _x X-coordinate for the Label position.
 * @param _y Y-coordinate for the Label position.
 * @param _screen Screen identifier where the Label will be displayed.
 */
Label::Label(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen),
m_text(nullptr),
m_previousText(nullptr),
m_prefix(nullptr),
m_suffix(nullptr),
m_shouldRedraw(false),
m_lastArea{0, 0, 0, 0},
m_fontSize(1),
m_decimalPlaces(1)
{
  m_config = {.text = nullptr, .fontFamily = nullptr, .datum = 0, .fontColor = 0, .backgroundColor = 0, .prefix = nullptr, .suffix = nullptr};
  ESP_LOGD(TAG, "Label created at (%d, %d) on screen %d", _x, _y, _screen);
}

/**
 * @brief Cleans up allocated memory before new assignment.
 * 
 * Frees all dynamically allocated memory for text, prefix, and suffix.
 */
void Label::cleanupMemory()
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
  
  // Libera memória do prefixo se existir
  if (m_prefix != nullptr) {
    delete[] m_prefix;
    m_prefix = nullptr;
  }
  
  // Libera memória do sufixo se existir
  if (m_suffix != nullptr) {
    delete[] m_suffix;
    m_suffix = nullptr;
  }
}

/**
 * @brief Destructor for the Label class.
 * 
 * Frees allocated memory for text, previous text, and clears font pointer.
 */
Label::~Label()
{
  // Clean up all allocated memory
  cleanupMemory();
  
  #if defined(USING_GRAPHIC_LIB)
  // A fonte não é liberada aqui porque é apenas uma referência
  m_config.fontFamily = nullptr;
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
    return m_callback;
}

void Label::setPrefix(const char* str){ 
  if(m_prefix){
    delete[] m_prefix;
    m_prefix = nullptr;
  }

  if (str != nullptr) {
    // Aloca nova memória para prefixo
    m_prefix = new char[strlen(str) + 1];
    if (m_prefix) {
        strcpy(m_prefix, str);
    } else {
        ESP_LOGE(TAG, "Failed to allocate memory for prefix");
    }
  }
}

void Label::setSuffix(const char* str){
  if(m_suffix){
    delete[] m_suffix;
    m_suffix = nullptr;
  }

  if (str != nullptr) {
    // Aloca nova memória para sufixo
    m_suffix = new char[strlen(str) + 1];
    if (m_suffix) {
        strcpy(m_suffix, str);
    } else {
        ESP_LOGE(TAG, "Failed to allocate memory for suffix");
    }
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
  CHECK_LOADED_VOID
  if(str == nullptr){
    return;
  }

  uint16_t prefixLength = m_prefix ? strlen(m_prefix) : 0;
  uint16_t suffixLength = m_suffix ? strlen(m_suffix) : 0;
  uint16_t textLength = strlen(str);

  // Libera a memória anterior se existir
  if(m_text){
    delete[] m_text;
    m_text = nullptr;
  }
  
  // Aloca nova memória
  m_text = new char[textLength + prefixLength + suffixLength + 1];
  if(m_text){
    // Safely copy prefix, text, and suffix
    if (m_prefix) {
      strcpy(m_text, m_prefix);
    } else {
      m_text[0] = '\0';
    }
    strcat(m_text, str);
    if (m_suffix) {
      strcat(m_text, m_suffix);
    }
    m_shouldRedraw = true;
  } else {
    ESP_LOGE(TAG, "Failed to allocate memory for label text");
    // Clean up prefix and suffix if text allocation failed
    if (m_prefix) {
      delete[] m_prefix;
      m_prefix = nullptr;
    }
    if (m_suffix) {
      delete[] m_suffix;
      m_suffix = nullptr;
    }
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
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  #if defined(USING_GRAPHIC_LIB)
  CHECK_CURRENTSCREEN_VOID
  CHECK_LOADED_VOID
  CHECK_SHOULDREDRAW_VOID

  ESP_LOGD(TAG, "Redraw label to value %s", m_text);
  WidgetBase::objTFT->setTextColor(m_config.fontColor);
  WidgetBase::objTFT->setFont(m_config.fontFamily);
  WidgetBase::objTFT->setTextSize(m_fontSize);
  printText(m_text, m_xPos, m_yPos, m_config.datum, m_lastArea, m_config.backgroundColor);
  WidgetBase::objTFT->setTextSize(1);
  WidgetBase::objTFT->setFont((GFXfont *)0);
  //WidgetBase::objTFT->setCursor(xPos, yPos);
  //WidgetBase::objTFT->print(text);

  
  //previousText = text;
  m_shouldRedraw = false;
  #endif
}

/**
 * @brief Forces the label to update, refreshing its visual state on next redraw.
 */
void Label::forceUpdate()
{
  m_shouldRedraw = true;
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
    ESP_LOGD(TAG, "Decimal places set to %d", m_decimalPlaces);
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
 * @brief Configures the Label widget with parameters defined in a configuration structure.
 * @param config Structure containing the label configuration parameters.
 */
void Label::setup(const LabelConfig& config)
{
  CHECK_TFT_VOID
  if (m_loaded) {
    ESP_LOGD(TAG, "Label widget already configured");
    return;
  }

  // Clean up existing memory first
  cleanupMemory();

  // Copy config to member
  m_config = config;

  // Deep copy for text, prefix, and suffix
  const char* text = config.text ? config.text : "";
  const char* prefix = config.prefix ? config.prefix : "";
  const char* suffix = config.suffix ? config.suffix : "";

  uint16_t prefixLength = strlen(prefix);
  uint16_t suffixLength = strlen(suffix);
  uint16_t textLength = strlen(text);

  // Allocate memory for prefix
  m_prefix = new char[prefixLength + 1];
  if (!m_prefix) {
    ESP_LOGE(TAG, "Failed to allocate memory for prefix");
    return;
  }
  strcpy(m_prefix, prefix);

  // Allocate memory for suffix
  m_suffix = new char[suffixLength + 1];
  if (!m_suffix) {
    ESP_LOGE(TAG, "Failed to allocate memory for suffix");
    delete[] m_prefix;
    m_prefix = nullptr;
    return;
  }
  strcpy(m_suffix, suffix);

  // Allocate memory for combined text
  m_text = new char[textLength + prefixLength + suffixLength + 1];
  if(!m_text) {
    ESP_LOGE(TAG, "Failed to allocate memory for label text");
    delete[] m_prefix;
    delete[] m_suffix;
    m_prefix = nullptr;
    m_suffix = nullptr;
    ESP_LOGE(TAG, "Failed to allocate memory for label text");
    return;
  }
  // Safely copy prefix, text, and suffix
  strcpy(m_text, prefix);
  strcat(m_text, text);
  strcat(m_text, suffix);
    
    #if defined(USING_GRAPHIC_LIB)
    m_config.fontFamily = config.fontFamily;
    #endif
    m_config.datum = config.datum;
    m_config.fontColor = config.fontColor;
    m_config.backgroundColor = config.backgroundColor;
    
    m_lastArea.x = 0;
    m_lastArea.y = 0;
    m_lastArea.width = 0;
    m_lastArea.height = 0;
    m_shouldRedraw = true;
    m_loaded = true;
}

void Label::show()
{
    m_visible = true;
    m_shouldRedraw = true;
}

void Label::hide()
{
    m_visible = false;
    m_shouldRedraw = true;
}