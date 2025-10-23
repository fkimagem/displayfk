#include "wcircularbar.h"
#include <esp_log.h>

const char* CircularBar::TAG = "CircularBar";

/**
 * @brief Constructor for the CircularBar class.
 * @param _x X position of the circular bar.
 * @param _y Y position of the circular bar.
 * @param _screen Screen number.
 */
CircularBar::CircularBar(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen), m_lastValue(0), m_value(0)
{
    m_config = {.radius = 0, .minValue = 0, .maxValue = 100, .startAngle = 0, .endAngle = 360, 
                .thickness = 10, .color = 0, .backgroundColor = 0, .textColor = 0, 
                .backgroundText = 0, .showValue = true, .inverted = false};
    ESP_LOGD(TAG, "CircularBar created at (%d, %d) on screen %d", _x, _y, _screen);
}

/**
 * @brief Destructor for the CircularBar class.
 */
CircularBar::~CircularBar() {
    ESP_LOGD(TAG, "CircularBar destroyed at (%d, %d)", m_xPos, m_yPos);
    if (m_callback != nullptr) { m_callback = nullptr; }
}

/**
 * @brief Detects a touch on the circular bar.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is detected, false otherwise.
 */
bool CircularBar::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  // CircularBar doesn't handle touch events
  UNUSED(_xTouch);
  UNUSED(_yTouch);
  return false;
}

/**
 * @brief Gets the callback function for the circular bar.
 * @return Pointer to the callback function.
 */
functionCB_t CircularBar::getCallbackFunc() { return m_callback; }

/**
 * @brief Draws the background of the CircularBar widget.
 *
 * Renders the background arc of the circular bar and initializes the display.
 * Only draws if the widget is on the current screen and properly loaded.
 */
void CircularBar::drawBackground() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_INITIALIZED_VOID
  CHECK_LOADED_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_CURRENTSCREEN_VOID


#if defined(DISP_DEFAULT)
uint8_t borderOffset = 1;
  WidgetBase::objTFT->fillArc(m_xPos, m_yPos, m_config.radius + borderOffset,
                              (m_config.radius - m_config.thickness - borderOffset),
                              m_config.startAngle, m_config.endAngle, m_config.backgroundColor);
#endif
  m_lastValue = m_config.minValue;
  redraw();
}

/**
 * @brief Sets the current value of the CircularBar widget.
 * @param newValue New value to display on the circular bar.
 *
 * Updates the current value and marks the widget for redraw.
 */
void CircularBar::setValue(int newValue) {
  CHECK_LOADED_VOID
  
  m_lastValue = m_value;
  m_value = newValue;
  m_shouldRedraw = true;
  
  ESP_LOGD(TAG, "CircularBar value set to: %d", newValue);
}

/**
 * @brief Redraws the CircularBar widget on the screen, updating its appearance
 * based on the current value.
 *
 * Displays the circular bar with the current value and updates the visual
 * representation. Only redraws if the widget is on the current screen and needs
 * updating.
 */
void CircularBar::redraw() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_INITIALIZED_VOID
  CHECK_LOADED_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_SHOULDREDRAW_VOID

