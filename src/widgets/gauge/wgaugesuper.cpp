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
 * @brief Starts the gauge.
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

  

  // Configura a fonte
  // m_usedFont = &Roboto_Bold10pt7b;
  WidgetBase::objTFT->setFont(m_usedFont);

  // Calcula as dimensões do texto para os valores
  char strMin[12], strMax[12];// Maximo de 12 caracteres para os valores
  sprintf(strMin, "%d", m_vmin * 10);// Multiplico por 10 para ter mais um digito e servir de offset para o texto
  sprintf(strMax, "%d", m_vmax * 10);// Multiplico por 10 para ter mais um digito e servir de offset para o texto
  TextBound_t dimensionMin = getTextBounds(strMin, xPos, yPos);// Pega as dimensões do texto
  TextBound_t dimensionMax = getTextBounds(strMax, xPos, yPos);// Pega as dimensões do texto
  m_textBoundForValue = dimensionMax.width > dimensionMin.width ? dimensionMax : dimensionMin;// Pega o maior valor entre os dois

  // Configura o título
  m_showTitle = (m_title != nullptr && strlen(m_title) > 0);// Verifica se o titulo existe e tem caracteres
  
  m_value = m_vmin;// Define o valor inicial
  m_lastValue = m_vmax;// Define o valor inicial

  
  //TextBound_t t;
  //t.height = 20;
  //t.width = m_showLabels ? 50 : 10;

  // https://pt.wikipedia.org/wiki/Segmento_circular

  if(!m_showLabels){
    m_textBoundForValue.width = 5;
    m_textBoundForValue.height = 5;
  }
  

  // Calcula o raio e ângulo do gauge
  int corda = (m_width - 2 * m_textBoundForValue.width) * 0.9;// Largura do gauge menos o texto dos valores multiplicado por 0.9 para ter um espaço para o texto
  int aberturaArcoTotal = 2 * m_maxAngle;// Calcula o ângulo total do arco
  int raioSugerido = corda / (2.0 * fastSin(aberturaArcoTotal / 2.0));// Calcula o raio sugerido
  int altura = raioSugerido * (1 - fastCos(aberturaArcoTotal / 2.0));
  m_radius = raioSugerido;
  DEBUG_D("Raio gauge %i\tsegmento %i", m_radius, altura);

  //m_offsetYAgulha = (40 + m_textBoundForValue.height + m_borderSize);
  int seno = fastSin((90 - m_maxAngle)) * m_radius;
  m_offsetYAgulha = seno - (m_borderSize + m_textBoundForValue.height * 2);
  DEBUG_D("Offset da agulha: %i\n", m_offsetYAgulha);
  m_rotation = (-(m_maxAngle + m_rotation));
  m_divisor = 1;

  // Configura a origem do gauge
  m_origem.x = xPos;
  m_origem.y = yPos + m_offsetYAgulha;

  // O 3 da borderSize é para ter um espaço para as label e a borda de cima
  m_height = (m_radius - m_offsetYAgulha) + (3 * m_borderSize) + (m_textBoundForValue.height * 3);

  

  // Ajusta o ângulo máximo
  m_maxAngle = constrain(m_maxAngle, 22, 80);

  // Reseta o valor do textbound para desenhar o value
  m_textBoundForValue.x = 0;
  m_textBoundForValue.y = 0;
  m_textBoundForValue.width = 0;
  m_textBoundForValue.height = 0;

  // Calcula as dimensões disponíveis
  m_availableWidth = m_width - (2 * m_borderSize);
  m_availableHeight = m_height - (2 * m_borderSize);

  m_update = true;
}

