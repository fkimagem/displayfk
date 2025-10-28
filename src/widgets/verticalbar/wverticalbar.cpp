#include "wverticalbar.h"

const char* VBar::TAG = "[VBar]";

/**
 * @brief Construtor do widget VBar.
 * @param _x Coordenada X para a posição do VBar.
 * @param _y Coordenada Y para a posição do VBar.
 * @param _screen Identificador da tela onde o VBar será exibido.
 * @details Inicializa o widget VBar com posição e tela especificadas.
 */
VBar::VBar(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen)
{
}

/**
 * @brief Limpa recursos alocados pelo VBar.
 * @details Desaloca memória dinâmica e remove referências a objetos.
 */
void VBar::cleanupMemory() {
  // VBar não aloca memória dinâmica no momento
}

/**
 * @brief Destrutor do widget VBar.
 * @details Limpa recursos alocados.
 */
VBar::~VBar()
{
  cleanupMemory();
}

/**
 * @brief Detecta um toque no widget VBar.
 * @param _xTouch Ponteiro para a coordenada X do toque.
 * @param _yTouch Ponteiro para a coordenada Y do toque.
 * @return True se o toque é detectado, False caso contrário.
 * @details Método override que sempre retorna false. O VBar não tem detecção de toque ativa.
 */
bool VBar::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
  return false;
}

/**
 * @brief Recupera a função callback para o widget VBar.
 * @return Ponteiro para a função callback.
 * @details Retorna o ponteiro para a função que será executada quando o VBar for utilizado.
 */
functionCB_t VBar::getCallbackFunc()
{
  return m_callback;
}

/**
 * @brief Define o valor do widget VBar.
 * @param newValue Novo valor para definir.
 * @details Atualiza o valor da barra:
 *          - Restringe valor usando constrain() dentro da faixa min/max
 *          - Marca para redesenho
 */
void VBar::setValue(uint32_t newValue)
{
  m_currentValue = constrain(newValue, m_config.minValue, m_config.maxValue);
  // Serial.println("ajusta currentValue: " + String(currentValue));
  m_shouldRedraw = true;
  // redraw();
}

/**
 * @brief Redesenha o widget VBar.
 * @details Atualiza a exibição da barra:
 *          - Valida TFT, visibilidade, tela atual, uso de teclado, carregamento e flag de redesenho
 *          - Calcula altura/largura proporcional usando map() baseado no valor atual
 *          - Atualiza área preenchida (limpando se valor diminuiu, preenchendo se aumentou)
 *          - Suporta orientação vertical e horizontal
 *          - Usa cantos arredondados conforme configurado
 *          - Armazena último valor e reseta flag de redesenho
 */
