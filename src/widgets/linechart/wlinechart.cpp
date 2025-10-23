#include "wlinechart.h"

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

// Static member initialization
const char* LineChart::TAG = "LineChart";


void LineChart::initMutex()
{
  #if defined(DISP_DEFAULT)
  if (!m_mutex)
  {
    m_mutex = xSemaphoreCreateMutex();
  }
  #endif
}

void LineChart::destroyMutex()
{
  #if defined(DISP_DEFAULT)
  if (m_mutex)
  {
    vSemaphoreDelete(m_mutex);
    m_mutex = nullptr;
  }
  #endif
}

/**
 * @brief Constructor for the LineChart class.
 * @param _x X position of the widget.
 * @param _y Y position of the widget.
 * @param _screen Screen number.
 */
LineChart::LineChart(uint16_t _x, uint16_t _y, uint8_t _screen) 
    : WidgetBase(_x, _y, _screen),
      m_dotRadius(2),
      m_minSpaceToShowDot(10),
      m_maxHeight(0),
      m_maxWidth(0),
      m_maxAmountValues(0),
      m_amountPoints(0),
      m_spaceBetweenPoints(0.0f),
      m_leftPadding(0),
      m_topBottomPadding(0),
      m_aux(0),
      m_blocked(false),
      m_borderSize(2),
      m_availableWidth(0),
      m_availableheight(0),
      m_yTovmin(0),
      m_yTovmax(0),
      m_shouldRedraw(true),
      m_valuesAllocated(false),
      m_colorsSeriesAllocated(false),
      m_values(nullptr),
      m_mutex(nullptr)
{
  // Note: Initialization list follows declaration order in .h file:
  // m_config, m_dotRadius, m_minSpaceToShowDot (have defaults in .h)
  // m_maxHeight, m_maxWidth, etc. (will be initialized in start())
  // m_blocked, m_shouldRedraw (initialized in list above)
  // m_valuesAllocated, m_colorsSeriesAllocated (have defaults = false)
  // m_values, m_mutex (have defaults = nullptr)
  
  // Initialize config with default values
  m_config = {
    .width = 0,
    .height = 0,
    .minValue = 0,
    .maxValue = 100,
    .amountSeries = 0,
    .colorsSeries = nullptr,
    .gridColor = 0,
    .borderColor = 0,
    .backgroundColor = 0,
    .textColor = 0,
    .verticalDivision = 0,
    .workInBackground = false,
    .showZeroLine = true,
    .boldLine = false,
    .showDots = false,
    .maxPointsAmount = 0,
    #if defined(USING_GRAPHIC_LIB)
    .font = nullptr,
    #endif
    .subtitles = nullptr
  };
  
  // Initialize color and subtitle arrays to zero/nullptr
  for (uint8_t i = 0; i < MAX_SERIES; i++) {
    m_colorsSeries[i] = 0;
    m_subtitles[i] = nullptr;
  }
  
  #if defined(DISP_DEFAULT)
  initMutex();
  #endif
  
  ESP_LOGD(TAG, "LineChart created at (%d, %d) on screen %d", _x, _y, _screen);
}

/**
 * @brief Destructor for the LineChart class.
 *
 * Frees all allocated memory for chart data series.
 */
LineChart::~LineChart()
{
  ESP_LOGD(TAG, "LineChart destroyed at (%d, %d)", m_xPos, m_yPos);
  
  #if defined(DISP_DEFAULT)
  // Clean up memory using centralized method
  cleanupMemory();
  clearColorsSeries();
  clearSubtitles();
  
  // Destroy mutex
  destroyMutex();
  #endif
  
  // Clear callback
  if (m_callback != nullptr) {
    m_callback = nullptr;
  }
  
  // Reset state
  m_loaded = false;
  m_shouldRedraw = false;
}

/**
 * @brief Centralized memory cleanup method.
 *
 * Safely deallocates all dynamically allocated memory with proper tracking.
 */
void LineChart::cleanupMemory()
{
  #if defined(DISP_DEFAULT)
  // Clean up values array
  clearValues();
  
  #endif
}

void LineChart::clearColorsSeries()
{
  ESP_LOGD(TAG, "Resetting internal color array");
  for (uint8_t i = 0; i < MAX_SERIES; i++) {
    m_colorsSeries[i] = 0;
  }
}

