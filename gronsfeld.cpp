#include "gronsfeld.h"
#include "utils.h"
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

using namespace std;

//парсит строковый ключ, извлекая только цифры
//возвращает вектор чисел от 0 до 9

vector<int> parseKey(const string& keyStr) {
    vector<int> result;
    for (char c : keyStr) {
        if (c >= '0' && c <= '9') {
            result.push_back(c - '0');
        }
    }
    return result;
}


//расширяет ключ до нужной длины путем повторения

void extendKey(vector<int>& key, size_t length) {
    if (key.empty()) return;
    
    size_t keyLength = key.size();
    for (size_t i = keyLength; i < length; i++) {
        key.push_back(key[i % keyLength]);
    }
}

//шифрование текста
//алгоритм: каждый символ сдвигается на величину, определяемую соответствующей цифрой ключа
//для кириллицы: 33-буквенный алфавит
//для латиницы: 26-буквенный алфавит
//остальные символы не шифруются
 
string encryptGronsfeld(const string& plaintext, const string& keyStr, bool useCyrillic) {
    if (plaintext.empty()) return plaintext;
    
    vector<int> key = parseKey(keyStr);
    if (key.empty()) {
        throw invalid_argument("Ключ должен содержать хотя бы одну цифру");
    }
    
    extendKey(key, plaintext.length());
    
    string ciphertext;
    ciphertext.reserve(plaintext.length());
    
    for (size_t i = 0; i < plaintext.length(); i++) {
        char currentChar = plaintext[i];
        int shift = key[i];
        
        //шифрование кириллицы
        if (isCyrillic(currentChar)) {
            char base;
            int alphabetSize = 33;
            
            //определяем базовый символ и алфавит
            if (currentChar >= RUS_A_UPPER && currentChar <= RUS_YA_UPPER) {
                base = RUS_A_UPPER;
            } else if (currentChar >= RUS_A_LOWER && currentChar <= RUS_YA_LOWER) {
                base = RUS_A_LOWER;
            } else if (currentChar == RUS_YO_UPPER) {
                base = RUS_YO_UPPER;
            } else if (currentChar == RUS_YO_LOWER) {
                base = RUS_YO_LOWER;
            } else {
                ciphertext += currentChar;
                continue;
            }
            
            //создаем кириллический алфавит с правильным порядком (Ё после Е)
            vector<char> cyrAlphabet;
            if (base == RUS_A_UPPER || base == RUS_YO_UPPER) {
                //верхний регистр: А-Е, Ё, Ж-Я
                for (char c = RUS_A_UPPER; c <= RUS_E_UPPER; c++) cyrAlphabet.push_back(c);
                cyrAlphabet.push_back(RUS_YO_UPPER);
                for (char c = RUS_ZH_UPPER; c <= RUS_YA_UPPER; c++) cyrAlphabet.push_back(c);
            } else {
                //нижний регистр: а-е, ё, ж-я
                for (char c = RUS_A_LOWER; c <= RUS_E_LOWER; c++) cyrAlphabet.push_back(c);
                cyrAlphabet.push_back(RUS_YO_LOWER);
                for (char c = RUS_ZH_LOWER; c <= RUS_YA_LOWER; c++) cyrAlphabet.push_back(c);
            }
            
            //находим позицию символа в алфавите
            int currentPos = -1;
            for (size_t j = 0; j < cyrAlphabet.size(); j++) {
                if (cyrAlphabet[j] == currentChar) {
                    currentPos = j;
                    break;
                }
            }
            
            //сдвигаем позицию и берем по модулю
            if (currentPos != -1) {
                int newPos = (currentPos + shift) % alphabetSize;
                ciphertext += cyrAlphabet[newPos];
            } else {
                ciphertext += currentChar;
            }
        }
        //шифрование латиницы
        else if (isLatin(currentChar)) {
            char base;
            int alphabetSize = 26;
            
            if (currentChar >= 'A' && currentChar <= 'Z') {
                base = 'A';
            } else if (currentChar >= 'a' && currentChar <= 'z') {
                base = 'a';
            } else {
                ciphertext += currentChar;
                continue;
            }
            
            //сдвиг символа в пределах алфавита
            char encryptedChar = base + (currentChar - base + shift) % alphabetSize;
            ciphertext += encryptedChar;
        }
        //неизменяемы символы
        else {
            ciphertext += currentChar;
        }
    }
    
    return ciphertext;
}

//дешифрование текста
//алгоритм: обратный сдвиг на величину ключа
string decryptGronsfeld(const string& ciphertext, const string& keyStr, bool useCyrillic) {
    if (ciphertext.empty()) return ciphertext;
    
    vector<int> key = parseKey(keyStr);
    if (key.empty()) {
        throw invalid_argument("Ключ должен содержать хотя бы одну цифру");
    }
    
    extendKey(key, ciphertext.length());
    
    string plaintext;
    plaintext.reserve(ciphertext.length());
    
    for (size_t i = 0; i < ciphertext.length(); i++) {
        char currentChar = ciphertext[i];
        int shift = key[i];
        
        //дешифрование кириллицы
        if (isCyrillic(currentChar)) {
            char base;
            int alphabetSize = 33;
            
            if (currentChar >= RUS_A_UPPER && currentChar <= RUS_YA_UPPER) {
                base = RUS_A_UPPER;
            } else if (currentChar >= RUS_A_LOWER && currentChar <= RUS_YA_LOWER) {
                base = RUS_A_LOWER;
            } else if (currentChar == RUS_YO_UPPER) {
                base = RUS_YO_UPPER;
            } else if (currentChar == RUS_YO_LOWER) {
                base = RUS_YO_LOWER;
            } else {
                plaintext += currentChar;
                continue;
            }
            
            //создаем кириллический алфавит
            vector<char> cyrAlphabet;
            if (base == RUS_A_UPPER || base == RUS_YO_UPPER) {
                for (char c = RUS_A_UPPER; c <= RUS_E_UPPER; c++) cyrAlphabet.push_back(c);
                cyrAlphabet.push_back(RUS_YO_UPPER);
                for (char c = RUS_ZH_UPPER; c <= RUS_YA_UPPER; c++) cyrAlphabet.push_back(c);
            } else {
                for (char c = RUS_A_LOWER; c <= RUS_E_LOWER; c++) cyrAlphabet.push_back(c);
                cyrAlphabet.push_back(RUS_YO_LOWER);
                for (char c = RUS_ZH_LOWER; c <= RUS_YA_LOWER; c++) cyrAlphabet.push_back(c);
            }
            
            //находим позицию и выполняем обратный сдвиг
            int currentPos = -1;
            for (size_t j = 0; j < cyrAlphabet.size(); j++) {
                if (cyrAlphabet[j] == currentChar) {
                    currentPos = j;
                    break;
                }
            }
            
            if (currentPos != -1) {
                int newPos = (currentPos - shift + alphabetSize) % alphabetSize;
                plaintext += cyrAlphabet[newPos];
            } else {
                plaintext += currentChar;
            }
        }
        //дешифрование латиницы
        else if (isLatin(currentChar)) {
            char base;
            int alphabetSize = 26;
            
            if (currentChar >= 'A' && currentChar <= 'Z') {
                base = 'A';
            } else if (currentChar >= 'a' && currentChar <= 'z') {
                base = 'a';
            } else {
                plaintext += currentChar;
                continue;
            }
            
            //обратный сдвиг для латиницы
            char decryptedChar = base + (currentChar - base - shift + alphabetSize) % alphabetSize;
            plaintext += decryptedChar;
        }
        //неизменяемые символы
        else {
            plaintext += currentChar;
        }
    }
    
    return plaintext;
}