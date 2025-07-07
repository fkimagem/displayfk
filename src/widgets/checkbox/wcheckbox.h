#ifndef WCHECKBOX
#define WCHECKBOX

#include "../widgetbase.h"

/// @brief Configuration structure for CheckBox
struct CheckBoxConfig {
  uint16_t size;          ///< Size of the CheckBox (width and height will be equal)
  uint16_t checkedColor;  ///< Color displayed when the checkbox is checked
  functionCB_t callback;  ///< Callback function to execute when the CheckBox state changes
};

/// @brief Represents a checkbox widget, allowing toggling between checked and unchecked states.
class CheckBox : public WidgetBase
{
private:
  unsigned long m_myTime; ///< Timestamp for handling timing-related functions.
  uint16_t m_width; ///< Width of the CheckBox.
  uint16_t m_height; ///< Height of the CheckBox.
  uint16_t m_checkedColor; ///< Color displayed when the checkbox is checked.
  bool m_status; ///< Current checked/unchecked status of the CheckBox.
  bool m_shouldRedraw; ///< Flag to indicate if the checkbox should be redrawn.
  bool m_enabled = true; ///< Indicates whether the checkbox is enabled or disabled.

  void changeState();
  void setup(uint16_t _size, uint16_t _checkedColor, functionCB_t _cb);

public:
  CheckBox(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~CheckBox();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void redraw();
  void forceUpdate();
  void setup(const CheckBoxConfig& config);
  bool getStatus();
  void setStatus(bool status);
  bool getEnabled();
  void setEnabled(bool newState);
};

#endif