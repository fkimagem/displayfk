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
  static const char* TAG;
  bool m_status; ///< Current on/off status of the button.
  bool m_enabled = true; ///< Indicates whether the button is enabled or disabled.
  RectButtonConfig m_config;
  
  void cleanupMemory();
  void start();

public:
  RectButton(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~RectButton();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void changeState();
  void redraw() override;
  void setup(const RectButtonConfig& config);
  bool getStatus();
  void forceUpdate() override;
  void setStatus(bool _status);
  bool getEnabled();
  void setEnabled(bool newState);
  void show() override;
  void hide() override;
};

#endif