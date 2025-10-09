#ifndef WRECTBUTTON
#define WRECTBUTTON

#include "../widgetbase.h"

/// @brief Configuration structure for RectButton
struct RectButtonConfig {
  uint16_t width;         ///< Width of the button
  uint16_t height;        ///< Height of the button
  uint16_t pressedColor;  ///< Color displayed when the button is pressed
  functionCB_t callback;  ///< Callback function to execute when the button is interacted with
};

/// @brief Represents a rectangular button widget with customizable size and color.
class RectButton : public WidgetBase
{
private:
  unsigned long _myTime; ///< Timestamp for handling timing-related functions.
  uint16_t width; ///< Width of the rectangular button.
  uint16_t height; ///< Height of the rectangular button.
  uint16_t pressedColor; ///< Color displayed when the button is pressed.
  bool status; ///< Current on/off status of the button.
  bool m_shouldRedraw = false; ///< Flag to indicate if the button should be redrawn.
  bool m_enabled = true; ///< Indicates whether the button is enabled or disabled.

  void start();
  void setup(uint16_t _width, uint16_t _height, uint16_t _pressedColor, functionCB_t _cb);

public:
  RectButton(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~RectButton();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void changeState();
  void redraw();
  void setup(const RectButtonConfig& config);
  bool getStatus();
  void forceUpdate();
  void setStatus(bool _status);
  bool getEnabled();
  void setEnabled(bool newState);
  void show() override;
  void hide() override;
};

#endif