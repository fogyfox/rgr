#include "utils.h"
#include <string>
#include <cctype>
#include <vector>
using namespace std;

//функции для работы с пробелами и подчеркиваниями
string replaceSpacesWithUnderscores(const string& text) {
    string result = text;
    for (char& c : result) if (c == ' ') c = '_';
    return result;
}

string restoreUnderscoresToSpaces(const string& text) {
    string result = text;
    for (char& c : result) if (c == '_') c = ' ';
    return result;
}

//функции для работы с символами
bool isLatin(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

//проверяет, является ли символ в позиции pos кириллическим в UTF-8
bool isCyrillicUTF8(const string& str, size_t pos) {
    if (pos >= str.length()) return false;
    
    unsigned char c = static_cast<unsigned char>(str[pos]);
    // Кириллица в UTF-8: первый байт 0xD0 или 0xD1
    return (c == 0xD0 || c == 0xD1) && (pos + 1 < str.length());
}

//проверяет конкретный кириллический символ
bool isCyrillicChar(const string& str, size_t pos) {
    if (!isCyrillicUTF8(str, pos)) return false;
    
    unsigned char first = static_cast<unsigned char>(str[pos]);
    unsigned char second = static_cast<unsigned char>(str[pos + 1]);
    
    if (first == 0xD0) {
        return (second >= 0x90 && second <= 0xBF); // А-Я, Ё
    } else if (first == 0xD1) {
        return (second >= 0x80 && second <= 0x8F); // а-п
    } else if (first == 0xD2) {
        return (second == 0x80); // р
    }
    return false;
}

//определяет базовый символ для кириллицы
string getCyrillicBase(const string& str, size_t pos) {
    if (!isCyrillicUTF8(str, pos)) return "";
    
    unsigned char first = static_cast<unsigned char>(str[pos]);
    unsigned char second = static_cast<unsigned char>(str[pos + 1]);
    
    if (first == 0xD0) {
        if (second >= 0x90 && second <= 0x95) return RUS_A_UPPER; // А-Е
        if (second == 0x81) return RUS_YO_UPPER; // Ё
        if (second >= 0x96 && second <= 0xAF) return RUS_A_UPPER; // Ж-Я
    } else if (first == 0xD1) {
        if (second >= 0x80 && second <= 0x85) return RUS_A_LOWER; // а-е
        if (second == 0x91) return RUS_YO_LOWER; // ё
        if (second >= 0x86 && second <= 0x8F) return RUS_A_LOWER; // ж-п
    } else if (first == 0xD2) {
        if (second == 0x80) return RUS_A_LOWER; // р
    }
    
    return "";
}

//функция для получения кириллического алфавита в UTF-8
vector<string> getCyrillicAlphabet(bool isUpper) {
    vector<string> alphabet;
    
    if (isUpper) {
        // А-Е (0xD090-0xD095)
        for (unsigned char second = 0x90; second <= 0x95; second++) {
            string letter;
            letter += static_cast<char>(0xD0);
            letter += static_cast<char>(second);
            alphabet.push_back(letter);
        }
        // Ё (0xD081)
        alphabet.push_back("\xD0\x81");
        // Ж-Я (0xD096-0xD0AF)
        for (unsigned char second = 0x96; second <= 0xAF; second++) {
            string letter;
            letter += static_cast<char>(0xD0);
            letter += static_cast<char>(second);
            alphabet.push_back(letter);
        }
    } else {
        // а-е (0xD0B0-0xD0B5)
        for (unsigned char second = 0xB0; second <= 0xB5; second++) {
            string letter;
            letter += static_cast<char>(0xD0);
            letter += static_cast<char>(second);
            alphabet.push_back(letter);
        }
        // ё (0xD191)
        alphabet.push_back("\xD1\x91");
        // ж-я (0xD0B6-0xD0BF, 0xD180-0xD18F)
        for (unsigned char second = 0xB6; second <= 0xBF; second++) {
            string letter;
            letter += static_cast<char>(0xD0);
            letter += static_cast<char>(second);
            alphabet.push_back(letter);
        }
        for (unsigned char second = 0x80; second <= 0x8F; second++) {
            string letter;
            letter += static_cast<char>(0xD1);
            letter += static_cast<char>(second);
            alphabet.push_back(letter);
        }
    }
    
    return alphabet;
}

//подсчитывает колличество символов в строке
size_t countEffectiveChars(const string& text) {
    size_t count = 0;
    for (size_t i = 0; i < text.length(); ) {
        if (isCyrillicUTF8(text, i)) {
            count++;
            i += 2;
        } else {
            count++;
            i++;
        }
    }
    return count;
}
