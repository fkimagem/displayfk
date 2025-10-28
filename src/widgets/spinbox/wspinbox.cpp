#include "wspinbox.h"

const char* SpinBox::TAG = "SpinBox";



/**
 * @brief Diminui o valor atual pelo valor do passo.
 * @details Decrementa o valor atual garantindo que permaneça dentro da faixa definida:
 *          - Calcula novo valor (atual - passo)
 *          - Restringe valor usando constrain() entre minValue e maxValue
 */
void SpinBox::decreaseValue() {
  int temp = m_currentValue - m_config.step;
  m_currentValue = constrain(temp, m_config.minValue, m_config.maxValue);
}

/**
 * @brief Aumenta o valor atual pelo valor do passo.
 * @details Incrementa o valor atual garantindo que permaneça dentro da faixa definida:
 *          - Calcula novo valor (atual + passo)
 *          - Restringe valor usando constrain() entre minValue e maxValue
 */
void SpinBox::increaseValue() {
  int temp = m_currentValue + m_config.step;
  m_currentValue = constrain(temp, m_config.minValue, m_config.maxValue);
}

/**
 * @brief Construtor para a classe SpinBox.
 * @param _x Coordenada X para a posição do SpinBox.
 * @param _y Coordenada Y para a posição do SpinBox.
 * @param _screen Identificador da tela onde o SpinBox será exibido.
 * @details Inicializa o widget com valores padrão e configuração vazia.
 *          A caixa de spin não será funcional até que setup() seja chamado.
 */
SpinBox::SpinBox(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen){

      m_config = {.width = 0, .height = 0, .step = 0, .minValue = 0, .maxValue = 0, .startValue = 0, .color = 0, .textColor = 0, .callback = nullptr};
      ESP_LOGD(TAG, "SpinBox created at (%d, %d) on screen %d", _x, _y, _screen);
    }

/**
 * @brief Destrutor da classe SpinBox.
 * @details Limpa quaisquer recursos usados pelo SpinBox.
 */
SpinBox::~SpinBox() {
    cleanupMemory();
}

/**
 * @brief Limpa memória usada pelo SpinBox.
 * @details SpinBox não usa alocação dinâmica de memória.
 */
void SpinBox::cleanupMemory() {
    ESP_LOGD(TAG, "SpinBox memory cleanup completed");
}

/**
 * @brief Detecta se o SpinBox foi tocado e manipula ações de incremento/decremento.
 * @param _xTouch Ponteiro para a coordenada X do toque.
 * @param _yTouch Ponteiro para a coordenada Y do toque.
 * @return True se o toque está dentro da área do SpinBox, False caso contrário.
 * @details Detecta toques nas áreas dos botões de decremento (esquerda) ou incremento (direita),
 *          e processa a mudança de valor conforme apropriado:
 *          - Valida visibilidade, uso de teclado, tela atual, carregamento e debounce
 *          - Define áreas de detecção para botão esquerdo (decremento) e direito (incremento)
 *          - Detecta toque na área de decremento: chama decreaseValue() e marca para redesenho
 *          - Detecta toque na área de incremento: chama increaseValue() e marca para redesenho
 */
bool SpinBox::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  CHECK_VISIBLE_BOOL
  CHECK_USINGKEYBOARD_BOOL
  CHECK_CURRENTSCREEN_BOOL
  CHECK_LOADED_BOOL
  CHECK_DEBOUNCE_CLICK_BOOL
  bool detectado = false;

  uint16_t topY = m_yPos;
  // uint16_t bottomY = m_yPos + m_height;

  Rect_t boundsAreaDecrement = {.x = m_xPos, .y = topY, .width = (uint16_t)(m_config.width / 2), .height = m_config.height};
  Rect_t boundsAreaIncrement = {.x = (uint16_t)(m_xPos + (m_config.width / 2)), .y = topY, .width = (uint16_t)(m_config.width / 2),
                                .height = m_config.height};

  /*uint16_t d_x = m_xPos;
  uint16_t d_x_max = m_xPos + m_width;

  uint16_t y_min = m_yPos;
  uint16_t y_max = m_yPos + m_height;*/

  // uint16_t i_x = m_xPos + m_width - m_height;
  // uint16_t i_x_max = m_xPos + m_width;

  // Detect decrement
  if ((*_xTouch > boundsAreaDecrement.x) &&
      (*_xTouch < boundsAreaDecrement.x + boundsAreaDecrement.width) &&
      (*_yTouch > boundsAreaDecrement.y) &&
      (*_yTouch < boundsAreaDecrement.y + boundsAreaDecrement.height)) {
    decreaseValue();
    m_shouldRedraw = true;
    m_myTime = millis();
    detectado = true;
  }

  if (detectado) {
    return true;
  }

  // Detect increment
  if ((*_xTouch > boundsAreaIncrement.x) &&
      (*_xTouch < boundsAreaIncrement.x + boundsAreaIncrement.width) &&
      (*_yTouch > boundsAreaIncrement.y) &&
      (*_yTouch < boundsAreaIncrement.y + boundsAreaIncrement.height)) {
    increaseValue();
    m_shouldRedraw = true;
    m_myTime = millis();
    detectado = true;
  }

  return detectado;
}

