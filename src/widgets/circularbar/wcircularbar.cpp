#include "wcircularbar.h"

#define DEBUG_CIRCULARBAR

#if defined(DEBUG_CIRCULARBAR)
#define DEBUG_D(format, ...) log_d(format, ##__VA_ARGS__)
#define DEBUG_E(format, ...) log_e(format, ##__VA_ARGS__)
#define DEBUG_W(format, ...) log_w(format, ##__VA_ARGS__)
#else
#define DEBUG_D(format, ...)
#define DEBUG_E(format, ...)
#define DEBUG_W(format, ...)
#endif

/**
 * @brief Constructor for the CircularBar class.
 * @param _x X position of the circular bar.
 * @param _y Y position of the circular bar.
 * @param _screen Screen number.
 */
CircularBar::CircularBar(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen) {}

/**
 * @brief Destructor for the CircularBar class.
 */
CircularBar::~CircularBar() {}

/**
 * @brief Detects a touch on the circular bar.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is detected, false otherwise.
 */
bool CircularBar::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  return false;
}

/**
 * @brief Gets the callback function for the circular bar.
 * @return Pointer to the callback function.
 */
functionCB_t CircularBar::getCallbackFunc() { return cb; }

/**
 * @brief Draws the background of the CircularBar widget.
 *
 * Renders the background arc of the circular bar and initializes the display.
 * Only draws if the widget is on the current screen and properly loaded.
 */
void CircularBar::drawBackground() {
  CHECK_TFT_VOID

  if (!visible) {
    return;
  }
#if defined(DISP_DEFAULT)
  if (WidgetBase::currentScreen != screen ||
      WidgetBase::usingKeyboard == true || !loaded) {
    return;
  }

  uint8_t borderOffset = 1;
#if defined(DISP_DEFAULT)
  WidgetBase::objTFT->fillArc(xPos, yPos, m_radius + borderOffset,
                              (m_radius - m_lineWeight - borderOffset),
                              m_startAngle, m_endAngle, m_bkColor);
#endif
  m_lastValue = m_vmin;
  redraw();
#endif
}

/**
 * @brief Sets the current value of the CircularBar widget.
 * @param newValue New value to display on the circular bar.
 *
 * Updates the current value and marks the widget for redraw.
 */
void CircularBar::setValue(int newValue) {
  m_lastValue = m_value;
  m_value = newValue;
  m_shouldRedraw = true;
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
  if (!visible) {
    return;
  }
#if defined(DISP_DEFAULT)
  if (WidgetBase::currentScreen != screen ||
      WidgetBase::usingKeyboard == true || !m_shouldRedraw || !loaded) {
    return;
  }

  if (millis() - m_myTime < 50) {
    return;
  }

  m_shouldRedraw = false;

  int angleValue = map(m_value, m_vmin, m_vmax, m_startAngle, m_endAngle);
  int lastAngleValue =
      map(m_lastValue, m_vmin, m_vmax, m_startAngle, m_endAngle);

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
    DEBUG_D("Decreasing -> x: %d, y: %d, r1: %d, r2: %d, start: %d, end: %d",
            xPos, yPos, m_radius, (m_radius - m_lineWeight), angleValue,
            lastAngleValue);
    // WidgetBase::objTFT->fillArc(xPos, yPos, m_radius, (m_radius -
    // m_lineWeight), angleValue, lastAngleValue, m_middleColor);// Paint the
    // difference
    WidgetBase::objTFT->fillArc(xPos, yPos, m_radius, (m_radius - m_lineWeight),
                                angleValue, lastAngleValue,
                                m_bkColor); // Paint the difference
    // lastCursoColor = m_middleColor;
    lastCursoColor = m_bkColor;
  } else if (angleValue > lastAngleValue) {
    DEBUG_D("Increasing -> x: %d, y: %d, r1: %d, r2: %d, start: %d, end: %d",
            xPos, yPos, m_radius, (m_radius - m_lineWeight), lastAngleValue,
            angleValue);
    WidgetBase::objTFT->fillArc(xPos, yPos, m_radius, (m_radius - m_lineWeight),
                                lastAngleValue, angleValue,
                                m_lineColor); // Paint the difference
    lastCursoColor = m_lineColor;
  }else{
    UNUSED(lastCursoColor);
  }

  if (m_lineWeight >= 10) {
    // WidgetBase::objTFT->fillCircle(xEnd, yEnd, m_lineWeight / 2 - 1,
    // !m_invertedFill ? m_middleColor : m_lineColor);
    //  WidgetBase::objTFT->fillCircle(xEnd, yEnd, m_lineWeight / 2 - 1,
    //  m_lineColor);
    // WidgetBase::objTFT->fillCircle(xLastCursor, yLastCursor, m_lineWeight / 2
    // - 1, lastCursoColor); WidgetBase::objTFT->fillCircle(xLastCursor,
    // yLastCursor, m_lineWeight / 2 - 1, lastCursoColor);

    // WidgetBase::objTFT->fillCircle(xStart, yStart, m_lineWeight / 2 - 1,
    // m_lineColor);
    // WidgetBase::objTFT->fillCircle(xCursor, yCursor, m_lineWeight / 2 - 1,
    // !m_invertedFill ? m_lineColor : m_middleColor);
  }

  if (m_showValue) {
    WidgetBase::objTFT->fillCircle(xPos, yPos, (m_radius - m_lineWeight) - 5,
                                   m_backgroundText);

    char char_arr[100];
    sprintf(char_arr, "%d", m_value);

    // String str = String(_value);
    WidgetBase::objTFT->setTextColor(m_textColor);
    WidgetBase::objTFT->setFont(&RobotoBold10pt7b);
    // updateFont(FontType::BOLD);

    printText(char_arr, xPos, yPos, MC_DATUM);

    updateFont(FontType::UNLOAD);
  }
