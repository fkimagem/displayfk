#ifndef CHARSTRING_H
#define CHARSTRING_H

/// @brief Maximum length for the string buffer, including null terminator
#define MAX_LENGTH_CSTR 50

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

    int toInt() const;

    float toFloat() const;

    bool containsChar(char c) const;

private:
    char m_str[MAX_LENGTH_CSTR + 1]; // Internal buffer for the string content, with space for a null terminator.
    int m_length; // Current length of the string, excluding the null terminator.
};
#endif // CHARSTRING_H