void LineChart::clearSubtitles()
{
  ESP_LOGD(TAG, "Resetting internal subtitles array");
  for (uint8_t i = 0; i < MAX_SERIES; i++) {
    m_subtitles[i] = nullptr;
  }
}

void LineChart::clearValues()
{
  ESP_LOGD(TAG, "Resetting internal values array");
  if (m_valuesAllocated && m_values != nullptr)
  {
    // Use a safe loop limit to avoid issues with uninitialized config
    uint16_t maxSeries = (m_config.amountSeries > 0 && m_config.amountSeries <= MAX_SERIES) ? 
                         m_config.amountSeries : MAX_SERIES;
    
    for (uint16_t i = 0; i < maxSeries; i++)
    {
      if (m_values[i] != nullptr)
      {
        free(m_values[i]);
        m_values[i] = nullptr;
      }
    }
    free(m_values);
    m_values = nullptr;
    m_valuesAllocated = false;
  }
}


/**
 * @brief Validates the LineChart configuration.
 * @param config Configuration structure to validate.
 * @return True if configuration is valid, false otherwise.
 */
bool LineChart::validateConfig(const LineChartConfig& config)
{
  // Validate dimensions
  if (config.width <= 0 || config.height <= 0) {
    ESP_LOGE(TAG, "Invalid dimensions: width=%d, height=%d", config.width, config.height);
    return false;
  }
  
  // Validate value range
  if (config.minValue >= config.maxValue) {
    ESP_LOGE(TAG, "Invalid range: minValue=%d must be less than maxValue=%d", config.minValue, config.maxValue);
    return false;
  }
  
  // Validate series amount
  if (config.amountSeries == 0) {
    ESP_LOGE(TAG, "Invalid amountSeries: must be greater than 0");
    return false;
  }
  
  if (config.amountSeries > MAX_SERIES) {
    ESP_LOGW(TAG, "Too many series: %d (max: %d)", config.amountSeries, MAX_SERIES);
    return false;
  }
  
  // Validate colors array
  if (config.colorsSeries == nullptr) {
    ESP_LOGE(TAG, "colorsSeries array is null");
    return false;
  }
  
  // Validate max points amount
  if (config.maxPointsAmount < 2) {
    ESP_LOGE(TAG, "Invalid maxPointsAmount: %d (must be at least 2)", config.maxPointsAmount);
    return false;
  }
  
  return true;
}

/**
 * @brief Detects touch events on the LineChart.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return Always returns false as LineChart does not respond to touch events.
 */
bool LineChart::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
  return false;
}

/**
 * @brief Retrieves the callback function associated with the LineChart.
 * @return Pointer to the callback function.
 */
functionCB_t LineChart::getCallbackFunc()
{
  return m_callback;
}

/**
 * @brief Resets and frees all data arrays.
 *
 * This method cleans up memory allocated for chart data series.
 */
void LineChart::resetArray()
{
  #if defined(DISP_DEFAULT)
  if (m_mutex)
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    
  // Use centralized cleanup
  cleanupMemory();
  
  if (m_mutex)
    xSemaphoreGive(m_mutex);
  #endif
}

/**
 * @brief Initializes the chart parameters and prepares the data structures.
 *
 * Calculates layout dimensions, prepares grid dimensions, and initializes data arrays.
 */
