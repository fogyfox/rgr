#include "gronsfeld.h"
#include "utils.h"
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

using namespace std;

vector<int> parseKey(const string& keyStr) {
    vector<int> result;
    for (char c : keyStr) {
        if (c >= '0' && c <= '9') {
            result.push_back(c - '0');
        }
    }
    return result;
}

string encryptGronsfeld(const string& plaintext, const string& keyStr, bool useCyrillic) {
    if (plaintext.empty()) return plaintext;
    
    vector<int> key = parseKey(keyStr);
    if (key.empty()) {
        throw invalid_argument("Ключ должен содержать хотя бы одну цифру");
    }
    
    string ciphertext;
    size_t textIndex = 0;
    size_t keyIndex = 0;
    
    while (textIndex < plaintext.length()) {
        char currentChar = plaintext[textIndex];
        
        // Обработка кириллицы в UTF-8
        if (useCyrillic && isCyrillicUTF8(plaintext, textIndex)) {
            string cyrChar = plaintext.substr(textIndex, 2);
            
            // Определение регистра
            bool isUpper;
            unsigned char first = static_cast<unsigned char>(cyrChar[0]);
            unsigned char second = static_cast<unsigned char>(cyrChar[1]);

            if (first == 0xD0) {
                // D0 90-D0 AF - заглавные, D0 B0-D0 BF - строчные
                isUpper = (second >= 0x90 && second <= 0xAF);
            } else {
                // D1 80-D1 8F - строчные, D1 90-D1 9F - строчные (кроме Ё)
                isUpper = false;
            }

            // Корректировка для Ё
            if (cyrChar == "\xD0\x81") isUpper = true;
            if (cyrChar == "\xD1\x91") isUpper = false;
            
            vector<string> alphabet = getCyrillicAlphabet(isUpper);
            int alphabetSize = 33;
            
            int currentPos = -1;
            for (size_t j = 0; j < alphabet.size(); j++) {
                if (alphabet[j] == cyrChar) {
                    currentPos = j;
                    break;
                }
            }
            
            if (currentPos != -1) {
                int shift = key[keyIndex % key.size()];
                int newPos = (currentPos + shift) % alphabetSize;
                string encryptedChar = alphabet[newPos];
                ciphertext += encryptedChar;
                keyIndex++;  // Увеличиваем keyIndex только для кириллических букв
            } else {
                ciphertext += cyrChar;
            }
            
            textIndex += 2;
        }
        // Обработка латиницы
        else if (isLatin(currentChar)) {
            int shift = key[keyIndex % key.size()];
            char base;
            int alphabetSize = 26;
            
            if (currentChar >= 'A' && currentChar <= 'Z') {
                base = 'A';
            } else {
                base = 'a';
            }
            
            char encryptedChar = base + (currentChar - base + shift) % alphabetSize;
            ciphertext += encryptedChar;
            keyIndex++;  // Увеличиваем keyIndex только для латинских букв
            textIndex++;
        }
        // Обработка не-буквенных символов
        else {
            ciphertext += currentChar;
            textIndex++;
            // Не увеличиваем keyIndex для не-буквенных символов
        }
    }
    
    return ciphertext;
}

string decryptGronsfeld(const string& ciphertext, const string& keyStr, bool useCyrillic) {
    if (ciphertext.empty()) return ciphertext;
    
    vector<int> key = parseKey(keyStr);
    if (key.empty()) {
        throw invalid_argument("Ключ должен содержать хотя бы одну цифру");
    }
    
    string plaintext;
    size_t textIndex = 0;
    size_t keyIndex = 0;
    
    while (textIndex < ciphertext.length()) {
        char currentChar = ciphertext[textIndex];
        
        // Обработка кириллицы в UTF-8
        if (useCyrillic && isCyrillicUTF8(ciphertext, textIndex)) {
            string cyrChar = ciphertext.substr(textIndex, 2);
            
            // Определение регистра (аналогично шифрованию)
            bool isUpper;
            unsigned char first = static_cast<unsigned char>(cyrChar[0]);
            unsigned char second = static_cast<unsigned char>(cyrChar[1]);

            if (first == 0xD0) {
                isUpper = (second >= 0x90 && second <= 0xAF);
            } else {
                isUpper = false;
            }

            if (cyrChar == "\xD0\x81") isUpper = true;
            if (cyrChar == "\xD1\x91") isUpper = false;
            
            vector<string> alphabet = getCyrillicAlphabet(isUpper);
            int alphabetSize = 33;
            
            int currentPos = -1;
            for (size_t j = 0; j < alphabet.size(); j++) {
                if (alphabet[j] == cyrChar) {
                    currentPos = j;
                    break;
                }
            }
            
            if (currentPos != -1) {
                int shift = key[keyIndex % key.size()];
                int newPos = (currentPos - shift + alphabetSize) % alphabetSize;
                plaintext += alphabet[newPos];
                keyIndex++;  // Увеличиваем keyIndex только для кириллических букв
            } else {
                plaintext += cyrChar;
            }
            
            textIndex += 2;
        }
        // Обработка латиницы
        else if (isLatin(currentChar)) {
            int shift = key[keyIndex % key.size()];
            char base;
            int alphabetSize = 26;
            
            if (currentChar >= 'A' && currentChar <= 'Z') {
                base = 'A';
            } else {
                base = 'a';
            }
            
            char decryptedChar = base + (currentChar - base - shift + alphabetSize) % alphabetSize;
            plaintext += decryptedChar;
            keyIndex++;  // Увеличиваем keyIndex только для латинских букв
            textIndex++;
        }
        // Обработка не-буквенных символов
        else {
            plaintext += currentChar;
            textIndex++;
            // Не увеличиваем keyIndex для не-буквенных символов
        }
    }
    
    return plaintext;
}