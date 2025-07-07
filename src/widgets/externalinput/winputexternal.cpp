#include "winputexternal.h"

InputExternal::InputExternal(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen)
{
    m_width = 100;
    m_height = 30;
    m_shouldRedraw = true;
    m_loaded = false;
}

InputExternal::~InputExternal()
{
}

bool InputExternal::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
    if (WidgetBase::currentScreen != screen || !loaded)
  {
    return false;
  }

  if (millis() - m_myTime < TIMEOUT_CLICK)
  {
    return false;
  }
  m_myTime = millis();

  // bool detectado = false;
  uint16_t xMax = xPos + m_width;
  uint16_t yMax = yPos + m_height;

  if ((*_xTouch > xPos) && (*_xTouch < xMax) && (*_yTouch > yPos) && (*_yTouch < yMax) && WidgetBase::usingKeyboard == false)
  {
    //WidgetBase::usingKeyboard = true;
    log_d("Open keyboard");

    return true;
  }

  return false;
}

functionCB_t InputExternal::getCallbackFunc()
{
    return cb;
}

void InputExternal::redraw()
{
    if (WidgetBase::currentScreen != screen || !loaded || !m_shouldRedraw)
  {
    return;
  }

  m_shouldRedraw = false;

  if (m_font)
  {
    WidgetBase::objTFT->setFont(m_font);
  }
  else
  {
    updateFont(FontType::NORMAL);
  }

  log_d("Redraw textbox with value %s", m_value.getString());

  TextBound_t area;
  WidgetBase::objTFT->getTextBounds("M", xPos, yPos, &area.x, &area.y, &area.width, &area.height);

  WidgetBase::objTFT->fillRect(xPos, yPos, m_width, m_height, m_backgroundColor);
  WidgetBase::objTFT->drawRect(xPos, yPos, m_width, m_height, m_letterColor);
  WidgetBase::objTFT->setTextColor(m_letterColor);

  uint16_t qtdLetrasMax = m_width / area.width;
  const char *conteudo = m_value.getLastChars(qtdLetrasMax);

  log_d("Draw %d letters from %s in space %d", qtdLetrasMax, conteudo, m_width);

  printText(conteudo, xPos + m_padding, yPos + m_height / 2, ML_DATUM);

  updateFont(FontType::UNLOAD);
}

void InputExternal::forceUpdate()
{
    m_shouldRedraw = true;
}

void InputExternal::setup(const InputExternalConfig& config)
{
    if(m_loaded){
        log_d("InputExternal already loaded");
        return;
    }
    m_width = config.width;
    m_height = config.height;
    m_letterColor = config.letterColor;
    m_backgroundColor = config.backgroundColor;
    m_value = config.startValue;
    m_font = config.font;
    m_funcPtr = config.funcPtr;
    cb = config.cb;
    m_loaded = true;
}

void InputExternal::setValue(const char* str)
{
    m_value = str;
}

const char* InputExternal::getValueCChar()
{
    return m_value.getLastChars(m_width);
}

int InputExternal::getValueInt()
{
    return m_value.toInt();
}

float InputExternal::getValueFloat()
{
    return m_value.toFloat();
}


