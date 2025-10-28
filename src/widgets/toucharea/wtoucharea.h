#ifndef WTOUCHAREA
#define WTOUCHAREA

#include "../widgetbase.h"

/// @brief Estrutura de configuração para o TouchArea.
/// @details Esta estrutura contém todos os parâmetros necessários para configurar uma área de toque.
///          Deve ser preenchida e passada para o método setup().
struct TouchAreaConfig {
  uint16_t width;         ///< Largura da área de toque.
  uint16_t height;        ///< Altura da área de toque.
  functionCB_t callback;  ///< Função callback para executar quando a área é tocada.
};

/// @brief Representa um widget de área sensível ao toque com tamanho customizável e rastreamento de status.
/// @details Esta classe herda de @ref WidgetBase e fornece funcionalidade completa para criar e gerenciar
///          áreas sensíveis ao toque invisíveis na tela. O TouchArea define uma região retangular que
///          detecta toques e executa callbacks, mas não renderiza nenhuma aparência visual.
///          O widget pode ser configurado com diferentes tamanhos e callbacks.
///          O TouchArea é totalmente funcional com suporte a toque em displays capacitivos e touchscreen,
///          detectando toques dentro de sua área e disparando callbacks.
class TouchArea : public WidgetBase
{
public:
  TouchArea(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~TouchArea();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void changeState();
  void redraw() override;
  void forceUpdate() override;
  void setup(const TouchAreaConfig& config);
  bool getStatus();
  void onClick();
  void hide() override;
  void show() override;

private:
  static const char* TAG; ///< Tag estática para identificação em logs do ESP32.
  bool m_status; ///< Status atual do TouchArea.
  TouchAreaConfig m_config; ///< Estrutura de configuração para o TouchArea.

  void cleanupMemory();
  void setup(uint16_t _width, uint16_t _height, functionCB_t _cb);
};

#endif
