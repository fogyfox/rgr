#include "gronsfeld.h"
#include "utils.h"
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

using namespace std;

//парсирование ключа
vector<int> parseKey(const string& keyStr) {
    vector<int> result;
    for (char c : keyStr) {
        if (c >= '0' && c <= '9') {
            result.push_back(c - '0');
        }
    }
    return result;
}

//шифрование текста
string encryptGronsfeld(const string& plaintext, const string& keyStr, bool useCyrillic) {
    if (plaintext.empty()) return plaintext;
    
    vector<int> key = parseKey(keyStr);
    if (key.empty()) {
        throw invalid_argument("Ключ должен содержать хотя бы одну цифру");
    }
    
    string ciphertext;
    size_t keyIndex = 0;
    
    for (size_t i = 0; i < plaintext.length(); ) {
        char currentChar = plaintext[i];
        int shift = key[keyIndex % key.size()];
        
        //обработка кириллицы в UTF-8
        if (useCyrillic && isCyrillicUTF8(plaintext, i)) {
            string cyrChar = plaintext.substr(i, 2);
            
            //определение регистра
            bool isUpper;
            unsigned char first = static_cast<unsigned char>(cyrChar[0]);
            unsigned char second = static_cast<unsigned char>(cyrChar[1]);

            if (first == 0xD0) {
                isUpper = (second >= 0x90 && second <= 0xAF);
            } else {
                isUpper = false;
            }

            //корректировка для Ё
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
                int newPos = (currentPos + shift) % alphabetSize;
                string encryptedChar = alphabet[newPos];
                ciphertext += encryptedChar;
                keyIndex++;
            } else {
                ciphertext += cyrChar;
            }
            
            i += 2;
        }
        //обработка ВСЕХ остальных символов ASCII
        else {
            unsigned char encryptedChar = (static_cast<unsigned char>(currentChar) + shift) % 256;
            ciphertext += static_cast<char>(encryptedChar);
            keyIndex++;
            i++;
        }
    }
    
    return ciphertext;
}

//дешифрование текста
string decryptGronsfeld(const string& ciphertext, const string& keyStr, bool useCyrillic) {
    if (ciphertext.empty()) return ciphertext;
    
    vector<int> key = parseKey(keyStr);
    if (key.empty()) {
        throw invalid_argument("Ключ должен содержать хотя бы одну цифру");
    }
    
    string plaintext;
    size_t keyIndex = 0;
    
    for (size_t i = 0; i < ciphertext.length(); ) {
        char currentChar = ciphertext[i];
        int shift = key[keyIndex % key.size()];
        
        //обработка кириллицы в UTF-8
        if (useCyrillic && isCyrillicUTF8(ciphertext, i)) {
            string cyrChar = ciphertext.substr(i, 2);
            
            //определение регистра
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
                int newPos = (currentPos - shift + alphabetSize) % alphabetSize;
                plaintext += alphabet[newPos];
                keyIndex++;
            } else {
                plaintext += cyrChar;
            }
            
            i += 2;
        }
        //обработка остальных символов ASCII
        else {
            unsigned char decryptedChar = (static_cast<unsigned char>(currentChar) - shift + 256) % 256;
            plaintext += static_cast<char>(decryptedChar);
            keyIndex++;
            i++;
        }
    }
    
    return plaintext;
}