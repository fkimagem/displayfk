#ifndef WSPINBOX
#define WSPINBOX

#include "../widgetbase.h"

/// @brief Configuration structure for SpinBox
struct SpinBoxConfig {
  uint16_t width;         ///< Width of the SpinBox
  uint16_t height;        ///< Height of the SpinBox
  uint16_t step;          ///< Step value for the spin box increments and decrements
  int minValue;                ///< Minimum value of the SpinBox range
  int maxValue;                ///< Maximum value of the SpinBox range
  int startValue;         ///< Current value of the SpinBox
  uint16_t color;         ///< Color for the display elements
  uint16_t textColor;     ///< Text color
  functionCB_t callback;  ///< Callback function to execute when the value changes
};

/// @brief Represents a spin box widget for numeric input with increment and decrement buttons.
class SpinBox : public WidgetBase
{
private:
  static const char* TAG;
  uint16_t m_pressedColor; ///< Color displayed when the SpinBox is pressed.
  bool m_status; ///< Current status of the SpinBox (pressed or not).
  int m_currentValue; ///< Current numeric value of the SpinBox.
  uint8_t m_radius = 10; ///< Radius for the circular buttons in the SpinBox.
  uint8_t m_offset = 5; ///< Offset for positioning elements within the SpinBox.
  TextBound_t m_lastArea = {0,0,0,0}; ///< Last area of the SpinBox.


  SpinBoxConfig m_config; ///< Configuration structure for the SpinBox.

  void cleanupMemory();
  void start();
  void increaseValue();
  void decreaseValue();
  void setup(uint16_t _width, uint16_t _height, uint16_t _step, int _min, int _max, int _startValue, uint16_t _cor, uint16_t _textCor, functionCB_t _cb);

public:
  SpinBox(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~SpinBox();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void redraw() override;
  void forceUpdate() override;
  void drawBackground();
  void setup(const SpinBoxConfig& config);
  int getValue();
  void setValue(int _value);
  void show() override;
  void hide() override;
};

#endif