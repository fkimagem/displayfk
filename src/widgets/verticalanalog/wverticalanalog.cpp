#include "wverticalanalog.h"

const char* VAnalog::TAG = "[VAnalog]";

/**
 * @brief Construtor do widget VAnalog.
 * @param _x Coordenada X para a posição do VAnalog.
 * @param _y Coordenada Y para a posição do VAnalog.
 * @param _screen Identificador da tela onde o VAnalog será exibido.
 * @details Inicializa o widget VAnalog com posição e tela especificadas.
 */
VAnalog::VAnalog(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen)
{
  m_margin = {.top = 2, .bottom = 2, .left = 2, .right = 2};
}

/**
 * @brief Limpa recursos alocados pelo VAnalog.
 * @details Desaloca memória dinâmica e remove referências a objetos.
 */
void VAnalog::cleanupMemory() {
  // VAnalog não aloca memória dinâmica no momento
}

/**
 * @brief Destrutor do widget VAnalog.
 * @details Limpa recursos alocados.
 */
VAnalog::~VAnalog()
{
  cleanupMemory();
}

/**
 * @brief Detecta se o VAnalog foi tocado.
 * @param _xTouch Ponteiro para a coordenada X do toque.
 * @param _yTouch Ponteiro para a coordenada Y do toque.
 * @return True se o toque é detectado, False caso contrário.
 * @details Método override que sempre retorna false. O VAnalog não tem detecção de toque ativa.
 */
bool VAnalog::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
  return false;
}

/**
 * @brief Recupera a função callback associada ao VAnalog.
 * @return Ponteiro para a função callback.
 * @details Retorna o ponteiro para a função que será executada quando o VAnalog for utilizado.
 */
functionCB_t VAnalog::getCallbackFunc()
{
  return m_callback;
}

/**
 * @brief Inicia o widget VAnalog.
 * @details Configura dimensões, preenchimento e área de desenho para o display analógico:
 *          - Define padding e largura fixa do widget
 *          - Restringe altura dentro de bounds válidos (40 até altura da tela)
 *          - Calcula área de desenho baseada em padding
 */
void VAnalog::start()
{
  m_config.width = constrain(m_config.width, 40, WidgetBase::objTFT->width());
  m_config.height = constrain(m_config.height, 40, WidgetBase::objTFT->height());
  uint8_t offset = 3;


  m_textArea.width = m_config.width - m_margin.left - m_margin.right;
  m_textArea.height = 20;
  m_textArea.x = m_xPos + m_margin.left;
  m_textArea.y = m_yPos + m_config.height - m_margin.bottom - m_textArea.height;

  m_arrowArea.x = m_xPos + m_margin.left;
  m_arrowArea.y = m_yPos + m_margin.top + (m_arrowSize / 2);
  m_arrowArea.width = 10;
  m_arrowArea.height = m_config.height - m_margin.top - m_margin.bottom - m_textArea.height - m_arrowSize;

  m_drawArea.width = m_config.width - m_margin.left - m_margin.right - m_arrowArea.width;
  m_drawArea.height = m_config.height - m_margin.top - m_margin.bottom - m_textArea.height - m_arrowSize;
  m_drawArea.x = m_xPos + m_margin.left + m_arrowArea.width + offset;
  m_drawArea.y = m_yPos + m_margin.top + (m_arrowSize / 2);

  m_currentValue = m_config.minValue;
  m_lastValue = m_currentValue;
}

/**
 * @brief Desenha o fundo do widget VAnalog incluindo elementos estáticos.
 * @details Desenha o container visual e marcações da escala para o display analógico:
 *          - Valida TFT e visibilidade
 *          - Desenha fundo e borda
 *          - Calcula intervalo de marcações baseado em steps
 *          - Desenha marcações da escala (linhas menores a cada divisão, linhas maiores a cada 5 divisões)
 *          - Define posição inicial da seta e chama redraw()
 */
