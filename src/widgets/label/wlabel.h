#ifndef WLABEL
#define WLABEL

#include "../widgetbase.h"

/// @brief Configuration structure for Label
struct LabelConfig {
  const char* text;          ///< Initial text to display
  #if defined(USING_GRAPHIC_LIB)
  const GFXfont* fontFamily; ///< Pointer to the font used for the text
  #endif
  uint16_t datum;            ///< Text alignment setting
  uint16_t fontColor;            ///< Color of the text
  uint16_t backgroundColor;          ///< Background color of the label
  const char* prefix;        ///< Prefix text to display
  const char* suffix;        ///< Suffix text to display
};

/// @brief Represents a text label widget with customizable font and colors.
class Label : public WidgetBase
{
private:
  static const char* TAG;
  char *m_text = nullptr; ///< Pointer to the current text displayed by the label.
  char* m_previousText = nullptr; ///< Pointer to the previously displayed text for comparison.
  char* m_prefix = nullptr; ///< Pointer to the prefix text displayed by the label.
  char* m_suffix = nullptr; ///< Pointer to the suffix text displayed by the label.
  
  bool m_shouldRedraw = false; ///< Flag indicating if the label should be redrawn.
  TextBound_t m_lastArea = {0, 0, 0, 0}; ///< Last calculated area for the label.
  uint8_t m_fontSize;///< Font size
  uint8_t m_decimalPlaces;///< Number of decimal places to display
  LabelConfig m_config; ///< Configuration structure for the label

  void cleanupMemory(); ///< Cleans up allocated memory before new assignment
public:
  Label(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~Label();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void setText(const char* str);
  void setText(const String& str);
  void setPrefix(const char* str);
  void setSuffix(const char* str);
  void setTextFloat(float value, uint8_t decimalPlaces = 2);
  void setTextInt(int value);
  void redraw() override;
  void forceUpdate() override;
  void setDecimalPlaces(uint8_t places);
  void setFontSize(uint8_t newSize);
  void setup(const LabelConfig& config);
  void show() override;
  void hide() override;
  
  //void setup(const int _value, const GFXfont *_fontFamily, uint16_t _datum, uint16_t _color, uint16_t _bkColor);
};

#endif