void LineChart::start()
{
  CHECK_TFT_VOID
  #if defined(DISP_DEFAULT)
  
  // Swap min/max if they are reversed
  if (m_config.maxValue < m_config.minValue)
  {
    int temp = m_config.minValue;
    m_config.minValue = m_config.maxValue;
    m_config.maxValue = temp;
    ESP_LOGW(TAG, "minValue and maxValue were swapped");
  }

  #if defined(USING_GRAPHIC_LIB)
  WidgetBase::objTFT->setFont(m_config.font);
  #endif

  int16_t x, y;
  uint16_t textSizeMin, textSizeMax, h;
  char char_arr_min[20];
  char char_arr_max[20];
  snprintf(char_arr_min, sizeof(char_arr_min), "%d", m_config.minValue);
  snprintf(char_arr_max, sizeof(char_arr_max), "%d", m_config.maxValue);

  objTFT->getTextBounds(char_arr_min, 10, 10, &x, &y, &textSizeMin, &h);
  objTFT->getTextBounds(char_arr_max, 10, 10, &x, &y, &textSizeMax, &h);

  int16_t biggerText = max(textSizeMin, textSizeMax);

  updateFont(FontType::UNLOAD);

  biggerText += 5;

  m_maxAmountValues = m_config.width - (biggerText + 10) - (m_borderSize * 2);
  m_amountPoints = CLAMP(m_config.maxPointsAmount, 2, m_maxAmountValues);

  m_spaceBetweenPoints = m_maxAmountValues / (float)(m_amountPoints - 1);
  if (m_spaceBetweenPoints < m_minSpaceToShowDot) {
    // Don't modify m_config.showDots, just log warning
    ESP_LOGD(TAG, "Space between points (%.2f) is too small to show dots", m_spaceBetweenPoints);
  }
  ESP_LOGD(TAG, "MaxWidth: %d, Space between points: %.2f", m_maxAmountValues, m_spaceBetweenPoints);

  m_maxWidth = m_maxAmountValues;
  m_maxHeight = m_config.height - (m_topBottomPadding * 2) - (m_borderSize * 2);

  if (m_config.verticalDivision > m_maxHeight)
  {
    ESP_LOGW(TAG, "Amount of vertical divisions (%d) is too large for height (%d)", m_config.verticalDivision, m_maxHeight);
  }

  uint16_t restoAreaPlot = 0;
  if (m_config.verticalDivision > 0)
  {
    uint16_t sizeVSpace = round(m_maxHeight / m_config.verticalDivision);
    ESP_LOGD(TAG, "Calculating GridCells MaxH:%u Qty:%u Space:%u Occupied:%u Rest:%u", 
             m_maxHeight, m_config.verticalDivision, sizeVSpace, 
             sizeVSpace * m_config.verticalDivision, m_maxHeight - (sizeVSpace * m_config.verticalDivision));
    restoAreaPlot = m_maxHeight - (sizeVSpace * m_config.verticalDivision);
    if (restoAreaPlot > 0)
    {
      m_maxHeight -= restoAreaPlot;
    }
    sizeVSpace = round(m_maxHeight / m_config.verticalDivision);
    ESP_LOGD(TAG, "Recalculating GridCells MaxH:%u Qty:%u Space:%u Occupied:%u Rest:%u", 
             m_maxHeight, m_config.verticalDivision, sizeVSpace, 
             sizeVSpace * m_config.verticalDivision, m_maxHeight - (sizeVSpace * m_config.verticalDivision));
  }

  m_leftPadding = biggerText;

  m_yTovmin = (m_yPos + m_config.height) - (m_topBottomPadding + m_borderSize + restoAreaPlot);
  m_yTovmax = m_yPos + m_topBottomPadding + m_borderSize;

  resetArray();

  // Allocate values array
  m_values = (LineChartValue_t **)malloc(sizeof(LineChartValue_t *) * m_config.amountSeries);
  if (!m_values) {
    ESP_LOGE(TAG, "Failed to allocate memory for values array");
    return;
  }
  m_valuesAllocated = true;

  for (uint16_t i = 0; i < m_config.amountSeries; i++)
  {
    m_values[i] = (LineChartValue_t *)malloc(sizeof(LineChartValue_t) * m_amountPoints);
    if (!m_values[i])
    {
      ESP_LOGE(TAG, "Failed to allocate memory for series %d", i);
      for (uint16_t j = 0; j < i; j++)
        free(m_values[j]);
      free(m_values);
      m_values = nullptr;
      m_valuesAllocated = false;
      return;
    }

    for (uint16_t j = 0; j < m_amountPoints; j++)
    {
      m_values[i][j].oldValue = m_config.minValue;
      m_values[i][j].currentValue = m_config.minValue;
    }
  }
  
  ESP_LOGD(TAG, "LineChart initialized: %dx%d, %d series, %d points", 
           m_config.width, m_config.height, m_config.amountSeries, m_amountPoints);

  // Show HEX colors of the series
  for (uint16_t i = 0; i < m_config.amountSeries; i++)
  {
    ESP_LOGD(TAG, "Series %d color: #%06X", i, m_colorsSeries[i]);
  }
  #endif
}

