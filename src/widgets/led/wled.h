#ifndef WLED
#define WLED

#include "../widgetbase.h"
#include <Arduino.h>
#include <esp_log.h>

/// @brief Configuration structure for Led
struct LedConfig {
  uint16_t radius; ///< Radius of the LED
  uint16_t colorOn; ///< Color displayed when the LED is on
  uint16_t colorOff; ///< Color displayed when the LED is off (optional, 0 = auto)
};

/// @brief Represents an LED widget with customizable size and color.
class Led : public WidgetBase
{
private:
  static const char* TAG; ///< Tag for logging identification
  static constexpr uint8_t m_colorLightGradientSize = 5; ///< Size of the color gradient array
  
  // Configuration
  
  // Internal state
  bool m_lastStatus; ///< Stores the last status of the LED for comparison
  bool m_status; ///< Current on/off status of the LED
  bool m_shouldRedraw; ///< Flag indicating if the LED should be redrawn
  bool m_initialized; ///< Flag to track if the widget has been properly initialized
  LedConfig m_config; ///< Configuration for the Led
  uint16_t m_colorLightGradient[m_colorLightGradientSize]; ///< Color gradient for the light effect
  
  // Helper methods
  void cleanupMemory();
  bool validateConfig(const LedConfig& config);
  void updateGradient();
  uint16_t getOffColor();
  uint16_t getBackgroundColor();

public:
  Led(uint16_t _x, uint16_t _y, uint8_t _screen);
  virtual ~Led();

  // Required overrides
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void redraw() override;
  void forceUpdate() override;
  void show() override;
  void hide() override;
  
  // Configuration
  void setup(const LedConfig& config);
  
  // Widget-specific methods
  void drawBackground();
  void setState(bool newValue);
  bool getState() const;
};
#endif