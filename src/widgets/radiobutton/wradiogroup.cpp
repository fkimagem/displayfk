#include "wradiogroup.h"

const char* RadioGroup::TAG = "RadioGroup";

/**
 * @brief Constructor for the RadioGroup class.
 * @param _screen Screen identifier where the RadioGroup will be displayed.
 *
 * Creates a RadioGroup with position (0,0) on the specified screen.
 */
RadioGroup::RadioGroup(uint8_t _screen)
    : WidgetBase(0, 0, _screen), m_shouldRedraw(true), m_config{} {
      m_config = {
        .group = 0,
        .radius = 0,
        .amount = 0,
        .buttons = nullptr,
        .defaultClickedId = 0,
        .callback = nullptr
      };
    }

/**
 * @brief Destructor for the RadioGroup class.
 */
RadioGroup::~RadioGroup() {
    cleanupMemory();
}

void RadioGroup::cleanupMemory() {
  if (m_config.buttons) {
    delete[] m_config.buttons;
    m_config.buttons = nullptr;
  }
  if (m_buttons) {
    delete[] m_buttons;
    m_buttons = nullptr;
  }
  ESP_LOGD(TAG, "RadioGroup memory cleanup completed");
}

/**
 * @brief Forces the RadioGroup to redraw.
 *
 * Sets the flag to redraw the RadioGroup on the next redraw cycle.
 */
void RadioGroup::forceUpdate() { m_shouldRedraw = true; }

/**
 * @brief Detects if any radio button within the group has been touched.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if a radio button was touched, otherwise false.
 *
 * Checks if the touch event occurred within any of the radio buttons
 * and updates the selected state if a button is touched.
 */
bool RadioGroup::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  CHECK_VISIBLE_BOOL
  CHECK_USINGKEYBOARD_BOOL
  CHECK_CURRENTSCREEN_BOOL
  CHECK_LOADED_BOOL
  CHECK_DEBOUNCE_CLICK_BOOL

  for (int16_t i = 0; i < m_config.amount; i++) {
    radio_t r = m_buttons[i];
    bool inBounds = POINT_IN_RECT(*_xTouch, *_yTouch, r.x, r.y, m_config.radius, m_config.radius);
    if(inBounds) {
      m_clickedId = r.id;
      m_shouldRedraw = true;
      m_myTime = millis();
      return true;
    } 

  }
  return false;
}

/**
 * @brief Retrieves the callback function associated with the RadioGroup.
 * @return Pointer to the callback function.
 */
functionCB_t RadioGroup::getCallbackFunc() { return m_callback; }

/**
 * @brief Redraws the radio buttons on the screen, updating their appearance.
 *
 * Draws all radio buttons in the group, highlighting the currently selected
 * one. Only redraws if the RadioGroup is on the current screen and needs
 * updating.
 */
void RadioGroup::redraw() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_LOADED_VOID
  CHECK_SHOULDREDRAW_VOID

  m_shouldRedraw = false;

  // uint16_t darkBg = WidgetBase::lightMode ? CFK_GREY3 : CFK_GREY11;
  uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
  uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

  ESP_LOGD(TAG, "Redraw radiogroup");

  for (int16_t i = 0; i < m_config.amount; i++) {
    radio_t r = m_buttons[i];

    WidgetBase::objTFT->fillCircle(r.x, r.y, m_config.radius, lightBg);    // Botao
    WidgetBase::objTFT->drawCircle(r.x, r.y, m_config.radius, baseBorder); // Botao
    if (r.id == m_clickedId) {
      WidgetBase::objTFT->fillCircle(r.x, r.y, m_config.radius * 0.75,
                                     r.color); // fundo dentro
      WidgetBase::objTFT->drawCircle(r.x, r.y, m_config.radius * 0.75,
                                     baseBorder); // borda dentro
    }
  }
}

/**
 * @brief Sets the selected radio button by its ID.
 * @param clickedId ID of the radio button to select.
 *
 * Updates the currently selected radio button and triggers the callback if
 * provided. No effect if the specified ID is already selected or doesn't exist
 * in the group.
 */
void RadioGroup::setSelected(uint16_t clickedId) {
  if (!m_loaded) {
    ESP_LOGE(TAG, "RadioGroup widget not loaded");
    return;
  }
  if (m_clickedId == clickedId) {
    ESP_LOGD(TAG, "RadioGroup widget already selected this id");
    return;
  }
  for (int16_t i = 0; i < m_config.amount; i++) {
    radio_t r = (m_buttons[i]);
    if (r.id == clickedId) {
      m_clickedId = r.id;
      m_shouldRedraw = true;
      if (m_callback != nullptr) {
        WidgetBase::addCallback(m_callback, WidgetBase::CallbackOrigin::SELF);
      }
    }
  }
}


/**
 * @brief Configures the RadioGroup with parameters defined in a configuration
 * structure.
 * @param config Structure containing the radio group configuration parameters.
 */
void RadioGroup::setup(const RadioGroupConfig &config) {
  CHECK_TFT_VOID
  if (m_loaded) {
    ESP_LOGW(TAG, "RadioGroup already initialized");
    return;
  }

  // Validate config parameters
  if (config.amount == 0) {
    ESP_LOGE(TAG, "Invalid config: amount cannot be 0");
    return;
  }

  // Clean up any existing memory before setting new config
  cleanupMemory();
  
  // Copy non-pointer members first
  m_config.group = config.group;
  m_config.radius = config.radius;
  m_config.amount = config.amount;
  m_config.defaultClickedId = config.defaultClickedId;
  m_config.callback = config.callback;
  
  // Deep copy buttons array
  if (config.buttons && config.amount > 0) {
    m_buttons = new radio_t[config.amount];
    if (m_buttons == nullptr) {
      ESP_LOGE(TAG, "Failed to allocate memory for buttons array");
      return;
    }
    
    // Copy each button
    for (int i = 0; i < config.amount; i++) {
      m_buttons[i] = config.buttons[i];
    }
    
    // Update config to point to our deep copy
    m_config.buttons = m_buttons;
  } else {
    m_buttons = nullptr;
    m_config.buttons = nullptr;
  }
  
  m_clickedId = config.defaultClickedId;
  m_callback = config.callback;
  m_loaded = true;
  
  ESP_LOGD(TAG, "RadioGroup configured: group=%d, radius=%d, amount=%d", 
           m_config.group, m_config.radius, m_config.amount);
}

/**
 * @brief Retrieves the ID of the currently selected radio button.
 * @return ID of the selected radio button.
 */
uint16_t RadioGroup::getSelected() { return m_clickedId; }

/**
 * @brief Retrieves the group ID of the RadioGroup.
 * @return Group identifier for the RadioGroup.
 */
uint16_t RadioGroup::getGroupId() { return m_config.group; }

void RadioGroup::show() {
  m_visible = true;
  m_shouldRedraw = true;
}

void RadioGroup::hide() {
  m_visible = false;
  m_shouldRedraw = true;
}