/**
 * @brief Draws the background of the chart including axes, labels, and grid.
 *
 * Handles drawing the chart framework, axes, labels, and grid lines.
 */
void LineChart::drawBackground()
{
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_LOADED_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID
  
  #if defined(DISP_DEFAULT)
  if (!m_shouldRedraw) {
    return;
  }

  ESP_LOGD(TAG, "Drawing LineChart background");

  // Draw border
  for (auto i = 0; i < m_borderSize; ++i)
  {
    WidgetBase::objTFT->drawRect(m_xPos + i, m_yPos + i, m_config.width - (2 * i), m_config.height - (2 * i), m_config.borderColor);
  }

  // Fill background
  WidgetBase::objTFT->fillRect(m_xPos + m_borderSize, m_yPos + m_borderSize, 
                                m_config.width - (2 * m_borderSize), m_config.height - (2 * m_borderSize), 
                                m_config.backgroundColor);

  // Set text font and color
  objTFT->setFont((GFXfont *)0);
  WidgetBase::objTFT->setTextColor(m_config.textColor);
  
  // Draw max value label
  printText(String(m_config.maxValue).c_str(), m_xPos + m_borderSize + m_leftPadding - 4, m_yTovmax - m_topBottomPadding, TR_DATUM);
  
  // Draw min value label
  printText(String(m_config.minValue).c_str(), m_xPos + m_borderSize + m_leftPadding - 4, m_yTovmin, BR_DATUM);
  
  // Draw zero line label if applicable
  if (m_config.showZeroLine && m_config.minValue <= 0 && m_config.maxValue >= 0)
  {
    uint16_t yZero = map(0, m_config.minValue, m_config.maxValue, m_yTovmin, m_yTovmax);
    uint16_t distEntreZeroEMin = abs(m_yTovmin - yZero);
    uint16_t distEntreZeroEMax = abs(yZero - m_yTovmax);
    uint8_t distMinEntreLinhas = 15;
    if (distEntreZeroEMin >= distMinEntreLinhas && distEntreZeroEMax >= distMinEntreLinhas)
    {
      printText("0", m_xPos + m_borderSize + m_leftPadding - 4, yZero, BR_DATUM);
    }
  }
  updateFont(FontType::UNLOAD);

  // Draw axes
  WidgetBase::objTFT->drawFastVLine(m_xPos + m_leftPadding + m_borderSize - 1, m_yPos + m_topBottomPadding + m_borderSize, m_maxHeight + 1, m_config.textColor);
  WidgetBase::objTFT->drawFastHLine(m_xPos + m_leftPadding + m_borderSize - 1, m_yTovmin + 1, m_maxWidth, m_config.textColor);
  #endif
}

/**
 * @brief Adds a new value to the specified series of the chart.
 * @param serieIndex Index of the series to add the value to.
 * @param newValue New value to add to the series.
 * @return True if the value was added successfully, otherwise false.
 *
 * Adds a new data point to a series, shifting existing points and updating the display.
 */
bool LineChart::push(uint16_t serieIndex, int newValue)
{
  #if defined(DISP_DEFAULT)
  if (m_mutex)
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    
  // Validate parameters
  if (!m_values || serieIndex >= m_config.amountSeries || m_amountPoints == 0)
  {
    if (m_mutex)
      xSemaphoreGive(m_mutex);
    ESP_LOGW(TAG, "Invalid push: values=%p, serieIndex=%d, amountSeries=%d, amountPoints=%d", 
             m_values, serieIndex, m_config.amountSeries, m_amountPoints);
    return false;
  }
  
  // Check if loaded and if should work in current screen
  if (!m_loaded || (!m_config.workInBackground && WidgetBase::currentScreen != m_screen))
  {
    if (m_mutex)
      xSemaphoreGive(m_mutex);
    return false;
  }
  
  // Wait if blocked
  while (m_blocked)
  {
    vTaskDelay(pdMS_TO_TICKS(1));
  }
  
  LineChartValue_t *linha = m_values[serieIndex];
  newValue = constrain(newValue, m_config.minValue, m_config.maxValue);
  
  // Shift all values back one position
  for (uint16_t i = 1; i < m_amountPoints; i++)
  {
    linha[i - 1].currentValue = linha[i].currentValue;
  }
  linha[m_amountPoints - 1].currentValue = newValue;

  // Don't update oldValue here - that happens in copyCurrentValuesToOldValues()
  m_shouldRedraw = true;
  
  if (m_mutex)
    xSemaphoreGive(m_mutex);

  return true;
  #else
  return false;
  #endif
}

