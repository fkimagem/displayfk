#include "wgaugesuper.h"

//#define DEBUG_GAUGE

#if defined(DEBUG_GAUGE)
#define DEBUG_D(format, ...) log_d(format, ##__VA_ARGS__)
#define DEBUG_E(format, ...) log_e(format, ##__VA_ARGS__)
#define DEBUG_W(format, ...) log_w(format, ##__VA_ARGS__)
#else
#define DEBUG_D(format, ...) 
#define DEBUG_E(format, ...) 
#define DEBUG_W(format, ...) 
#endif

/**
 * @brief Constructor for the GaugeSuper class.
 * @param _x X position of the gauge.
 * @param _y Y position of the gauge.
 * @param _screen Screen number.
 */
GaugeSuper::GaugeSuper(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen)
{
  m_textColor = CFK_BLACK;
  m_bkColor = CFK_WHITE;
  m_titleColor = CFK_NAVY;
}

/**
 * @brief Destructor for the GaugeSuper class.
 */
GaugeSuper::~GaugeSuper()
{
  delete[] m_intervals;
  m_intervals = nullptr;

  delete[] m_colors;
  m_colors = nullptr;

  delete[] m_title;
  m_title = nullptr;
}

/**
 * @brief Detects a touch on the gauge.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is detected, false otherwise.
 */
bool GaugeSuper::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
  UNUSED(_xTouch);
  UNUSED(_yTouch);
  return false;
}

/**
 * @brief Retrieves the callback function for the gauge.
 * @return Pointer to the callback function.
 */
functionCB_t GaugeSuper::getCallbackFunc()
{
  return cb;
}

/**
 * @brief Initializes the GaugeSuper widget with default calculations and constraints.
 * 
 * Calculates gauge dimensions, text bounds, and sets up initial values.
 * Constrains the maximum angle between 22 and 80 degrees.
 */
void GaugeSuper::start()
{
  if (m_vmax < m_vmin)
  {
    std::swap(m_vmin, m_vmax);
    /*int aux = m_vmin;
    m_vmin = m_vmax;
    m_vmax = aux;*/
  }

  

  // Configure the font
  // m_usedFont = &RobotoBold10pt7b;
  WidgetBase::objTFT->setFont(m_usedFont);

  // Calculate text dimensions for values
  char strMin[12], strMax[12];// Maximum of 12 characters for values
  sprintf(strMin, "%d", m_vmin * 10);// Multiply by 10 to have one more digit and serve as text offset
  sprintf(strMax, "%d", m_vmax * 10);// Multiply by 10 to have one more digit and serve as text offset
  TextBound_t dimensionMin = getTextBounds(strMin, xPos, yPos);// Get text dimensions
  TextBound_t dimensionMax = getTextBounds(strMax, xPos, yPos);// Get text dimensions
  m_textBoundForValue = dimensionMax.width > dimensionMin.width ? dimensionMax : dimensionMin;// Get the larger value between the two

  // Configure the title
  m_showTitle = (m_title != nullptr && strlen(m_title) > 0);// Check if title exists and has characters
  
  m_value = m_vmin;// Define initial value
  m_lastValue = m_vmax;// Define initial value

  
  //TextBound_t t;
  //t.height = 20;
  //t.width = m_showLabels ? 50 : 10;

  // https://pt.wikipedia.org/wiki/Segmento_circular

  if(!m_showLabels){
    m_textBoundForValue.width = 5;
    m_textBoundForValue.height = 5;
  }
  

  // Calculate gauge radius and angle
  int corda = (m_width - 2 * m_textBoundForValue.width) * 0.9;// Gauge width minus value text multiplied by 0.9 to have space for text
  int aberturaArcoTotal = 2 * m_maxAngle;// Calculate total arc angle
  int raioSugerido = corda / (2.0 * fastSin(aberturaArcoTotal / 2.0));// Calculate suggested radius
  int altura = raioSugerido * (1 - fastCos(aberturaArcoTotal / 2.0));
  m_radius = raioSugerido;
  DEBUG_D("Gauge radius %i\tsegment %i", m_radius, altura);

  //m_offsetYAgulha = (40 + m_textBoundForValue.height + m_borderSize);
  int seno = fastSin((90 - m_maxAngle)) * m_radius;
  m_offsetYAgulha = seno - (m_borderSize + m_textBoundForValue.height * 2);
  DEBUG_D("Needle offset: %i\n", m_offsetYAgulha);
  m_rotation = (-(m_maxAngle + m_rotation));
  m_divisor = 1;

  // Configure gauge origin
  m_origem.x = xPos;
  m_origem.y = yPos + m_offsetYAgulha;

  // The 3 in borderSize is to have space for labels and top border
  m_height = (m_radius - m_offsetYAgulha) + (3 * m_borderSize) + (m_textBoundForValue.height * 3);

  

  // Adjust maximum angle
  m_maxAngle = constrain(m_maxAngle, 22, 80);

  // Reset textbound value to draw the value
  m_textBoundForValue.x = 0;
  m_textBoundForValue.y = 0;
  m_textBoundForValue.width = 0;
  m_textBoundForValue.height = 0;

  // Calculate available dimensions
  m_availableWidth = m_width - (2 * m_borderSize);
  m_availableHeight = m_height - (2 * m_borderSize);

  m_update = true;
}

