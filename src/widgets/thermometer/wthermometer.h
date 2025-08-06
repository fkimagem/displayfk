#ifndef WTHERMOMETER
#define WTHERMOMETER

#include "../widgetbase.h"
#include "../label/wlabel.h" // Para ponteiro Label

/// @brief Configuration structure for VBar
struct ThermometerConfig {
  uint16_t width = 0;          ///< Width of the VBar display
  uint16_t height = 0;         ///< Height of the VBar display
  uint16_t filledColor = 0x0;    ///< Color used for the filled portion of the bar
  int minValue = 0;                ///< Minimum value of the bar range
  int maxValue = 0;                ///< Maximum value of the bar range
  Label* subtitle = nullptr;
  const char* unit = nullptr;
};

/// @brief Represents a vertical bar widget, used to display a value as a filled bar within a specified range.
class Thermometer : public WidgetBase
{
private:
    static constexpr uint8_t m_colorLightGradientSize = 5;
  bool m_update; ///< Flag indicating if the display needs to be updated.
  uint16_t m_filledColor; ///< Color used for the filled portion of the bar.
  uint32_t m_vmin; ///< Minimum value of the bar range.
  uint32_t m_vmax; ///< Maximum value of the bar range.
  uint32_t m_width; ///< Width of the VBar display.
  uint32_t m_height; ///< Height of the VBar display.
  float m_currentValue; ///< Current value represented by the filled portion of the bar.
  float m_lastValue; ///< Last value represented by the filled portion of the bar.
  Circle_t m_bulb; ///< Circle representing the bulb of the thermometer.
  Rect_t m_fillArea; ///< Rectangle representing the filled area of the thermometer.
  Rect_t m_glassArea; ///< Rectangle representing the filled area of the thermometer.
  uint16_t m_colorLightGradient[Thermometer::m_colorLightGradientSize]; ///< Color gradient for the light effect.
    uint16_t m_border = 5;
    ThermometerConfig m_config;

  void start();
  void setup(uint16_t _width, uint16_t _height, uint16_t _filledColor, int _vmin, int _vmax);

public:
  Thermometer(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~Thermometer();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void setValue(float newValue);
  void redraw();
  void forceUpdate();
  void drawBackground();
  void setup(const ThermometerConfig& config);
};
#endif