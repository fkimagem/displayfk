#ifndef WVERTICALBAR
#define WVERTICALBAR

#include "../widgetbase.h"

/// @brief Configuration structure for VBar
struct VerticalBarConfig {
  uint16_t width;          ///< Width of the VBar display
  uint16_t height;         ///< Height of the VBar display
  uint16_t filledColor;    ///< Color used for the filled portion of the bar
  int minValue;                ///< Minimum value of the bar range
  int maxValue;                ///< Maximum value of the bar range
  int round;
  Orientation orientation; ///< Orientation of the bar (vertical or horizontal)
};

/// @brief Represents a vertical bar widget, used to display a value as a filled bar within a specified range.
class VBar : public WidgetBase
{
private:
  static const char* TAG; ///< Tag for logging
  uint16_t m_filledColor; ///< Color used for the filled portion of the bar.
  uint32_t m_vmin; ///< Minimum value of the bar range.
  uint32_t m_vmax; ///< Maximum value of the bar range.
  uint32_t m_width; ///< Width of the VBar display.
  uint32_t m_height; ///< Height of the VBar display.
  uint32_t m_currentValue; ///< Current value represented by the filled portion of the bar.
  uint32_t m_lastValue; ///< Last value represented by the filled portion of the bar.
  Orientation m_orientation; ///< Orientation of the bar (vertical or horizontal).
  int m_round = 0;
  bool m_shouldRedraw = false; ///< Flag indicating if the VBar should be redrawn.

  void start();
  void setup(uint16_t _width, uint16_t _height, uint16_t _filledColor, int _vmin, int _vmax, int _round, Orientation _orientation);

public:
  VBar(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~VBar();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void setValue(uint32_t newValue);
  void redraw() override;
  void forceUpdate() override;
  void drawBackground();
  void setup(const VerticalBarConfig& config);
  void show() override;
  void hide() override;
};
#endif