/**
 * @brief Clears previous plotted values from the chart.
 *
 * Erases all previously plotted lines by drawing over them with the background color.
 */
void LineChart::clearPreviousValues()
{
  CHECK_TFT_VOID
  #if defined(DISP_DEFAULT)
  // Only execute if we can get the mutex immediately
  if (m_mutex && xSemaphoreTake(m_mutex, 0) != pdTRUE)
    return;
    
  // Paint old lines with background color
  uint16_t startXPlot = m_xPos + m_leftPadding + m_borderSize + 1;
  for (uint16_t serieIndex = 0; serieIndex < m_config.amountSeries; serieIndex++)
  {
    LineChartValue_t *linha = m_values[serieIndex];

    for (uint16_t i = 1; i < m_amountPoints; ++i)
    {
      int16_t x0 = round(startXPlot + ((i - 1) * m_spaceBetweenPoints));
      int16_t x1 = round(startXPlot + (i * m_spaceBetweenPoints));
      int16_t y0 = round(map(linha[i - 1].oldValue, m_config.minValue, m_config.maxValue, m_yTovmin, m_yTovmax));
      int16_t y1 = round(map(linha[i].oldValue, m_config.minValue, m_config.maxValue, m_yTovmin, m_yTovmax));
      WidgetBase::objTFT->drawLine(x0, y0, x1, y1, m_config.backgroundColor);
      
      if (m_config.boldLine)
      {
        WidgetBase::objTFT->drawLine(x0, y0 - 1, x1, y1 - 1, m_config.backgroundColor);
        WidgetBase::objTFT->drawLine(x0, y0 + 1, x1, y1 + 1, m_config.backgroundColor);
      }
      
      if (m_config.showDots && m_spaceBetweenPoints >= m_minSpaceToShowDot) {
        WidgetBase::objTFT->fillCircle(x0, y0, m_dotRadius, m_config.backgroundColor);
      }
    }
  }
  
  if (m_mutex)
    xSemaphoreGive(m_mutex);
  #endif
}

/**
 * @brief Draws the grid lines for the chart.
 *
 * Draws both horizontal and vertical gridlines based on the chart configuration.
 */
void LineChart::drawGrid()
{
  CHECK_TFT_VOID
  #if defined(DISP_DEFAULT)
  // Draw grid
  if (m_config.verticalDivision > 0)
  {
    uint16_t sizeVSpace = round(m_maxHeight / m_config.verticalDivision);
    
    // Draw horizontal grid lines
    for (auto i = 0; i < m_config.verticalDivision; ++i)
    {
      uint16_t yPosLine = m_yPos + m_topBottomPadding + (sizeVSpace * i);
      if (yPosLine < m_yPos + m_topBottomPadding + m_maxHeight)
      {
        WidgetBase::objTFT->drawFastHLine(m_xPos + m_leftPadding + 2, yPosLine, m_maxWidth, m_config.gridColor);
      }
    }
    WidgetBase::objTFT->drawFastHLine(m_xPos + m_leftPadding + 2, m_yPos + m_topBottomPadding, m_maxWidth, m_config.gridColor);

    // Draw vertical grid lines
    uint16_t maxX = m_xPos + m_leftPadding + 2 + m_maxWidth;
    for (auto i = (m_xPos + m_leftPadding + 2); i < maxX; i += sizeVSpace)
    {
      WidgetBase::objTFT->drawFastVLine(i, m_yPos + m_topBottomPadding, m_maxHeight, m_config.gridColor);
    }
    WidgetBase::objTFT->drawFastVLine(maxX, m_yPos + m_topBottomPadding, m_maxHeight, m_config.gridColor);
  }
  #endif
}

