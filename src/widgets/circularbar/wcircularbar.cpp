#include "wcircularbar.h"
#include <esp_log.h>

const char* CircularBar::TAG = "CircularBar";

/**
 * @brief Construtor da classe CircularBar.
 * @param _x Coordenada X da posição central da barra na tela.
 * @param _y Coordenada Y da posição central da barra na tela.
 * @param _screen Identificador da tela onde a barra será exibida.
 * @details Inicializa a barra circular com valores padrão: raio 0, faixa 0-100,
 *          ângulos 0-360°, espessura 10 pixels, cores padrão, exibição de valor habilitada.
 *          A barra deve ser configurada com setup() antes de ser exibida.
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
 * @brief Destrutor da classe CircularBar.
 * @details Registra o evento no log do ESP32 e limpa o ponteiro da função callback.
 *          Todos os recursos são liberados automaticamente pela herança de @ref WidgetBase.
 */
CircularBar::~CircularBar() {
    ESP_LOGD(TAG, "CircularBar destroyed at (%d, %d)", m_xPos, m_yPos);
    if (m_callback != nullptr) { m_callback = nullptr; }
}

/**
 * @brief Detecta se o CircularBar foi tocado pelo usuário.
 * @param _xTouch Ponteiro para a coordenada X do toque na tela.
 * @param _yTouch Ponteiro para a coordenada Y do toque na tela.
 * @return Sempre retorna False, pois CircularBar não processa eventos de toque.
 * @details Este widget é apenas visual e não responde a interações de toque.
 *          Os parâmetros são marcados como UNUSED para evitar avisos do compilador.
 */
bool CircularBar::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  // CircularBar doesn't handle touch events
  UNUSED(_xTouch);
  UNUSED(_yTouch);
  return false;
}

/**
 * @brief Recupera a função callback associada ao CircularBar.
 * @return Ponteiro para a função callback, ou nullptr se nenhuma foi definida.
 * @details A função callback é executada quando o estado da barra muda.
 *          Para mais informações sobre callbacks, consulte @ref WidgetBase.
 */
functionCB_t CircularBar::getCallbackFunc() { return m_callback; }

/**
 * @brief Desenha o fundo do widget CircularBar.
 * @details Renderiza o arco de fundo da barra circular e inicializa a exibição.
 *          Este método deve ser chamado uma vez para configurar o fundo antes
 *          de usar setValue() para atualizar o valor. Apenas desenha se o widget
 *          está na tela atual e adequadamente carregado.
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
 * @brief Define o valor atual do widget CircularBar.
 * @param newValue Novo valor a ser exibido na barra circular.
 * @details Este método permite atualizar o valor da barra:
 *          - Armazena o valor anterior em m_lastValue
 *          - Define o novo valor em m_value
 *          - Marca o widget para redesenho
 *          - Registra o evento no log do ESP32
 *          O valor será mapeado proporcionalmente para o ângulo da barra na próxima chamada de redraw().
 */
void CircularBar::setValue(int newValue) {
  CHECK_LOADED_VOID
  
  m_lastValue = m_value;
  m_value = newValue;
  m_shouldRedraw = true;
  
  ESP_LOGD(TAG, "CircularBar value set to: %d", newValue);
}

/**
 * @brief Redesenha o widget CircularBar na tela, atualizando sua aparência baseada no valor atual.
 * @details Este método é responsável por renderizar a barra circular na tela:
 *          - Verifica todas as condições necessárias para o redesenho
 *          - Mapeia o valor atual para ângulos usando a função map()
 *          - Desenha apenas a diferença entre o valor anterior e atual para eficiência
 *          - Usa fillArc() da biblioteca @ref Arduino_GFX_Library para o desenho
 *          - Exibe o valor atual no centro se showValue estiver habilitado
 *          - Aplica debounce para evitar redesenhos excessivos
 *          Apenas redesenha se a barra está visível, inicializada, carregada, na tela atual
 *          e a flag m_shouldRedraw está configurada como true.
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
 * @brief Força uma atualização imediata do CircularBar.
 * @details Define a flag m_shouldRedraw para true, forçando o redesenho da barra
 *          no próximo ciclo de atualização da tela. Útil para garantir que mudanças
 *          no valor sejam visíveis imediatamente.
 */
void CircularBar::forceUpdate() { 
  m_shouldRedraw = true; 
  ESP_LOGD(TAG, "CircularBar force update requested");
}


/**
 * @brief Configura o CircularBar com parâmetros definidos em uma estrutura de configuração.
 * @param config Estrutura @ref CircularBarConfig contendo os parâmetros de configuração da barra.
 * @details Este método deve ser chamado após criar o objeto para configurá-lo adequadamente:
 *          - Copia todas as configurações para m_config
 *          - Valida e corrige valores min/max se necessário
 *          - Inicializa os valores atual e anterior com o valor mínimo
 *          - Desabilita exibição de valor se o raio for muito pequeno (< 20 pixels)
 *          - Marca o widget como inicializado e carregado
 *          - A barra não será exibida corretamente até que este método seja chamado
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

/**
 * @brief Torna a barra circular visível na tela.
 * @details Define m_visible como true e marca para redesenho. A barra será
 *          desenhada na próxima chamada de redraw() se estiver na tela atual.
 */
void CircularBar::show() {
  m_visible = true;
  m_shouldRedraw = true;
  ESP_LOGD(TAG, "CircularBar shown at (%d, %d)", m_xPos, m_yPos);
}

/**
 * @brief Oculta a barra circular da tela.
 * @details Define m_visible como false e marca para redesenho. A barra não será
 *          mais desenhada até que show() seja chamado novamente.
 */
void CircularBar::hide() {
  m_visible = false;
  m_shouldRedraw = true;
  ESP_LOGD(TAG, "CircularBar hidden at (%d, %d)", m_xPos, m_yPos);
}