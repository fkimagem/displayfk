//#ifndef WCIRCLEBUTTON
//#define WCIRCLEBUTTON
#pragma once
#include "../widgetbase.h"

/// @brief Configuration structure for CircleButton
struct CircleButtonConfig {
  uint16_t radius;         ///< Radius of the circular button
  uint16_t pressedColor;   ///< Color displayed when the button is pressed
  functionCB_t callback;   ///< Callback function to execute when the button is pressed
};

/// @brief Represents a circular button widget.
class CircleButton : public WidgetBase
{
private:
  unsigned long m_myTime; ///< Timestamp for handling timing-related functions.
  uint16_t m_radius;       ///< Radius of the circular button.
  uint16_t m_pressedColor; ///< Color displayed when the button is pressed.
  bool m_status;           ///< Current status of the button (pressed or not).
  bool m_shouldRedraw = false;     ///< Flag to indicate if the button should be redrawn.
  bool m_enabled = true;   ///< Indicates whether the button is enabled or disabled.
  
  void start();
  void changeState();
  void setup(uint16_t _radius, uint16_t _pressedColor, functionCB_t _cb);

public:
  CircleButton(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~CircleButton();
  void forceUpdate();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void redraw();
  void setup(const CircleButtonConfig& config);
  bool getStatus();
  void setStatus(bool _status);
  bool getEnabled();
  void setEnabled(bool newState);
  void show() override;
  void hide() override;
};

//#endif