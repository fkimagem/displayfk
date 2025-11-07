#include "wtextbutton.h"

const char* TextButton::TAG = "TextButton";

/**
 * @brief Construtor para a classe TextButton.
 * @param _x Coordenada X para a posição do TextButton.
 * @param _y Coordenada Y para a posição do TextButton.
 * @param _screen Identificador da tela onde o TextButton será exibido.
 * @details Inicializa o widget com configuração vazia.
 *          O botão não será funcional até que setup() seja chamado.
 */
TextButton::TextButton(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen), m_config{} {
      m_offsetMargin = 10;
      m_config = {.width = 0, .height = 0, .radius = 0, .backgroundColor = 0, .textColor = 0, .text = "", .callback = nullptr};
      ESP_LOGD(TAG, "TextButton created at (%d, %d) on screen %d", _x, _y, _screen);
    }

/**
 * @brief Destrutor da classe TextButton.
 * @details Limpa quaisquer recursos usados pelo TextButton.
 */
TextButton::~TextButton() {
    cleanupMemory();
}

/**
 * @brief Limpa memória usada pelo TextButton.
 * @details TextButton não usa alocação dinâmica de memória.
 *          m_text é um ponteiro const para dados externos.
 */
void TextButton::cleanupMemory() {
    ESP_LOGD(TAG, "TextButton memory cleanup completed");
}

/**
 * @brief Detecta se o TextButton foi tocado.
 * @param _xTouch Ponteiro para a coordenada X do toque.
 * @param _yTouch Ponteiro para a coordenada Y do toque.
 * @return True se o toque está dentro da área do TextButton, False caso contrário.
 * @details Quando tocado, executa onClick() e marca para redesenho:
 *          - Valida visibilidade, uso de teclado, tela atual, carregamento e debounce
 *          - Verifica se botão está habilitado (retorna false se desabilitado)
 *          - Detecta se toque está dentro dos bounds do botão
 *          - Chama onClick() e marca para redesenho se toque detectado
 */
bool TextButton::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  CHECK_VISIBLE_BOOL
  CHECK_USINGKEYBOARD_BOOL
  CHECK_CURRENTSCREEN_BOOL
  CHECK_LOADED_BOOL
  CHECK_DEBOUNCE_CLICK_BOOL
  CHECK_ENABLED_BOOL

  

  bool inBounds = POINT_IN_RECT(*_xTouch, *_yTouch, m_xPos, m_yPos, m_config.width, m_config.height);
  if(inBounds) {
    m_myTime = millis();
    onClick();
    m_shouldRedraw = true;
    return true;
  }

  return false;
}

/**
 * @brief Recupera a função callback associada ao TextButton.
 * @return Ponteiro para a função callback.
 * @details Retorna o ponteiro para a função que será executada quando o TextButton for utilizado.
 */
functionCB_t TextButton::getCallbackFunc() { return m_callback; }

/**
 * @brief Recupera o estado atual habilitado do botão.
 * @return True se o botão está habilitado, False caso contrário.
 * @details Retorna o valor de m_enabled.
 */
bool TextButton::getEnabled() { return m_enabled; }

/**
 * @brief Define o estado habilitado do botão.
 * @param newState True para habilitar o botão, False para desabilitá-lo.
 * @details Atualiza o valor de m_enabled.
 */
void TextButton::setEnabled(bool newState) { m_enabled = newState; }

/**
 * @brief Redesenha o botão na tela, atualizando sua aparência.
 * @details Exibe o botão com cores, dimensões e texto especificados.
 *          A aparência ajusta baseado no modo de display atual (claro ou escuro):
 *          - Valida visibilidade, TFT, tela atual, uso de teclado e carregamento
 *          - Desenha retângulo arredondado de fundo com cor de fundo
 *          - Desenha borda arredondada com cor adaptada ao modo claro/escuro
 *          - Seleciona melhor fonte RobotoBold para o texto
 *          - Exibe texto centralizado no botão
 */
void TextButton::redraw() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_LOADED_VOID

  // uint16_t darkBg = WidgetBase::lightMode ? CFK_GREY3 : CFK_GREY11;
  // uint16_t lightBg = WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
  uint16_t baseBorder = WidgetBase::lightMode ? CFK_BLACK : CFK_WHITE;

  WidgetBase::objTFT->fillRoundRect(m_xPos + 1, m_yPos + 1, m_config.width - 2,
                                    m_config.height - 2, m_config.radius,
                                    m_config.backgroundColor); // Botao
  WidgetBase::objTFT->drawRoundRect(m_xPos, m_yPos, m_config.width, m_config.height, m_config.radius,
                                    baseBorder); // borda Botao

  WidgetBase::objTFT->setTextColor(m_config.textColor);

  // WidgetBase::objTFT->setFont(&RobotoBold10pt7b);
  WidgetBase::objTFT->setFont(getBestRobotoBold(
      m_config.width - (2 * m_offsetMargin), m_config.height - (2 * m_offsetMargin), m_config.text));
  printText(m_config.text, m_xPos + m_config.width / 2, m_yPos + (m_config.height / 2), MC_DATUM);
  // showOrigin(CFK_RED);
  // WidgetBase::objTFT->drawCircle(m_xPos + width/2, m_yPos + (height / 2), 4,
  // CFK_WHITE);

  updateFont(FontType::UNLOAD);
}