/**
 * @brief Draws a single data series on the chart.
 * @param serieIndex Index of the series to draw.
 *
 * Plots a single series of data points on the chart.
 */
void LineChart::drawSerie(uint8_t serieIndex)
{
  CHECK_TFT_VOID
  #if defined(DISP_DEFAULT)
  // Only execute if we can get the mutex immediately
  if (m_mutex && xSemaphoreTake(m_mutex, 0) != pdTRUE)
    return;

  uint16_t startXPlot = m_xPos + m_leftPadding + m_borderSize + 1;

  LineChartValue_t *linha = m_values[serieIndex];
  uint16_t colorLine = m_colorsSeries[serieIndex];

  for (auto i = 1; i < m_amountPoints; ++i)
  {
    int16_t x0 = round(startXPlot + ((i - 1) * m_spaceBetweenPoints));
    int16_t x1 = round(startXPlot + (i * m_spaceBetweenPoints));
    int16_t y0 = round(map(linha[i - 1].currentValue, m_config.minValue, m_config.maxValue, m_yTovmin, m_yTovmax));
    int16_t y1 = round(map(linha[i].currentValue, m_config.minValue, m_config.maxValue, m_yTovmin, m_yTovmax));

    WidgetBase::objTFT->drawLine(x0, y0, x1, y1, colorLine);
    
    if (m_config.boldLine)
    {
      WidgetBase::objTFT->drawLine(x0, y0 - 1, x1, y1 - 1, colorLine);
      WidgetBase::objTFT->drawLine(x0, y0 + 1, x1, y1 + 1, colorLine);
    }
    
    if (m_config.showDots && m_spaceBetweenPoints >= m_minSpaceToShowDot) {
      WidgetBase::objTFT->fillCircle(x0, y0, m_dotRadius, colorLine);
    }
  }

  // Update subtitle if provided
  if (m_config.subtitles && m_config.subtitles[serieIndex]) {
    int lastValueOfSerie = linha[m_amountPoints - 1].currentValue;
    m_config.subtitles[serieIndex]->setTextInt(lastValueOfSerie);
  }

  if (m_mutex)
    xSemaphoreGive(m_mutex);
  #endif
}

/**
 * @brief Draws a horizontal marker line at a specific value.
 * @param value The value at which to draw the marker line.
 *
 * Draws a horizontal line across the chart at the specified value position.
 */
void LineChart::drawMarkLineAt(int value)
{
  #if defined(DISP_DEFAULT)
  uint16_t yZero = map(value, m_config.minValue, m_config.maxValue, m_yTovmin, m_yTovmax);
  WidgetBase::objTFT->drawFastHLine(m_xPos + m_leftPadding + m_borderSize, yZero, m_maxWidth, m_config.textColor);
  #endif
}

/**
 * @brief Debug method that prints the values of a series to the ESP log.
 * @param serieIndex Index of the series to print values for.
 *
 * Outputs both old and current values of the specified series to log.
 */
void LineChart::printValues(uint8_t serieIndex)
{
  #if defined(DISP_DEFAULT)
  if (serieIndex >= m_config.amountSeries) {
    ESP_LOGW(TAG, "Invalid serieIndex: %d", serieIndex);
    return;
  }
  
  ESP_LOGD(TAG, "Series %d - Old values:", serieIndex);
  for (uint16_t i = 0; i < m_amountPoints; i++)
  {
    ESP_LOGD(TAG, "  [%d]: %d", i, m_values[serieIndex][i].oldValue);
  }
  
  ESP_LOGD(TAG, "Series %d - Current values:", serieIndex);
  for (uint16_t i = 0; i < m_amountPoints; i++)
  {
    ESP_LOGD(TAG, "  [%d]: %d", i, m_values[serieIndex][i].currentValue);
  }
  #endif
}

/**
 * @brief Draws all data series on the chart.
 *
 * Calls drawSerie for each data series to plot them all on the chart.
 */
void LineChart::drawAllSeries()
{
  #if defined(DISP_DEFAULT)
  for (uint16_t serieIndex = 0; serieIndex < m_config.amountSeries; serieIndex++)
  {
    drawSerie(serieIndex);
  }
  #endif
}

/**
 * @brief Copies current values to old values for all data series.
 *
 * Updates the old values to match the current values before redrawing.
 */
