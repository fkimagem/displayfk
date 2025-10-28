#ifndef WCircularBar
#define WCircularBar

#include "../widgetbase.h"

#if defined(DISP_DEFAULT)
#include "../../fonts/RobotoRegular/RobotoRegular10pt7b.h"
#endif

/// @brief Estrutura de configuração para o CircularBar.
/// @details Esta estrutura contém todos os parâmetros necessários para configurar uma barra circular.
///          Deve ser preenchida e passada para o método setup().
struct CircularBarConfig {
  uint16_t radius; ///< Raio da barra circular em pixels.
  int minValue; ///< Valor mínimo da faixa da barra.
  int maxValue; ///< Valor máximo da faixa da barra.
  uint16_t startAngle; ///< Ângulo inicial da barra circular em graus.
  uint16_t endAngle; ///< Ângulo final da barra circular em graus.
  uint8_t thickness; ///< Espessura da linha da barra circular em pixels.
  uint16_t color; ///< Cor RGB565 da barra circular quando preenchida.
  uint16_t backgroundColor; ///< Cor RGB565 de fundo da barra circular.
  uint16_t textColor; ///< Cor RGB565 do texto que exibe o valor.
  uint16_t backgroundText; ///< Cor RGB565 do fundo da área do texto.
  bool showValue; ///< Flag para mostrar/ocultar o texto do valor.
  bool inverted; ///< Flag para indicar se a direção de preenchimento está invertida.
};

/// @brief Widget de barra circular que exibe um valor como um arco preenchido dentro de uma faixa especificada.
/// @details Esta classe herda de @ref WidgetBase e fornece funcionalidade completa para criar e gerenciar 
///          barras circulares interativas na tela. O CircularBar desenha um arco circular que se preenche
///          proporcionalmente ao valor atual dentro da faixa configurada. O widget pode ser configurado
///          com diferentes raios, ângulos, espessuras, cores e pode exibir o valor atual no centro.
///          A barra é totalmente funcional com suporte a animação suave e atualização em tempo real.
class CircularBar : public WidgetBase
{
public:
  CircularBar(uint16_t _x, uint16_t _y, uint8_t _screen);
  virtual ~CircularBar();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void redraw() override;
  void forceUpdate() override;
  void show() override;
  void hide() override;
  void setup(const CircularBarConfig& config);
  void drawBackground();
  void setValue(int newValue);

private:
  static const char* TAG; ///< Tag estática para identificação em logs do ESP32.
  int m_lastValue; ///< Último valor representado pela barra.
  int m_value; ///< Valor atual a ser exibido na barra.
  int m_rotation; ///< Ângulo de rotação do gauge, onde 0 está no meio direito.
  TextBound_t m_lastArea; ///< Última área calculada para o rótulo.
  CircularBarConfig m_config; ///< Estrutura contendo configuração completa da barra circular.

  void start();
};
#endif