/**
 * @brief Draws the background of the GaugeSuper widget.
 * 
 * Renders the gauge background including colored strips, markers, and labels.
 * Only draws if the widget is on the current screen and properly loaded.
 */
void GaugeSuper::drawBackground()
{
  if (WidgetBase::currentScreen != screen || WidgetBase::usingKeyboard == true || !loaded)
  {
    return;
  }

  uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

  // updateFont(FontType::NORMAL);
  WidgetBase::objTFT->setFont(m_usedFont);

  DEBUG_D("Draw background GaugeSuper");

  m_indexCurrentStrip = 0;  // Index of first color to paint the strip background
  m_stripColor = CFK_WHITE; // the beginning of the strip is white
  m_ltx = 0;                // last x position of needle base
  m_lastPointNeedle.x = m_origem.x;
  m_lastPointNeedle.y = m_origem.y; // needle positions

  for (auto i = 0; i < m_borderSize; ++i)
  {
    WidgetBase::objTFT->drawRect((xPos - (m_width / 2)) + i, (yPos - (m_height)) + i, m_width - (2 * i), m_height - (2 * i), m_borderColor);
  }

  WidgetBase::objTFT->fillRect(xPos - (m_availableWidth / 2), yPos - (m_availableHeight + m_borderSize), m_availableWidth, m_availableHeight, m_bkColor);
  WidgetBase::objTFT->drawRect(xPos - (m_availableWidth / 2), yPos - (m_availableHeight + m_borderSize), m_availableWidth, m_availableHeight, baseBorder);

  // WidgetBase::objTFT->drawCircle(_origem.x, _origem.y, _radius, CFK_RED);
  // WidgetBase::objTFT->fillCircle(_origem.x, _origem.y, 2, CFK_RED);

  // Draw colored strip with intervals less than 5 to be more precise
  for (int i = 0; i <= (2 * m_maxAngle); i += 1)
  {
    // Apply rotation
    int angulo = i + m_rotation;

    // Larger tick size
    int tl = 15;

    // Coordinates to draw the tick
    float sx = fastCos(angulo);
    float sy = fastSin(angulo);

    uint16_t x0 = sx * (m_radius + tl) + m_origem.x;
    uint16_t y0 = sy * (m_radius + tl) + m_origem.y;
    uint16_t x1 = sx * m_radius + m_origem.x;
    uint16_t y1 = sy * m_radius + m_origem.y;

    // Coordinates of next tick to fill space with desired color
    float sx2 = fastCos(angulo + 1);
    float sy2 = fastSin(angulo + 1);
    int x2 = sx2 * (m_radius + tl) + m_origem.x;
    int y2 = sy2 * (m_radius + tl) + m_origem.y;
    int x3 = sx2 * m_radius + m_origem.x;
    int y3 = sy2 * m_radius + m_origem.y;

    int vFaixa = map(i, 0, (2 * m_maxAngle), m_vmin, m_vmax); // Transform the for loop from -50 to 50 into value between min and max to paint
    if (vFaixa >= m_intervals[m_indexCurrentStrip] && m_indexCurrentStrip < m_amountInterval)
    {
      m_stripColor = m_colors[m_indexCurrentStrip];
      m_indexCurrentStrip++;
    }
    if (i < 2 * m_maxAngle)
    {
      WidgetBase::objTFT->fillTriangle(x0, y0, x1, y1, x2, y2, m_stripColor);
      WidgetBase::objTFT->fillTriangle(x1, y1, x2, y2, x3, y3, m_stripColor);
    }
  }
  // End of colored arc drawing

  // Calculate and draw the multiplier
  WidgetBase::objTFT->setTextColor(m_textColor); // Text color

  uint32_t absMin = 0;
  int auxmin = abs(m_vmin);
  while (auxmin > 1)
  {
    absMin++;
    auxmin = auxmin / 10;
  }

  uint32_t absMax = 0;
  int auxmax = abs(m_vmax);
  while (auxmax > 1)
  {
    absMax++;
    auxmax = auxmax / 10;
  }
  /*if (absMin > 3 || absMax > 3)
  {
    m_divisor = absMin > absMax ? pow(10, (absMin - 3)) : pow(10, (absMax - 3));
  }*/


  if (m_divisor > 1)
  {
#if defined(DISP_DEFAULT)

    char char_arr[20];
    sprintf(char_arr, "x%d", m_divisor);

    //printText(char_arr, xPos + (m_availableWidth / 2) - 1, yPos + (m_availableHeight / 2) - 1, BR_DATUM);
#endif
  }

  if (m_showLabels)
  {

    // Reset values to draw texts
    m_indexCurrentStrip = 0; // Index of first interval to write the value

    // Loop to draw interval texts
    for (int i = 0; i <= (2 * m_maxAngle); i += 1)
    {
      int angulo = i + m_rotation;

      // Larger tick size
      int tl = 15;

      // Coordinates to draw the tick
      float sx = fastCos(angulo);
      float sy = fastSin(angulo);
      uint16_t x0 = sx * (m_radius + tl) + m_origem.x;
      uint16_t y0 = sy * (m_radius + tl) + m_origem.y;

      int vFaixa = map(i, 0, 2 * m_maxAngle, m_vmin, m_vmax);
      if (vFaixa >= m_intervals[m_indexCurrentStrip] && m_indexCurrentStrip < m_amountInterval)
      {
        int aX = sx * (m_radius + tl + 2) + m_origem.x;
        int aY = sy * (m_radius + tl + 2) + m_origem.y;
        uint8_t alinhamento = TL_DATUM;
        if (i == m_maxAngle)
        {
          alinhamento = BC_DATUM;
        }
        else if (i < m_maxAngle)
        {
          alinhamento = BR_DATUM;
          aX += 5;
        }
        else
        {
          alinhamento = BL_DATUM;
        }
#if defined(DISP_DEFAULT)
        char char_arr[20];
        sprintf(char_arr, "%d", m_intervals[m_indexCurrentStrip] / m_divisor);
        printText(char_arr, aX, aY, alinhamento);
        //printText(String(m_intervals[m_indexCurrentStrip] / m_divisor).c_str(), aX, aY, alinhamento);
#endif
        DEBUG_D("Writing \"%d\" at %d, %d", m_intervals[m_indexCurrentStrip] / m_divisor, aX, aY);
        m_indexCurrentStrip++;
      }
      if (i == 2 * m_maxAngle)
      {
#if defined(DISP_DEFAULT)
        printText(String(m_vmax / m_divisor).c_str(), x0, y0, BL_DATUM);
#endif

        DEBUG_D("Writing %d at %d, %d", m_vmax / m_divisor, x0, y0);
      }
    }
  }

  // Draw ticks every 5 degrees
  for (int i = 0; i <= 2 * m_maxAngle; i += 5)
  {
    int angulo = i + m_rotation;
    // Larger tick size
    int tl = 15;

    // Coordinates to draw the tick
    float sx = fastCos(angulo);
    float sy = fastSin(angulo);
    uint16_t x0 = sx * (m_radius + tl) + m_origem.x;
    uint16_t y0 = sy * (m_radius + tl) + m_origem.y;
    uint16_t x1 = sx * m_radius + m_origem.x;
    uint16_t y1 = sy * m_radius + m_origem.y;

    // Smaller tick size
    if (i % 25 != 0)
      tl = 8;

    // Recalculate coordinates if tick changes size, in case angle is not multiple of 25
    x0 = sx * (m_radius + tl) + m_origem.x;
    y0 = sy * (m_radius + tl) + m_origem.y;
    x1 = sx * m_radius + m_origem.x;
    y1 = sy * m_radius + m_origem.y;

    // Draw the tick
    WidgetBase::objTFT->drawLine(x0, y0, x1, y1, m_marcadoresColor);

    // Calculate positions to draw base arc
    sx = fastCos(angulo + 5);
    sy = fastSin(angulo + 5);
    x0 = sx * m_radius + m_origem.x;
    y0 = sy * m_radius + m_origem.y;

    // Draw the arc, don't draw the last part
    if (i < 2 * m_maxAngle)
      WidgetBase::objTFT->drawLine(x0, y0, x1, y1, m_marcadoresColor);
  }

  m_isFirstDraw = true;

  // WidgetBase::objTFT->fillCircle(m_origem.x, m_origem.y, 2, CFK_RED);
  // WidgetBase::objTFT->drawCircle(m_origem.x, m_origem.y, m_radius, CFK_RED);
}

