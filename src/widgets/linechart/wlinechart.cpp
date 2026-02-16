#include "wlinechart.h"

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

// Static member initialization
const char* LineChart::TAG = "LineChart";

/**
 * @brief Inicializa o mutex para proteção de acesso aos dados.
 */
void LineChart::initMutex()
{
#if defined(DISP_DEFAULT)
  if (!m_mutex) {
    m_mutex = xSemaphoreCreateMutex();
  }
#endif
}

/**
 * @brief Destrói o mutex de proteção de dados.
 */
void LineChart::destroyMutex()
{
#if defined(DISP_DEFAULT)
  if (m_mutex) {
    vSemaphoreDelete(m_mutex);
    m_mutex = nullptr;
  }
#endif
}

/**
 * @brief Construtor da classe LineChart.
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
    m_borderSize(2),
    m_yTovmin(0),
    m_yTovmax(0),
    m_shouldRedraw(true),
    m_dataVersion(0),
    m_ringValues(nullptr),
    m_snapshotValues(nullptr),
    m_lastDrawnValues(nullptr),
    m_mutex(nullptr)
{
  // Default config
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

  for (uint8_t i = 0; i < MAX_SERIES; i++) {
    m_colorsSeries[i] = 0;
    m_subtitles[i] = nullptr;
    m_headBySeries[i] = 0;
  }

#if defined(DISP_DEFAULT)
  initMutex();
#endif

  ESP_LOGD(TAG, "LineChart created at (%d, %d) on screen %d", _x, _y, _screen);
}

/**
 * @brief Destrutor.
 */
LineChart::~LineChart()
{
  ESP_LOGD(TAG, "LineChart destroyed at (%d, %d)", m_xPos, m_yPos);

#if defined(DISP_DEFAULT)
  cleanupMemory();
  clearColorsSeries();
  clearSubtitles();
  destroyMutex();
#endif

  if (m_callback != nullptr) {
    m_callback = nullptr;
  }

  m_loaded = false;
  m_shouldRedraw = false;
}

/**
 * @brief Limpeza centralizada de memória.
 */
void LineChart::cleanupMemory()
{
#if defined(DISP_DEFAULT)
  clearBuffers();
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

/**
 * @brief Libera buffers alocados (ring/snapshot/lastDrawn).
 */
void LineChart::clearBuffers()
{
  ESP_LOGD(TAG, "Clearing LineChart buffers");

  auto free2D = [&](int** &ptr) {
    if (!ptr) return;
    uint16_t maxSeries = (m_config.amountSeries > 0 && m_config.amountSeries <= MAX_SERIES) ?
                         m_config.amountSeries : MAX_SERIES;
    for (uint16_t i = 0; i < maxSeries; i++) {
      if (ptr[i]) {
        free(ptr[i]);
        ptr[i] = nullptr;
      }
    }
    free(ptr);
    ptr = nullptr;
  };

  free2D(m_ringValues);
  free2D(m_snapshotValues);
  free2D(m_lastDrawnValues);

}

/**
 * @brief Valida a configuração do LineChart.
 */
bool LineChart::validateConfig(const LineChartConfig& config)
{
  if (config.width == 0 || config.height == 0) {
    ESP_LOGE(TAG, "Invalid dimensions: width=%d, height=%d", config.width, config.height);
    return false;
  }
  if (config.minValue >= config.maxValue) {
    ESP_LOGE(TAG, "Invalid range: minValue=%d must be less than maxValue=%d", config.minValue, config.maxValue);
    return false;
  }
  if (config.amountSeries == 0) {
    ESP_LOGE(TAG, "Invalid amountSeries: must be greater than 0");
    return false;
  }
  if (config.amountSeries > MAX_SERIES) {
    ESP_LOGW(TAG, "Too many series: %d (max: %d)", config.amountSeries, MAX_SERIES);
    return false;
  }
  if (config.colorsSeries == nullptr) {
    ESP_LOGE(TAG, "colorsSeries array is null");
    return false;
  }
  if (config.maxPointsAmount < 2) {
    ESP_LOGE(TAG, "Invalid maxPointsAmount: %d (must be at least 2)", config.maxPointsAmount);
    return false;
  }
  return true;
}

bool LineChart::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
  return false;
}

functionCB_t LineChart::getCallbackFunc()
{
  return m_callback;
}

/**
 * @brief Inicializa parâmetros e buffers.
 */
