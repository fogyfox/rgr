#ifndef UTILS_H
#define UTILS_H

#include <string>

// Константы для кириллических символов
const char RUS_A_UPPER = -64;    // А
const char RUS_YA_UPPER = -33;   // Я
const char RUS_A_LOWER = -32;    // а
const char RUS_YA_LOWER = -1;    // я
const char RUS_YO_UPPER = -15;   // Ё
const char RUS_YO_LOWER = -16;   // ё
const char RUS_E_UPPER = -27;    // Е
const char RUS_ZH_UPPER = -25;   // Ж
const char RUS_E_LOWER = -59;    // е
const char RUS_ZH_LOWER = -57;   // ж

// Функции для работы с пробелами и подчеркиваниями
std::string replaceSpacesWithUnderscores(const std::string& text);
std::string restoreUnderscoresToSpaces(const std::string& text);

// Функции для работы с символами
bool isLatin(char c);
bool isCyrillic(char c);
char toUpperCyrillic(char c);
char toLowerCyrillic(char c);

#endif