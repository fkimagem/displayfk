#include "wgaugesuper.h"
#include <esp_log.h>

const char* GaugeSuper::TAG = "GaugeSuper";

/**
 * @brief Constructor for the GaugeSuper class.
 * @param _x X position of the gauge.
 * @param _y Y position of the gauge.
 * @param _screen Screen number.
 */
GaugeSuper::GaugeSuper(uint16_t _x, uint16_t _y, uint8_t _screen) 
    : WidgetBase(_x, _y, _screen), 
      // Internal state (in order of declaration)
      m_ltx(0), m_lastPointNeedle(), m_stripColor(CFK_WHITE), m_indexCurrentStrip(0), 
      m_divisor(1), m_isFirstDraw(true),
      // Dynamic arrays
      m_intervals(nullptr), m_colors(nullptr), m_title(nullptr),
      // Memory management flags
      m_intervalsAllocated(false), m_colorsAllocated(false), m_titleAllocated(false),
      // Calculated dimensions
      m_height(0), m_radius(0), m_currentValue(0), m_lastValue(0),
      // Drawing parameters
      m_stripWeight(16), m_maxAngle(40), m_offsetYAgulha(40), m_rotation(90), 
      m_distanceAgulhaArco(2), m_borderSize(5), m_availableWidth(0), m_availableHeight(0)
{
  // Initialize m_config with default values
  m_config = {.width = 0, .title = nullptr, .intervals = nullptr, .colors = nullptr, 
              .amountIntervals = 0, .minValue = 0, .maxValue = 100, .borderColor = CFK_BLACK,
              .textColor = CFK_BLACK, .backgroundColor = CFK_WHITE, .titleColor = CFK_NAVY,
              .needleColor = CFK_RED, .markersColor = CFK_BLACK, .showLabels = false
              #if defined(USING_GRAPHIC_LIB)
              , .fontFamily = nullptr
              #endif
              };
  
  // Dynamic arrays already initialized in member initializer list
  
  ESP_LOGD(TAG, "GaugeSuper created at (%d, %d) on screen %d", _x, _y, _screen);
}

/**
 * @brief Destructor for the GaugeSuper class.
 */
GaugeSuper::~GaugeSuper()
{
  ESP_LOGD(TAG, "GaugeSuper destroyed at (%d, %d)", m_xPos, m_yPos);
  
  // Use centralized cleanup method
  cleanupMemory();
  
  // Clear callback
  if (m_callback != nullptr) {
    m_callback = nullptr;
  }
}

/**
 * @brief Detects a touch on the gauge.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is detected, false otherwise.
 */
bool GaugeSuper::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
  // GaugeSuper doesn't handle touch events, it's a display-only widget
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
  return m_callback;
}

/**
 * @brief Initializes the GaugeSuper widget with default calculations and constraints.
 * 
 * Calculates gauge dimensions, text bounds, and sets up initial values.
 * Constrains the maximum angle between 22 and 80 degrees.
 */
