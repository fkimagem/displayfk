#include "wlabel.h"
#include "wlabel.h"

const char* Label::TAG = "Label";

/**
 * @brief Construtor da classe Label.
 * @param _x Coordenada X para a posição do Label.
 * @param _y Coordenada Y para a posição do Label.
 * @param _screen Identificador da tela onde o Label será exibido.
 * @details Inicializa o widget com valores padrão e configuração vazia.
 *          O rótulo não será funcional até que setup() seja chamado.
 */
Label::Label(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen),
m_text(nullptr),
m_previousText(nullptr),
m_prefix(nullptr),
m_suffix(nullptr),
m_shouldRedraw(false),
m_lastArea{0, 0, 0, 0},
m_fontSize(1),
m_decimalPlaces(1)
{
  m_config = {.text = nullptr, .fontFamily = nullptr, .datum = 0, .fontColor = 0, .backgroundColor = 0, .prefix = nullptr, .suffix = nullptr};
  ESP_LOGD(TAG, "Label created at (%d, %d) on screen %d", _x, _y, _screen);
}

/**
 * @brief Limpa memória alocada antes de nova atribuição.
 * @details Libera toda memória alocada dinamicamente para texto, prefixo e sufixo.
 *          Este método é chamado automaticamente por setup() e pelo destrutor.
 */
void Label::cleanupMemory()
{
  // Libera memória do texto atual se existir
  if (m_text != nullptr) {
    delete[] m_text;
    m_text = nullptr;
  }
  
  // Libera memória do texto anterior se existir
  if (m_previousText != nullptr) {
    delete[] m_previousText;
    m_previousText = nullptr;
  }
  
  // Libera memória do prefixo se existir
  if (m_prefix != nullptr) {
    delete[] m_prefix;
    m_prefix = nullptr;
  }
  
  // Libera memória do sufixo se existir
  if (m_suffix != nullptr) {
    delete[] m_suffix;
    m_suffix = nullptr;
  }
}

/**
 * @brief Destrutor da classe Label.
 * @details Libera memória alocada para texto, texto anterior e limpa ponteiro de fonte.
 *          Chama cleanupMemory() para garantir que toda memória seja liberada corretamente.
 */
Label::~Label()
{
  // Clean up all allocated memory
  cleanupMemory();
  
  #if defined(USING_GRAPHIC_LIB)
  // A fonte não é liberada aqui porque é apenas uma referência
  m_config.fontFamily = nullptr;
  #endif
}

/**
 * @brief Detecta se o Label foi tocado.
 * @param _xTouch Ponteiro para a coordenada X do toque na tela.
 * @param _yTouch Ponteiro para a coordenada Y do toque na tela.
 * @return Sempre retorna False, pois labels não respondem a eventos de toque.
 * @details Este método é implementado para conformidade com a interface de WidgetBase
 *          mas não processa toques, já que labels são elementos apenas visuais.
 */
bool Label::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
  return false;
}

/**
 * @brief Recupera a função callback associada ao Label.
 * @return Ponteiro para a função callback.
 * @details Retorna o ponteiro para a função que será executada quando o rótulo for tocado.
 *          Note que labels não detectam toques por padrão.
 */
functionCB_t Label::getCallbackFunc()
{
    return m_callback;
}

/**
 * @brief Define o texto de prefixo do rótulo.
 * @param str Texto do prefixo (nullptr para remover o prefixo).
 * @details Define o prefixo que será exibido antes do texto principal.
 *          Libera memória anterior do prefixo se existir e aloca nova memória para o novo prefixo.
 */
void Label::setPrefix(const char* str){   
  if(m_prefix){
    delete[] m_prefix;
    m_prefix = nullptr;
  }

  if (str != nullptr) {
    // Aloca nova memória para prefixo
    m_prefix = new char[strlen(str) + 1];
    if (m_prefix) {
        strcpy(m_prefix, str);
    } else {
        ESP_LOGE(TAG, "Failed to allocate memory for prefix");
    }
  }
}

/**
 * @brief Define o texto de sufixo do rótulo.
 * @param str Texto do sufixo (nullptr para remover o sufixo).
 * @details Define o sufixo que será exibido após o texto principal.
 *          Libera memória anterior do sufixo se existir e aloca nova memória para o novo sufixo.
 */
void Label::setSuffix(const char* str){
  if(m_suffix){
    delete[] m_suffix;
    m_suffix = nullptr;
  }

  if (str != nullptr) {
    // Aloca nova memória para sufixo
    m_suffix = new char[strlen(str) + 1];
    if (m_suffix) {
        strcpy(m_suffix, str);
    } else {
        ESP_LOGE(TAG, "Failed to allocate memory for suffix");
    }
  }
}

