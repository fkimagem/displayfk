#ifndef WTOGGLE
#define WTOGGLE

#include "../widgetbase.h"

/// @brief Configuration structure for ToggleButton
struct ToggleButtonConfig {
  uint16_t width;         ///< Width of the button
  uint16_t height;        ///< Height of the button
  uint16_t pressedColor;  ///< Color displayed when the button is toggled on
  functionCB_t callback;  ///< Callback function to execute when the button is toggled
};

/// @brief Represents a toggle button widget with customizable size and color, which switches state on each press.
class ToggleButton : public WidgetBase
{
private:
  unsigned long m_myTime; ///< Timestamp for handling timing-related functions.
  uint16_t m_width; ///< Width of the ToggleButton.
  uint16_t m_height; ///< Height of the ToggleButton.
  uint16_t m_pressedColor; ///< Color displayed when the button is toggled on.
  bool m_status; ///< Current on/off status of the button.
  bool m_shouldRedraw; ///< Flag to indicate if the button should be redrawn.
  bool m_enabled;

  void start();
  void setup(uint16_t _width, uint16_t _height, uint16_t _pressedColor, functionCB_t _cb);
  

public:
  ToggleButton(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~ToggleButton();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void changeState();
  void redraw();
  void setup(const ToggleButtonConfig& config);
  bool getStatus();
  bool getEnabled();
  void setEnabled(bool newState);
  void setStatus(bool status);
  void forceUpdate();
};
#endif