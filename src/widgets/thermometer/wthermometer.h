#ifndef WTHERMOMETER
#define WTHERMOMETER

#include "../widgetbase.h"
#include "../label/wlabel.h" // Para ponteiro Label

/// @brief Estrutura de configuração para o Thermometer.
/// @details Esta estrutura contém todos os parâmetros necessários para configurar um termômetro.
///          Deve ser preenchida e passada para o método setup().
struct ThermometerConfig {
  uint16_t width;          ///< Largura da exibição do termômetro.
  uint16_t height;         ///< Altura da exibição do termômetro.
  uint16_t filledColor;    ///< Cor usada para a porção preenchida do termômetro.
  uint16_t backgroundColor;///< Cor usada para o fundo do termômetro.
  uint16_t markColor; ///< Cor usada para as marcas no termômetro.
  int minValue;                ///< Valor mínimo da faixa do termômetro.
  int maxValue;                ///< Valor máximo da faixa do termômetro.
  Label* subtitle; ///< Ponteiro para um widget Label para exibir o valor do termômetro.
  const char* unit; ///< Unidade de medida para o termômetro (ex: "°C", "°F").
  uint8_t decimalPlaces; ///< Número de casas decimais para exibição do valor.
};

/// @brief Widget de termômetro vertical para exibir valores como uma barra preenchida dentro de uma faixa especificada.
/// @details Esta classe herda de @ref WidgetBase e fornece funcionalidade completa para criar e gerenciar 
///          termômetros verticais interativos na tela. O Thermometer desenha um termômetro com bulbo
///          na base, tubo de vidro central com marcações, e área de preenchimento que varia conforme o valor.
///          O widget pode ser configurado com diferentes larguras, alturas, faixas de valores, cores de
///          preenchimento, fundo e marcas. O termômetro é totalmente funcional com suporte a atualização
///          de valores em tempo real, gradientes de cor no bulbo para efeito de luz, e opcionalmente um
///          Label para exibição do valor numérico.
class Thermometer : public WidgetBase
{
public:
  Thermometer(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~Thermometer();
  
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void redraw() override;
  void forceUpdate() override;
  
  void drawBackground();
  void setup(const ThermometerConfig& config);
  void setValue(float newValue);
  
  void show() override;
  void hide() override;

private:
  static const char* TAG; ///< Tag estática para identificação em logs do ESP32.
  
  static constexpr uint8_t m_colorLightGradientSize = 5; ///< Tamanho do array de gradiente de cor.
  float m_currentValue; ///< Valor atual representado pela porção preenchida do termômetro.
  float m_lastValue; ///< Último valor representado pela porção preenchida do termômetro.
  Circle_t m_bulb; ///< Círculo representando o bulbo do termômetro.
  Rect_t m_fillArea; ///< Retângulo representando a área preenchida do termômetro.
  Rect_t m_glassArea; ///< Retângulo representando a área de vidro do termômetro.
  uint16_t m_colorLightGradient[5]; ///< Gradiente de cor para o efeito de luz no bulbo.
  uint16_t m_border; ///< Tamanho da borda ao redor do termômetro.
  ThermometerConfig m_config; ///< Estrutura contendo configuração do termômetro.
  bool m_shouldRedraw; ///< Flag indicando se o termômetro deve ser redesenhado.
  
  void cleanupMemory();
  void start();
  void setup(uint16_t _width, uint16_t _height, uint16_t _filledColor, int _vmin, int _vmax);
};

#endif