void VAnalog::drawBackground()
{
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_LOADED_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_SHOULDREDRAW_VOID

  WidgetBase::objTFT->fillRect(m_xPos, m_yPos, m_config.width, m_config.height, m_config.backgroundColor); // Area do grafico lightBg
  WidgetBase::objTFT->drawRect(m_xPos, m_yPos, m_config.width, m_config.height, m_config.borderColor);                 // contorno darkBg

  // fillRect m_drawArea, m_textArea and m_arrowArea
  // WidgetBase::objTFT->fillRect(m_drawArea.x, m_drawArea.y, m_drawArea.width, m_drawArea.height, CFK_RED);
  // WidgetBase::objTFT->fillRect(m_textArea.x, m_textArea.y, m_textArea.width, m_textArea.height, CFK_AQUA);
  // WidgetBase::objTFT->fillRect(m_arrowArea.x, m_arrowArea.y, m_arrowArea.width, m_arrowArea.height, CFK_BLUE);


  #if defined(DISP_DEFAULT)
  ESP_LOGD(TAG, "Redraw background vanalog at %i,%i with %i x %i", m_xPos, m_yPos, m_config.width, m_config.height);


  

  uint16_t intervaloMarca = round(m_drawArea.height / (m_config.steps));

  for (uint16_t i = 0; i <= m_config.steps; i++)
  {
    uint16_t yLinha = intervaloMarca * i + (m_drawArea.y);
    uint16_t lineLength = i % 5 == 0 ? 10 : 5;
    ESP_LOGD(TAG, "Line %i with length %i", i, lineLength);

    WidgetBase::objTFT->drawFastHLine(m_drawArea.x, yLinha, lineLength, m_config.textColor);
  }
  //redraw();
  ESP_LOGD(TAG, "Finish draw vanalog");
  #endif
}

/**
 * @brief Define o valor a ser exibido na escala analógica.
 * @param newValue Novo valor para definir.
 * @param _viewValue Se deve exibir o texto do valor numérico.
 * @details Atualiza o valor atual para exibir e se deve mostrar o texto numérico:
 *          - Restringe valor usando constrain() dentro da faixa min/max
 *          - Define flag de atualização de texto
 *          - Marca para redesenho
 */
void VAnalog::setValue(int newValue, bool _viewValue)
{
  m_currentValue = constrain(newValue, m_config.minValue, m_config.maxValue);
  m_updateText = _viewValue;
  ////Serial.println("ajusta currentValue: " + String(currentValue));
  m_shouldRedraw = true;
}

/**
 * @brief Redesenha o VAnalog na tela, atualizando sua aparência.
 * @details Atualiza a posição da seta marcadora e do texto numérico se habilitado:
 *          - Valida TFT e visibilidade
 *          - Apaga seta antiga preenchendo triângulo com cor de fundo
 *          - Calcula nova posição da seta baseada no valor atual (usando porcentagem)
 *          - Desenha nova seta
 *          - Atualiza texto numérico se habilitado (limpa área e desenha valor)
 *          - Reseta flag de redesenho
 */
void VAnalog::redraw()
{
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_LOADED_VOID
  CHECK_SHOULDREDRAW_VOID
  m_shouldRedraw = false;

  #if defined(DISP_DEFAULT)
  
  clearArrow();
  drawArrow();
  drawText();
  m_lastValue = m_currentValue;
  
  #endif
 /* #if defined(DISP_DEFAULT)
  ////Serial.println(WidgetBase::usingKeyboard);
  if (m_updateText)
  {
    WidgetBase::objTFT->setFont(&RobotoRegular10pt7b);
  }

  m_lastValue = m_currentValue;
  WidgetBase::objTFT->fillTriangle(m_xPos + 2, m_lastYArrow - 3, m_xPos + 15, m_lastYArrow, m_xPos + 2, m_lastYArrow + 3, m_config.backgroundColor);
  uint32_t vatual = m_currentValue - m_config.minValue;
  float total = m_config.maxValue - m_config.minValue;
  float porcentagem = (vatual / total) * m_drawArea;
  uint32_t yMarca = m_yPos + m_paddingDraw + m_drawArea - porcentagem;
  m_lastYArrow = yMarca;

  WidgetBase::objTFT->fillTriangle(m_xPos + 2, yMarca - 3, m_xPos + 15, yMarca, m_xPos + 2, yMarca + 3, m_config.arrowColor);

  // WidgetBase::objTFT->drawCentreString("0", m_xPos, yPos - 20, 2);
  if (m_updateText)
  {
    WidgetBase::objTFT->setTextColor(m_config.textColor);
    WidgetBase::objTFT->setFont(&RobotoRegular5pt7b);
    TextBound_t textBg;
    textBg.x = m_xPos + 2;
    textBg.y = m_yPos + m_config.height - 2 - m_paddingDraw - m_padding;
    textBg.width = m_config.width - 4;
    textBg.height = m_paddingDraw + m_padding;
    WidgetBase::objTFT->fillRect(textBg.x, textBg.y, textBg.width, textBg.height, m_config.backgroundColor);
    printText(String(m_currentValue).c_str(), m_xPos + (m_config.width / 2), m_yPos + m_config.height - m_paddingDraw, BC_DATUM);
  }
  m_shouldRedraw = false;
  #endif*/
}

