#ifndef WLABEL
#define WLABEL

#include "../widgetbase.h"

/// @brief Configuration structure for Label
struct LabelConfig {
  const char* text;          ///< Initial text to display
  const GFXfont* fontFamily; ///< Pointer to the font used for the text
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
  bool m_update; ///< Flag indicating if the label text should be updated.
  char *m_text = nullptr; ///< Pointer to the current text displayed by the label.
  char* m_previousText = nullptr; ///< Pointer to the previously displayed text for comparison.
  char* m_prefix = nullptr; ///< Pointer to the prefix text displayed by the label.
  char* m_suffix = nullptr; ///< Pointer to the suffix text displayed by the label.

  uint16_t m_datum; ///< Text alignment setting for the label.
  uint16_t m_letterColor; ///< Color of the text.
  uint16_t m_backgroundColor; ///< Background color of the label.
  GFXfont *m_fontFamily = nullptr; ///< Pointer to the font used by the label.
  TextBound_t m_lastArea = {0, 0, 0, 0}; ///< Last calculated area for the label.
  uint8_t m_decimalPlaces;///< Number of decimal places to display
  uint8_t m_fontSize;///< Font size

  void setup(const char *_text, const GFXfont *_fontFamily, uint16_t _datum, uint16_t _color, uint16_t _bkColor, const char* _prefix, const char* _suffix);
  //void setup(const String &_text, const GFXfont *_fontFamily, uint16_t _datum, uint16_t _color, uint16_t _bkColor, const char* _prefix, const char* _suffix);
  void setup(const float _value, const GFXfont *_fontFamily, uint16_t _datum, uint16_t _color, uint16_t _bkColor, const char* _prefix, const char* _suffix);

public:
  Label(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~Label();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void setText(const char* str);
  void setText(const String& str);
  void setTextFloat(float value, uint8_t decimalPlaces = 2);
  void setTextInt(int value);
  void redraw();
  void forceUpdate();
  void setDecimalPlaces(uint8_t places);
  void setFontSize(uint8_t newSize);
  void setup(const LabelConfig& config);
  
  //void setup(const int _value, const GFXfont *_fontFamily, uint16_t _datum, uint16_t _color, uint16_t _bkColor);
};

#endif