void GaugeSuper::start()
{
  CHECK_TFT_VOID
  #if defined(DISP_DEFAULT)
  if (m_config.maxValue < m_config.minValue)
  {
    int temp = m_config.minValue;
    m_config.minValue = m_config.maxValue;
    m_config.maxValue = temp;
  }

  WidgetBase::objTFT->setFont(m_usedFont);

  // Calculate text dimensions for values
  char strMin[12], strMax[12];// Maximum of 12 characters for values
  sprintf(strMin, "%d", m_config.minValue * 10);// Multiply by 10 to have one more digit and serve as text offset
  sprintf(strMax, "%d", m_config.maxValue * 10);// Multiply by 10 to have one more digit and serve as text offset
  TextBound_t dimensionMin = getTextBounds(strMin, m_xPos, m_yPos);// Get text dimensions
  TextBound_t dimensionMax = getTextBounds(strMax, m_xPos, m_yPos);// Get text dimensions
  m_textBoundForValue = dimensionMax.width > dimensionMin.width ? dimensionMax : dimensionMin;// Get the larger value between the two

  // Configure the title
  // Title visibility is determined by m_config.title != nullptr
  
  m_currentValue = m_config.minValue;// Define initial value
  m_lastValue = m_config.maxValue;// Define initial value

  
  //TextBound_t t;
  //t.height = 20;
  //t.width = m_showLabels ? 50 : 10;

  // https://pt.wikipedia.org/wiki/Segmento_circular

  if(!m_config.showLabels){
    m_textBoundForValue.width = 5;
    m_textBoundForValue.height = 5;
  }
  

  // Calculate gauge radius and angle
  int corda = (m_config.width - 2 * m_textBoundForValue.width) * 0.9;// Gauge width minus value text multiplied by 0.9 to have space for text
  int aberturaArcoTotal = 2 * m_maxAngle;// Calculate total arc angle
  int raioSugerido = corda / (2.0 * fastSin(aberturaArcoTotal / 2.0));// Calculate suggested radius
  int altura = raioSugerido * (1 - fastCos(aberturaArcoTotal / 2.0));
  m_radius = raioSugerido;
  ESP_LOGD(TAG, "Gauge radius %i\tsegment %i", m_radius, altura);
  UNUSED(altura);

  //m_offsetYAgulha = (40 + m_textBoundForValue.height + m_borderSize);
  int seno = fastSin((90 - m_maxAngle)) * m_radius;
  m_offsetYAgulha = seno - (m_borderSize + m_textBoundForValue.height * 2);
  ESP_LOGD(TAG, "Needle offset: %i", m_offsetYAgulha);
  m_rotation = (-(m_maxAngle + m_rotation));
  m_divisor = 1;

  // Configure gauge origin
  m_origem.x = m_xPos;
  m_origem.y = m_yPos + m_offsetYAgulha;

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
  m_availableWidth = m_config.width - (2 * m_borderSize);
  m_availableHeight = m_height - (2 * m_borderSize);

  m_shouldRedraw = true;
  
  // Configuration is now directly accessible through m_config
  #endif
}

/**
 * @brief Draws the background of the GaugeSuper widget.
 * 
 * Renders the gauge background including colored strips, markers, and labels.
 * Only draws if the widget is on the current screen and properly loaded.
 */
