#ifndef WLED
#define WLED

#include "../widgetbase.h"
#include <Arduino.h>
#include <esp_log.h>

/// @brief Estrutura de configuração para o Led.
/// @details Esta estrutura contém todos os parâmetros necessários para configurar um LED.
///          Deve ser preenchida e passada para o método setup().
struct LedConfig {
  uint16_t radius; ///< Raio do LED em pixels. Recomendado: 5-50.
  uint16_t colorOn; ///< Cor RGB565 exibida quando o LED está ligado.
  uint16_t colorOff; ///< Cor RGB565 exibida quando o LED está desligado (opcional, 0 = automático).
  bool initialState; ///< Estado inicial do LED (true = ligado, false = desligado).
};

/// @brief Widget de LED com tamanho e cor personalizáveis.
/// @details Esta classe herda de @ref WidgetBase e fornece funcionalidade completa para criar e gerenciar 
///          LEDs interativos na tela. O Led desenha um círculo com efeito de brilho quando ligado,
///          usando gradientes de cor para criar um efeito visual realista. O widget pode ser configurado
///          com diferentes raios, cores para estado ligado/desligado e suporta modo claro/escuro.
///          O LED é totalmente funcional com suporte a estados ligado/desligado e atualização visual.
class Led : public WidgetBase
{
public:
  Led(uint16_t _x, uint16_t _y, uint8_t _screen);
  virtual ~Led();

  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void redraw() override;
  void forceUpdate() override;
  void show() override;
  void hide() override;
  
  void setup(const LedConfig& config);
  void drawBackground();
  void setState(bool newValue);
  bool getState() const;
  void setColor(uint16_t color);

private:
  static const char* TAG; ///< Tag estática para identificação em logs do ESP32.
  static constexpr uint8_t m_colorLightGradientSize = 5; ///< Tamanho do array de gradiente de cor.
  
  bool m_lastStatus; ///< Armazena o último status do LED para comparação.
  bool m_status; ///< Status atual do LED (ligado/desligado).
  bool m_shouldRedraw; ///< Flag indicando se o LED deve ser redesenhado.
  bool m_initialized; ///< Flag para rastrear se o widget foi adequadamente inicializado.
  LedConfig m_config; ///< Estrutura contendo configuração completa do LED (raio, cores).
  uint16_t m_colorLightGradient[5]; ///< Gradiente de cor para o efeito de luz.
  
  void cleanupMemory();
  bool validateConfig(const LedConfig& config);
  void updateGradient();
  uint16_t getOffColor();
  uint16_t getBackgroundColor();
};
#endif