/**
 * @brief Draws the background of the gauge.
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

  m_indexCurrentStrip = 0;  // Index da primeira cor a ser pega para pintar o fundo da faixa
  m_stripColor = CFK_WHITE; // o inicio da faixa é branco
  m_ltx = 0;                // ultima posição x da base da agulha
  m_lastPointNeedle.x = m_origem.x;
  m_lastPointNeedle.y = m_origem.y; // posições da agulha

  for (auto i = 0; i < m_borderSize; ++i)
  {
    WidgetBase::objTFT->drawRect((xPos - (m_width / 2)) + i, (yPos - (m_height)) + i, m_width - (2 * i), m_height - (2 * i), m_borderColor);
  }

  WidgetBase::objTFT->fillRect(xPos - (m_availableWidth / 2), yPos - (m_availableHeight + m_borderSize), m_availableWidth, m_availableHeight, m_bkColor);
  WidgetBase::objTFT->drawRect(xPos - (m_availableWidth / 2), yPos - (m_availableHeight + m_borderSize), m_availableWidth, m_availableHeight, baseBorder);

  // WidgetBase::objTFT->drawCircle(_origem.x, _origem.y, _radius, CFK_RED);
  // WidgetBase::objTFT->fillCircle(_origem.x, _origem.y, 2, CFK_RED);

  // Desenha a faixa de cor com intervals menores do que 5 para ficar mais certinho
  for (int i = 0; i <= (2 * m_maxAngle); i += 1)
  {
    // Aplica a rotação
    int angulo = i + m_rotation;

    // Tamanho tick maior
    int tl = 15;

    // Coordenadas para desenhar o tick
    float sx = fastCos(angulo);
    float sy = fastSin(angulo);

    uint16_t x0 = sx * (m_radius + tl) + m_origem.x;
    uint16_t y0 = sy * (m_radius + tl) + m_origem.y;
    uint16_t x1 = sx * m_radius + m_origem.x;
    uint16_t y1 = sy * m_radius + m_origem.y;

    // Coordenadas do proximo tick para preencher o espaço com a cor desejada
    float sx2 = fastCos(angulo + 1);
    float sy2 = fastSin(angulo + 1);
    int x2 = sx2 * (m_radius + tl) + m_origem.x;
    int y2 = sy2 * (m_radius + tl) + m_origem.y;
    int x3 = sx2 * m_radius + m_origem.x;
    int y3 = sy2 * m_radius + m_origem.y;

    int vFaixa = map(i, 0, (2 * m_maxAngle), m_vmin, m_vmax); // Tranforma o for de -50 a 50 no value entre min e max para poder pintar
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
  // Fim do desenho do arco colorido

  // Calcula e desenha o multiplcador
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

    // Reseta os values para desenhar os textos
    m_indexCurrentStrip = 0; // Index do primeiro intervalo para escrever o value

    // Repetição para desenhar os textos dos intervals
    for (int i = 0; i <= (2 * m_maxAngle); i += 1)
    {
      int angulo = i + m_rotation;

      // Tamanho tick maior
      int tl = 15;

      // Coordenadas para desenhar o tick
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

  // Desenha os ticks a cada 5 graus
  for (int i = 0; i <= 2 * m_maxAngle; i += 5)
  {
    int angulo = i + m_rotation;
    // Tamanho tick maior
    int tl = 15;

    // Coordenadas para desenhar o tick
    float sx = fastCos(angulo);
    float sy = fastSin(angulo);
    uint16_t x0 = sx * (m_radius + tl) + m_origem.x;
    uint16_t y0 = sy * (m_radius + tl) + m_origem.y;
    uint16_t x1 = sx * m_radius + m_origem.x;
    uint16_t y1 = sy * m_radius + m_origem.y;

    // Tamanho tick menor
    if (i % 25 != 0)
      tl = 8;

    // Recalcula as coordenadas caso o tick mude o tamanho, no caso, se o angulo nao for multiplo de 25
    x0 = sx * (m_radius + tl) + m_origem.x;
    y0 = sy * (m_radius + tl) + m_origem.y;
    x1 = sx * m_radius + m_origem.x;
    y1 = sy * m_radius + m_origem.y;

    // Desenha o tick
    WidgetBase::objTFT->drawLine(x0, y0, x1, y1, m_marcadoresColor);

    // Calcula as posições para desenhar o arco base
    sx = fastCos(angulo + 5);
    sy = fastSin(angulo + 5);
    x0 = sx * m_radius + m_origem.x;
    y0 = sy * m_radius + m_origem.y;

    // Desenha o arco, não desenha a ultima parte
    if (i < 2 * m_maxAngle)
      WidgetBase::objTFT->drawLine(x0, y0, x1, y1, m_marcadoresColor);
  }

  m_isFirstDraw = true;

  // WidgetBase::objTFT->fillCircle(m_origem.x, m_origem.y, 2, CFK_RED);
  // WidgetBase::objTFT->drawCircle(m_origem.x, m_origem.y, m_radius, CFK_RED);
}

/**
 * @brief Sets the value of the gauge.
 * @param newValue New value to set.
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
 * @brief Redraws the gauge.
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

  // Desenhar aqui
  char buf[8];
  dtostrf(m_value, 4, 0, buf);

  // Como o desenho esta girado -90 e os angulo de desenho são -50 e 50.
  // int diff10 = (maxAngle + 10) - 90;

  int sdeg = map(m_value, m_vmin, m_vmax, 0, 2 * m_maxAngle); // Mapeia os value de entrada min e max com extrapolação de 10 para o angulo com extrapolação de 10
  int angulo = sdeg + m_rotation;

  // Calcula as componetes da agulha de acordo com o angulo

  float sx = fastCos(angulo);
  float sy = fastSin(angulo);

  // Usa a tangente para calcular a posição X que a agulha deve começar ja que a origem.y é abaixo do limite do grafico
  // O -90 é para simular que angulo total de abertura é girado 90 graus para calculo correto da tangente (de -90 a 90)
  float tx = fastTan(angulo - 90);

  // Apaga a agulha antiga
  if (!m_isFirstDraw)
  {
    WidgetBase::objTFT->drawLine(m_origem.x - 1 + round(m_ltx * m_offsetYAgulha), m_origem.y - m_offsetYAgulha - m_borderSize - 2, m_lastPointNeedle.x - 1, m_lastPointNeedle.y, m_bkColor); // -1 é para nao desenha em cima da linha fina da borda
    WidgetBase::objTFT->drawLine(m_origem.x + 0 + round(m_ltx * m_offsetYAgulha), m_origem.y - m_offsetYAgulha - m_borderSize - 2, m_lastPointNeedle.x + 0, m_lastPointNeedle.y, m_bkColor); // -1 é para nao desenha em cima da linha fina da borda
    WidgetBase::objTFT->drawLine(m_origem.x + 1 + round(m_ltx * m_offsetYAgulha), m_origem.y - m_offsetYAgulha - m_borderSize - 2, m_lastPointNeedle.x + 1, m_lastPointNeedle.y, m_bkColor); // -1 é para nao desenha em cima da linha fina da borda
  }

  WidgetBase::objTFT->setTextColor(m_textColor);
  if (m_showLabels)
  {
#if defined(DISP_DEFAULT)
    // WidgetBase::printText()
    uint16_t auxX = xPos - (m_availableWidth / 2) + 1;
    uint16_t auxY = yPos - m_borderSize;
    //TextBound_t tb_value = getTextBounds(buf, auxX, auxY);
    printText(buf, auxX, auxY, BL_DATUM, m_textBoundForValue, m_bkColor);
#endif
  }
  if (m_showTitle)
  {
    // Redesenha os textos
    // updateFont(FontType::BOLD);
    WidgetBase::objTFT->setTextColor(m_titleColor);
#if defined(DISP_DEFAULT)
    WidgetBase::objTFT->setFont(m_usedFont);

    TextBound_t tb_title = getTextBounds(m_title, xPos, yPos - (m_borderSize * 2));
    printText(m_title, xPos, yPos - (m_borderSize * 2), BC_DATUM);
    updateFont(FontType::UNLOAD);
#endif
  }
  // guarda os valuees dalinha para poder apagar depois
  m_ltx = tx;
  m_lastPointNeedle.x = sx * (m_radius - m_distanceAgulhaArco) + m_origem.x; //-2 é a distancia entre o final da agulha e o arco
  m_lastPointNeedle.y = sy * (m_radius - m_distanceAgulhaArco) + m_origem.y;

  // Desenha a nova linha
  // Desenha 3 linhas para aumentar a espessura

  WidgetBase::objTFT->drawLine(m_origem.x - 1 + round(m_ltx * m_offsetYAgulha), m_origem.y - m_offsetYAgulha - m_borderSize - 2, m_lastPointNeedle.x - 1, m_lastPointNeedle.y, m_agulhaColor);     // -1 é para nao desenha em cima da linha fina da borda
  WidgetBase::objTFT->drawLine(m_origem.x + 0 + round(m_ltx * m_offsetYAgulha), m_origem.y - m_offsetYAgulha - m_borderSize - 2, m_lastPointNeedle.x + 0, m_lastPointNeedle.y, m_agulhaColor); // -1 é para nao desenha em cima da linha fina da borda
  WidgetBase::objTFT->drawLine(m_origem.x + 1 + round(m_ltx * m_offsetYAgulha), m_origem.y - m_offsetYAgulha - m_borderSize - 2, m_lastPointNeedle.x + 1, m_lastPointNeedle.y, m_agulhaColor);     // -1 é para nao desenha em cima da linha fina da borda

  m_update = false;
  m_isFirstDraw = false;
  updateFont(FontType::UNLOAD);
}

/**
 * @brief Forces the gauge to update.
 */
void GaugeSuper::forceUpdate()
{
  m_update = true;
}

/**
 * @brief Sets up the gauge.
 * @param width Width of the gauge.
 * @param title Title of the gauge.
 * @param intervals Intervals of the gauge.
 * @param colors Colors of the gauge.
 * @param amountIntervals Amount of intervals of the gauge.
 * @param vmin Minimum value of the gauge.
 * @param vmax Maximum value of the gauge.
 * @param borderColor Border color of the gauge.
 * @param textColor Text color of the gauge.
 * @param backgroundColor Background color of the gauge.
 * @param titleColor Title color of the gauge.
 * @param showLabels True if the labels are shown, false otherwise. 
 * @param _fontFamily Font of the gauge.
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

  // Libera memória anterior se existir
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

  // Configura as dimensões e cores
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

  // Aloca e copia os arrays
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

  // Copia os dados
  for (uint8_t i = 0; i < amountIntervals; i++)
  {
    m_intervals[i] = intervals[i];
    m_colors[i] = colors[i];
  }

  // Aloca e copia o título
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

  // Inicializa o gauge
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