/**
 * @brief Sets the current value of the GaugeSuper widget.
 * @param newValue New value to display on the gauge.
 * 
 * Updates the current value and marks the widget for redraw if the value changed.
 */
void GaugeSuper::setValue(int newValue)
{
  if (WidgetBase::currentScreen != screen || WidgetBase::usingKeyboard == true || !loaded)
  {
    return;
  }
  m_value = constrain(newValue, m_vmin, m_vmax);

  if (m_lastValue != m_value)
  {
    m_update = true;
    DEBUG_D("Set GaugeSuper value to %d", m_value);
  }
  else
  {
    DEBUG_D("Set GaugeSuper value to %d but not updated", m_value);
  }

  // redraw();
}

/**
 * @brief Redraws the GaugeSuper widget on the screen, updating the needle position.
 * 
 * Displays the gauge with the current value and updates the needle position.
 * Only redraws if the widget is on the current screen and needs updating.
 */
void GaugeSuper::redraw()
{
  if (WidgetBase::currentScreen != screen || WidgetBase::usingKeyboard == true || !m_update || !loaded)
  {
    return;
  }

  if (millis() - m_myTime < 50)
  {
    return;
  }

  DEBUG_D("Redrawing GaugeSuper");
  // updateFont(FontType::NORMAL);
  WidgetBase::objTFT->setFont(m_usedFont);

  m_myTime = millis();
  m_lastValue = m_value;

  // Draw here
  char buf[8];
  sprintf(buf, "%d", m_value);

  // Since the drawing is rotated -90 and drawing angles are -50 and 50.
  // int diff10 = (maxAngle + 10) - 90;

  int sdeg = map(m_value, m_vmin, m_vmax, 0, 2 * m_maxAngle); // Map input values min and max with extrapolation of 10 to angle with extrapolation of 10
  int angulo = sdeg + m_rotation;

  // Calculate needle components according to angle

  float sx = fastCos(angulo);
  float sy = fastSin(angulo);

  // Use tangent to calculate X position where needle should start since origin.y is below graph limit
  // The -90 is to simulate that total opening angle is rotated 90 degrees for correct tangent calculation (from -90 to 90)
  float tx = fastTan(angulo - 90);

  // Erase old needle
  if (!m_isFirstDraw)
  {
    WidgetBase::objTFT->drawLine(m_origem.x - 1 + round(m_ltx * m_offsetYAgulha), m_origem.y - m_offsetYAgulha - m_borderSize - 2, m_lastPointNeedle.x - 1, m_lastPointNeedle.y, m_bkColor); // -1 is to not draw on top of thin border line
    WidgetBase::objTFT->drawLine(m_origem.x + 0 + round(m_ltx * m_offsetYAgulha), m_origem.y - m_offsetYAgulha - m_borderSize - 2, m_lastPointNeedle.x + 0, m_lastPointNeedle.y, m_bkColor); // -1 is to not draw on top of thin border line
    WidgetBase::objTFT->drawLine(m_origem.x + 1 + round(m_ltx * m_offsetYAgulha), m_origem.y - m_offsetYAgulha - m_borderSize - 2, m_lastPointNeedle.x + 1, m_lastPointNeedle.y, m_bkColor); // -1 is to not draw on top of thin border line
  }

  WidgetBase::objTFT->setTextColor(m_textColor);
  if (m_showLabels)
  {
#if defined(DISP_DEFAULT)
    // WidgetBase::printText()
    uint16_t auxX = xPos - (m_availableWidth / 2) + 1;
    uint16_t auxY = yPos - m_borderSize;
    //TextBound_t tb_value = getTextBounds(buf, auxX, auxY);
    //printText(buf, auxX, auxY, BL_DATUM, m_textBoundForValue, m_bkColor); //Mostrar valor do gauge
#endif
  }
  if (m_showTitle)
  {
    // Redraw texts
    // updateFont(FontType::BOLD);
    WidgetBase::objTFT->setTextColor(m_titleColor);
#if defined(DISP_DEFAULT)
    WidgetBase::objTFT->setFont(m_usedFont);

    TextBound_t tb_title = getTextBounds(m_title, xPos, yPos - (m_borderSize * 2));
    printText(m_title, xPos, yPos - (m_borderSize * 2), BC_DATUM);
    updateFont(FontType::UNLOAD);
#endif
  }
  // store line values to erase later
  m_ltx = tx;
  m_lastPointNeedle.x = sx * (m_radius - m_distanceAgulhaArco) + m_origem.x; //-2 is the distance between needle end and arc
  m_lastPointNeedle.y = sy * (m_radius - m_distanceAgulhaArco) + m_origem.y;

  // Draw new line
  // Draw 3 lines to increase thickness

  WidgetBase::objTFT->drawLine(m_origem.x - 1 + round(m_ltx * m_offsetYAgulha), m_origem.y - m_offsetYAgulha - m_borderSize - 2, m_lastPointNeedle.x - 1, m_lastPointNeedle.y, m_agulhaColor);     // -1 is to not draw on top of thin border line
  WidgetBase::objTFT->drawLine(m_origem.x + 0 + round(m_ltx * m_offsetYAgulha), m_origem.y - m_offsetYAgulha - m_borderSize - 2, m_lastPointNeedle.x + 0, m_lastPointNeedle.y, m_agulhaColor); // -1 is to not draw on top of thin border line
  WidgetBase::objTFT->drawLine(m_origem.x + 1 + round(m_ltx * m_offsetYAgulha), m_origem.y - m_offsetYAgulha - m_borderSize - 2, m_lastPointNeedle.x + 1, m_lastPointNeedle.y, m_agulhaColor);     // -1 is to not draw on top of thin border line

  m_update = false;
  m_isFirstDraw = false;
  updateFont(FontType::UNLOAD);
}

