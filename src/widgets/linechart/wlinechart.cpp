#include "wlinechart.h"

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

// Static member initialization
const char* LineChart::TAG = "LineChart";


/**
 * @brief Inicializa o mutex para proteção de acesso aos dados.
 * @details Cria um mutex usando FreeRTOS se DISP_DEFAULT está definido.
 */
void LineChart::initMutex()
{
  #if defined(DISP_DEFAULT)
  if (!m_mutex)
  {
    m_mutex = xSemaphoreCreateMutex();
  }
  #endif
}

/**
 * @brief Destrói o mutex de proteção de dados.
 * @details Libera recursos do mutex criado por initMutex().
 */
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
 * @brief Construtor da classe LineChart.
 * @param _x Posição X do widget.
 * @param _y Posição Y do widget.
 * @param _screen Número da tela.
 * @details Inicializa o widget com valores padrão e configuração vazia.
 *          O gráfico não será funcional até que setup() seja chamado.
 *          Inicializa o mutex para proteção de acesso aos dados.
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
 * @brief Destrutor da classe LineChart.
 * @details Libera toda memória alocada para séries de dados do gráfico.
 *          Chama cleanupMemory(), clearColorsSeries() e clearSubtitles().
 *          Destrói o mutex para liberar recursos.
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
 * @brief Método centralizado de limpeza de memória.
 * @details Desaloca com segurança toda memória alocada dinamicamente com rastreamento adequado.
 *          Chama clearValues() para liberar arrays de valores.
 */
void LineChart::cleanupMemory()
{
  #if defined(DISP_DEFAULT)
  // Clean up values array
  clearValues();
  
  #endif
}

/**
 * @brief Limpa o array interno de cores das séries.
 * @details Reseta todos os valores do array m_colorsSeries para zero.
 */
void LineChart::clearColorsSeries()
{
  ESP_LOGD(TAG, "Resetting internal color array");
  for (uint8_t i = 0; i < MAX_SERIES; i++) {
    m_colorsSeries[i] = 0;
  }
}

/**
 * @brief Limpa o array interno de ponteiros para legendas.
 * @details Reseta todos os ponteiros do array m_subtitles para nullptr.
 */
void LineChart::clearSubtitles()
{
  ESP_LOGD(TAG, "Resetting internal subtitles array");
  for (uint8_t i = 0; i < MAX_SERIES; i++) {
    m_subtitles[i] = nullptr;
  }
}

/**
 * @brief Limpa e desaloca o array de valores das séries.
 * @details Libera memória do array 2D m_values se foi alocado.
 *          Usa flags m_valuesAllocated para rastreamento seguro.
 */
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
 * @brief Valida a configuração do LineChart.
 * @param config Estrutura de configuração a validar.
 * @return True se a configuração é válida, False caso contrário.
 * @details Verifica se todos os parâmetros essenciais estão dentro dos limites aceitáveis:
 *          - Dimensões devem ser maiores que zero
 *          - minValue deve ser menor que maxValue
 *          - amountSeries deve estar entre 1 e MAX_SERIES
 *          - colorsSeries array não pode ser nulo
 *          - maxPointsAmount deve ser pelo menos 2
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
 * @brief Detecta eventos de toque no LineChart.
 * @param _xTouch Ponteiro para a coordenada X do toque.
 * @param _yTouch Ponteiro para a coordenada Y do toque.
 * @return Sempre retorna False, pois LineChart não responde a eventos de toque.
 * @details Este método é implementado para conformidade com a interface de WidgetBase
 *          mas não processa toques, já que gráficos são elementos apenas visuais.
 */
bool LineChart::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
  return false;
}

/**
 * @brief Recupera a função callback associada ao LineChart.
 * @return Ponteiro para a função callback.
 * @details Retorna o ponteiro para a função que será executada quando o gráfico for tocado.
 *          Note que LineChart não detecta toques por padrão.
 */
functionCB_t LineChart::getCallbackFunc()
{
  return m_callback;
}