/**
 * @brief Recupera a função callback associada ao SpinBox.
 * @return Ponteiro para a função callback.
 * @details Retorna o ponteiro para a função que será executada quando o SpinBox for utilizado.
 */
functionCB_t SpinBox::getCallbackFunc() { return m_callback; }

/**
 * @brief Redesenha o SpinBox na tela, atualizando sua aparência.
 * @details Atualiza a exibição do valor atual na caixa de spin,
 *          apenas redesenha se o SpinBox estiver na tela atual e precisar atualização:
 *          - Valida TFT, visibilidade, tela atual, uso de teclado, carregamento e flag de redesenho
 *          - Calcula dimensões dos botões e área disponível para texto
 *          - Define cor do texto e desenha retângulo de fundo para valor
 *          - Seleciona melhor fonte RobotoBold para o texto
 *          - Exibe valor atual centralizado na área de texto
 */
void SpinBox::redraw() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_LOADED_VOID
  CHECK_SHOULDREDRAW_VOID

  m_shouldRedraw = false;

  uint16_t btnW = m_config.height - (2 * m_offset);
  uint16_t btnH = m_config.height - (2 * m_offset);

  uint16_t availableW = m_config.width - (2 * m_offset);
  uint16_t availableH = m_config.height - (2 * m_offset);

  WidgetBase::objTFT->setTextColor(m_config.textColor);

  WidgetBase::objTFT->fillRoundRect(
      m_xPos + (2 * m_offset) + btnW, m_yPos + m_offset,
      m_config.width - (4 * m_offset + 2 * btnW), btnH, m_radius, m_config.color);


  uint16_t offsetFont = 10;
  WidgetBase::objTFT->setFont(getBestRobotoBold( availableW - offsetFont, availableH - offsetFont, String(m_currentValue).c_str()));
  printText(String(m_currentValue).c_str(), m_xPos + m_config.width / 2,
            m_yPos + (m_config.height / 2) - 3, MC_DATUM, m_lastArea, m_config.color);
  updateFont(FontType::UNLOAD);

}

/**
 * @brief Desenha o fundo do SpinBox, incluindo elementos estáticos.
 * @details Cria o container visual, botões de mais e menos para o SpinBox.
 *          Este método é tipicamente chamado uma vez durante setup ao invés de em cada redesenho:
 *          - Desenha retângulo principal com bordas arredondadas
 *          - Desenha borda do container
 *          - Desenha dois botões com cores clarificadas (lados esquerdo e direito)
 *          - Desenha bordas dos botões
 *          - Desenha sinais de menos (-) no botão esquerdo e mais (+) no botão direito
 */