/**
 * @brief Inicializa as configurações do botão.
 * @details Garante que as dimensões do botão estejam dentro de intervalos válidos baseado no tamanho do display.
 *          Marca o widget como carregado quando completo:
 *          - Restringe largura e altura dentro de bounds válidos (5 até largura/altura da tela)
 *          - Marca como carregado
 */
void TextButton::start() {
#if defined(DISP_DEFAULT)
  m_config.width = constrain(m_config.width, 5, WidgetBase::objTFT->width());
  m_config.height = constrain(m_config.height, 5, WidgetBase::objTFT->height());
#endif

  m_loaded = true;
}

/**
 * @brief Simula um clique no TextButton.
 * @details Dispara a função callback atribuída se ela existir.
 *          Útil para ativar o botão programaticamente:
 *          - Valida se widget está carregado
 *          - Dispara callback se configurado
 */
void TextButton::onClick() {
  if (!m_loaded) {
    ESP_LOGE(TAG, "TextButton widget not loaded");
    return;
  }
  if (m_callback != nullptr) {
    WidgetBase::addCallback(m_callback, WidgetBase::CallbackOrigin::SELF);
  }
}

/**
 * @brief Configura o TextButton com dimensões, cores, texto e função callback específicos.
 * @param _width Largura do botão.
 * @param _height Altura do botão.
 * @param _radius Raio para os cantos arredondados do botão.
 * @param _pressedColor Cor exibida quando o botão está pressionado.
 * @param _textColor Cor do texto exibido no botão.
 * @param _text Texto para exibir no botão.
 * @param _cb Função callback para executar quando o botão é pressionado.
 * @details Configura o widget com os parâmetros fornecidos:
 *          - Define texto e callback
 *          - Marca como carregado
 */
void TextButton::setup(uint16_t _width, uint16_t _height, uint16_t _radius,
                       uint16_t _pressedColor, uint16_t _textColor,
                       const char *_text, functionCB_t _cb) {
  CHECK_TFT_VOID
  if (m_loaded) {
    ESP_LOGD(TAG, "TextButton widget already configured");
    return;
  }
  m_text = _text;
  m_callback = _cb;

  m_loaded = true;
}

/**
 * @brief Configura o TextButton com parâmetros definidos em uma estrutura de configuração.
 * @param config Estrutura @ref TextButtonConfig contendo os parâmetros de configuração.
 * @details Este método deve ser chamado após criar o objeto para configurá-lo adequadamente:
 *          - Limpa memória existente usando cleanupMemory()
 *          - Copia configuração e ponteiros
 *          - Marca o widget como carregado e inicializado
 *          O botão não será exibido corretamente até que este método seja chamado.
 */
void TextButton::setup(const TextButtonConfig &config) {
  CHECK_TFT_VOID
  if (m_loaded) {
    ESP_LOGW(TAG, "TextButton already initialized");
    return;
  }

  // Clean up any existing memory before setting new config
  cleanupMemory();
  
  // Deep copy configuration
  m_config = config;
  m_text = config.text; // Note: This is a const pointer, no deep copy needed
  m_callback = config.callback;
  
  m_loaded = true;
  m_enabled = true;
  
  ESP_LOGD(TAG, "TextButton configured: %dx%d, radius=%d, text='%s'", 
           m_config.width, m_config.height, m_config.radius, m_config.text);
}

/**
 * @brief Torna o TextButton visível na tela.
 * @details Define o widget como visível e marca para redesenho.
 */
void TextButton::show() {
  m_visible = true;
  m_shouldRedraw = true;
}

/**
 * @brief Oculta o TextButton da tela.
 * @details Define o widget como invisível e marca para redesenho.
 */
void TextButton::hide() {
  m_visible = false;
  m_shouldRedraw = true;
}

/**
 * @brief Força o TextButton a atualizar.
 * @details Define a flag de atualização para disparar um redesenho no próximo ciclo.
 */
void TextButton::forceUpdate() { m_shouldRedraw = true; }