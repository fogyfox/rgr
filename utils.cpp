#include "utils.h"

// Функции для работы с пробелами и подчеркиваниями
std::string replaceSpacesWithUnderscores(const std::string& text) {
    std::string result = text;
    for (char& c : result) if (c == ' ') c = '_';
    return result;
}

std::string restoreUnderscoresToSpaces(const std::string& text) {
    std::string result = text;
    for (char& c : result) if (c == '_') c = ' ';
    return result;
}

// Функции для работы с символами
bool isLatin(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool isCyrillic(char c) {
    return (c >= RUS_A_UPPER && c <= RUS_YA_UPPER) || 
           (c >= RUS_A_LOWER && c <= RUS_YA_LOWER) ||
           (c == RUS_YO_UPPER) || (c == RUS_YO_LOWER);
}

char toUpperCyrillic(char c) {
    if (c >= RUS_A_LOWER && c <= RUS_YA_LOWER) {
        return c - 32;
    } else if (c == RUS_YO_LOWER) {
        return RUS_YO_UPPER;
    }
    return c;
}

char toLowerCyrillic(char c) {
    if (c >= RUS_A_UPPER && c <= RUS_YA_UPPER) {
        return c + 32;
    } else if (c == RUS_YO_UPPER) {
        return RUS_YO_LOWER;
    }
    return c;
}