/**
 * @brief Reseta e libera todos os arrays de dados.
 * @details Este método limpa memória alocada para séries de dados do gráfico.
 *          Usa mutex para acesso thread-safe e chama cleanupMemory().
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
 * @brief Inicializa parâmetros do gráfico e prepara estruturas de dados.
 * @details Calcula dimensões do layout, prepara dimensões da grade e inicializa arrays de dados:
 *          - Calcula tamanho de texto para rótulos min/max
 *          - Calcula espaçamento entre pontos
 *          - Calcula dimensões de plotagem disponíveis
 *          - Aloca memória para arrays de valores
 *          - Inicializa valores com minValue
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
 * @brief Desenha o fundo do gráfico incluindo eixos, rótulos e grade.
 * @details Manipula desenho da estrutura do gráfico, eixos, rótulos e linhas da grade:
 *          - Desenha borda do gráfico
 *          - Preenche fundo
 *          - Desenha rótulos de valores mínimo, máximo e zero (se aplicável)
 *          - Desenha eixos
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
 * @brief Adiciona um novo valor à série especificada do gráfico.
 * @param serieIndex Índice da série para adicionar o valor.
 * @param newValue Novo valor para adicionar à série.
 * @return True se o valor foi adicionado com sucesso, False caso contrário.
 * @details Adiciona novo ponto de dados a uma série, deslocando pontos existentes:
 *          - Restringe valor dentro da faixa min/max
 *          - Desloca todos os valores uma posição para trás
 *          - Insere novo valor na última posição
 *          - Atualiza flag de redesenho
 *          - Usa mutex para acesso thread-safe
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
 * @brief Limpa valores anteriormente plotados do gráfico.
 * @details Apaga todas as linhas anteriormente plotadas desenhando sobre elas com cor de fundo:
 *          - Desenha linhas antigas com cor de fundo
 *          - Suporta linhas em negrito
 *          - Remove pontos se showDots estiver habilitado
 *          - Usa mutex para acesso thread-safe (não-bloqueante)
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
 * @brief Desenha as linhas da grade do gráfico.
 * @details Desenha linhas horizontais e verticais da grade baseado na configuração:
 *          - Linhas horizontais baseadas em verticalDivision
 *          - Linhas verticais espaçadas igualmente
 *          - Usa gridColor para desenhar as linhas
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
 * @brief Desenha uma única série de dados no gráfico.
 * @param serieIndex Índice da série para desenhar.
 * @details Plota uma única série de pontos de dados no gráfico:
 *          - Desenha linhas conectando pontos consecutivos
 *          - Suporta linhas em negrito desenhando múltiplas linhas
 *          - Desenha pontos se showDots estiver habilitado
 *          - Atualiza legendas se configuradas
 *          - Usa mutex para acesso thread-safe (não-bloqueante)
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
 * @brief Desenha uma linha marcadora horizontal em um valor específico.
 * @param value O valor no qual desenhar a linha marcadora.
 * @details Desenha uma linha horizontal através do gráfico na posição de valor especificada.
 *          Usado para marcar a linha zero ou outros valores importantes.
 */
void LineChart::drawMarkLineAt(int value)
{
  #if defined(DISP_DEFAULT)
  uint16_t yZero = map(value, m_config.minValue, m_config.maxValue, m_yTovmin, m_yTovmax);
  WidgetBase::objTFT->drawFastHLine(m_xPos + m_leftPadding + m_borderSize, yZero, m_maxWidth, m_config.textColor);
  #endif
}

/**
 * @brief Método de debug que imprime os valores de uma série no log do ESP.
 * @param serieIndex Índice da série para imprimir valores.
 * @details Exibe valores antigos e atuais da série especificada no log.
 *          Útil para depuração e verificação de dados do gráfico.
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
 * @brief Desenha todas as séries de dados no gráfico.
 * @details Chama drawSerie para cada série de dados para plotar todas no gráfico.
 *          Itera sobre todas as séries configuradas e as desenha uma por uma.
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
 * @brief Copia valores atuais para valores antigos para todas as séries de dados.
 * @details Atualiza os valores antigos para corresponder aos valores atuais antes de redesenhar.
 *          Usado para limpar linhas antigas na próxima atualização. Usa mutex para acesso thread-safe.
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
 * @brief Redesenha o LineChart completo.
 * @details Atualiza o display do gráfico com valores atuais, limpando linhas antigas e desenhando novas:
 *          - Limpa linhas anteriores desenhando com cor de fundo
 *          - Copia valores atuais para antigos
 *          - Desenha grade
 *          - Desenha linha zero se aplicável
 *          - Desenha todas as séries
 *          - Usa flags de bloqueio para evitar atualizações durante redesenho
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
 * @brief Força o LineChart a atualizar.
 * @details Define a flag de atualização para disparar um redesenho no próximo ciclo.
 */
void LineChart::forceUpdate()
{
  m_shouldRedraw = true;
}

/**
 * @brief Configura o LineChart usando uma estrutura de configuração.
 * @param config Estrutura @ref LineChartConfig contendo todos os parâmetros de configuração.
 * @details Este método deve ser chamado após criar o objeto para configurá-lo adequadamente:
 *          - Valida a configuração usando validateConfig()
 *          - Atribui a estrutura de configuração
 *          - Copia array de cores para buffer interno
 *          - Copia ponteiros de legendas
 *          - Chama start() para inicializar estruturas e cálculos
 *          - Marca o widget como carregado
 *          O gráfico não será exibido corretamente até que este método seja chamado.
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

/**
 * @brief Torna o gráfico de linhas visível na tela.
 * @details Define o widget como visível e marca para redesenho.
 */
void LineChart::show()
{
  m_visible = true;
  m_shouldRedraw = true;
  ESP_LOGD(TAG, "LineChart shown at (%d, %d)", m_xPos, m_yPos);
}

/**
 * @brief Oculta o gráfico de linhas da tela.
 * @details Define o widget como invisível e desativa o redesenho.
 */
void LineChart::hide()
{
  m_visible = false;
  m_shouldRedraw = false;
  ESP_LOGD(TAG, "LineChart hidden at (%d, %d)", m_xPos, m_yPos);
}