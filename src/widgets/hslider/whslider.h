#ifndef WSLIDER
#define WSLIDER

#include "../widgetbase.h"

/// @brief Configuration structure for HSlider
struct HSliderConfig {
  uint16_t width;         ///< Width of the slider track
  uint16_t pressedColor;  ///< Color when the slider handle is pressed
  int minValue;               ///< Minimum value of the slider range
  int maxValue;               ///< Maximum value of the slider range
  uint32_t radius;        ///< Radius of the slider handle
  functionCB_t callback;  ///< Callback function to execute on slider value change
};

/// @brief Represents a horizontal slider widget.
class HSlider : public WidgetBase
{
private:
  unsigned long m_myTime; ///< Timestamp for handling timing-related functions.
  uint16_t m_currentPos; ///< Current position of the slider handle.
  uint16_t m_lastPos; ///< Last recorded position of the slider handle.
  uint16_t m_pressedColor; ///< Color displayed when the slider is pressed.
  int m_vmin; ///< Minimum value for the slider range.
  int m_vmax; ///< Maximum value for the slider range.
  uint32_t m_width; ///< Width of the slider track.
  uint32_t m_height; ///< Height of the slider track.
  uint8_t m_radius = 20; ///< Radius of the slider handle.
  uint8_t m_contentRadius; ///< Radius of the content area within the handle.
  uint16_t m_centerV; ///< Center vertical position of the slider track.
  uint16_t m_minX; ///< Minimum X-coordinate for the slider handle position.
  uint16_t m_maxX; ///< Maximum X-coordinate for the slider handle position.
  int m_value = 0; ///< Current value of the slider.
  bool m_shouldRedraw = false; ///< Flag to indicate if the slider should be redrawn.
  bool m_enabled = true; ///< Indicates whether the slider is enabled or disabled.

  void setup(uint16_t _width, uint16_t _pressedColor, int _vmin, int _vmax, uint32_t _radius, functionCB_t _cb);

public:
  HSlider(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~HSlider();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void redraw();
  void drawBackground();
  void setup(const HSliderConfig& config);
  int getValue();
  void setValue(int _value);
  void forceUpdate();
  bool getEnabled();
  void setEnabled(bool newState);
  void show() override;
  void hide() override;
};
#endif