void GaugeSuper::drawBackground()
{
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  #if defined(DISP_DEFAULT)
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_LOADED_VOID
  
  // Validate arrays before drawing
  if (m_config.amountIntervals > 0) {
    if (m_intervals == nullptr || m_colors == nullptr) {
      ESP_LOGE(TAG, "Cannot draw background: intervals or colors arrays are null");
      return;
    }
  }

  uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

  // updateFont(FontType::NORMAL);
  WidgetBase::objTFT->setFont(m_usedFont);

  ESP_LOGD(TAG, "Draw background GaugeSuper");

  m_indexCurrentStrip = 0;  // Index of first color to paint the strip background
  m_stripColor = CFK_WHITE; // the beginning of the strip is white
  m_ltx = 0;                // last x position of needle base
  m_lastPointNeedle.x = m_origem.x;
  m_lastPointNeedle.y = m_origem.y; // needle positions

  for (auto i = 0; i < m_borderSize; ++i)
  {
    WidgetBase::objTFT->drawRect((m_xPos - (m_config.width / 2)) + i, (m_yPos - (m_height)) + i, m_config.width - (2 * i), m_height - (2 * i), m_config.borderColor);
  }

  WidgetBase::objTFT->fillRect(m_xPos - (m_availableWidth / 2), m_yPos - (m_availableHeight + m_borderSize), m_availableWidth, m_availableHeight, m_config.backgroundColor);
  WidgetBase::objTFT->drawRect(m_xPos - (m_availableWidth / 2), m_yPos - (m_availableHeight + m_borderSize), m_availableWidth, m_availableHeight, baseBorder);

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

    int vFaixa = map(i, 0, (2 * m_maxAngle), m_config.minValue, m_config.maxValue); // Transform the for loop from -50 to 50 into value between min and max to paint
    if (vFaixa >= m_intervals[m_indexCurrentStrip] && m_indexCurrentStrip < m_config.amountIntervals)
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
  WidgetBase::objTFT->setTextColor(m_config.textColor); // Text color

  uint32_t absMin = 0;
  int auxmin = abs(m_config.minValue);
  while (auxmin > 1)
  {
    absMin++;
    auxmin = auxmin / 10;
  }

  uint32_t absMax = 0;
  int auxmax = abs(m_config.maxValue);
  while (auxmax > 1)
  {
    absMax++;
    auxmax = auxmax / 10;
  }


  if (m_divisor > 1)
  {

    char char_arr[20];
    sprintf(char_arr, "x%d", m_divisor);
  }

  if (isLabelsVisible())
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

      int vFaixa = map(i, 0, 2 * m_maxAngle, m_config.minValue, m_config.maxValue);
      if (vFaixa >= m_intervals[m_indexCurrentStrip] && m_indexCurrentStrip < m_config.amountIntervals)
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
        char char_arr[20];
        sprintf(char_arr, "%d", m_intervals[m_indexCurrentStrip] / m_divisor);
        printText(char_arr, aX, aY, alinhamento);
        //printText(String(m_intervals[m_indexCurrentStrip] / m_divisor).c_str(), aX, aY, alinhamento);
        ESP_LOGD(TAG, "Writing \"%d\" at %d, %d", m_intervals[m_indexCurrentStrip] / m_divisor, aX, aY);
        m_indexCurrentStrip++;
      }
      if (i == 2 * m_maxAngle)
      {
        printText(String(m_config.maxValue / m_divisor).c_str(), x0, y0, BL_DATUM);

        ESP_LOGD(TAG, "Writing %d at %d, %d", m_config.maxValue / m_divisor, x0, y0);
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
    WidgetBase::objTFT->drawLine(x0, y0, x1, y1, m_config.markersColor);

    // Calculate positions to draw base arc
    sx = fastCos(angulo + 5);
    sy = fastSin(angulo + 5);
    x0 = sx * m_radius + m_origem.x;
    y0 = sy * m_radius + m_origem.y;

    // Draw the arc, don't draw the last part
    if (i < 2 * m_maxAngle)
      WidgetBase::objTFT->drawLine(x0, y0, x1, y1, m_config.markersColor);
  }

  m_isFirstDraw = true;

  #endif

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
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_LOADED_VOID
  CHECK_INITIALIZED_VOID
  
  m_currentValue = constrain(newValue, m_config.minValue, m_config.maxValue);

  if (m_lastValue != m_currentValue)
  {
    m_shouldRedraw = true;
    ESP_LOGD(TAG, "Set GaugeSuper value to %d", m_currentValue);
  }
  else
  {
    ESP_LOGD(TAG, "Set GaugeSuper value to %d but not updated", m_currentValue);
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
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  #if defined(DISP_DEFAULT)
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_LOADED_VOID
  CHECK_SHOULDREDRAW_VOID
  CHECK_DEBOUNCE_REDRAW_VOID

  ESP_LOGD(TAG, "Redrawing GaugeSuper");
  // updateFont(FontType::NORMAL);
  WidgetBase::objTFT->setFont(m_usedFont);

  m_myTime = millis();
  m_lastValue = m_currentValue;

  // Draw here
  char buf[8];
  sprintf(buf, "%d", m_currentValue);

  // Since the drawing is rotated -90 and drawing angles are -50 and 50.
  // int diff10 = (maxAngle + 10) - 90;

  int sdeg = map(m_currentValue, m_config.minValue, m_config.maxValue, 0, 2 * m_maxAngle); // Map input values min and max with extrapolation of 10 to angle with extrapolation of 10
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
    WidgetBase::objTFT->drawLine(m_origem.x - 1 + round(m_ltx * m_offsetYAgulha), m_origem.y - m_offsetYAgulha - m_borderSize - 2, m_lastPointNeedle.x - 1, m_lastPointNeedle.y, m_config.backgroundColor); // -1 is to not draw on top of thin border line
    WidgetBase::objTFT->drawLine(m_origem.x + 0 + round(m_ltx * m_offsetYAgulha), m_origem.y - m_offsetYAgulha - m_borderSize - 2, m_lastPointNeedle.x + 0, m_lastPointNeedle.y, m_config.backgroundColor); // -1 is to not draw on top of thin border line
    WidgetBase::objTFT->drawLine(m_origem.x + 1 + round(m_ltx * m_offsetYAgulha), m_origem.y - m_offsetYAgulha - m_borderSize - 2, m_lastPointNeedle.x + 1, m_lastPointNeedle.y, m_config.backgroundColor); // -1 is to not draw on top of thin border line
  }

  WidgetBase::objTFT->setTextColor(m_config.textColor);
  if (isLabelsVisible())
  {
    // WidgetBase::printText()
    //uint16_t auxX = m_xPos - (m_availableWidth / 2) + 1;
    //uint16_t auxY = m_yPos - m_borderSize;
    //TextBound_t tb_value = getTextBounds(buf, auxX, auxY);
    //printText(buf, auxX, auxY, BL_DATUM, m_textBoundForValue, m_bkColor); //Mostrar valor do gauge
  }
  if (isTitleVisible())
  {
    // Redraw texts
    // updateFont(FontType::BOLD);
    WidgetBase::objTFT->setTextColor(m_config.titleColor);
    WidgetBase::objTFT->setFont(m_usedFont);

    //TextBound_t tb_title = getTextBounds(m_title, m_xPos, m_yPos - (m_borderSize * 2));
    printText(m_title, m_xPos, m_yPos - (m_borderSize * 2), BC_DATUM);
    updateFont(FontType::UNLOAD);
  }
  // store line values to erase later
  m_ltx = tx;
  m_lastPointNeedle.x = sx * (m_radius - m_distanceAgulhaArco) + m_origem.x; //-2 is the distance between needle end and arc
  m_lastPointNeedle.y = sy * (m_radius - m_distanceAgulhaArco) + m_origem.y;

  // Draw new line
  // Draw 3 lines to increase thickness

  WidgetBase::objTFT->drawLine(m_origem.x - 1 + round(m_ltx * m_offsetYAgulha), m_origem.y - m_offsetYAgulha - m_borderSize - 2, m_lastPointNeedle.x - 1, m_lastPointNeedle.y, m_config.needleColor);     // -1 is to not draw on top of thin border line
  WidgetBase::objTFT->drawLine(m_origem.x + 0 + round(m_ltx * m_offsetYAgulha), m_origem.y - m_offsetYAgulha - m_borderSize - 2, m_lastPointNeedle.x + 0, m_lastPointNeedle.y, m_config.needleColor); // -1 is to not draw on top of thin border line
  WidgetBase::objTFT->drawLine(m_origem.x + 1 + round(m_ltx * m_offsetYAgulha), m_origem.y - m_offsetYAgulha - m_borderSize - 2, m_lastPointNeedle.x + 1, m_lastPointNeedle.y, m_config.needleColor);     // -1 is to not draw on top of thin border line

  m_shouldRedraw = false;
  m_isFirstDraw = false;
  updateFont(FontType::UNLOAD);
  #endif
}