/**
 * @brief Define o texto do rótulo.
 * @param str String a ser definida como texto do rótulo.
 * @details Aplica prefixo e sufixo à string fornecida e atualiza o rótulo:
 *          - Libera memória anterior do texto
 *          - Calcula tamanho total necessário (texto + prefixo + sufixo)
 *          - Aloca nova memória e concatena prefixo, texto e sufixo
 *          - Marca para redesenho se bem-sucedido
 *          Se a alocação falhar, limpa prefixo e sufixo para prevenir memória órfã.
 */
void Label::setText(const char* str)
{
  CHECK_LOADED_VOID
  if(str == nullptr){
    return;
  }

  uint16_t prefixLength = m_prefix ? strlen(m_prefix) : 0;
  uint16_t suffixLength = m_suffix ? strlen(m_suffix) : 0;
  uint16_t textLength = strlen(str);

  // Libera a memória anterior se existir
  if(m_text){
    delete[] m_text;
    m_text = nullptr;
  }
  
  // Aloca nova memória
  m_text = new char[textLength + prefixLength + suffixLength + 1];
  if(m_text){
    // Safely copy prefix, text, and suffix
    if (m_prefix) {
      strcpy(m_text, m_prefix);
    } else {
      m_text[0] = '\0';
    }
    strcat(m_text, str);
    if (m_suffix) {
      strcat(m_text, m_suffix);
    }
    m_shouldRedraw = true;
  } else {
    ESP_LOGE(TAG, "Failed to allocate memory for label text");
    // Clean up prefix and suffix if text allocation failed
    if (m_prefix) {
      delete[] m_prefix;
      m_prefix = nullptr;
    }
    if (m_suffix) {
      delete[] m_suffix;
      m_suffix = nullptr;
    }
  }
}

/**
 * @brief Define o texto do rótulo.
 * @param str Objeto String a ser definido como texto do rótulo.
 * @details Converte o objeto String para C-string e chama a versão char* de setText.
 */
void Label::setText(const String &str)
{
  setText(str.c_str());
}

/**
 * @brief Define o texto do rótulo como um valor float formatado.
 * @param value Valor float a ser definido como texto do rótulo.
 * @param decimalPlaces Quantidade de casas decimais a mostrar.
 * @details Converte o float para string com casas decimais especificadas e atualiza o rótulo:
 *          - Atualiza o número de casas decimais usando setDecimalPlaces()
 *          - Usa dtostrf() para converter float para string com formatação
 *          - Chama setText() para atualizar o texto do rótulo
 */
void Label::setTextFloat(float value, uint8_t decimalPlaces)
{
  /*if(decimalPlaces == 0){
    setTextInt(int(s));
  }else{
    char convertido[16];
    sprintf(convertido, "%.*f", s, decimalPlaces);
    setText(convertido);
  }*/

  setDecimalPlaces(decimalPlaces);

  char convertido[20];
  dtostrf(value, 2, m_decimalPlaces, convertido);
  //sprintf(convertido, "%.2f", s);
  //log_d("Novo float: ", convertido);
  setText(convertido);
}

/**
 * @brief Define o texto do rótulo como um valor inteiro.
 * @param value Valor inteiro a ser definido como texto do rótulo.
 * @details Converte o inteiro para string e atualiza o rótulo:
 *          - Usa sprintf() para converter inteiro para string
 *          - Chama setText() para atualizar o texto do rótulo
 */
void Label::setTextInt(int value)
{
  char convertido[16];
  sprintf(convertido, "%d", value);
  //log_d("setTextInt %d => %s", s, convertido);
  setText(convertido);
}

/**
 * @brief Redesenha o rótulo na tela, atualizando sua aparência.
 * @details Renderiza o texto atual com fonte e cores especificadas:
 *          - Verifica todas as condições necessárias para o redesenho
 *          - Define cor do texto, fonte e tamanho
 *          - Usa printText() para renderizar com alinhamento e fundo apropriados
 *          - Restaura configurações de fonte e tamanho
 *          Apenas redesenha se o rótulo está visível, na tela atual,
 *          carregado e a flag m_shouldRedraw está configurada como true.
 */
void Label::redraw()
{
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  #if defined(USING_GRAPHIC_LIB)
  CHECK_CURRENTSCREEN_VOID
  CHECK_LOADED_VOID
  CHECK_SHOULDREDRAW_VOID

  ESP_LOGD(TAG, "Redraw label to value %s", m_text);
  WidgetBase::objTFT->setTextColor(m_config.fontColor);
  WidgetBase::objTFT->setFont(m_config.fontFamily);
  WidgetBase::objTFT->setTextSize(m_fontSize);
  printText(m_text, m_xPos, m_yPos, m_config.datum, m_lastArea, m_config.backgroundColor);
  WidgetBase::objTFT->setTextSize(1);
  WidgetBase::objTFT->setFont((GFXfont *)0);
  //WidgetBase::objTFT->setCursor(xPos, yPos);
  //WidgetBase::objTFT->print(text);

  
  //previousText = text;
  m_shouldRedraw = false;
  #endif
}

