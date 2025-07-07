#include "charstring.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Default constructor that initializes an empty string.
 * Sets the internal buffer to an empty string and length to 0.
 */
CharString::CharString() {
    clear();
}

/**
 * @brief Constructs a CharString with an initial string.
 * @param initialStr Initial C-string to set as the CharString's content.
 * The string will be automatically truncated if it exceeds MAX_LENGTH_CSTR.
 */
CharString::CharString(const char *initialStr)
{
    clear();
    setString(initialStr, true);
}

/**
 * @brief Clears the content of the CharString.
 * Sets the first character to null terminator and length to 0.
 */
void CharString::clear() {
    m_str[0] = '\0';
    m_length = 0;
}

/**
 * @brief Adds a character to the end of the CharString.
 * @param c Character to add.
 * @return True if the character was added successfully, false if the CharString is at maximum capacity.
 */
bool CharString::addChar(char c) {
    if (m_length < MAX_LENGTH_CSTR) {
        m_str[m_length] = c;
        m_str[m_length + 1] = '\0'; // Null terminate the string
        m_length++;
        return true;
    }
    return false;
}

/**
 * @brief Removes the last character from the CharString.
 * @return True if a character was removed, false if the CharString was empty.
 */
bool CharString::removeLastChar() {
    if (m_length > 0) {
        m_length--;
        m_str[m_length] = '\0'; // Null terminate the string
        return true;
    }
    return false;
}

/**
 * @brief Counts the number of characters in the CharString.
 * @return The number of characters in the CharString.
 */
int CharString::countChars() const {
    return m_length;
}

/**
 * @brief Sets the CharString's content to a new C-string, with an option to truncate.
 * @param newStr The new C-string to set as content.
 * @param autocut If true, truncates the string if it exceeds the maximum length.
 * @return True if the string was set successfully.
 */
bool CharString::setString(const char* newStr, bool autocut) {
    int newLength = strlen(newStr);
    
    if (autocut && newLength > MAX_LENGTH_CSTR) {
        newLength = MAX_LENGTH_CSTR; // Cut the string to MAX_LENGTH
    }
    
    if (newLength > MAX_LENGTH_CSTR) {
        return false; // New string is too long
    }
    
    strncpy(m_str, newStr, newLength);
    m_str[newLength] = '\0'; // Null terminate the string
    m_length = newLength;
    return true;
}

/**
 * @brief Sets the CharString's content to an integer value.
 * @param value Integer value to set as the string.
 * @return True if the string was set successfully.
 */
bool CharString::setString(int value)
{
    char buffer[MAX_LENGTH_CSTR + 1];
    snprintf(buffer, sizeof(buffer), "%d", value);
    return setString(buffer, true);
}

/**
 * @brief Sets the CharString's content to a floating-point value.
 * @param value Floating-point value to set as the string.
 * @return True if the string was set successfully.
 */
bool CharString::setString(float value)
{
    char buffer[MAX_LENGTH_CSTR + 1];
    snprintf(buffer, sizeof(buffer), "%.6f", value);
    return setString(buffer, true);
}

/**
 * @brief Retrieves the CharString's content as a C-string.
 * @return A pointer to the CharString's content.
 */
const char* CharString::getString() const {
    return m_str;
}

/**
 * @brief Returns a substring of the CharString.
 * @param startIndex Starting index for the substring.
 * @param count Number of characters to include in the substring.
 * @return A CharString object containing the specified substring.
 */
CharString CharString::substring(int startIndex, int count) const {
    CharString result;
    if (startIndex < 0 || startIndex >= m_length || count <= 0) {
        return result; // Return empty string on invalid input
    }
    if (count > MAX_LENGTH_CSTR - startIndex) {
        count = MAX_LENGTH_CSTR - startIndex;
    }
    strncpy(result.m_str, &m_str[startIndex], count);
    result.m_str[count] = '\0'; // Null terminate the substring
    result.m_length = count;
    return result;
}

/**
 * @brief Returns a substring of the CharString as a C-string.
 * @param startIndex Starting index for the substring.
 * @param count Number of characters to include in the substring.
 * @return A C-string containing the specified substring.
 */
const char *CharString::substring_cstr(int startIndex, int count) const
{
    static char buffer[MAX_LENGTH_CSTR + 1]; // Static buffer to hold the substring
    if (startIndex < 0 || startIndex >= m_length || count <= 0) {
        buffer[0] = '\0'; // Return empty string on invalid input
        return buffer;
    }
    if (count > MAX_LENGTH_CSTR - startIndex) {
        count = MAX_LENGTH_CSTR - startIndex;
    }
    strncpy(buffer, &m_str[startIndex], count);
    buffer[count] = '\0'; // Null terminate the substring
    return buffer;
}

/**
 * @brief Retrieves the last specified number of characters from the CharString.
 * @param qtdLetras Number of characters to retrieve from the end.
 * @return A pointer to the last characters as a C-string.
 */
const char *CharString::getLastChars(int qtdLetras) const
{
    if (m_length <= qtdLetras) {
        return m_str;
    }
    return &m_str[m_length - qtdLetras];
}

/**
 * @brief Converts the CharString's content to an integer.
 * @return The integer value represented by the string.
 */
int CharString::toInt() const
{
    return atoi(m_str);
}

/**
 * @brief Converts the CharString's content to a floating-point number.
 * @return The floating-point value represented by the string.
 */
float CharString::toFloat() const
{
    return atof(m_str);
}

/**
 * @brief Checks if the CharString contains a specific character.
 * @param c Character to search for.
 * @return True if the character is found, otherwise false.
 */
bool CharString::containsChar(char c) const
{
    for (int i = 0; i < m_length; i++) {
        if (m_str[i] == c) {
            return true;
        }
    }
    return false;
}
