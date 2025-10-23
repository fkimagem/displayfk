#ifndef WCHECKBOX
#define WCHECKBOX

#include "../widgetbase.h"

enum class CheckBoxWeight {
  LIGHT = 1,
  MEDIUM = 2,
  HEAVY = 3
};

/// @brief Configuration structure for CheckBox
struct CheckBoxConfig {
  uint16_t size;          ///< Size of the CheckBox (width and height will be equal)
  uint16_t checkedColor;  ///< Color displayed when the checkbox is checked
  uint16_t uncheckedColor; ///< Color displayed when the checkbox is unchecked (optional, 0 = auto)
  CheckBoxWeight weight; ///< Weight of the checkbox (LIGHT, MEDIUM, HEAVY)
  functionCB_t callback;  ///< Callback function to execute when the CheckBox state changes
};



/// @brief Represents a checkbox widget, allowing toggling between checked and unchecked states.
class CheckBox : public WidgetBase
{
private:
  static const char *TAG;
  bool m_status; ///< Current checked/unchecked status of the CheckBox.
  uint8_t m_borderWidth; ///< Width of the checkbox border.
  uint8_t m_borderRadius; ///< Radius of the checkbox border.
  CheckBoxConfig m_config; ///< Configuration for the CheckBox.

  // Point to create line and draw a check mark. from m_topRightPoint to m_bottomCenterPoint then m_bottomCenterPoint to m_middleLeftPoint
  CoordPoint_t m_topRightPoint;
  CoordPoint_t m_bottomCenterPoint;
  CoordPoint_t m_middleLeftPoint;

  void changeState();
  uint16_t getUncheckedColor();
  uint16_t getBorderColor();
  void drawCheckmark();
  void drawBorder();

public:
  CheckBox(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~CheckBox();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void redraw() override;
  void forceUpdate() override;
  void setup(const CheckBoxConfig& config);
  bool getStatus() const;
  void setStatus(bool status);
  void show() override;
  void hide() override;
  void setSize(uint16_t newSize);
  uint16_t getSize() const;
  void setColors(uint16_t checkedColor, uint16_t uncheckedColor = 0);
};

#endif