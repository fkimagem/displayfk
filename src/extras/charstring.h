#ifndef CHARSTRING_H
#define CHARSTRING_H

/// @brief Maximum length for the string buffer, including null terminator
#define MAX_LENGTH_CSTR 50

/// @brief Enum for representing types of pressed keys.
enum class PressedKeyType
{
    NONE = 0,   ///< No key pressed.
    LETTER = 1, ///< Letter key pressed.
    NUMBER = 2, ///< Number key pressed.
    ANOTHER = 3, ///< Other type of key pressed.
    CONTROL = 4, ///< Control key pressed
    SHIFT = 5, ///< Shift key pressed
    CAPS = 6, ///< Caps Lock key pressed
    DEL = 7, ///< Delete key pressed
    CLR = 8, ///< Clear key pressed
    SIGN = 9, ///< Sign key pressed (e.g., +, -, *, /).
    RETURN = 10,  ///< Return/Enter key pressed.
    EMPTY = 11,
    INVERT_VALUE = 12, ///< Invert value key pressed (e.g., +/-).
    INCREMENT = 13, ///< Increment value key pressed (e.g., ++).
    DECREMENT = 14, ///< Decrement value key pressed (e.g., --).
    POINT = 15 ///< Point key pressed (e.g., .).

};
typedef struct{
    const char *label; ///< Key label
    PressedKeyType type; ///< Key type
}Key_t;

/// @brief Represents a custom string class with methods for manipulation, retrieval, and conversion.
class CharString {
public:
    CharString();

    CharString(const char* initialStr);

    void clear();

    bool addChar(char c);

    bool removeLastChar();

    int countChars() const;

    bool setString(const char* newStr, bool autocut);

    bool setString(int value);

    bool setString(float value);

    const char* getString() const;

    CharString substring(int startIndex, int count) const;

    const char* substring_cstr(int startIndex, int count) const;

    const char* getLastChars(int qtdLetras) const;
    const char* getFirstChars(int qtdLetras) const;

    int toInt() const;

    float toFloat() const;

    bool containsChar(char c) const;

private:
    char m_str[MAX_LENGTH_CSTR + 1]; // Internal buffer for the string content, with space for a null terminator.
    int m_length; // Current length of the string, excluding the null terminator.
};
#endif // CHARSTRING_H