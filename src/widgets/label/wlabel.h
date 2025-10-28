#ifndef WLABEL
#define WLABEL

#include "../widgetbase.h"

/// @brief Estrutura de configuração para o Label.
/// @details Esta estrutura contém todos os parâmetros necessários para configurar um rótulo de texto.
///          Deve ser preenchida e passada para o método setup().
struct LabelConfig {
  const char* text;          ///< Texto inicial a ser exibido.
  #if defined(USING_GRAPHIC_LIB)
  const GFXfont* fontFamily; ///< Ponteiro para a fonte usada para o texto.
  #endif
  uint16_t datum;            ///< Configuração de alinhamento do texto.
  uint16_t fontColor;        ///< Cor do texto.
  uint16_t backgroundColor;  ///< Cor de fundo do rótulo.
  const char* prefix;        ///< Texto de prefixo a ser exibido.
  const char* suffix;        ///< Texto de sufixo a ser exibido.
};

/// @brief Widget de rótulo de texto com fonte e cores personalizáveis.
/// @details Esta classe herda de @ref WidgetBase e fornece funcionalidade completa para criar e gerenciar 
///          rótulos de texto interativos na tela. O Label pode exibir texto simples, inteiros, floats
///          com casas decimais configuráveis, prefixos e sufixos. Suporta diferentes fontes, cores de
///          texto e fundo, alinhamento e tamanhos de fonte. O widget é totalmente funcional com
///          gerenciamento inteligente de memória para strings dinâmicas.
class Label : public WidgetBase
{
public:
  Label(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~Label();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void setText(const char* str);
  void setText(const String& str);
  void setPrefix(const char* str);
  void setSuffix(const char* str);
  void setTextFloat(float value, uint8_t decimalPlaces = 2);
  void setTextInt(int value);
  void redraw() override;
  void forceUpdate() override;
  void setDecimalPlaces(uint8_t places);
  void setFontSize(uint8_t newSize);
  void setup(const LabelConfig& config);
  void show() override;
  void hide() override;

private:
  static const char* TAG;
  char *m_text = nullptr; ///< Ponteiro para o texto atual exibido pelo rótulo.
  char* m_previousText = nullptr; ///< Ponteiro para o texto anteriormente exibido para comparação.
  char* m_prefix = nullptr; ///< Ponteiro para o texto de prefixo exibido pelo rótulo.
  char* m_suffix = nullptr; ///< Ponteiro para o texto de sufixo exibido pelo rótulo.
  bool m_shouldRedraw = false; ///< Flag indicando se o rótulo deve ser redesenhado.
  TextBound_t m_lastArea = {0, 0, 0, 0}; ///< Última área calculada para o rótulo.
  uint8_t m_fontSize; ///< Tamanho da fonte.
  uint8_t m_decimalPlaces; ///< Número de casas decimais para exibir.
  LabelConfig m_config; ///< Estrutura contendo configuração completa do rótulo.
  
  void cleanupMemory();
};

#endif