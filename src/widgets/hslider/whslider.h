#ifndef WSLIDER
#define WSLIDER

#include "../widgetbase.h"
#include <Arduino.h>
#include <esp_log.h>

/// @brief Configuration structure for HSlider
struct HSliderConfig {
  uint16_t width;         ///< Width of the slider track
  uint16_t pressedColor;  ///< Color when the slider handle is pressed
  uint16_t backgroundColor; ///< Background color of the slider
  int minValue;           ///< Minimum value of the slider range
  int maxValue;           ///< Maximum value of the slider range
  uint32_t radius;        ///< Radius of the slider handle
  functionCB_t callback;  ///< Callback function to execute on slider value change
};

/// @brief Represents a horizontal slider widget.
class HSlider : public WidgetBase
{
private:
  static const char* TAG; ///< Tag for logging identification
  
  // Configuration
  HSliderConfig m_config; ///< Configuration for the HSlider
  
  // Internal state
  uint16_t m_currentPos; ///< Current position of the slider handle
  uint16_t m_lastPos; ///< Last recorded position of the slider handle
  uint32_t m_height; ///< Height of the slider track
  uint8_t m_contentRadius; ///< Radius of the content area within the handle
  uint16_t m_centerV; ///< Center vertical position of the slider track
  uint16_t m_minX; ///< Minimum X-coordinate for the slider handle position
  uint16_t m_maxX; ///< Maximum X-coordinate for the slider handle position
  int m_value; ///< Current value of the slider
  
  // Helper methods
  void start();
  bool validateConfig(const HSliderConfig& config);
  void updateDimensions();
  void updateValue();

public:
  HSlider(uint16_t _x, uint16_t _y, uint8_t _screen);
  virtual ~HSlider();
  
  // Required overrides
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void redraw() override;
  void forceUpdate() override;
  void show() override;
  void hide() override;
  
  // Configuration
  void setup(const HSliderConfig& config);
  
  // Widget-specific methods
  int getValue() const;
  void setValue(int newValue);
  void drawBackground();
};
#endif