void LineChart::start()
{
  CHECK_TFT_VOID
#if defined(DISP_DEFAULT)

  // Ensure min/max in order
  if (m_config.maxValue < m_config.minValue) {
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
  biggerText += 10;

  m_maxAmountValues = m_config.width - (biggerText + 10) - (m_borderSize * 2);
  m_amountPoints = CLAMP(m_config.maxPointsAmount, 2, m_maxAmountValues);

  m_spaceBetweenPoints = m_maxAmountValues / (float)(m_amountPoints - 1);
  if (m_spaceBetweenPoints < m_minSpaceToShowDot) {
    ESP_LOGD(TAG, "Space between points (%.2f) is too small to show dots", m_spaceBetweenPoints);
  }

  m_maxWidth  = m_maxAmountValues;
  m_maxHeight = m_config.height - (m_topBottomPadding * 2) - (m_borderSize * 2);

  if (m_config.verticalDivision > m_maxHeight) {
    ESP_LOGW(TAG, "Amount of vertical divisions (%d) is too large for height (%d)", m_config.verticalDivision, m_maxHeight);
  }

  uint16_t restoAreaPlot = 0;
  if (m_config.verticalDivision > 0) {
    uint16_t sizeVSpace = round(m_maxHeight / m_config.verticalDivision);
    restoAreaPlot = m_maxHeight - (sizeVSpace * m_config.verticalDivision);
    if (restoAreaPlot > 0) {
      m_maxHeight -= restoAreaPlot;
    }
  }

  m_leftPadding = biggerText;
  m_yTovmin = (m_yPos + m_config.height) - (m_topBottomPadding + m_borderSize + restoAreaPlot);
  m_yTovmax = m_yPos + m_topBottomPadding + m_borderSize;

  // (Re)allocate buffers
  if (m_mutex) xSemaphoreTake(m_mutex, portMAX_DELAY);
  clearBuffers();

  // Allocate 2D arrays
  auto alloc2D = [&](int** &ptr) -> bool {
    ptr = (int**)malloc(sizeof(int*) * m_config.amountSeries);
    if (!ptr) return false;
    for (uint16_t i = 0; i < m_config.amountSeries; i++) {
      ptr[i] = (int*)malloc(sizeof(int) * m_amountPoints);
      if (!ptr[i]) {
        for (uint16_t j = 0; j < i; j++) free(ptr[j]);
        free(ptr);
        ptr = nullptr;
        return false;
      }
    }
    return true;
  };

  if (!alloc2D(m_ringValues) || !alloc2D(m_snapshotValues) || !alloc2D(m_lastDrawnValues)) {
    ESP_LOGE(TAG, "Failed to allocate buffers");
    clearBuffers();
    if (m_mutex) xSemaphoreGive(m_mutex);
    return;
  }

  // Initialize buffers with minValue
  for (uint16_t s = 0; s < m_config.amountSeries; s++) {
    for (uint16_t p = 0; p < m_amountPoints; p++) {
      m_ringValues[s][p]     = m_config.minValue;
      m_snapshotValues[s][p] = m_config.minValue;
      m_lastDrawnValues[s][p]= m_config.minValue;
    }
  }

  m_dataVersion = 0;
  m_shouldRedraw = true;

  if (m_mutex) xSemaphoreGive(m_mutex);

  ESP_LOGD(TAG, "LineChart initialized: %dx%d, %d series, %d points",
           m_config.width, m_config.height, m_config.amountSeries, m_amountPoints);

  for (uint16_t i = 0; i < m_config.amountSeries; i++) {
    ESP_LOGD(TAG, "Series %d color: #%06X", i, m_colorsSeries[i]);
  }
#endif
}

void LineChart::drawBackground()
{
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_LOADED_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID

#if defined(DISP_DEFAULT)
  // Desenho do "frame" só precisa acontecer quando houver redraw marcado.
  if (!m_shouldRedraw) return;

  ESP_LOGD(TAG, "Drawing LineChart background");

  for (auto i = 0; i < m_borderSize; ++i) {
    WidgetBase::objTFT->drawRect(m_xPos + i, m_yPos + i,
                                m_config.width - (2 * i), m_config.height - (2 * i),
                                m_config.borderColor);
  }

  WidgetBase::objTFT->fillRect(m_xPos + m_borderSize, m_yPos + m_borderSize,
                               m_config.width - (2 * m_borderSize),
                               m_config.height - (2 * m_borderSize),
                               m_config.backgroundColor);

  objTFT->setFont((GFXfont *)0);
  WidgetBase::objTFT->setTextColor(m_config.textColor);

  printText(String(m_config.maxValue).c_str(),
            m_xPos + m_borderSize + m_leftPadding - 4,
            m_yTovmax - m_topBottomPadding, TR_DATUM);

  printText(String(m_config.minValue).c_str(),
            m_xPos + m_borderSize + m_leftPadding - 4,
            m_yTovmin, BR_DATUM);

  if (m_config.showZeroLine && m_config.minValue <= 0 && m_config.maxValue >= 0) {
    uint16_t yZero = map(0, m_config.minValue, m_config.maxValue, m_yTovmin, m_yTovmax);
    uint16_t distEntreZeroEMin = abs(m_yTovmin - yZero);
    uint16_t distEntreZeroEMax = abs(yZero - m_yTovmax);
    uint8_t distMinEntreLinhas = 15;
    if (distEntreZeroEMin >= distMinEntreLinhas && distEntreZeroEMax >= distMinEntreLinhas) {
      printText("0", m_xPos + m_borderSize + m_leftPadding - 4, yZero, BR_DATUM);
    }
  }
  updateFont(FontType::UNLOAD);

  WidgetBase::objTFT->drawFastVLine(m_xPos + m_leftPadding + m_borderSize - 1,
                                   m_yPos + m_topBottomPadding + m_borderSize,
                                   m_maxHeight + 1, m_config.textColor);
  WidgetBase::objTFT->drawFastHLine(m_xPos + m_leftPadding + m_borderSize - 1,
                                   m_yTovmin + 1, m_maxWidth, m_config.textColor);
#endif
}

/**
 * @brief push() em O(1) usando ring buffer.
 * @details
 *  - travamos o mutex apenas para atualizar o buffer (microtempo).
 *  - não esperamos nenhum "blocked" aqui (evita deadlock).
 *  - incrementamos m_dataVersion para o draw task detectar mudanças.
 */
bool LineChart::push(uint16_t serieIndex, int newValue)
{
#if defined(DISP_DEFAULT)
  if (!m_loaded) return false;
  if (!m_config.workInBackground && WidgetBase::currentScreen != m_screen) return false;
  if (!m_ringValues || serieIndex >= m_config.amountSeries || m_amountPoints == 0) return false;

  if (m_mutex) xSemaphoreTake(m_mutex, portMAX_DELAY);

  newValue = constrain(newValue, m_config.minValue, m_config.maxValue);
  m_ringValues[serieIndex][m_headBySeries[serieIndex]] = newValue;

  m_dataVersion++;
  m_shouldRedraw = true;

  // Avança o head desta série (ring buffer)
  m_headBySeries[serieIndex] = (uint16_t)((m_headBySeries[serieIndex] + 1) % m_amountPoints);

  if (m_mutex) xSemaphoreGive(m_mutex);

  return true;
#else
  return false;
#endif
}

/**
 * @brief Copia o ring buffer para snapshot em ordem lógica.
 */
uint32_t LineChart::snapshotUnderMutex()
{
#if defined(DISP_DEFAULT)
  if (!m_mutex) return 0;

  xSemaphoreTake(m_mutex, portMAX_DELAY);

  // Captura versão e heads de forma consistente
  uint32_t ver = m_dataVersion;

  // Ordem lógica por série: mais antigo -> mais novo
  // O mais antigo está em headBySeries[s] (próxima posição a ser escrita).
  for (uint16_t s = 0; s < m_config.amountSeries; s++) {
    uint16_t head = m_headBySeries[s];
    for (uint16_t i = 0; i < m_amountPoints; i++) {
      uint16_t idx = (uint16_t)((head + i) % m_amountPoints);
      m_snapshotValues[s][i] = m_ringValues[s][idx];
    }
  }

  xSemaphoreGive(m_mutex);
  return ver;
#else
  return 0;
#endif
}

void LineChart::drawGrid()
{
  CHECK_TFT_VOID
#if defined(DISP_DEFAULT)
  if (m_config.verticalDivision > 0) {
    uint16_t sizeVSpace = round(m_maxHeight / m_config.verticalDivision);

    for (auto i = 0; i < m_config.verticalDivision; ++i) {
      uint16_t yPosLine = m_yPos + m_topBottomPadding + (sizeVSpace * i);
      if (yPosLine < m_yPos + m_topBottomPadding + m_maxHeight) {
        WidgetBase::objTFT->drawFastHLine(m_xPos + m_leftPadding + 2, yPosLine, m_maxWidth, m_config.gridColor);
      }
    }
    WidgetBase::objTFT->drawFastHLine(m_xPos + m_leftPadding + 2, m_yPos + m_topBottomPadding, m_maxWidth, m_config.gridColor);

    uint16_t maxX = m_xPos + m_leftPadding + 2 + m_maxWidth;
    for (auto i = (m_xPos + m_leftPadding + 2); i < maxX; i += sizeVSpace) {
      WidgetBase::objTFT->drawFastVLine(i, m_yPos + m_topBottomPadding, m_maxHeight, m_config.gridColor);
    }
    WidgetBase::objTFT->drawFastVLine(maxX, m_yPos + m_topBottomPadding, m_maxHeight, m_config.gridColor);
  }
#endif
}

void LineChart::drawMarkLineAt(int value)
{
#if defined(DISP_DEFAULT)
  uint16_t y = map(value, m_config.minValue, m_config.maxValue, m_yTovmin, m_yTovmax);
  WidgetBase::objTFT->drawFastHLine(m_xPos + m_leftPadding + m_borderSize, y, m_maxWidth, m_config.textColor);
#endif
}

/**
 * @brief Apaga uma série usando um buffer linear (último desenhado).
 * @note Não usa mutex: deve ser chamado apenas do task de desenho.
 */
void LineChart::eraseSerieFromBuffer(uint8_t serieIndex, const int* values)
{
  CHECK_TFT_VOID
#if defined(DISP_DEFAULT)
  uint16_t startXPlot = m_xPos + m_leftPadding + m_borderSize + 1;

  for (uint16_t i = 1; i < m_amountPoints; ++i) {
    int16_t x0 = round(startXPlot + ((i - 1) * m_spaceBetweenPoints));
    int16_t x1 = round(startXPlot + (i * m_spaceBetweenPoints));
    int16_t y0 = round(map(values[i - 1], m_config.minValue, m_config.maxValue, m_yTovmin, m_yTovmax));
    int16_t y1 = round(map(values[i],     m_config.minValue, m_config.maxValue, m_yTovmin, m_yTovmax));

    WidgetBase::objTFT->drawLine(x0, y0, x1, y1, m_config.backgroundColor);

    if (m_config.boldLine) {
      WidgetBase::objTFT->drawLine(x0, y0 - 1, x1, y1 - 1, m_config.backgroundColor);
      WidgetBase::objTFT->drawLine(x0, y0 + 1, x1, y1 + 1, m_config.backgroundColor);
    }

    if (m_config.showDots && m_spaceBetweenPoints >= m_minSpaceToShowDot) {
      WidgetBase::objTFT->fillCircle(x0, y0, m_dotRadius, m_config.backgroundColor);
    }
  }
#endif
}

/**
 * @brief Desenha uma série usando um buffer linear (snapshot).
 * @note Não usa mutex: deve ser chamado apenas do task de desenho.
 */
void LineChart::drawSerieFromBuffer(uint8_t serieIndex, const int* values)
{
  CHECK_TFT_VOID
#if defined(DISP_DEFAULT)
  uint16_t startXPlot = m_xPos + m_leftPadding + m_borderSize + 1;
  uint16_t colorLine = m_colorsSeries[serieIndex];

  for (uint16_t i = 1; i < m_amountPoints; ++i) {
    int16_t x0 = round(startXPlot + ((i - 1) * m_spaceBetweenPoints));
    int16_t x1 = round(startXPlot + (i * m_spaceBetweenPoints));
    int16_t y0 = round(map(values[i - 1], m_config.minValue, m_config.maxValue, m_yTovmin, m_yTovmax));
    int16_t y1 = round(map(values[i],     m_config.minValue, m_config.maxValue, m_yTovmin, m_yTovmax));

    WidgetBase::objTFT->drawLine(x0, y0, x1, y1, colorLine);

    if (m_config.boldLine) {
      WidgetBase::objTFT->drawLine(x0, y0 - 1, x1, y1 - 1, colorLine);
      WidgetBase::objTFT->drawLine(x0, y0 + 1, x1, y1 + 1, colorLine);
    }

    if (m_config.showDots && m_spaceBetweenPoints >= m_minSpaceToShowDot) {
      WidgetBase::objTFT->fillCircle(x0, y0, m_dotRadius, colorLine);
    }
  }

  if (m_config.subtitles && m_config.subtitles[serieIndex]) {
    int lastValue = values[m_amountPoints - 1];
    m_config.subtitles[serieIndex]->setTextInt(lastValue);
  }
#endif
}

void LineChart::eraseAllFromLastDrawn()
{
#if defined(DISP_DEFAULT)
  for (uint16_t s = 0; s < m_config.amountSeries; s++) {
    eraseSerieFromBuffer((uint8_t)s, m_lastDrawnValues[s]);
  }
#endif
}

void LineChart::drawAllFromSnapshot()
{
#if defined(DISP_DEFAULT)
  for (uint16_t s = 0; s < m_config.amountSeries; s++) {
    drawSerieFromBuffer((uint8_t)s, m_snapshotValues[s]);
  }
#endif
}

/**
 * @brief Debug: imprime snapshot atual (ordem lógica).
 */
void LineChart::printValues(uint8_t serieIndex)
{
#if defined(DISP_DEFAULT)
  if (serieIndex >= m_config.amountSeries) {
    ESP_LOGW(TAG, "Invalid serieIndex: %d", serieIndex);
    return;
  }
  ESP_LOGD(TAG, "Series %d - Snapshot values:", serieIndex);
  for (uint16_t i = 0; i < m_amountPoints; i++) {
    ESP_LOGD(TAG, "  [%d]: %d", i, m_snapshotValues[serieIndex][i]);
  }
#endif
}

/**
 * @brief Redesenha o LineChart completo (safe para tasks).
 * @details
 *  1) Early return se não está sujo.
 *  2) Snapshot sob mutex (rápido).
 *  3) Apaga último desenhado (sem mutex).
 *  4) Desenha grid/zero-line + séries (sem mutex).
 *  5) Salva snapshot como último desenhado.
 *  6) Se houve alterações durante o desenho, mantém m_shouldRedraw=true.
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
  if (!m_shouldRedraw) return;

  // Snapshot consistente e rápido
  uint32_t capturedVer = snapshotUnderMutex();

  // Apaga o que foi desenhado da última vez
  eraseAllFromLastDrawn();

  // Grade + linha zero
  drawGrid();
  if (m_config.showZeroLine && m_config.minValue <= 0 && m_config.maxValue >= 0) {
    drawMarkLineAt(0);
  }

  // Desenha valores atuais (snapshot)
  drawAllFromSnapshot();

  // Salva snapshot como último desenhado
  for (uint16_t s = 0; s < m_config.amountSeries; s++) {
    memcpy(m_lastDrawnValues[s], m_snapshotValues[s], sizeof(int) * m_amountPoints);
  }

  // Decide se limpa o dirty flag ou se agenda outro redraw (mudança durante desenho)
  uint32_t nowVer = 0;
  if (m_mutex) {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
    nowVer = m_dataVersion;
    xSemaphoreGive(m_mutex);
  }

  if (nowVer == capturedVer) {
    m_shouldRedraw = false;
  } else {
    // Dados mudaram durante o desenho: deixa marcado para o próximo ciclo.
    m_shouldRedraw = true;
  }
#endif
}

void LineChart::forceUpdate()
{
  m_shouldRedraw = true;
}

/**
 * @brief Configura o LineChart.
 */
void LineChart::setup(const LineChartConfig &config)
{
  CHECK_TFT_VOID
  if (m_loaded) {
    ESP_LOGW(TAG, "LineChart widget already configured");
    return;
  }

  if (!validateConfig(config)) {
    ESP_LOGE(TAG, "Invalid configuration provided");
    return;
  }

  m_config = config;

  // Copy colors
  if (config.colorsSeries && config.amountSeries > 0) {
    for (uint8_t i = 0; i < config.amountSeries && i < MAX_SERIES; ++i) {
      m_colorsSeries[i] = config.colorsSeries[i];
    }
    ESP_LOGD(TAG, "Copied %d colors to internal buffer", config.amountSeries);
  } else {
    ESP_LOGW(TAG, "No colors provided in configuration");
    for (uint8_t i = 0; i < MAX_SERIES; ++i) {
      m_colorsSeries[i] = CFK_WHITE;
    }
  }

  // Copy subtitles pointers
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