/**
 * @brief Forces an immediate update of the GaugeSuper widget.
 * 
 * Sets the flag to redraw the gauge on the next redraw cycle.
 */
void GaugeSuper::forceUpdate()
{
  m_update = true;
}

/**
 * @brief Configures the GaugeSuper widget with specific parameters.
 * @param width Width of the gauge.
 * @param title Title displayed on the gauge.
 * @param intervals Array of interval values.
 * @param colors Array of colors corresponding to intervals.
 * @param amountIntervals Number of intervals and colors.
 * @param vmin Minimum value of the gauge range.
 * @param vmax Maximum value of the gauge range.
 * @param borderColor Color of the gauge border.
 * @param textColor Color for text and value display.
 * @param backgroundColor Background color of the gauge.
 * @param titleColor Color of the title text.
 * @param agulhaColor Color of the needle.
 * @param marcadoresColor Color of the markers.
 * @param showLabels True if text labels should be displayed for intervals, false otherwise.
 * @param _fontFamily Font used for text rendering.
 * 
 * Initializes the gauge properties and marks it as loaded when complete.
 */
void GaugeSuper::setup(uint16_t width, const char *title, const int *intervals, const uint16_t *colors, uint8_t amountIntervals, int vmin, int vmax, uint16_t borderColor, uint16_t textColor, uint16_t backgroundColor, uint16_t titleColor, uint16_t agulhaColor, uint16_t marcadoresColor, bool showLabels, const GFXfont *_fontFamily)
{
  if (!WidgetBase::objTFT)
  {
    DEBUG_E("TFT not defined on WidgetBase");
    return;
  }
  if (loaded)
  {
    DEBUG_E("GaugeSuper widget already configured");
    return;
  }

  // Free previous memory if it exists
  if (m_intervals)
  {
    delete[] m_intervals;
    m_intervals = nullptr;
  }
  if (m_colors)
  {
    delete[] m_colors;
    m_colors = nullptr;
  }
  if (m_title)
  {
    delete[] m_title;
    m_title = nullptr;
  }

  // Configure dimensions and colors
  m_width = width;
  // m_height = height;
  m_vmin = vmin;
  m_vmax = vmax;
  m_borderColor = borderColor;
  m_textColor = textColor;
  m_bkColor = backgroundColor;
  m_titleColor = titleColor;
  m_showLabels = showLabels;
  m_amountInterval = amountIntervals;
  m_usedFont = _fontFamily;
  m_agulhaColor = agulhaColor;
  m_marcadoresColor = marcadoresColor;

  // Allocate and copy arrays
  m_intervals = new int[amountIntervals];
  m_colors = new uint16_t[amountIntervals];

  if (m_intervals == nullptr || m_colors == nullptr)
  {
    DEBUG_E("Failed to allocate memory for GaugeSuper arrays");
    if (m_intervals)
    {
      delete[] m_intervals;
      m_intervals = nullptr;
    }
    if (m_colors)
    {
      delete[] m_colors;
      m_colors = nullptr;
    }
    return;
  }

  // Copy data
  for (uint8_t i = 0; i < amountIntervals; i++)
  {
    m_intervals[i] = intervals[i];
    m_colors[i] = colors[i];
  }

  // Allocate and copy title
  if (title != nullptr)
  {
    size_t titleLen = strlen(title) + 1;
    m_title = new char[titleLen];
    if (m_title == nullptr)
    {
      DEBUG_E("Failed to allocate memory for GaugeSuper title");
      m_title = nullptr;
      return;
    }
    else
    {
      strcpy(m_title, title);
    }
  }

  // Initialize gauge
  start();

  loaded = true;
}


/**
 * @brief Configures the GaugeSuper with parameters defined in a configuration structure.
 * @param config Structure containing the GaugeSuper configuration parameters.
 */
void GaugeSuper::setup(const GaugeConfig& config)
{
  setup(config.width, config.title, config.intervals, config.colors, config.amountIntervals, 
        config.minValue, config.maxValue, config.borderColor, config.textColor, config.backgroundColor, 
        config.titleColor, config.needleColor, config.markersColor, config.showLabels, config.fontFamily);
}
