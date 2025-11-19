#ifndef WTEXTBUTTON
#define WTEXTBUTTON

#include "../widgetbase.h"

/// @brief Estrutura de configuração para o TextButton.
/// @details Esta estrutura contém todos os parâmetros necessários para configurar um botão de texto.
///          Deve ser preenchida e passada para o método setup().
struct TextButtonConfig {
  uint16_t width;         ///< Largura do botão.
  uint16_t height;        ///< Altura do botão.
  uint16_t radius;        ///< Raio para os cantos arredondados do botão.
  uint16_t backgroundColor;  ///< Cor exibida quando o botão está pressionado.
  uint16_t textColor;     ///< Cor do texto exibido no botão.
  const char* text;       ///< Texto para exibir no botão.
  const GFXfont* fontFamily; ///< Ponteiro para a fonte usada para o texto.
  functionCB_t callback;  ///< Função callback para executar quando o botão é pressionado.
};

/// @brief Widget de botão com texto personalizável, cores e cantos arredondados.
/// @details Esta classe herda de @ref WidgetBase e fornece funcionalidade completa para criar e gerenciar 
///          botões de texto interativos na tela. O TextButton desenha um botão com cantos arredondados
///          que exibe texto no centro, com cores de fundo e texto configuráveis. O widget pode ser
///          configurado com diferentes larguras, alturas, raios de bordas arredondadas, cores de fundo,
///          cores de texto, textos e callbacks. O botão é totalmente funcional com suporte a estados
///          habilitado/desabilitado, detecção de toque e callbacks para ações.
class TextButton : public WidgetBase
{
public:
  TextButton(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~TextButton();
  
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void redraw() override;
  void forceUpdate() override;
  
  void setup(const TextButtonConfig& config);
  void onClick();
  bool getEnabled();
  void setEnabled(bool newState);
  
  void show() override;
  void hide() override;

private:
  static const char* TAG; ///< Tag estática para identificação em logs do ESP32.
  
  unsigned long m_myTime; ///< Timestamp para manipular funções relacionadas a tempo.
  uint8_t m_offsetMargin; ///< Margem de offset para posicionar o texto dentro do botão.
  const char* m_text; ///< Texto exibido no botão.
  bool m_enabled; ///< Indica se o botão está criado ou desabilitado.
  TextButtonConfig m_config; ///< Estrutura contendo configuração do botão de texto.
  
  void cleanupMemory();
  void start();
  void setup(uint16_t _width, uint16_t _height, uint16_t _radius, uint16_t _pressedColor, uint16_t _textColor, const char* _text, functionCB_t _cb);
};

#endif