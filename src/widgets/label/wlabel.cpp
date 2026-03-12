#include "wlabel.h"

const char* Label::TAG = "Label";

Label::Label(uint16_t _x, uint16_t _y, uint8_t _screen)
  : WidgetBase(_x, _y, _screen)
{
  m_text[0] = '\0';
  m_prefix[0] = '\0';
  m_suffix[0] = '\0';
}

bool Label::detectTouch(uint16_t *, uint16_t *)
{
  return false;
}

functionCB_t Label::getCallbackFunc()
{
  return m_callback;
}

void Label::setPrefix(const char* str)
{
  if (!str) {
    m_prefix[0] = '\0';
    return;
  }

  strncpy(m_prefix, str, LABEL_MAX_PREFIX_LENGTH - 1);
  m_prefix[LABEL_MAX_PREFIX_LENGTH - 1] = '\0';
  m_shouldRedraw = true;
}

void Label::setSuffix(const char* str)
{
  if (!str) {
    m_suffix[0] = '\0';
    return;
  }

  strncpy(m_suffix, str, LABEL_MAX_SUFFIX_LENGTH - 1);
  m_suffix[LABEL_MAX_SUFFIX_LENGTH - 1] = '\0';
  m_shouldRedraw = true;
}

void Label::buildFinalText(const char* coreText)
{
  if (!coreText) return;

  m_text[0] = '\0';

  strncat(m_text, m_prefix, LABEL_MAX_TEXT_LENGTH - 1);
  strncat(m_text, coreText,
          LABEL_MAX_TEXT_LENGTH - strlen(m_text) - 1);
  strncat(m_text, m_suffix,
          LABEL_MAX_TEXT_LENGTH - strlen(m_text) - 1);

  m_shouldRedraw = true;
}

void Label::setText(const char* str)
{
  CHECK_LOADED_VOID
  if (!str) return;

  buildFinalText(str);
}

void Label::setText(const String& str)
{
  setText(str.c_str());
}

void Label::setTextFloat(float value, uint8_t decimalPlaces)
{
  setDecimalPlaces(decimalPlaces);

  char buffer[20];
  dtostrf(value, 1, m_decimalPlaces, buffer);
  buildFinalText(buffer);
}

void Label::setTextInt(int value)
{
  char buffer[16];
  snprintf(buffer, sizeof(buffer), "%d", value);
  buildFinalText(buffer);
}

void Label::redraw()
{
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  #if defined(USING_GRAPHIC_LIB)
  CHECK_CURRENTSCREEN_VOID
  CHECK_LOADED_VOID
  CHECK_SHOULDREDRAW_VOID

  WidgetBase::objTFT->setTextColor(m_config.fontColor);
  WidgetBase::objTFT->setFont(m_config.fontFamily);
  WidgetBase::objTFT->setTextSize(m_fontSize);

  printText(m_text, m_xPos, m_yPos, m_config.datum,
            m_lastArea, m_config.backgroundColor);

  WidgetBase::objTFT->setTextSize(1);
  WidgetBase::setFontNull();

  m_shouldRedraw = false;
  #endif
}

void Label::forceUpdate()
{
  m_shouldRedraw = true;
}

void Label::setDecimalPlaces(uint8_t places)
{
  m_decimalPlaces = constrain(places, 0, 5);
}

void Label::setFontSize(uint8_t newSize)
{
  m_fontSize = newSize;
}

void Label::setup(const LabelConfig& config)
{
  CHECK_TFT_VOID
  if (m_loaded) return;

  m_config = config;

  setPrefix(config.prefix);
  setSuffix(config.suffix);
  buildFinalText(config.text ? config.text : "");

  #if defined(USING_GRAPHIC_LIB)
  m_config.fontFamily = config.fontFamily;
  #endif

  m_lastArea = {0, 0, 0, 0};
  m_loaded = true;
  m_shouldRedraw = true;
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