void VBar::redraw()
{
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_LOADED_VOID
  CHECK_SHOULDREDRAW_VOID
  
  int innerX = m_xPos + 1;
  int innerY = m_yPos + 1;
  int innerHeight = m_config.height - 2;
  int innerWidth = m_config.width - 2;
  int minHeight = m_config.round;
  int minWidth = m_config.round;
  int innerRound = m_config.round > 0 ? m_config.round - 1 : m_config.round;

  if(m_config.orientation == Orientation::VERTICAL){
	
	uint32_t proportionalHeight = map(m_currentValue, m_config.minValue, m_config.maxValue, minHeight, innerHeight);
	
	if(m_currentValue < m_lastValue){
		uint32_t clearArea = map(m_config.maxValue - m_currentValue, m_config.minValue, m_config.maxValue, minHeight, innerHeight);
		WidgetBase::objTFT->fillRoundRect(innerX, innerY, innerWidth, clearArea, innerRound, CFK_GREY11); // fundo total
		
	}

    
    WidgetBase::objTFT->fillRoundRect(innerX, innerY + (innerHeight - proportionalHeight), innerWidth, proportionalHeight, innerRound, m_config.filledColor); // cor fill
    //WidgetBase::objTFT->drawRoundRect(innerX, innerY + (innerHeight - proportionalHeight), innerWidth, proportionalHeight, innerRound, CFK_BLACK);   // borda fill

  }else if(m_config.orientation == Orientation::HORIZONTAL){
	  
	  if(m_currentValue < m_lastValue){
		uint32_t clearArea = map(m_config.maxValue - m_currentValue, m_config.minValue, m_config.maxValue, minWidth, innerWidth);
		uint32_t xValue = map(m_currentValue, m_config.minValue, m_config.maxValue, innerX, innerX + innerWidth);
		
		WidgetBase::objTFT->fillRoundRect(xValue, innerY, clearArea, innerHeight, innerRound, CFK_GREY11); // fundo total
		
	}

    uint32_t proportionalWidth = map(m_currentValue, m_config.minValue, m_config.maxValue, minWidth, innerWidth); // O +1 é para tirar a borda da contagem
    WidgetBase::objTFT->fillRoundRect(innerX, innerY, proportionalWidth, innerHeight, innerRound, m_config.filledColor); // cor fill
	
    //WidgetBase::objTFT->drawRoundRect(xPos, yPos, m_width, m_height, m_round,CFK_BLACK);   // borda fill

  }

  
  m_lastValue = m_currentValue;
  m_shouldRedraw = false;
}

/**
 * @brief Inicia o widget VBar.
 * @details Garante que a largura e altura da barra estejam dentro de intervalos válidos:
 *          - Restringe altura e largura dentro de bounds válidos (20 até dimensão da tela)
 */
void VBar::start()
{
#if defined(USING_GRAPHIC_LIB)
  m_config.height = constrain(m_config.height, 20, WidgetBase::objTFT->height());
  m_config.width = constrain(m_config.width, 20, WidgetBase::objTFT->width());
#endif
}

/**
 * @brief Força o widget VBar a atualizar.
 * @details Define a flag de atualização para disparar um redesenho no próximo ciclo.
 */
void VBar::forceUpdate()
{
  m_shouldRedraw = true;
}

/**
 * @brief Desenha o fundo do widget VBar.
 * @details Desenha o container visual da barra incluindo fundo e borda:
 *          - Valida TFT e visibilidade
 *          - Desenha fundo com cantos arredondados
 *          - Desenha borda ao redor da barra
 */
void VBar::drawBackground()
{
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_LOADED_VOID
  CHECK_SHOULDREDRAW_VOID
  WidgetBase::objTFT->fillRoundRect(m_xPos, m_yPos, m_config.width, m_config.height, m_config.round, CFK_GREY11); // fundo total
  WidgetBase::objTFT->drawRoundRect(m_xPos, m_yPos, m_config.width, m_config.height, m_config.round, CFK_BLACK);     // borda total
}


/**
 * @brief Configura o VBar com parâmetros definidos em uma estrutura de configuração.
 * @param config Estrutura @ref VerticalBarConfig contendo os parâmetros de configuração.
 * @details Chama método setup() com parâmetros individuais da configuração.
 */
void VBar::setup(const VerticalBarConfig& config)
{
  if (!WidgetBase::objTFT)
  {
    ESP_LOGE(TAG, "TFT not defined on WidgetBase");
    return;
  }
  if (m_loaded)
  {
    ESP_LOGD(TAG, "VBar widget already configured");
    return;
  }
  m_config = config;
  start();
  m_loaded = true;
  m_initialized = true;
}

/**
 * @brief Torna o VBar visível na tela.
 * @details Define o widget como visível e marca para redesenho.
 */
void VBar::show()
{
    m_visible = true;
    m_shouldRedraw = true;
}

/**
 * @brief Oculta o VBar da tela.
 * @details Define o widget como invisível e marca para redesenho.
 */
void VBar::hide()
{
    m_visible = false;
    m_shouldRedraw = true;
}