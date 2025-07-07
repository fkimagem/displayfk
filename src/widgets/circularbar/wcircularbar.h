#ifndef WCircularBar
#define WCircularBar

#include "../widgetbase.h"
#include "../../fonts/RobotoRegular/Roboto_Regular10pt7b.h"

/// @brief Configuration structure for CircularBar
struct CircularBarConfig {
  uint16_t radius;        ///< Radius of the circular bar
  int minValue;               ///< Minimum value of the bar range
  int maxValue;               ///< Maximum value of the bar range
  uint16_t startAngle;    ///< Starting angle of the circular bar
  uint16_t endAngle;      ///< Ending angle of the circular bar
  uint8_t thickness;         ///< Thickness of the circular bar line
  uint16_t color;         ///< Color of the circular bar
  uint16_t backgroundColor;       ///< Background color of the CircularBar
  uint16_t textColor;     ///< Color of the text displaying the value
  bool showValue;         ///< Flag to show/hide the value text
  bool inverted;          ///< Flag to indicate if the fill direction is inverted
};

/// @brief Represents a circular bar widget used to display a value as a filled arc within a specified range.
class CircularBar : public WidgetBase
{
private:
  unsigned long m_myTime; ///< Timestamp for handling timing-related functions.
  bool m_update; ///< Flag indicating if the display needs to be updated.
  int m_vmin; ///< Minimum value of the bar range.
  int m_vmax; ///< Maximum value of the bar range.
  uint16_t m_startAngle; ///< Starting angle of the circular bar.
  uint16_t m_endAngle; ///< Ending angle of the circular bar.
  uint32_t m_radius; ///< Radius of the CircularBar.
  int m_lastValue; ///< Last value represented by the bar.
  int m_value; ///< Current value to show on the bar.
  uint16_t m_lineColor; ///< Color of the circular bar's edge.
  uint16_t m_textColor; ///< Color of the text displaying the value.
  uint16_t m_bkColor; ///< Background color of the CircularBar.
  uint8_t m_lineWeight; ///< Weight/thickness of the circular bar line.
  //uint16_t m_middleColor; ///< Optional color for the middle of the bar.
  //bool m_invertedFill = false; ///< Flag to indicate if the fill direction is inverted.
  int m_rotation = 0; ///< Rotation angle of the gauge, where 0 is at the middle right.
  bool m_showValue = true; ///< Flag to show/hide the value text.
  TextBound_t m_lastArea = {0, 0, 0, 0}; ///< Last calculated area for the label.

  void start();
  void setup(uint16_t radius, int vmin, int vmax, uint16_t startAngle, uint16_t endAngle, uint8_t weight, uint16_t color, uint16_t bkColor, uint16_t textColor, bool showLabel, bool inverted);

public:
  CircularBar(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~CircularBar();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void drawBackground();
  void setValue(int newValue);
  void redraw();
  void forceUpdate();
  void setup(const CircularBarConfig& config);
};
#endif