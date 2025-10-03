#include "wlinechart.h"

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

// #define DEBUG_LINECHART

#if defined(DEBUG_LINECHART)
// Debug macros - uncomment to enable debug messages
#define DEBUG_D(format, ...) log_d(format, ##__VA_ARGS__)
#define DEBUG_E(format, ...) log_e(format, ##__VA_ARGS__)
#define DEBUG_W(format, ...) log_w(format, ##__VA_ARGS__)
#else
// Comment the above and uncomment these to disable debug messages
#define DEBUG_D(format, ...)
#define DEBUG_E(format, ...)
#define DEBUG_W(format, ...)
#endif

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
LineChart::LineChart(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen)
{
  #if defined(DISP_DEFAULT)
  initMutex();
  #endif
}

/**
 * @brief Destructor for the LineChart class.
 *
 * Frees all allocated memory for chart data series.
 */
LineChart::~LineChart()
{
  #if defined(DISP_DEFAULT)
  destroyMutex();
  // Libera arrays de valores
  if (m_values)
  {
    for (uint16_t i = 0; i < m_amountSeries; i++)
    {
      if (m_values[i])
      {
        free(m_values[i]);
        m_values[i] = nullptr;
      }
    }
    free(m_values);
    m_values = nullptr;
  }

  // Libera array de cores se foi alocado dinamicamente
  if (m_colorsSeries)
  {
    // Não liberamos m_colorsSeries aqui, pois é fornecido externamente no setup
    m_colorsSeries = nullptr;
  }
  #endif
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
  return cb;
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
  if (m_values)
  {
    for (uint16_t i = 0; i < m_amountSeries; i++)
    {
      if (m_values[i])
        free(m_values[i]);
    }
    free(m_values);
    m_values = nullptr;
  }
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
  #if defined(DISP_DEFAULT)
  if (m_vmax < m_vmin)
  {
    std::swap(m_vmin, m_vmax);
  }

  WidgetBase::objTFT->setFont(m_letra);


  int16_t x, y;
  uint16_t textSizeMin, textSizeMax, h;
  char char_arr_min[20];
  char char_arr_max[20];
  snprintf(char_arr_min, sizeof(char_arr_min), "%d", m_vmin);
  snprintf(char_arr_max, sizeof(char_arr_max), "%d", m_vmax);

  objTFT->getTextBounds(char_arr_min, 10, 10, &x, &y, &textSizeMin, &h); // 10 é valor simbolico
  objTFT->getTextBounds(char_arr_max, 10, 10, &x, &y, &textSizeMax, &h); // 10 é valor simbolico

  int16_t biggerText = max(textSizeMin, textSizeMax);


  updateFont(FontType::UNLOAD);

  biggerText += 5;

  m_maxAmountValues = m_width - (biggerText + 10) - (m_borderSize * 2);
  m_amountPoints = CLAMP(m_amountPoints, 2, m_maxAmountValues);

  m_spaceBetweenPoints = m_maxAmountValues / (float)(m_amountPoints - 1);
  if(m_spaceBetweenPoints < m_minSpaceToShowDot){
    m_showDots = false;
    DEBUG_D("Space between points is too small to show dots");
  }
  DEBUG_D("MaxWid: %d, Espaco entre pontos %.2f", m_maxAmountValues, m_spaceBetweenPoints);

  m_maxWidth = m_maxAmountValues;
  m_maxHeight = m_height - (m_topBottomPadding * 2) - (m_borderSize * 2);

  if (m_verticalDivision > m_maxHeight)
  {
    m_verticalDivision = 0;
    DEBUG_W("Amount of vertical divisions is too large. The value 0 was set.");
  }

  uint16_t restoAreaPlot = 0;
  // Serial.printf("GridCells MaxH:%u\tQtd:%u\tEspaco:%u\tOcupado:%u\tResto:%u\n", _maxHeight, verticalDivision, sizeVSpace, sizeVSpace * verticalDivision, _maxHeight - (sizeVSpace * verticalDivision));
  if (m_verticalDivision > 0)
  {
    uint16_t sizeVSpace = round(m_maxHeight / m_verticalDivision);
    DEBUG_D("Calculando GridCells MaxH:%u\tQtd:%u\tEspaco:%u\tOcupado:%u\tResto:%u\n", m_maxHeight, m_verticalDivision, sizeVSpace, sizeVSpace * m_verticalDivision, m_maxHeight - (sizeVSpace * m_verticalDivision));
    restoAreaPlot = m_maxHeight - (sizeVSpace * m_verticalDivision);
    if (restoAreaPlot > 0)
    {
      m_maxHeight -= restoAreaPlot;
    }
    sizeVSpace = round(m_maxHeight / m_verticalDivision);
    DEBUG_D("Recalculo GridCells MaxH:%u\tQtd:%u\tEspaco:%u\tOcupado:%u\tResto:%u\n", m_maxHeight, m_verticalDivision, sizeVSpace, sizeVSpace * m_verticalDivision, m_maxHeight - (sizeVSpace * m_verticalDivision));
  }

  m_leftPadding = biggerText;

  m_yTovmin = (yPos + m_height) - (m_topBottomPadding + m_borderSize + restoAreaPlot);
  m_yTovmax = yPos + m_topBottomPadding + m_borderSize;

  resetArray();

  m_values = (LineChartValue_t **)malloc(sizeof(LineChartValue_t *) * m_amountSeries);
  if (!m_values)
    return;

  for (uint16_t i = 0; i < m_amountSeries; i++)
  {
    m_values[i] = (LineChartValue_t *)malloc(sizeof(LineChartValue_t) * m_amountPoints);
    if (!m_values[i])
    {
      for (uint16_t j = 0; j < i; j++)
        free(m_values[j]);
      free(m_values);
      m_values = nullptr;
      return;
    }

    for (uint16_t j = 0; j < m_amountPoints; j++)
    {
      m_values[i][j].oldValue = m_vmin;
      m_values[i][j].currentValue = m_vmin;
    }
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
#if defined(DISP_DEFAULT)
  if (WidgetBase::currentScreen != screen || WidgetBase::usingKeyboard == true || !m_update || !loaded)
  {
    return;
  }

  // uint16_t darkBg = WidgetBase::lightMode ? CFK_GREY3 : CFK_GREY11;
  // uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
  // uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

  DEBUG_D("Drawing LineChart background");

  for (auto i = 0; i < m_borderSize; ++i)
  {
    WidgetBase::objTFT->drawRect(xPos + i, yPos + i, m_width - (2 * i), m_height - (2 * i), m_borderColor);
  }

  WidgetBase::objTFT->fillRect(xPos + m_borderSize, yPos + m_borderSize, m_width - (2 * m_borderSize), m_height - (2 * m_borderSize), m_backgroundColor);
  WidgetBase::objTFT->setTextColor(CFK_RED);

  objTFT->setFont((GFXfont *)0);

  WidgetBase::objTFT->setTextColor(m_textColor);
  printText(String(m_vmax).c_str(), xPos + m_borderSize + m_leftPadding - 4, m_yTovmax - m_topBottomPadding, TR_DATUM);
  printText(String(m_vmin).c_str(), xPos + m_borderSize + m_leftPadding - 4, m_yTovmin, BR_DATUM);
  if (m_showZeroLine && m_vmin <= 0 && m_vmax >= 0)
  {
    uint16_t yZero = map(0, m_vmin, m_vmax, m_yTovmin, m_yTovmax);
    uint16_t distEntreZeroEMin = abs(m_yTovmin - yZero);
    uint16_t distEntreZeroEMax = abs(yZero - m_yTovmax);
    uint8_t distMinEntreLinhas = 15;
    if (distEntreZeroEMin >= distMinEntreLinhas && distEntreZeroEMax >= distMinEntreLinhas)
    {
      printText("0", xPos + m_borderSize + m_leftPadding - 4, yZero, BR_DATUM);
    }
  }
  updateFont(FontType::UNLOAD);

  WidgetBase::objTFT->drawFastVLine(xPos + m_leftPadding + m_borderSize - 1, yPos + m_topBottomPadding + m_borderSize, m_maxHeight + 1, m_textColor); // linhaVertical
  WidgetBase::objTFT->drawFastHLine(xPos + m_leftPadding + m_borderSize - 1, m_yTovmin + 1, m_maxWidth, m_textColor); // linhaVertical
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
    xSemaphoreTake(m_mutex, portMAX_DELAY); // prioridade total para push
  if (!m_values || serieIndex >= m_amountSeries || m_amountPoints == 0)
  {
    if (m_mutex)
      xSemaphoreGive(m_mutex);
    return false;
  }
  if (!loaded || (!m_workInBackground && WidgetBase::currentScreen != screen))
  {
    if (m_mutex)
      xSemaphoreGive(m_mutex);
    return false;
  }
  while (m_blocked)
  {
    vTaskDelay(pdMS_TO_TICKS(1));
  }
  LineChartValue_t *linha = m_values[serieIndex];
  newValue = constrain(newValue, m_vmin, m_vmax);
  // Desloca tudo uma posição para trás
  for (uint16_t i = 1; i < m_amountPoints; i++)
  {
    linha[i - 1].currentValue = linha[i].currentValue;
  }
  linha[m_amountPoints - 1].currentValue = newValue;

  

  // NÃO atualiza oldValue aqui!
  m_update = true;
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
 #if defined(DISP_DEFAULT)
  if (m_mutex && xSemaphoreTake(m_mutex, 0) != pdTRUE)
    return; // só executa se conseguir pegar o mutex imediatamente
            // Pinta a linha antiga com a cor do fundo
  uint16_t startXPlot = xPos + m_leftPadding + m_borderSize + 1;
  for (uint16_t serieIndex = 0; serieIndex < m_amountSeries; serieIndex++)
  {
    LineChartValue_t *linha = m_values[serieIndex];

    for (uint16_t i = 1; i < m_amountPoints; ++i)
    {
      int16_t x0 = round(startXPlot + ((i - 1) * m_spaceBetweenPoints));
      int16_t x1 = round(startXPlot + (i * m_spaceBetweenPoints));
      int16_t y0 = round(map(linha[i - 1].oldValue, m_vmin, m_vmax, m_yTovmin, m_yTovmax));
      int16_t y1 = round(map(linha[i].oldValue, m_vmin, m_vmax, m_yTovmin, m_yTovmax));
      WidgetBase::objTFT->drawLine(x0, y0, x1, y1, m_backgroundColor);
      if (m_boldLine)
      {
        WidgetBase::objTFT->drawLine(x0, y0 - 1, x1, y1 - 1, m_backgroundColor);
        WidgetBase::objTFT->drawLine(x0, y0 + 1, x1, y1 + 1, m_backgroundColor);
      }
      if(m_showDots){
        WidgetBase::objTFT->fillCircle(x0, y0, m_dotRadius, m_backgroundColor);
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
  #if defined(DISP_DEFAULT)
  // Desenha a grade
  if (m_verticalDivision > 0)
  {

    uint16_t sizeVSpace = round(m_maxHeight / m_verticalDivision);
    // log_d("Drawing grid, the size of the space between the lines is %u", sizeVSpace);
    // Serial.printf("GridCells MaxH:%u\tQtd:%u\tEspaco:%u\tOcupado:%u\tResto:%u\n", _maxHeight, verticalDivision, sizeVSpace, sizeVSpace * verticalDivision, _maxHeight - (sizeVSpace * verticalDivision));
    for (auto i = 0; i < m_verticalDivision; ++i)
    {
      uint16_t yPosLine = yPos + m_topBottomPadding + (sizeVSpace * i);
      if (yPosLine < yPos + m_topBottomPadding + m_maxHeight)
      {
        WidgetBase::objTFT->drawFastHLine(xPos + m_leftPadding + 2, yPosLine, m_maxWidth, m_gridColor);
      }
    }
    WidgetBase::objTFT->drawFastHLine(xPos + m_leftPadding + 2, yPos + m_topBottomPadding, m_maxWidth, m_gridColor);

    uint16_t maxX = xPos + m_leftPadding + 2 + m_maxWidth;
    for (auto i = (xPos + m_leftPadding + 2); i < maxX; i += sizeVSpace)
    {
      // uint16_t xPosLine = xPos + leftPadding + 2 + (sizeVSpace * i);
      WidgetBase::objTFT->drawFastVLine(i, yPos + m_topBottomPadding, m_maxHeight, m_gridColor);
    }
    WidgetBase::objTFT->drawFastVLine(maxX, yPos + m_topBottomPadding, m_maxHeight, m_gridColor);
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
 #if defined(DISP_DEFAULT)
  if (m_mutex && xSemaphoreTake(m_mutex, 0) != pdTRUE)
    return; // só executa se conseguir pegar o mutex imediatamente

  
  uint16_t startXPlot = xPos + m_leftPadding + m_borderSize + 1;

  LineChartValue_t *linha = m_values[serieIndex];
  uint16_t colorLine = m_colorsSeries[serieIndex];

  for (auto i = 1; i < m_amountPoints; ++i)
  {
    int16_t x0 = round(startXPlot + ((i - 1) * m_spaceBetweenPoints));
    int16_t x1 = round(startXPlot + (i * m_spaceBetweenPoints));
    int16_t y0 = round(map(linha[i - 1].currentValue, m_vmin, m_vmax, m_yTovmin, m_yTovmax));
    int16_t y1 = round(map(linha[i].currentValue, m_vmin, m_vmax, m_yTovmin, m_yTovmax));

    WidgetBase::objTFT->drawLine(x0, y0, x1, y1, colorLine);
    if (m_boldLine)
    {
      WidgetBase::objTFT->drawLine(x0, y0 - 1, x1, y1 - 1, colorLine);
      WidgetBase::objTFT->drawLine(x0, y0 + 1, x1, y1 + 1, colorLine);
    }
    if(m_showDots){
      WidgetBase::objTFT->fillCircle(x0, y0, m_dotRadius, colorLine);
    }
  }

  
  
  if(m_config.subtitles  && m_config.subtitles[serieIndex]){
    int lasValueOfSerie = linha[m_amountPoints - 1].currentValue;
    m_config.subtitles[serieIndex]->setTextInt(lasValueOfSerie);
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
  uint16_t yZero = map(value, m_vmin, m_vmax, m_yTovmin, m_yTovmax);
  WidgetBase::objTFT->drawFastHLine(xPos + m_leftPadding + m_borderSize, yZero, m_maxWidth, m_textColor); // linha 0
  #endif
}

/**
 * @brief Debug method that prints the values of a series to the Serial monitor.
 * @param serieIndex Index of the series to print values for.
 *
 * Outputs both old and current values of the specified series to Serial.
 */
void LineChart::printValues(uint8_t serieIndex)
{
  #if defined(DISP_DEFAULT)
  Serial.printf("Old: ");
  for (uint16_t i = 0; i < m_amountPoints; i++)
  {
    Serial.printf("%d\t", m_values[serieIndex][i].oldValue);
  }
  Serial.println();
  Serial.printf("Current: ");
  for (uint16_t i = 0; i < m_amountPoints; i++)
  {
    Serial.printf("%d\t", m_values[serieIndex][i].currentValue);
  }
  Serial.println();
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
  for (uint16_t serieIndex = 0; serieIndex < m_amountSeries; serieIndex++)
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
    for (uint16_t serieIndex = 0; serieIndex < m_amountSeries; serieIndex++)
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
  #if defined(DISP_DEFAULT)
  if (WidgetBase::currentScreen != screen || WidgetBase::usingKeyboard == true || !m_update || !loaded)
  {
    return;
  }

  if (millis() - m_myTime < 5)
  {
    return;
  }
  uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
  DEBUG_D("Redrawing LineChart");
  uint32_t startTime = micros();

  m_update = false;
  m_myTime = millis();
  m_blocked = true;

  // Apaga as linhas antigas
  clearPreviousValues();

  // Copia os valores atuais para os valores antigos
  copyCurrentValuesToOldValues();

  // Desenha a grade
  drawGrid();

  // Desenha a linha 0
  if (m_showZeroLine && m_vmin <= 0 && m_vmax >= 0)
  {
    drawMarkLineAt(0);
  }

  // Pinta as linhas
  drawAllSeries();

  uint32_t endTime = micros();
  DEBUG_D("Redrawing LineChart took %u us", endTime - startTime);

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
  m_update = true;
}

/**
 * @brief Configures the LineChart with the specified parameters.
 * @param _width Width of the chart.
 * @param _height Height of the chart.
 * @param _vmin Minimum value for the chart range.
 * @param _vmax Maximum value for the chart range.
 * @param _amountSeries Number of data series to display.
 * @param _colorsSeries Array of colors for each series.
 * @param _gridColor Color of the grid lines.
 * @param _borderColor Color of the chart border.
 * @param _backgroundColor Background color of the chart.
 * @param _textColor Color of text displayed on the chart.
 * @param _verticalDivision Number of vertical division lines.
 * @param _workInBackground Flag to enable background updates.
 * @param _showZeroLine Flag to show a line at value zero.
 * @param _boldLine Flag to make the line bold.
 * @param _showDots Flag to show the dots on the line.
 * @param _maxPointsAmount Maximum number of points to show on the chart.
 * @param _font Font used for text on the chart.
 */
  #if defined(USING_GRAPHIC_LIB)
void LineChart::setup(uint16_t _width, uint16_t _height, int _vmin, int _vmax, uint8_t _amountSeries, uint16_t *_colorsSeries, uint16_t _gridColor, uint16_t _borderColor, uint16_t _backgroundColor, uint16_t _textColor, uint16_t _verticalDivision, bool _workInBackground, bool _showZeroLine, bool _boldLine, bool _showDots, uint16_t _maxPointsAmount, const GFXfont *_font)
{
  if (!WidgetBase::objTFT)
  {
    DEBUG_E("TFT not defined on WidgetBase");
    return;
  }
  if (loaded)
  {
    DEBUG_E("LineChart widget already configured");
    return;
  }
  /*if(_amount < 2 || _amount > 100){
    DEBUG_E("LineChart can storage amount of values between 2 and 100. Check your parameters value.");
  }*/

  m_width = _width;
  m_height = _height;
  m_vmin = _vmin;
  m_vmax = _vmax;
  m_amountSeries = _amountSeries;
  m_colorsSeries = _colorsSeries;
  m_backgroundColor = _backgroundColor;
  m_textColor = _textColor;
  m_showZeroLine = _showZeroLine;

  m_gridColor = _gridColor;
  m_borderColor = _borderColor;
  m_verticalDivision = _verticalDivision;
  m_topBottomPadding = 5;
  m_workInBackground = _workInBackground;

  m_amountPoints = _maxPointsAmount;

  m_boldLine = _boldLine;
  m_showDots = _showDots;

  //_amountPoints = CLAMP(_amount, 2, 100);
  m_update = true;
  m_blocked = false;

  if (_font)
  {
    m_letra = _font;
  }
  else
  {
    m_letra = &RobotoRegular10pt7b;
  }

  start();

  loaded = true;
}
#endif
/**
 * @brief Configures the LineChart using a configuration structure.
 * @param config Structure containing all configuration parameters.
 */
void LineChart::setup(const LineChartConfig &config)
{
  m_config = config;

    #if defined(USING_GRAPHIC_LIB)
  setup(config.width, config.height, config.minValue, config.maxValue, config.amountSeries,
        config.colorsSeries, config.gridColor, config.borderColor, config.backgroundColor,
        config.textColor, config.verticalDivision, config.workInBackground, config.showZeroLine,
        config.boldLine, config.showDots, config.maxPointsAmount,
        config.font);
    #endif
}
