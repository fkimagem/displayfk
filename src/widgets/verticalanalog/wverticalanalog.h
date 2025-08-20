#ifndef WVANALOG
#define WVANALOG

#include "../widgetbase.h"
#if defined(USING_GRAPHIC_LIB)
#include "../../fonts/RobotoRegular/RobotoRegular10pt7b.h"
#endif

/// @brief Configuration structure for VAnalog
struct VerticalAnalogConfig {
  uint16_t width;           ///< Width of the VAnalog display
  uint16_t height;          ///< Height of the VAnalog display
  int minValue;                 ///< Minimum value of the scale
  int maxValue;                 ///< Maximum value of the scale
  uint8_t steps;            ///< Number of divisions on the scale
  uint16_t arrowColor;      ///< Color of the arrow marker
  uint16_t textColor;       ///< Color of the text display
  uint16_t backgroundColor; ///< Background color of the display
  uint16_t borderColor;     ///< Color of the display border
};

/// @brief Represents a vertical analog display widget, used to visualize numeric values on a vertical scale.
class VAnalog : public WidgetBase
{
private:
  uint16_t m_padding; ///< Padding around the display area.
  uint16_t m_paddingDraw; ///< Padding within the drawing area.
  uint16_t m_drawArea; ///< Area for drawing the analog scale.
  uint32_t m_lastYArrow; ///< Last Y-coordinate of the arrow marker.
  bool m_update; ///< Flag indicating if the display needs to be updated.
  bool m_updateText; ///< Flag indicating if the text display needs to be updated.
  uint16_t m_filledColor; ///< Color used for the filled portion of the scale.
  uint32_t m_vmin; ///< Minimum value of the scale.
  uint32_t m_vmax; ///< Maximum value of the scale.
  uint32_t m_width; ///< Width of the VAnalog display.
  uint32_t m_height; ///< Height of the VAnalog display.
  uint16_t m_steps; ///< Number of divisions on the scale.
  int m_currentValue; ///< Current value displayed on the scale.
  int m_lastValue; ///< Last value displayed on the scale.
  uint16_t m_backgroundColor; ///< Background color of the display.
  uint16_t m_textColor; ///< Color of the text display.
  uint16_t m_arrowColor; ///< Color of the arrow marker.
  uint16_t m_borderColor; ///< Color of the display border.

  void start();

  void setup(uint16_t _width, uint16_t _height, int _vmin, int _vmax, uint8_t _steps, uint16_t _arrowColor, uint16_t _textColor, uint16_t _backgroundColor, uint16_t _borderColor);

public:

  VAnalog(uint16_t _x, uint16_t _y, uint8_t _screen);

  ~VAnalog();

  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;

  functionCB_t getCallbackFunc() override;

  void drawBackground();

  void setValue(int newValue, bool _viewValue);

  void redraw();

  void forceUpdate();

  void setup(const VerticalAnalogConfig& config);
};

#endif