/**
 * @brief Forces an immediate update of the GaugeSuper widget.
 * 
 * Sets the flag to redraw the gauge on the next redraw cycle.
 */
void GaugeSuper::forceUpdate()
{
  m_shouldRedraw = true;
  ESP_LOGD(TAG, "GaugeSuper force update requested");
}


/**
 * @brief Configures the GaugeSuper with parameters defined in a configuration structure.
 * @param config Structure containing the GaugeSuper configuration parameters.
 */
void GaugeSuper::setup(const GaugeConfig& config)
{
  #if defined(USING_GRAPHIC_LIB)
  // Validate TFT object
  CHECK_TFT_VOID

  // Validate configuration first
  if (!validateConfig(config)) {
    ESP_LOGE(TAG, "Invalid configuration provided");
    return;
  }

  // Clean up any existing memory
  cleanupMemory();

  // Assign the configuration structure
  m_config = config;
  m_usedFont = config.fontFamily;
  
  // Copy title to internal buffer if provided
  if (config.title != nullptr) {
    size_t titleLen = strlen(config.title);
    
    // Validate title length (max 20 characters)
    if (titleLen > MAX_TITLE_LENGTH) {
      ESP_LOGW(TAG, "Title length (%d) exceeds maximum limit (%d). Truncating to %d characters", 
               titleLen, MAX_TITLE_LENGTH, MAX_TITLE_LENGTH);
      titleLen = MAX_TITLE_LENGTH;
    }
    
    // Clean up existing title before allocating new one
    if (m_titleAllocated && m_title != nullptr) {
      delete[] m_title;
      m_title = nullptr;
      m_titleAllocated = false;
    }
    
    m_title = new char[titleLen + 1];
    if (m_title != nullptr) {
      strncpy(m_title, config.title, titleLen);
      m_title[titleLen] = '\0'; // Ensure null termination
      m_titleAllocated = true;
    } else {
      ESP_LOGE(TAG, "Failed to allocate memory for title");
      return;
    }
  }
  
  // Copy intervals and colors to internal arrays if provided
  if (config.intervals != nullptr && config.colors != nullptr && config.amountIntervals > 0) {
    // Clamp amountIntervals to MAX_SERIES limit
    uint8_t clampedAmount = (config.amountIntervals > MAX_SERIES) ? MAX_SERIES : config.amountIntervals;
    
    // Clean up existing arrays before allocating new ones
    if (m_intervalsAllocated && m_intervals != nullptr) {
      delete[] m_intervals;
      m_intervals = nullptr;
      m_intervalsAllocated = false;
    }
    if (m_colorsAllocated && m_colors != nullptr) {
      delete[] m_colors;
      m_colors = nullptr;
      m_colorsAllocated = false;
    }
    
    // Allocate new memory
    m_intervals = new int[clampedAmount];
    m_colors = new uint16_t[clampedAmount];
    
    if (m_intervals != nullptr && m_colors != nullptr) {
      // Copy data
      for (uint8_t i = 0; i < clampedAmount; i++) {
        m_intervals[i] = config.intervals[i];
        m_colors[i] = config.colors[i];
      }
      m_intervalsAllocated = true;
      m_colorsAllocated = true;
    } else {
      ESP_LOGE(TAG, "Failed to allocate memory for intervals/colors");
      // Clean up partial allocations
      if (m_intervals != nullptr) {
        delete[] m_intervals;
        m_intervals = nullptr;
      }
      if (m_colors != nullptr) {
        delete[] m_colors;
        m_colors = nullptr;
      }
      cleanupMemory();
      return;
    }
  }
  // Initialize gauge
  start();

  m_loaded = true;
  m_initialized = true;
  
  ESP_LOGD(TAG, "GaugeSuper setup completed at (%d, %d)", m_xPos, m_yPos);
  #endif
}