/**
 * @brief Força o VAnalog a atualizar.
 * @details Define a flag de atualização para disparar um redesenho no próximo ciclo.
 */
void VAnalog::forceUpdate()
{
  m_shouldRedraw = true;
}


/**
 * @brief Configura o VAnalog com parâmetros definidos em uma estrutura de configuração.
 * @param config Estrutura @ref VerticalAnalogConfig contendo os parâmetros de configuração do display analógico vertical.
 * @details Chama método setup() com parâmetros individuais da configuração.
 */
void VAnalog::setup(const VerticalAnalogConfig& config)
{
  if (!WidgetBase::objTFT)
  {
    ESP_LOGE(TAG, "TFT not defined on WidgetBase");
    return;
  }
  if (m_loaded)
  {
    ESP_LOGD(TAG, "VAnalog widget already configured");
    return;
  }
  m_config = config;
  start();
  m_loaded = true;
  m_initialized = true;
}

/**
 * @brief Torna o VAnalog visível na tela.
 * @details Define o widget como visível e marca para redesenho.
 */
void VAnalog::show()
{
    m_visible = true;
    m_shouldRedraw = true;
}

/**
 * @brief Oculta o VAnalog da tela.
 * @details Define o widget como invisível e marca para redesenho.
 */
void VAnalog::hide()
{
    m_visible = false;
    m_shouldRedraw = true;
}


/**
 * @brief Calcula a posição vertical da seta marcadora com base no valor.
 * @param value Valor para calcular a posição.
 * @return Posição vertical da seta marcadora.
 */
uint16_t VAnalog::calculateArrowVerticalPosition(uint16_t value)
{
  return map(value, m_config.minValue, m_config.maxValue, m_drawArea.y + m_drawArea.height, m_drawArea.y);
}


/**
 * @brief Limpa a seta na tela.
 * @details Limpa a seta na tela com a cor de fundo específica.
 */
void VAnalog::clearArrow()
{
  uint16_t y = calculateArrowVerticalPosition(m_lastValue);
  int offset = m_arrowSize / 2;

  WidgetBase::objTFT->fillTriangle(m_arrowArea.x, y - offset, m_arrowArea.x + m_arrowArea.width, y, m_arrowArea.x, y + offset, m_config.backgroundColor);
}


/**
 * @brief Desenha a seta na tela.
 * @details Desenha a seta na tela com a cor específica.
 */
void VAnalog::drawArrow()
{

  uint16_t y = calculateArrowVerticalPosition(m_currentValue);
  int offset = m_arrowSize / 2;

  WidgetBase::objTFT->fillTriangle(m_arrowArea.x, y - offset, m_arrowArea.x + m_arrowArea.width, y, m_arrowArea.x, y + offset, m_config.arrowColor);
}


/**
 * @brief Desenha o texto na tela.
 * @details Desenha o texto na tela com a cor e fonte específicas.
 */
void VAnalog::drawText()
{
  WidgetBase::objTFT->setTextColor(m_config.textColor);
  WidgetBase::objTFT->setFont(&RobotoRegular5pt7b);
  WidgetBase::objTFT->fillRect(m_textArea.x, m_textArea.y, m_textArea.width, m_textArea.height, m_config.backgroundColor);
  printText(String(m_currentValue).c_str(), m_textArea.x + (m_textArea.width / 2), m_textArea.y + (m_textArea.height / 2), MC_DATUM);
}