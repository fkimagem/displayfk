// charstring.cpp
#include "charstring.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


/**
 * @brief Default constructor. Initializes an empty string.
 */
CharString::CharString() {
    clear();
}

/**
 * @brief Constructor that initializes the string with a C-string.
 * @param initialStr C-string to copy (may be null; treated as empty).
 */
CharString::CharString(const char *initialStr) {
    clear();
    setString(initialStr, true);
}

/**
 * @brief Clears the string, making it empty (length 0).
 */
void CharString::clear() {
    m_str[0] = '\0';
    m_length = 0;
}

/**
 * @brief Appends a single character to the end of the string.
 * @param c Character to append.
 * @return true if the character was added, false if buffer is full.
 */
bool CharString::addChar(char c) {
    if (m_length < MAX_LENGTH_CSTR) {
        m_str[m_length] = c;
        m_str[m_length + 1] = '\0';
        m_length++;
        return true;
    }
    return false;
}

/**
 * @brief Removes the last character from the string.
 * @return true if a character was removed, false if string was already empty.
 */
bool CharString::removeLastChar() {
    if (m_length > 0) {
        m_length--;
        m_str[m_length] = '\0';
        return true;
    }
    return false;
}

/**
 * @brief Returns the current length of the string (number of characters).
 * @return Number of characters in the string.
 */
int CharString::countChars() const {
    return m_length;
}

/**
 * @brief Replaces the string content with a new C-string.
 * @param newStr New string to copy (null clears the string).
 * @param autocut If true, long strings are truncated to MAX_LENGTH_CSTR; if false, returns false when too long.
 * @return true on success, false if newStr is null or (when autocut is false) string exceeds max length.
 */
bool CharString::setString(const char* newStr, bool autocut) {
    if (!newStr) {
        clear();
        return false;
    }

    size_t newLength = strlen(newStr);

    if (autocut && newLength > (size_t)MAX_LENGTH_CSTR) {
        newLength = (size_t)MAX_LENGTH_CSTR;
    }

    if (newLength > (size_t)MAX_LENGTH_CSTR) {
        return false;
    }

    if (newLength > 0) {
        memcpy(m_str, newStr, newLength);
    }
    m_str[newLength] = '\0';
    m_length = (int)newLength;
    return true;
}

/**
 * @brief Sets the string content to the decimal representation of an integer.
 * @param value Integer value to convert.
 * @return true on success (same as setString with autocut).
 */
bool CharString::setStringInt(int value) {
    char buffer[MAX_LENGTH_CSTR + 1];
    snprintf(buffer, sizeof(buffer), "%d", value);
    return setString(buffer, true);
}

/**
 * @brief Sets the string content to the fixed-point representation of a double.
 * @param value Double value to convert.
 * @param decimalPlaces Number of decimal places (capped at 6).
 * @return true on success (same as setString with autocut).
 */
bool CharString::setStringDouble(double value, uint8_t decimalPlaces) {
    if (decimalPlaces > 6) decimalPlaces = 6;

    char buffer[MAX_LENGTH_CSTR + 1];
    snprintf(buffer, sizeof(buffer), "%.*f", (int)decimalPlaces, value);

    return setString(buffer, true);
}

/**
 * @brief Returns a pointer to the internal null-terminated C-string.
 * @return Pointer to the internal buffer (valid until the object is modified or destroyed).
 */
const char* CharString::getString() const {
    return m_str;
}

/**
 * @brief Returns a new CharString containing a substring.
 * @param startIndex Starting index (0-based).
 * @param count Number of characters to copy.
 * @return A CharString with the substring; empty if indices are invalid.
 */
CharString CharString::substring(int startIndex, int count) const {
    CharString result;

    if (startIndex < 0 || startIndex >= m_length || count <= 0) {
        return result;
    }

    if (startIndex + count > m_length) {
        count = m_length - startIndex;
    }

    if (count > MAX_LENGTH_CSTR) {
        count = MAX_LENGTH_CSTR;
    }

    memcpy(result.m_str, &m_str[startIndex], (size_t)count);
    result.m_str[count] = '\0';
    result.m_length = count;
    return result;
}

