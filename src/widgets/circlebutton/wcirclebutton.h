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
  static const char* TAG; ///< Tag for logging identification
  bool m_status;           ///< Current status of the button (pressed or not).
  CircleButtonConfig m_config; ///< Configuration for the CircleButton.
  
  void start();
  void changeState();

public:
  CircleButton(uint16_t _x, uint16_t _y, uint8_t _screen);
  virtual ~CircleButton();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void redraw() override;
  void forceUpdate() override;
  void show() override;
  void hide() override;
  void setup(const CircleButtonConfig& config);
  bool getStatus() const;
  void setStatus(bool _status);
};

//#endif