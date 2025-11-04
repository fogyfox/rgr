#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <string_view>
#include <vector>
using namespace std;
//константы для кириллических символов в UTF-8
//двубайтовые символы в UTF-8
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

//функции для работы с пробелами и подчеркиваниями
string replaceSpacesWithUnderscores(const string& text);
string restoreUnderscoresToSpaces(const string& text);

//функции для работы с символами в UTF-8
bool isLatin(char c);
bool isCyrillicUTF8(const string& str, size_t pos);
bool isCyrillicChar(const string& str, size_t pos);
string getCyrillicBase(const string& str, size_t pos);
vector<string> getCyrillicAlphabet(bool isUpper);

//вспомогательные функции для табличной перестановки
size_t countEffectiveChars(const string& text);
string getCharAt(const string& text, size_t& index);
#endif