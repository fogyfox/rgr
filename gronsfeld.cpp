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

void extendKey(vector<int>& key, size_t length) {
    if (key.empty()) return;
    
    size_t keyLength = key.size();
    for (size_t i = keyLength; i < length; i++) {
        key.push_back(key[i % keyLength]);
    }
}

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
        
        // ПРОВЕРЯЕМ КИРИЛЛИЦУ
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
                ciphertext += currentChar;
                continue;
            }
            
            // ШИФРОВАНИЕ КИРИЛЛИЦЫ
            vector<char> cyrAlphabet;
            if (base == RUS_A_UPPER || base == RUS_YO_UPPER) {
                // ВЕРХНИЙ РЕГИСТР
                for (char c = RUS_A_UPPER; c <= RUS_E_UPPER; c++) cyrAlphabet.push_back(c);
                cyrAlphabet.push_back(RUS_YO_UPPER);
                for (char c = RUS_ZH_UPPER; c <= RUS_YA_UPPER; c++) cyrAlphabet.push_back(c);
            } else {
                // НИЖНИЙ РЕГИСТР
                for (char c = RUS_A_LOWER; c <= RUS_E_LOWER; c++) cyrAlphabet.push_back(c);
                cyrAlphabet.push_back(RUS_YO_LOWER);
                for (char c = RUS_ZH_LOWER; c <= RUS_YA_LOWER; c++) cyrAlphabet.push_back(c);
            }
            
            int currentPos = -1;
            for (size_t j = 0; j < cyrAlphabet.size(); j++) {
                if (cyrAlphabet[j] == currentChar) {
                    currentPos = j;
                    break;
                }
            }
            
            if (currentPos != -1) {
                int newPos = (currentPos + shift) % alphabetSize;
                ciphertext += cyrAlphabet[newPos];
            } else {
                ciphertext += currentChar;
            }
        }
        // ПРОВЕРЯЕМ ЛАТИНИЦУ
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
            
            // ШИФРОВАНИЕ ЛАТИНИЦЫ
            char encryptedChar = base + (currentChar - base + shift) % alphabetSize;
            ciphertext += encryptedChar;
        }
        // ВСЕ ОСТАЛЬНЫЕ СИМВОЛЫ
        else {
            ciphertext += currentChar;
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
    
    extendKey(key, ciphertext.length());
    
    string plaintext;
    plaintext.reserve(ciphertext.length());
    
    for (size_t i = 0; i < ciphertext.length(); i++) {
        char currentChar = ciphertext[i];
        int shift = key[i];
        
        // ПРОВЕРЯЕМ КИРИЛЛИЦУ
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
            
            // ДЕШИФРОВАНИЕ КИРИЛЛИЦЫ
            vector<char> cyrAlphabet;
            if (base == RUS_A_UPPER || base == RUS_YO_UPPER) {
                // ВЕРХНИЙ РЕГИСТР
                for (char c = RUS_A_UPPER; c <= RUS_E_UPPER; c++) cyrAlphabet.push_back(c);
                cyrAlphabet.push_back(RUS_YO_UPPER);
                for (char c = RUS_ZH_UPPER; c <= RUS_YA_UPPER; c++) cyrAlphabet.push_back(c);
            } else {
                // НИЖНИЙ РЕГИСТР
                for (char c = RUS_A_LOWER; c <= RUS_E_LOWER; c++) cyrAlphabet.push_back(c);
                cyrAlphabet.push_back(RUS_YO_LOWER);
                for (char c = RUS_ZH_LOWER; c <= RUS_YA_LOWER; c++) cyrAlphabet.push_back(c);
            }
            
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
        // ПРОВЕРЯЕМ ЛАТИНИЦУ
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
            
            // ДЕШИФРОВАНИЕ ЛАТИНИЦЫ
            char decryptedChar = base + (currentChar - base - shift + alphabetSize) % alphabetSize;
            plaintext += decryptedChar;
        }
        // ВСЕ ОСТАЛЬНЫЕ СИМВОЛЫ
        else {
            plaintext += currentChar;
        }
    }
    
    return plaintext;
}