void GaugeSuper::show()
{
    m_visible = true;
    m_shouldRedraw = true;
    ESP_LOGD(TAG, "GaugeSuper shown at (%d, %d)", m_xPos, m_yPos);
}

void GaugeSuper::hide()
{
    m_visible = false;
    m_shouldRedraw = true;
    ESP_LOGD(TAG, "GaugeSuper hidden at (%d, %d)", m_xPos, m_yPos);
}

/**
 * @brief Checks if the title should be visible.
 * @return True if title is visible, false otherwise.
 */
bool GaugeSuper::isTitleVisible() const
{
  return (m_title != nullptr && strlen(m_title) > 0);
}

/**
 * @brief Checks if labels should be visible.
 * @return True if labels are visible, false otherwise.
 */
bool GaugeSuper::isLabelsVisible() const
{
  return m_config.showLabels;
}

/**
 * @brief Centralized memory cleanup method.
 * 
 * Safely deallocates all dynamic memory and resets allocation flags.
 */
void GaugeSuper::cleanupMemory()
{
  // Clean up intervals array
  if (m_intervalsAllocated && m_intervals != nullptr) {
    delete[] m_intervals;
    m_intervals = nullptr;
    m_intervalsAllocated = false;
  }

  // Clean up colors array
  if (m_colorsAllocated && m_colors != nullptr) {
    delete[] m_colors;
    m_colors = nullptr;
    m_colorsAllocated = false;
  }

  // Clean up title string
  if (m_titleAllocated && m_title != nullptr) {
    delete[] m_title;
    m_title = nullptr;
    m_titleAllocated = false;
  }
  
  // Reset all pointers to nullptr for safety
  m_intervals = nullptr;
  m_colors = nullptr;
  m_title = nullptr;
}

// Color and value access methods removed - now using m_config directly

/**
 * @brief Validates the configuration structure.
 * @param config Configuration to validate.
 * @return True if valid, false otherwise.
 */
bool GaugeSuper::validateConfig(const GaugeConfig& config)
{
  // Validate basic parameters
  if (config.width <= 0) {
    ESP_LOGE(TAG, "Invalid width: %d", config.width);
    return false;
  }
  
  if (config.minValue >= config.maxValue) {
    ESP_LOGE(TAG, "Invalid range: min=%d, max=%d", config.minValue, config.maxValue);
    return false;
  }
  
  if (config.amountIntervals > MAX_SERIES) {
    ESP_LOGW(TAG, "Too many intervals: %d (max: %d)", config.amountIntervals, MAX_SERIES);
    return false;
  }
  
  // Validate intervals and colors arrays
  if (config.amountIntervals > 0) {
    if (config.intervals == nullptr || config.colors == nullptr) {
      ESP_LOGE(TAG, "Intervals or colors array is null");
      return false;
    }
  }
  
  return true;
}