/**
 * @brief Writes a substring into a caller-provided buffer (safe version).
 * @param startIndex Starting index (0-based).
 * @param count Number of characters to copy.
 * @param out Output buffer.
 * @param outSize Size of the output buffer (including null terminator).
 * @return Pointer to out, or "" if out is null or outSize is 0.
 */
const char* CharString::substring_cstr(int startIndex, int count, char* out, size_t outSize) const {
    if (!out || outSize == 0) return "";

    out[0] = '\0';

    if (startIndex < 0 || startIndex >= m_length || count <= 0) {
        return out;
    }

    if (startIndex + count > m_length) {
        count = m_length - startIndex;
    }

    size_t maxCopy = outSize - 1;
    if ((size_t)count > maxCopy) {
        count = (int)maxCopy;
    }

    memcpy(out, &m_str[startIndex], (size_t)count);
    out[count] = '\0';
    return out;
}

/**
 * @brief Returns a pointer to the last N characters of the string (no copy).
 * @param qtdLetras Number of characters from the end (if larger than length, returns full string).
 * @return Pointer into internal buffer, or "" if qtdLetras <= 0.
 */
const char* CharString::getLastChars(int qtdLetras) const {
    if (qtdLetras <= 0) return "";

    if (m_length <= qtdLetras) {
        return m_str;
    }
    return &m_str[m_length - qtdLetras];
}

/**
 * @brief Copies the first N characters into a caller-provided buffer (safe version).
 * @param qtdLetras Number of characters to copy from the start.
 * @param out Output buffer.
 * @param outSize Size of the output buffer (including null terminator).
 * @return Pointer to out, or "" if out is null or outSize is 0.
 */
const char* CharString::getFirstChars(int qtdLetras, char* out, size_t outSize) const {
    if (!out || outSize == 0) return "";

    out[0] = '\0';

    if (qtdLetras <= 0 || m_length == 0) {
        return out;
    }

    if (qtdLetras > m_length) {
        qtdLetras = m_length;
    }

    size_t maxCopy = outSize - 1;
    if ((size_t)qtdLetras > maxCopy) {
        qtdLetras = (int)maxCopy;
    }

    memcpy(out, m_str, (size_t)qtdLetras);
    out[qtdLetras] = '\0';
    return out;
}

/**
 * @brief Parses the string as a decimal integer.
 * @return The parsed integer value, or 0 if the string is not a valid number.
 */
int CharString::toInt() const {
    char *end = nullptr;
    long val = strtol(m_str, &end, 10);

    if (end == m_str) {
        return 0;
    }
    return (int)val;
}

/**
 * @brief Parses the string as a floating-point number.
 * @return The parsed value as float, or 0.0f if the string is not a valid number.
 */
float CharString::toDouble() const {
    char *end = nullptr;
    double num = strtod(m_str, &end);

    if (end == m_str) {
        return 0.0f;
    }
    return (float)num;
}

/**
 * @brief Checks whether the string contains a given character.
 * @param c Character to search for.
 * @return true if the character is present, false otherwise.
 */
bool CharString::containsChar(char c) const {
    for (int i = 0; i < m_length; i++) {
        if (m_str[i] == c) return true;
    }
    return false;
}

/**
 * @brief Appends a C-string to the end of this string.
 * @param s C-string to append (null returns false).
 * @param autocut If true, only the part that fits is appended; if false, returns false when it would overflow.
 * @return true on success, false if s is null or (when autocut is false) append would exceed max length.
 */
bool CharString::append(const char* s, bool autocut) {
    if (!s) return false;

    size_t addLen = strlen(s);
    size_t spaceLeft = (size_t)MAX_LENGTH_CSTR - (size_t)m_length;

    if (addLen > spaceLeft) {
        if (!autocut) return false;
        addLen = spaceLeft;
    }

    if (addLen == 0) return true;

    memcpy(&m_str[m_length], s, addLen);
    m_length += (int)addLen;
    m_str[m_length] = '\0';
    return true;
}

/**
 * @brief Appends a single character (alias for addChar).
 * @param c Character to append.
 * @return true if the character was added, false if buffer is full.
 */
bool CharString::appendChar(char c) {
    return addChar(c);
}

