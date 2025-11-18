#include "vigenere.h"
#include "utils.h"
#include <string>
#include <algorithm>
#include <stdexcept>
#include <vector>

using namespace std;

//функция для получения позиции кириллического символа
int getCyrillicPositionUTF8(const string& str, size_t pos) {
    if (!isCyrillicUTF8(str, pos)) return -1;
    
    string cyrChar = str.substr(pos, 2);
    //проверяем оба регистра одновременно
    vector<string> alphabetLower = getCyrillicAlphabet(false);
    vector<string> alphabetUpper = getCyrillicAlphabet(true);
    
    for (size_t i = 0; i < alphabetLower.size(); i++) {
        if (alphabetLower[i] == cyrChar || alphabetUpper[i] == cyrChar) {
            return i;
        }
    }
    
    return -1;
}

//получает кириллический символ по позиции с сохранением регистра оригинала
string getCyrillicFromPositionUTF8(int pos, const string& originalChar) {
    if (pos < 0 || pos >= 33) return "?";
    
    vector<string> alphabetLower = getCyrillicAlphabet(false);
    vector<string> alphabetUpper = getCyrillicAlphabet(true);
    
    //определяем регистр оригинального символа
    bool isLower = false;
    for (size_t i = 0; i < alphabetLower.size(); i++) {
        if (alphabetLower[i] == originalChar) {
            isLower = true;
            break;
        }
    }
    
    //для заглавных используем upper, для строчных - lower
    if (isLower) {
        return alphabetLower[pos];
    } else {
        return alphabetUpper[pos];
    }
}

//функция для получения числового значения символа ключа (0-255)
int getKeyValue(const string& key, size_t& keyPos) {
    if (key.empty()) return 0;
    
    if (keyPos >= key.length()) {
        keyPos = 0;
    }
    
    int value;
    if (isCyrillicUTF8(key, keyPos)) {
        //для кириллицы используем позицию в алфавите (0-32)
        value = getCyrillicPositionUTF8(key, keyPos);
        if (value == -1) value = 0;
        keyPos += 2;
    } else {
        //для латинских символов используем позицию в алфавите (0-25)
        unsigned char c = key[keyPos];
        if (c >= 'A' && c <= 'Z') {
            value = c - 'A';
        } else if (c >= 'a' && c <= 'z') {
            value = c - 'a';
        } else {
            //для остальных символов используем ASCII код по модулю 26
            value = c % 26;
        }
        keyPos++;
    }
    
    return value;
}

//подготавливает ключ
string prepareKey(const string& key) {
    return key;
}

//создает расширенный ключ той же длины, что и текст
string expandKey(const string& text, const string& key) {
    if (key.empty()) return "";
    
    string expandedKey;
    size_t keyPos = 0;
    
    for (size_t i = 0; i < text.length(); ) {
        if (keyPos >= key.length()) {
            keyPos = 0;
        }
        
        if (isCyrillicUTF8(key, keyPos)) {
            expandedKey += key[keyPos];
            expandedKey += key[keyPos + 1];
            keyPos += 2;
        } else {
            expandedKey += key[keyPos];
            keyPos++;
        }
        
        //пропускаем другое символы в тексте, но используем ключ для них
        if (isCyrillicUTF8(text, i)) {
            i += 2;
        } else {
            i++;
        }
    }
    
    return expandedKey;
}

//функция шифрования
string encryptVigenere(const string& plaintext, const string& key, bool useCyrillic) {
    if (plaintext.empty()) return plaintext;
    
    string preparedKey = prepareKey(key);
    if (preparedKey.empty()) {
        throw invalid_argument("Ключ не должен быть пустым");
    }
    
    string ciphertext;
    size_t keyPos = 0;
    
    for (size_t i = 0; i < plaintext.length(); ) {
        int shift = getKeyValue(preparedKey, keyPos);
        
        //шифрование кириллицы
        if (useCyrillic && isCyrillicUTF8(plaintext, i)) {
            string cyrChar = plaintext.substr(i, 2);
            int textPos = getCyrillicPositionUTF8(plaintext, i);
            
            if (textPos != -1) {
                int newPos = (textPos + shift + 1) % 33;
                string encryptedChar = getCyrillicFromPositionUTF8(newPos, cyrChar);
                ciphertext += encryptedChar;
            } else {
                ciphertext += cyrChar;
            }
            i += 2;
        }
        //шифрование символов ASCII
        else {
            unsigned char currentChar = plaintext[i];
            unsigned char encryptedChar;

            if (isalpha(currentChar)) {
                //для латинских букв - шифрование с сохранением регистра
                char base = isupper(currentChar) ? 'A' : 'a';
                encryptedChar = (currentChar - base + shift) % 26 + base;
            } else {
                //для остальных символов - простое сложение по модулю 256
                encryptedChar = (currentChar + shift + 1) % 256;
            }

            ciphertext += static_cast<char>(encryptedChar);
            i++;
        }
    }
    
    return ciphertext;
}

//функция дешифрования
string decryptVigenere(const string& ciphertext, const string& key, bool useCyrillic) {
    if (ciphertext.empty()) return ciphertext;
    
    string preparedKey = prepareKey(key);
    if (preparedKey.empty()) {
        throw invalid_argument("Ключ не должен быть пустым");
    }
    
    string plaintext;
    size_t keyPos = 0;
    
    for (size_t i = 0; i < ciphertext.length(); ) {
        int shift = getKeyValue(preparedKey, keyPos);
        
        //дешифрование кириллицы
        if (useCyrillic && isCyrillicUTF8(ciphertext, i)) {
            string cyrChar = ciphertext.substr(i, 2);
            int textPos = getCyrillicPositionUTF8(ciphertext, i);
            
            if (textPos != -1) {
                int newPos = (textPos - shift - 1 + 33) % 33;
                string decryptedChar = getCyrillicFromPositionUTF8(newPos, cyrChar);
                plaintext += decryptedChar;
            } else {
                plaintext += cyrChar;
            }
            i += 2;
        }
        //дешифрование остальных символов ASCII
        else {
            unsigned char currentChar = ciphertext[i];
            unsigned char decryptedChar;
            
            if (isalpha(currentChar)) {
                //для латинских букв - дешифрование с сохранением регистра
                char base = isupper(currentChar) ? 'A' : 'a';
                decryptedChar = (currentChar - base - shift + 26) % 26 + base;
            } else {
                //для остальных символов - простое вычитание по модулю 256
                decryptedChar = (currentChar - shift + 256 + 1) % 256;
            }
            
            plaintext += static_cast<char>(decryptedChar);
            i++;
        }
    }
    
    return plaintext;
}

//бинарное шифрование Виженера
string encryptVigenereBinary(const string& data, const string& key) {
    if (data.empty() || key.empty()) return data;
    
    string result;
    size_t keyLen = key.size();
    
    for (size_t i = 0; i < data.length(); ++i) {
        unsigned char b = static_cast<unsigned char>(data[i]);
        unsigned char k = static_cast<unsigned char>(key[i % keyLen]);
        result += static_cast<char>((b + k) % 256);
    }
    
    return result;
}

//бинарное дешифрование Виженера
string decryptVigenereBinary(const string& data, const string& key) {
    if (data.empty() || key.empty()) return data;
    
    string result;
    size_t keyLen = key.size();
    
    for (size_t i = 0; i < data.length(); ++i) {
        unsigned char b = static_cast<unsigned char>(data[i]);
        unsigned char k = static_cast<unsigned char>(key[i % keyLen]);
        result += static_cast<char>((b - k + 256) % 256);
    }
    
    return result;
}