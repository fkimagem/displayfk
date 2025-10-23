#ifndef WCircularBar
#define WCircularBar

#include "../widgetbase.h"

#if defined(DISP_DEFAULT)
#include "../../fonts/RobotoRegular/RobotoRegular10pt7b.h"
#endif

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
  uint16_t backgroundText;///< Background color of the text area
  bool showValue;         ///< Flag to show/hide the value text
  bool inverted;          ///< Flag to indicate if the fill direction is inverted
};

/// @brief Represents a circular bar widget used to display a value as a filled arc within a specified range.
class CircularBar : public WidgetBase
{
private:
  static const char* TAG; ///< Tag for logging identification
  int m_lastValue; ///< Last value represented by the bar.
  int m_value; ///< Current value to show on the bar.
  int m_rotation = 0; ///< Rotation angle of the gauge, where 0 is at the middle right.
  TextBound_t m_lastArea = {0, 0, 0, 0}; ///< Last calculated area for the label.
  CircularBarConfig m_config; ///< Configuration for the CircularBar.

  void start();

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
};
#endif