/**
 * @brief Compares this string with a C-string for equality.
 * @param s C-string to compare (null returns false).
 * @return true if content is equal, false otherwise.
 */
bool CharString::equals(const char* s) const {
    if (!s) return false;
    // m_str é sempre null-terminated
    return (strcmp(m_str, s) == 0);
}

/**
 * @brief Compares this string with another CharString for equality.
 * @param other CharString to compare with.
 * @return true if content is equal, false otherwise.
 */
bool CharString::equals(const CharString& other) const {
    // comparação rápida por tamanho + strcmp
    if (m_length != other.m_length) return false;
    return (strcmp(m_str, other.m_str) == 0);
}

/**
 * @brief Checks whether the string starts with the given prefix.
 * @param prefix C-string prefix (null returns false).
 * @return true if the string starts with prefix, false otherwise.
 */
bool CharString::startsWith(const char* prefix) const {
    if (!prefix) return false;
    size_t plen = strlen(prefix);
    if (plen > (size_t)m_length) return false;
    return (memcmp(m_str, prefix, plen) == 0);
}

/**
 * @brief Checks whether the string ends with the given suffix.
 * @param suffix C-string suffix (null returns false).
 * @return true if the string ends with suffix, false otherwise.
 */
bool CharString::endsWith(const char* suffix) const {
    if (!suffix) return false;
    size_t slen = strlen(suffix);
    if (slen > (size_t)m_length) return false;
    return (memcmp(&m_str[m_length - (int)slen], suffix, slen) == 0);
}

/**
 * @brief Finds the first occurrence of a character starting at a given index.
 * @param c Character to search for.
 * @param startIndex Index to start searching from (0-based).
 * @return Index of the first occurrence, or -1 if not found.
 */
int CharString::indexOf(char c, int startIndex) const {
    if (startIndex < 0) startIndex = 0;
    if (startIndex >= m_length) return -1;

    for (int i = startIndex; i < m_length; i++) {
        if (m_str[i] == c) return i;
    }
    return -1;
}

/**
 * @brief Finds the first occurrence of a substring starting at a given index.
 * @param needle C-string to search for (null returns -1).
 * @param startIndex Index to start searching from (0-based).
 * @return Index of the first character of the match, or -1 if not found.
 */
int CharString::indexOf(const char* needle, int startIndex) const {
    if (!needle) return -1;
    if (startIndex < 0) startIndex = 0;
    if (startIndex >= m_length) return -1;

    size_t nlen = strlen(needle);
    if (nlen == 0) return startIndex; // string vazia "encontra" no início
    if (nlen > (size_t)m_length) return -1;

    // procura simples O(n*m) (ok p/ buffers pequenos)
    for (int i = startIndex; i <= m_length - (int)nlen; i++) {
        if (m_str[i] == needle[0] && memcmp(&m_str[i], needle, nlen) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Removes leading and trailing whitespace from the string (in-place).
 */
void CharString::trim() {
    if (m_length == 0) return;

    int left = 0;
    while (left < m_length && isspace((unsigned char)m_str[left])) {
        left++;
    }

    int right = m_length - 1;
    while (right >= left && isspace((unsigned char)m_str[right])) {
        right--;
    }

    int newLen = (right >= left) ? (right - left + 1) : 0;

    if (newLen == 0) {
        clear();
        return;
    }

    if (left > 0) {
        memmove(m_str, &m_str[left], (size_t)newLen);
    }
    m_str[newLen] = '\0';
    m_length = newLen;
}

/**
 * @brief Checks whether the string is empty (length 0).
 * @return true if empty, false otherwise.
 */
bool CharString::isEmpty() const {
    return (m_length == 0);
}

/**
 * @brief Converts all characters in the string to uppercase (in-place).
 */
void CharString::toUpper() {
    for (int i = 0; i < m_length; i++) {
        m_str[i] = (char)toupper((unsigned char)m_str[i]);
    }
}

/**
 * @brief Converts all characters in the string to lowercase (in-place).
 */
void CharString::toLower() {
    for (int i = 0; i < m_length; i++) {
        m_str[i] = (char)tolower((unsigned char)m_str[i]);
    }
}
