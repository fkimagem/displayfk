#ifndef WRADIOGROUP
#define WRADIOGROUP

#include "../widgetbase.h"

/// @brief Represents a single radio button with position, ID, and color.
typedef struct
{
  uint16_t x; ///< X-coordinate of the radio button.
  uint16_t y; ///< Y-coordinate of the radio button.
  uint8_t id; ///< Identifier for the radio button within the group.
  uint16_t color; ///< Color of the radio button.
} radio_t;

/// @brief Configuration structure for RadioGroup
struct RadioGroupConfig {
  uint8_t group;               ///< Group identifier for the radio buttons
  uint16_t radius;             ///< Radius of each radio button
  uint8_t amount;              ///< Number of radio buttons in the group
  const radio_t* buttons;      ///< Pointer to an array defining the radio buttons
  uint8_t defaultClickedId;    ///< ID of the radio button to be selected by default
  functionCB_t callback;       ///< Callback function to execute on selection change
};

/// @brief Represents a group of radio buttons, allowing selection of one option within the group.
class RadioGroup : public WidgetBase
{
private:
  static const char* TAG;
  radio_t *m_buttons = nullptr; ///< Pointer to an array of radio button definitions.
  uint8_t m_clickedId; ///< ID of the currently selected radio button.
  bool m_shouldRedraw = false; ///< Flag to indicate if the radio buttons should be redrawn.
  RadioGroupConfig m_config;

  void cleanupMemory();

public:
  RadioGroup(uint8_t _screen);
  ~RadioGroup();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void redraw() override;
  void setSelected(uint16_t clickedId);
  void setup(const RadioGroupConfig& config);
  uint16_t getSelected();
  uint16_t getGroupId();
  void forceUpdate() override;
  void show() override;
  void hide() override;
};

#endif