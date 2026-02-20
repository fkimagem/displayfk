// charstring.h
#ifndef CHARSTRING_H
#define CHARSTRING_H

#include <stdint.h>
#include <stddef.h>

#ifndef MAX_LENGTH_CSTR
// Ajuste aqui se você quiser outro tamanho padrão
#define MAX_LENGTH_CSTR 64
#endif


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
    POINT = 15, ///< Point key pressed (e.g., .).
    ESC = 16 ///< Escape key pressed (e.g., ESC).

};
typedef struct{
    const char *label; ///< Key label
    PressedKeyType type; ///< Key type
}Key_t;

class CharString {
public:
    CharString();
    explicit CharString(const char *initialStr);

    void clear();

    bool addChar(char c);
    bool removeLastChar();

    int  countChars() const;

    bool setString(const char* newStr, bool autocut = true);
    bool setStringInt(int value);
    bool setStringDouble(double value, uint8_t decimalPlaces);

    const char* getString() const;

    CharString substring(int startIndex, int count) const;

    // ✅ Somente versão segura: caller fornece buffer
    const char* substring_cstr(int startIndex, int count, char* out, size_t outSize) const;

    const char* getLastChars(int qtdLetras) const;

    // ✅ Somente versão segura: caller fornece buffer
    const char* getFirstChars(int qtdLetras, char* out, size_t outSize) const;

    int   toInt() const;
    float toDouble() const;

    bool containsChar(char c) const;

    // Append
    bool append(const char* s, bool autocut = true);
    bool appendChar(char c); // alias seguro de addChar (se quiser separar)

    // Comparação
    bool equals(const char* s) const;
    bool equals(const CharString& other) const;

    // Prefixo/Sufixo
    bool startsWith(const char* prefix) const;
    bool endsWith(const char* suffix) const;

    // Busca
    int  indexOf(char c, int startIndex = 0) const;
    int  indexOf(const char* needle, int startIndex = 0) const;

    // Trim (remove espaços nas pontas)
    void trim();

    // Utilidades
    bool isEmpty() const;
    void toUpper();
    void toLower();


private:
    char m_str[MAX_LENGTH_CSTR + 1];
    int  m_length;
};

#endif // CHARSTRING_H
