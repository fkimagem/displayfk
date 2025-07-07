#ifndef WLED
#define WLED

#include "../widgetbase.h"

/// @brief Configuration structure for Led
struct LedConfig {
  uint16_t radius; ///< Radius of the LED.
  uint16_t colorOn; ///< Color displayed when the LED is on.
};

/// @brief Represents an LED widget with customizable size and color.
class Led : public WidgetBase
{
private:
  bool m_lastStatus; ///< Stores the last status of the LED for comparison.
  bool m_update; ///< Flag indicating if the LED should be updated.
  uint16_t m_radius; ///< Radius of the LED.
  uint16_t m_colorOn; ///< Color displayed when the LED is on.
  bool m_status = false; ///< Current on/off status of the LED.

  void setup(uint16_t _radius, uint16_t _colorOn);

public:
 
  Led(uint16_t _x, uint16_t _y, uint8_t _screen);

 
  ~Led();

  
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;

  
  void setState(bool newValue);

  
  void redraw();

  
  void forceUpdate();

  
  void setup(const LedConfig& config);
};
#endif