void SpinBox::drawBackground() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_LOADED_VOID

  uint16_t btnW = m_config.height - (2 * m_offset);
  uint16_t btnH = m_config.height - (2 * m_offset);

  // uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;

  WidgetBase::objTFT->fillRoundRect(m_xPos, m_yPos, m_config.width, m_config.height, m_radius,
                                    m_config.color);
  WidgetBase::objTFT->drawRoundRect(m_xPos, m_yPos, m_config.width, m_config.height, m_radius,
                                    CFK_BLACK);

  WidgetBase::objTFT->fillRoundRect(
      m_xPos + m_offset, m_yPos + m_offset, btnW, btnH, m_radius,
      WidgetBase::lightenColor565(m_config.color, 4));
  WidgetBase::objTFT->fillRoundRect(
      m_xPos + m_config.width - m_offset - btnW, m_yPos + m_offset, btnW, btnH, m_radius,
      WidgetBase::lightenColor565(m_config.color, 4));

  WidgetBase::objTFT->drawRoundRect(m_xPos + m_offset, m_yPos + m_offset, btnW,
                                    btnH, m_radius, CFK_BLACK);
  WidgetBase::objTFT->drawRoundRect(m_xPos + m_config.width - m_offset - btnW,
                                    m_yPos + m_offset, btnW, btnH, m_radius,
                                    CFK_BLACK);

  CoordPoint_t btnLeft = {static_cast<uint16_t>(m_xPos + m_offset + (btnW / 2)),
                          static_cast<uint16_t>(m_yPos + m_offset + (btnH / 2))};
  CoordPoint_t btnRight = {
      static_cast<uint16_t>(m_xPos + m_config.width - (m_offset + (btnW / 2))),
      static_cast<uint16_t>(m_yPos + m_offset + (btnH / 2))};

  // WidgetBase::objTFT->fillCircle(btnLeft.x, btnLeft.y, 4, CFK_BLUE);
  const uint8_t sinalW = btnW / 2;
  const uint8_t sinalH = ceil(sinalW / 10.0);
  // Sinal menos
  WidgetBase::objTFT->fillRect(btnLeft.x - (sinalW / 2),
                               btnLeft.y - (sinalH / 2), sinalW, sinalH,
                               m_config.textColor);

  // Sinal mais
  WidgetBase::objTFT->fillRect(btnRight.x - (sinalW / 2),
                               btnRight.y - (sinalH / 2), sinalW, sinalH,
                               m_config.textColor);
  WidgetBase::objTFT->fillRect(btnRight.x - (sinalH / 2),
                               btnRight.y - (sinalW / 2), sinalH, sinalW,
                               m_config.textColor);

  m_shouldRedraw = true;


  // WidgetBase::objTFT->fillCircle(btnRight.x, btnRight.y, 4, CFK_YELLOW);
}


/**
 * @brief Configura o SpinBox com периметров definidos em uma estrutura de configuração.
 * @param config Estrutura @ref SpinBoxConfig contendo os parâmetros de configuração.
 * @details Este método deve ser chamado após criar o objeto para configurá-lo adequadamente:
 *          - Limpa memória existente usando cleanupMemory()
 *          - Copia profundamente a configuração
 *          - Valida e corrige se minValue > maxValue (troca valores)
 *          - Restringe startValue dentro da faixa válida
 *          - Define callback
 *          - Marca o widget como carregado
 *          A caixa de spin não será exibida corretamente até que este método seja chamado.
 */
void SpinBox::setup(const SpinBoxConfig &config) {
  CHECK_TFT_VOID
  if (m_loaded) {
    ESP_LOGW(TAG, "SpinBox already initialized");
    return;
  }

  // Clean up any existing memory before setting new config
  cleanupMemory();
  
  // Deep copy configuration
  m_config = config;
  
  // Set member variables from config
  if (m_config.minValue > m_config.maxValue) {
    int temp = m_config.minValue;
    m_config.minValue = m_config.maxValue;
    m_config.maxValue = temp;
  }

  m_currentValue = constrain(m_config.startValue, m_config.minValue, m_config.maxValue);
  m_callback = config.callback;
  m_loaded = true;
}

/**
 * @brief Recupera o valor atual do SpinBox.
 * @return Valor numérico atual.
 * @details Retorna o valor armazenado em m_currentValue.
 */
int SpinBox::getValue() { return m_currentValue; }

/**
 * @brief Define o valor atual do SpinBox.
 * @param _value Novo valor numérico para definir.
 * @details Atualiza o valor do SpinBox, garante que está dentro da faixa definida,
 *          e marca o widget para redesenho. Dispara o callback se fornecido:
 *          - Valida se widget está carregado
 *          - Restringe valor usando constrain() dentro da faixa min/max
 *          - Marca para redesenho
 *          - Dispara callback se configurado
 */
void SpinBox::setValue(int _value) {
  if (!m_loaded) {
    ESP_LOGE(TAG, "Spinbox widget not loaded");
    return;
  }

  m_currentValue = constrain(_value, m_config.minValue, m_config.maxValue);
  m_shouldRedraw = true;

  if (m_callback != nullptr) {
    WidgetBase::addCallback(m_callback, WidgetBase::CallbackOrigin::SELF);
  }
}

/**
 * @brief Torna o SpinBox visível na tela.
 * @details Define o widget como visível e marca para redesenho.
 */
void SpinBox::show() {
  m_visible = true;
  m_shouldRedraw = true;
}

/**
 * @brief Oculta o SpinBox da tela.
 * @details Define o widget como invisível e marca para redesenho.
 */
void SpinBox::hide() {
  m_visible = false;
  m_shouldRedraw = true;
}

/**
 * @brief Força o SpinBox a atualizar.
 * @details Define a flag de atualização para disparar um redesenho no próximo ciclo.
 */
void SpinBox::forceUpdate() { m_shouldRedraw = true; }