void LineChart::copyCurrentValuesToOldValues()
{
  #if defined(DISP_DEFAULT)
  if (m_mutex && xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE)
  {
    for (uint16_t serieIndex = 0; serieIndex < m_config.amountSeries; serieIndex++)
    {
      LineChartValue_t *linha = m_values[serieIndex];
      for (uint16_t i = 0; i < m_amountPoints; i++)
      {
        linha[i].oldValue = linha[i].currentValue;
      }
    }
    xSemaphoreGive(m_mutex);
  }
  #endif
}

/**
 * @brief Redraws the entire LineChart.
 *
 * Updates the chart display with the current values, clearing old lines and drawing new ones.
 */
void LineChart::redraw()
{
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_LOADED_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_DEBOUNCE_REDRAW_VOID
  
  #if defined(DISP_DEFAULT)
  if (!m_shouldRedraw) {
    return;
  }

  ESP_LOGD(TAG, "Redrawing LineChart");

  m_shouldRedraw = false;
  m_myTime = millis();
  m_blocked = true;

  // Clear old lines
  clearPreviousValues();

  // Copy current values to old values
  copyCurrentValuesToOldValues();

  // Draw grid
  drawGrid();

  // Draw zero line if applicable
  if (m_config.showZeroLine && m_config.minValue <= 0 && m_config.maxValue >= 0)
  {
    drawMarkLineAt(0);
  }

  // Draw all series
  drawAllSeries();

  m_blocked = false;
  #endif
}

/**
 * @brief Forces the LineChart to update.
 *
 * Sets the update flag to trigger a redraw on the next cycle.
 */
void LineChart::forceUpdate()
{
  m_shouldRedraw = true;
}

/**
 * @brief Configures the LineChart using a configuration structure.
 * @param config Structure containing all configuration parameters.
 */
void LineChart::setup(const LineChartConfig &config)
{
  CHECK_TFT_VOID
  if (m_loaded) {
    ESP_LOGW(TAG, "LineChart widget already configured");
    return;
  }
  
  // Validate configuration first
  if (!validateConfig(config)) {
    ESP_LOGE(TAG, "Invalid configuration provided");
    return;
  }
  
  // Assign the configuration structure
  m_config = config;
  
  // Deep copy colorsSeries array to internal buffer
  if (config.colorsSeries && config.amountSeries > 0) {
    for (uint8_t i = 0; i < config.amountSeries && i < MAX_SERIES; ++i) {
      m_colorsSeries[i] = config.colorsSeries[i];
    }
    ESP_LOGD(TAG, "Copied %d colors to internal buffer", config.amountSeries);
  } else {
    ESP_LOGW(TAG, "No colors provided in configuration");
    // Initialize with default colors
    for (uint8_t i = 0; i < MAX_SERIES; ++i) {
      m_colorsSeries[i] = CFK_WHITE;
    }
  }

  // Copy subtitles pointer array (shallow copy of pointers, not Labels themselves)
  if (config.subtitles && config.amountSeries > 0) {
    for (uint8_t i = 0; i < config.amountSeries && i < MAX_SERIES; ++i) {
      m_subtitles[i] = config.subtitles[i];
    }
    m_config.subtitles = m_subtitles;
    ESP_LOGD(TAG, "Copied %d subtitle pointers", config.amountSeries);
  } else {
    for (uint8_t i = 0; i < MAX_SERIES; ++i) {
      m_subtitles[i] = nullptr;
    }
    m_config.subtitles = nullptr;
  }

  
  m_topBottomPadding = 5;
  m_shouldRedraw = true;
  m_blocked = false;

  start();

  m_loaded = true;
  
  ESP_LOGD(TAG, "LineChart setup completed at (%d, %d)", m_xPos, m_yPos);
}

void LineChart::show()
{
  m_visible = true;
  m_shouldRedraw = true;
  ESP_LOGD(TAG, "LineChart shown at (%d, %d)", m_xPos, m_yPos);
}

void LineChart::hide()
{
  m_visible = false;
  m_shouldRedraw = false;
  ESP_LOGD(TAG, "LineChart hidden at (%d, %d)", m_xPos, m_yPos);
}