#endif
}

/**
 * @brief Forces an immediate update of the CircularBar widget.
 *
 * Sets the flag to redraw the circular bar on the next redraw cycle.
 */
void CircularBar::forceUpdate() { m_shouldRedraw = true; }

/**
 * @brief Configures the CircularBar widget with specific parameters.
 * @param radius Radius of the circular bar.
 * @param vmin Minimum value of the circular bar range.
 * @param vmax Maximum value of the circular bar range.
 * @param startAngle Starting angle of the circular bar in degrees.
 * @param endAngle Ending angle of the circular bar in degrees.
 * @param weight Thickness of the circular bar line.
 * @param color Color of the circular bar.
 * @param bkColor Background color of the circular bar.
 * @param textColor Color of the text displaying the value.
 * @param backgroundText Background color of the text area.
 * @param showLabel True if the value text should be displayed, false otherwise.
 * @param inverted True if the fill direction is inverted, false otherwise.
 *
 * Initializes the circular bar properties and marks it as loaded when complete.
 */
void CircularBar::setup(uint16_t radius, int vmin, int vmax,
                        uint16_t startAngle, uint16_t endAngle, uint8_t weight,
                        uint16_t color, uint16_t bkColor, uint16_t textColor,
                        uint16_t backgroundText, bool showLabel,
                        bool inverted) {
  m_radius = radius;
  m_lineWeight = weight;
  m_lineColor = color;
  m_bkColor = bkColor;
  m_textColor = textColor;
  m_backgroundText = backgroundText;

  // m_middleColor = WidgetBase::blendColors(m_bkColor, m_lineColor, 0.25);

  m_startAngle = startAngle;
  m_endAngle = endAngle;

  // m_invertedFill = inverted;

  m_vmin = vmin;
  m_vmax = vmax;

  if (m_vmax < m_vmin) {
    int aux = m_vmin;
    m_vmin = m_vmax;
    m_vmax = aux;
  }

  // If it's counter-clockwise fill, invert the angles
  /*if(m_invertedFill){
      int aux = m_startAngle;
      m_startAngle = m_endAngle;
      m_endAngle = aux;

      uint16_t auxColor = m_lineColor;
      //m_lineColor = m_middleColor;
      //m_middleColor = auxColor;
  }*/

  m_value = m_vmin;
  m_lastValue = m_vmin;

  if ((m_radius - m_lineWeight) < 20 || !m_showValue) {
    m_showValue = false;
  }

  m_shouldRedraw = true;
  loaded = true;
}

/**
 * @brief Configures the CircularBar with parameters defined in a configuration
 * structure.
 * @param config Structure containing the CircularBar configuration parameters.
 */
void CircularBar::setup(const CircularBarConfig &config) {
  setup(config.radius, config.minValue, config.maxValue, config.startAngle,
        config.endAngle, config.thickness, config.color, config.backgroundColor,
        config.textColor, config.backgroundText, config.showValue,
        config.inverted);
}

void CircularBar::show() {
  visible = true;
  m_shouldRedraw = true;
}

void CircularBar::hide() {
  visible = false;
  m_shouldRedraw = true;
}