#if defined(DISP_DEFAULT)

  CHECK_DEBOUNCE_REDRAW_VOID

  m_shouldRedraw = false;

  int angleValue = map(m_value, m_config.minValue, m_config.maxValue, m_config.startAngle, m_config.endAngle);
  int lastAngleValue =
      map(m_lastValue, m_config.minValue, m_config.maxValue, m_config.startAngle, m_config.endAngle);

      /*
  int xEnd = 0, xStart = 0, xCursor = 0, xLastCursor = 0;
  int yEnd = 0, yStart = 0, yCursor = 0, yLastCursor = 0;

  xStart =
      (cos(m_startAngle * DEG_TO_RAD) * (m_radius - m_lineWeight / 2)) + xPos;
  yStart =
      (sin(m_startAngle * DEG_TO_RAD) * (m_radius - m_lineWeight / 2)) + yPos;

  xEnd = (cos(m_endAngle * DEG_TO_RAD) * (m_radius - m_lineWeight / 2)) + xPos;
  yEnd = (sin(m_endAngle * DEG_TO_RAD) * (m_radius - m_lineWeight / 2)) + yPos;

  xCursor =
      (cos(angleValue * DEG_TO_RAD) * (m_radius - m_lineWeight / 2)) + xPos;
  yCursor =
      (sin(angleValue * DEG_TO_RAD) * (m_radius - m_lineWeight / 2)) + yPos;

  xLastCursor =
      (cos(lastAngleValue * DEG_TO_RAD) * (m_radius - m_lineWeight / 2)) + xPos;
  yLastCursor =
      (sin(lastAngleValue * DEG_TO_RAD) * (m_radius - m_lineWeight / 2)) + yPos;
*/
  uint16_t lastCursoColor = 0;

  // If the angle is 'going back' (decreasing)
  if (angleValue < lastAngleValue) {
    WidgetBase::objTFT->fillArc(m_xPos, m_yPos, m_config.radius, (m_config.radius - m_config.thickness),
                                angleValue, lastAngleValue,
                                m_config.backgroundColor); // Paint the difference
    lastCursoColor = m_config.backgroundColor;
  } else if (angleValue > lastAngleValue) {
    WidgetBase::objTFT->fillArc(m_xPos, m_yPos, m_config.radius, (m_config.radius - m_config.thickness),
                                lastAngleValue, angleValue,
                                m_config.color); // Paint the difference
    lastCursoColor = m_config.color;
  }else{
    UNUSED(lastCursoColor);
  }

  if (m_config.thickness >= 10) {
    // Optional: Add circle endpoints for thick bars
  }

  if (m_config.showValue) {
    WidgetBase::objTFT->fillCircle(m_xPos, m_yPos, (m_config.radius - m_config.thickness) - 5,
                                   m_config.backgroundText);

    char char_arr[100];
    sprintf(char_arr, "%d", m_value);

    WidgetBase::objTFT->setTextColor(m_config.textColor);
    WidgetBase::objTFT->setFont(&RobotoBold10pt7b);

    printText(char_arr, m_xPos, m_yPos, MC_DATUM);

    updateFont(FontType::UNLOAD);
  }
#endif
}

/**
 * @brief Forces an immediate update of the CircularBar widget.
 *
 * Sets the flag to redraw the circular bar on the next redraw cycle.
 */
void CircularBar::forceUpdate() { 
  m_shouldRedraw = true; 
  ESP_LOGD(TAG, "CircularBar force update requested");
}


/**
 * @brief Configures the CircularBar with parameters defined in a configuration
 * structure.
 * @param config Structure containing the CircularBar configuration parameters.
 */
void CircularBar::setup(const CircularBarConfig &config) {
  // Validate TFT object
  CHECK_TFT_VOID
  
  m_config = config;
  
  // Validate and swap min/max if needed
  if (m_config.maxValue < m_config.minValue) {
    int aux = m_config.minValue;
    m_config.minValue = m_config.maxValue;
    m_config.maxValue = aux;
  }

  m_value = m_config.minValue;
  m_lastValue = m_config.minValue;

  // Disable value display if radius is too small
  if ((m_config.radius - m_config.thickness) < 20 || !m_config.showValue) {
    m_config.showValue = false;
  }

  m_shouldRedraw = true;
  m_loaded = true;
  m_initialized = true;
  ESP_LOGD(TAG, "CircularBar setup completed at (%d, %d) with radius %d", 
                m_xPos, m_yPos, m_config.radius);
}

void CircularBar::show() {
  m_visible = true;
  m_shouldRedraw = true;
  ESP_LOGD(TAG, "CircularBar shown at (%d, %d)", m_xPos, m_yPos);
}

void CircularBar::hide() {
  m_visible = false;
  m_shouldRedraw = true;
  ESP_LOGD(TAG, "CircularBar hidden at (%d, %d)", m_xPos, m_yPos);
}