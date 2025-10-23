#ifndef WTHERMOMETER
#define WTHERMOMETER

#include "../widgetbase.h"
#include "../label/wlabel.h" // Para ponteiro Label

/// @brief Configuration structure for VBar
struct ThermometerConfig {
  uint16_t width;          ///< Width of the VBar display
  uint16_t height;         ///< Height of the VBar display
  uint16_t filledColor;    ///< Color used for the filled portion of the bar
  uint16_t backgroundColor;///< Color used for the background of the bar
  uint16_t markColor; ///< Color used for the marks on the bar
  int minValue;                ///< Minimum value of the bar range
  int maxValue;                ///< Maximum value of the bar range
  Label* subtitle;
  const char* unit;
  uint8_t decimalPlaces;
};

/// @brief Represents a vertical bar widget, used to display a value as a filled bar within a specified range.
class Thermometer : public WidgetBase
{
private:
  static const char* TAG; ///< Tag for logging
  static constexpr uint8_t m_colorLightGradientSize = 5;
  // uint16_t m_filledColor; ///< Color used for the filled portion of the bar.
  // uint32_t m_vmin; ///< Minimum value of the bar range.
  // uint32_t m_vmax; ///< Maximum value of the bar range.
  // uint32_t m_width; ///< Width of the VBar display.
  // uint32_t m_height; ///< Height of the VBar display.
  float m_currentValue; ///< Current value represented by the filled portion of the bar.
  float m_lastValue; ///< Last value represented by the filled portion of the bar.
  Circle_t m_bulb; ///< Circle representing the bulb of the thermometer.
  Rect_t m_fillArea; ///< Rectangle representing the filled area of the thermometer.
  Rect_t m_glassArea; ///< Rectangle representing the filled area of the thermometer.
  uint16_t m_colorLightGradient[Thermometer::m_colorLightGradientSize]; ///< Color gradient for the light effect.
  uint16_t m_border = 5;
  ThermometerConfig m_config;
  bool m_shouldRedraw = false; ///< Flag indicating if the Thermometer should be redrawn.

  void cleanupMemory();
  void start();
  void setup(uint16_t _width, uint16_t _height, uint16_t _filledColor, int _vmin, int _vmax);

public:
  Thermometer(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~Thermometer();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void setValue(float newValue);
  void redraw() override;
  void forceUpdate() override;
  void drawBackground();
  void setup(const ThermometerConfig& config);
  void show() override;
  void hide() override;
};
#endif