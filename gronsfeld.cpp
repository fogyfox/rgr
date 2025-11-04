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
    
    string ciphertext;
    size_t textIndex = 0;
    size_t keyIndex = 0;
    
    while (textIndex < plaintext.length()) {
        char currentChar = plaintext[textIndex];
        
        //обработка кириллицы в UTF-8
        if (useCyrillic && isCyrillicUTF8(plaintext, textIndex)) {
            // Получаем полный символ (2 байта)
            string cyrChar = plaintext.substr(textIndex, 2);
            
            //определяем регистр и получаем алфавит
            bool isUpper = (static_cast<unsigned char>(cyrChar[0]) == 0xD0 && 
                           static_cast<unsigned char>(cyrChar[1]) >= 0x90);
            vector<string> alphabet = getCyrillicAlphabet(isUpper);
            int alphabetSize = 33;
            
            //находим позицию символа в алфавите
            int currentPos = -1;
            for (size_t j = 0; j < alphabet.size(); j++) {
                if (alphabet[j] == cyrChar) {
                    currentPos = j;
                    break;
                }
            }
            
            //выполняем сдвиг
            if (currentPos != -1 && keyIndex < key.size()) {
                int shift = key[keyIndex % key.size()];
                int newPos = (currentPos + shift) % alphabetSize;
                ciphertext += alphabet[newPos];
                keyIndex++;
            } else {
                ciphertext += cyrChar;
            }
            
            textIndex += 2; //пропускаем 2 байта UTF-8
        }
        //обработка латиницы
        else if (isLatin(currentChar)) {
            if (keyIndex < key.size()) {
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
                keyIndex++;
            } else {
                ciphertext += currentChar;
            }
            textIndex++;
        }
        // Остальные символы
        else {
            ciphertext += currentChar;
            textIndex++;
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
        
        //обработка кириллицы в UTF-8
        if (useCyrillic && isCyrillicUTF8(ciphertext, textIndex)) {
            string cyrChar = ciphertext.substr(textIndex, 2);
            
            bool isUpper = (static_cast<unsigned char>(cyrChar[0]) == 0xD0 && 
                           static_cast<unsigned char>(cyrChar[1]) >= 0x90);
            vector<string> alphabet = getCyrillicAlphabet(isUpper);
            int alphabetSize = 33;
            
            int currentPos = -1;
            for (size_t j = 0; j < alphabet.size(); j++) {
                if (alphabet[j] == cyrChar) {
                    currentPos = j;
                    break;
                }
            }
            
            if (currentPos != -1 && keyIndex < key.size()) {
                int shift = key[keyIndex % key.size()];
                int newPos = (currentPos - shift + alphabetSize) % alphabetSize;
                plaintext += alphabet[newPos];
                keyIndex++;
            } else {
                plaintext += cyrChar;
            }
            
            textIndex += 2;
        }
        //обработка латиницы
        else if (isLatin(currentChar)) {
            if (keyIndex < key.size()) {
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
                keyIndex++;
            } else {
                plaintext += currentChar;
            }
            textIndex++;
        }
        //остальные символы
        else {
            plaintext += currentChar;
            textIndex++;
        }
    }
    
    return plaintext;
}