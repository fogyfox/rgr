#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

using namespace std;

// Константы для кириллических символов в UTF-8
const string RUS_A_UPPER = "\xD0\x90";    // А
const string RUS_YA_UPPER = "\xD0\xAF";   // Я
const string RUS_A_LOWER = "\xD0\xB0";    // а
const string RUS_YA_LOWER = "\xD1\x8F";   // я
const string RUS_YO_UPPER = "\xD0\x81";   // Ё
const string RUS_YO_LOWER = "\xD1\x91";   // ё
const string RUS_E_UPPER = "\xD0\x95";    // Е
const string RUS_ZH_UPPER = "\xD0\x96";   // Ж
const string RUS_E_LOWER = "\xD0\xB5";    // е
const string RUS_ZH_LOWER = "\xD0\xB6";   // ж

#ifdef __cplusplus
extern "C" {
#endif

// Функции для работы с пробелами и подчеркиваниями
string replaceSpacesWithUnderscores(const string& text);
string restoreUnderscoresToSpaces(const string& text);

// Функции для работы с символами
bool isLatin(char c);
bool isCyrillicUTF8(const string& str, size_t pos);
bool isCyrillicChar(const string& str, size_t pos);
string getCyrillicBase(const string& str, size_t pos);
vector<string> getCyrillicAlphabet(bool isUpper);
size_t countEffectiveChars(const string& text);
string getCharAt(const string& text, size_t& index);

#ifdef __cplusplus
}
#endif

#endif