/**
 * @brief Força o rótulo a atualizar, refrescando seu estado visual no próximo redesenho.
 * @details Define a flag m_shouldRedraw como true para garantir que o widget
 *          seja redesenhado na próxima oportunidade.
 */
void Label::forceUpdate()
{
  m_shouldRedraw = true;
}

/**
 * @brief Define quantas casas decimais a string vai mostrar.
 * @param places Quantidade (deve estar entre 0 e 5).
 * @details Restringe o valor para a faixa válida (0-5) usando constrain().
 *          Se o valor mudou, atualiza a variável m_decimalPlaces e registra no log.
 */
void Label::setDecimalPlaces(uint8_t places)
{
  places = constrain(places, 0, 5);
  if(places != m_decimalPlaces){
    m_decimalPlaces = places;
    ESP_LOGD(TAG, "Decimal places set to %d", m_decimalPlaces);
  }
}

/**
 * @brief Define o tamanho da fonte.
 * @param newSize Tamanho a ser definido para a fonte.
 * @details Define diretamente o tamanho da fonte que será usado na renderização do texto.
 */
void Label::setFontSize(uint8_t newSize)
{
  m_fontSize = newSize;
}

/**
 * @brief Configura o widget Label com parâmetros definidos em uma estrutura de configuração.
 * @param config Estrutura @ref LabelConfig contendo os parâmetros de configuração do rótulo.
 * @details Este método deve ser chamado após criar o objeto para configurá-lo adequadamente:
 *          - Verifica se TFT está definido e se widget já está carregado
 *          - Limpa memória existente usando cleanupMemory()
 *          - Copia texto, prefixo e sufixo para buffers internos com validação de tamanho
 *          - Atribui configuração de fonte, alinhamento e cores
 *          - Inicializa área do último texto desenhado
 *          - Marca o widget como inicializado e carregado
 *          O rótulo não será exibido corretamente até que este método seja chamado.
 */
void Label::setup(const LabelConfig& config)
{
  CHECK_TFT_VOID
  if (m_loaded) {
    ESP_LOGD(TAG, "Label widget already configured");
    return;
  }

  // Clean up existing memory first
  cleanupMemory();

  // Copy config to member
  m_config = config;

  // Deep copy for text, prefix, and suffix
  const char* text = config.text ? config.text : "";
  const char* prefix = config.prefix ? config.prefix : "";
  const char* suffix = config.suffix ? config.suffix : "";

  uint16_t prefixLength = strlen(prefix);
  uint16_t suffixLength = strlen(suffix);
  uint16_t textLength = strlen(text);

  // Allocate memory for prefix
  m_prefix = new char[prefixLength + 1];
  if (!m_prefix) {
    ESP_LOGE(TAG, "Failed to allocate memory for prefix");
    return;
  }
  strcpy(m_prefix, prefix);

  // Allocate memory for suffix
  m_suffix = new char[suffixLength + 1];
  if (!m_suffix) {
    ESP_LOGE(TAG, "Failed to allocate memory for suffix");
    delete[] m_prefix;
    m_prefix = nullptr;
    return;
  }
  strcpy(m_suffix, suffix);

  // Allocate memory for combined text
  m_text = new char[textLength + prefixLength + suffixLength + 1];
  if(!m_text) {
    ESP_LOGE(TAG, "Failed to allocate memory for label text");
    delete[] m_prefix;
    delete[] m_suffix;
    m_prefix = nullptr;
    m_suffix = nullptr;
    ESP_LOGE(TAG, "Failed to allocate memory for label text");
    return;
  }
  // Safely copy prefix, text, and suffix
  strcpy(m_text, prefix);
  strcat(m_text, text);
  strcat(m_text, suffix);
    
    #if defined(USING_GRAPHIC_LIB)
    m_config.fontFamily = config.fontFamily;
    #endif
    m_config.datum = config.datum;
    m_config.fontColor = config.fontColor;
    m_config.backgroundColor = config.backgroundColor;
    
    m_lastArea.x = 0;
    m_lastArea.y = 0;
    m_lastArea.width = 0;
    m_lastArea.height = 0;
    m_shouldRedraw = true;
    m_loaded = true;
}

/**
 * @brief Torna o rótulo visível na tela.
 * @details Define o widget como visível e marca para redesenho.
 */
void Label::show()
{
    m_visible = true;
    m_shouldRedraw = true;
}

/**
 * @brief Oculta o rótulo da tela.
 * @details Define o widget como invisível e marca para redesenho.
 */
void Label::hide()
{
    m_visible = false;
    m_shouldRedraw = true;
}