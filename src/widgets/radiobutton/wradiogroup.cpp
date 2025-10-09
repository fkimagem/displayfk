#include "wradiogroup.h"

/**
 * @brief Constructor for the RadioGroup class.
 * @param _screen Screen identifier where the RadioGroup will be displayed.
 * 
 * Creates a RadioGroup with position (0,0) on the specified screen.
 */
RadioGroup::RadioGroup(uint8_t _screen) : WidgetBase(0, 0, _screen),
m_shouldRedraw(true)
{
}

/**
 * @brief Destructor for the RadioGroup class.
 */
RadioGroup::~RadioGroup()
{
}

/**
 * @brief Forces the RadioGroup to redraw.
 * 
 * Sets the flag to redraw the RadioGroup on the next redraw cycle.
 */
void RadioGroup::forceUpdate(){
  m_shouldRedraw = true;
}

/**
 * @brief Detects if any radio button within the group has been touched.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if a radio button was touched, otherwise false.
 * 
 * Checks if the touch event occurred within any of the radio buttons
 * and updates the selected state if a button is touched.
 */
bool RadioGroup::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
  if (WidgetBase::usingKeyboard || WidgetBase::currentScreen != screen || !loaded)
  {
    return false;
  }

  if (millis() - m_myTime < TIMEOUT_CLICK)
  {
    return false;
  }
  m_myTime = millis();
  bool detectado = false;

  for (int16_t i = 0; i < m_amount; i++)
  {
    radio_t r = m_buttons[i];
    int32_t deltaX = (*_xTouch - r.x) * (*_xTouch - r.x);
    int32_t deltaY = (*_yTouch - r.y) * (*_yTouch - r.y);
    int32_t radiusQ = m_radius * m_radius;

    if ((deltaX < radiusQ) && (deltaY < radiusQ))
    {
      m_clickedId = r.id;
      m_shouldRedraw = true;
      detectado = true;
    }
  }
  return detectado;
}

/**
 * @brief Retrieves the callback function associated with the RadioGroup.
 * @return Pointer to the callback function.
 */
functionCB_t RadioGroup::getCallbackFunc()
{
  return cb;
}

/**
 * @brief Redraws the radio buttons on the screen, updating their appearance.
 * 
 * Draws all radio buttons in the group, highlighting the currently selected one.
 * Only redraws if the RadioGroup is on the current screen and needs updating.
 */
void RadioGroup::redraw()
{
  if(!visible){return;}
  #if defined(DISP_DEFAULT)
  if (WidgetBase::currentScreen != screen || !loaded || !m_shouldRedraw)
  {
    return;
  }

  m_shouldRedraw = false;

  //uint16_t darkBg = WidgetBase::lightMode ? CFK_GREY3 : CFK_GREY11;
  uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
  uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

  log_d("Redraw radiogroup");

  for (int16_t i = 0; i < m_amount; i++)
  {
    radio_t r = m_buttons[i];

    WidgetBase::objTFT->fillCircle(r.x, r.y, m_radius, lightBg);        // Botao
    WidgetBase::objTFT->drawCircle(r.x, r.y, m_radius, baseBorder);     // Botao
    if (r.id == m_clickedId)
    {
      WidgetBase::objTFT->fillCircle(r.x, r.y, m_radius * 0.75, r.color);      // fundo dentro
      WidgetBase::objTFT->drawCircle(r.x, r.y, m_radius * 0.75, baseBorder); // borda dentro
    }
  }
  #endif
}

/**
 * @brief Sets the selected radio button by its ID.
 * @param clickedId ID of the radio button to select.
 * 
 * Updates the currently selected radio button and triggers the callback if provided.
 * No effect if the specified ID is already selected or doesn't exist in the group.
 */
void RadioGroup::setSelected(uint16_t clickedId)
{
  if(!loaded)
  {
    log_e("RadioGroup widget not loaded");
    return;
  }
  if(m_clickedId == clickedId){
    log_d("RadioGroup widget already selected this id");
    return;
  }
  for (int16_t i = 0; i < m_amount; i++)
  {
    radio_t r = (m_buttons[i]);
    if (r.id == clickedId)
    {
      m_clickedId = r.id;
      m_shouldRedraw = true;
      if(cb != nullptr){
        WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::SELF);
      }
    }
  }
}

/**
 * @brief Configures the RadioGroup widget with specific settings.
 * @param _group Group identifier for the radio buttons.
 * @param _radius Radius of each radio button.
 * @param _amount Number of radio buttons in the group.
 * @param _buttons Pointer to an array defining the radio buttons.
 * @param _defaultClickedId ID of the radio button to be selected by default.
 * @param _cb Callback function to execute on selection change.
 * 
 * Initializes the RadioGroup properties and marks it as loaded when complete.
 */
void RadioGroup::setup(uint8_t _group, uint16_t _radius, uint8_t _amount, const radio_t *_buttons, uint8_t _defaultClickedId, functionCB_t _cb)
{
  if (!WidgetBase::objTFT)
  {
    log_e("TFT not defined on WidgetBase");
    return;
  }
  if (loaded)
  {
    log_d("RadioGroup widget already configured");
    return;
  }

  m_group = _group;
  m_radius = _radius;
  m_amount = _amount;
  cb = _cb;
  m_buttons = _buttons;
  m_clickedId = _defaultClickedId;

  /*if(buttons){
    delete buttons;
  }
  buttons = new radio_t[amount];
  for(auto i = 0; i < _amount; ++i){
    buttons[i].x = _buttons[i].x;
    buttons[i].y = _buttons[i].y;
    buttons[i].id = _buttons[i].id;
    buttons[i].cor = _buttons[i].cor;
  }*/

  loaded = true;
}

/**
 * @brief Configures the RadioGroup with parameters defined in a configuration structure.
 * @param config Structure containing the radio group configuration parameters.
 */
void RadioGroup::setup(const RadioGroupConfig& config)
{
  setup(config.group, config.radius, config.amount, config.buttons, config.defaultClickedId, config.callback);
}

/**
 * @brief Retrieves the ID of the currently selected radio button.
 * @return ID of the selected radio button.
 */
uint16_t RadioGroup::getSelected()
{
  return m_clickedId;
}

/**
 * @brief Retrieves the group ID of the RadioGroup.
 * @return Group identifier for the RadioGroup.
 */
uint16_t RadioGroup::getGroupId()
{
  return m_group;
}

void RadioGroup::show()
{
    visible = true;
    m_shouldRedraw = true;
}

void RadioGroup::hide()
{
    visible = false;
    m_shouldRedraw = true;
}