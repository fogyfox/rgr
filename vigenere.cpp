#include "vigenere.h"
#include "utils.h"
#include <string>
#include <algorithm>
#include <stdexcept>

using namespace std;

int getCyrillicPosition(char c) {
    if (c >= RUS_A_UPPER && c <= RUS_E_UPPER) {
        return c - RUS_A_UPPER;
    } else if (c == RUS_YO_UPPER) {
        return 6;
    } else if (c >= RUS_ZH_UPPER && c <= RUS_YA_UPPER) {
        return c - RUS_A_UPPER + 1;
    }
    return -1;
}

char getCyrillicFromPosition(int pos) {
    if (pos >= 0 && pos <= 5) {
        return RUS_A_UPPER + pos;
    } else if (pos == 6) {
        return RUS_YO_UPPER;
    } else if (pos >= 7 && pos <= 32) {
        return RUS_A_UPPER + pos - 1;
    }
    return '?';
}

string prepareKey(const string& key, bool useCyrillic) {
    string preparedKey;
    for (char c : key) {
        if (useCyrillic ? isCyrillic(c) : isLatin(c)) {
            if (useCyrillic) {
                preparedKey += toUpperCyrillic(c);
            } else {
                preparedKey += toupper(c);
            }
        }
    }
    return preparedKey;
}

// НОВАЯ ФУНКЦИЯ - автоматическое определение алфавита
bool detectAlphabet(const string& text, const string& key) {
    // Считаем кириллические и латинские символы в тексте и ключе
    int cyrillicCount = 0;
    int latinCount = 0;
    
    for (char c : text) {
        if (isCyrillic(c)) cyrillicCount++;
        else if (isLatin(c)) latinCount++;
    }
    
    for (char c : key) {
        if (isCyrillic(c)) cyrillicCount++;
        else if (isLatin(c)) latinCount++;
    }
    
    // Если есть кириллические символы - используем кириллицу, иначе латиницу
    return cyrillicCount > 0;
}

string expandKey(const string& text, const string& key, bool useCyrillic) {
    string expandedKey;
    expandedKey.reserve(text.length());
    
    int keyIndex = 0;
    for (char c : text) {
        if ((useCyrillic && isCyrillic(c)) || (!useCyrillic && isLatin(c))) {
            expandedKey += key[keyIndex % key.length()];
            keyIndex++;
        } else {
            expandedKey += c;
        }
    }
    return expandedKey;
}

string encryptVigenere(const string& plaintext, const string& key, bool useCyrillic) {
    string ciphertext;
    ciphertext.reserve(plaintext.length());
    
    // АВТОМАТИЧЕСКОЕ ОПРЕДЕЛЕНИЕ АЛФАВИТА
    bool actualUseCyrillic = useCyrillic;
    if (key == "auto") {
        actualUseCyrillic = detectAlphabet(plaintext, key);
    }
    
    string preparedKey = prepareKey(key, actualUseCyrillic);
    if (preparedKey.empty()) {
        // Пробуем другой алфавит, если текущий не подошел
        actualUseCyrillic = !useCyrillic;
        preparedKey = prepareKey(key, actualUseCyrillic);
        if (preparedKey.empty()) {
            throw invalid_argument("Ключ должен содержать хотя бы одну букву (кириллицу или латиницу)");
        }
    }
    
    string expandedKey = expandKey(plaintext, preparedKey, actualUseCyrillic);
    
    int alphabetSize = actualUseCyrillic ? 33 : 26;
    
    for (size_t i = 0; i < plaintext.length(); i++) {
        char p = plaintext[i];
        
        if ((actualUseCyrillic && isCyrillic(p)) || (!actualUseCyrillic && isLatin(p))) {
            char k = expandedKey[i];
            char encryptedChar;
            
            if (actualUseCyrillic) {
                int pPos = getCyrillicPosition(toUpperCyrillic(p));
                int kPos = getCyrillicPosition(k);
                
                if (pPos != -1 && kPos != -1) {
                    int encryptedPos = (pPos + kPos) % alphabetSize;
                    encryptedChar = getCyrillicFromPosition(encryptedPos);
                    
                    if (p >= RUS_A_LOWER && p <= RUS_YA_LOWER) {
                        encryptedChar = toLowerCyrillic(encryptedChar);
                    }
                } else {
                    encryptedChar = p;
                }
            } else {
                if (isupper(p)) {
                    encryptedChar = ((p - 'A') + (k - 'A')) % alphabetSize + 'A';
                } else {
                    encryptedChar = ((p - 'a') + (k - 'A')) % alphabetSize + 'a';
                }
            }
            
            ciphertext += encryptedChar;
        } else {
            ciphertext += p;
        }
    }
    
    return ciphertext;
}

string decryptVigenere(const string& ciphertext, const std::string& key, bool useCyrillic) {
    string plaintext;
    plaintext.reserve(ciphertext.length());
    
    // АВТОМАТИЧЕСКОЕ ОПРЕДЕЛЕНИЕ АЛФАВИТА
    bool actualUseCyrillic = useCyrillic;
    if (key == "auto") {
        actualUseCyrillic = detectAlphabet(ciphertext, key);
    }
    
    string preparedKey = prepareKey(key, actualUseCyrillic);
    if (preparedKey.empty()) {
        // Пробуем другой алфавит, если текущий не подошел
        actualUseCyrillic = !useCyrillic;
        preparedKey = prepareKey(key, actualUseCyrillic);
        if (preparedKey.empty()) {
            throw invalid_argument("Ключ должен содержать хотя бы одну букву (кириллицу или латиницу)");
        }
    }
    
    string expandedKey = expandKey(ciphertext, preparedKey, actualUseCyrillic);
    
    int alphabetSize = actualUseCyrillic ? 33 : 26;
    
    for (size_t i = 0; i < ciphertext.length(); i++) {
        char c = ciphertext[i];
        
        if ((actualUseCyrillic && isCyrillic(c)) || (!actualUseCyrillic && isLatin(c))) {
            char k = expandedKey[i];
            char decryptedChar;
            
            if (actualUseCyrillic) {
                int cPos = getCyrillicPosition(toUpperCyrillic(c));
                int kPos = getCyrillicPosition(k);
                
                if (cPos != -1 && kPos != -1) {
                    int decryptedPos = (cPos - kPos + alphabetSize) % alphabetSize;
                    decryptedChar = getCyrillicFromPosition(decryptedPos);
                    
                    if (c >= RUS_A_LOWER && c <= RUS_YA_LOWER) {
                        decryptedChar = toLowerCyrillic(decryptedChar);
                    }
                } else {
                    decryptedChar = c;
                }
            } else {
                if (isupper(c)) {
                    decryptedChar = ((c - 'A') - (k - 'A') + alphabetSize) % alphabetSize + 'A';
                } else {
                    decryptedChar = ((c - 'a') - (k - 'A') + alphabetSize) % alphabetSize + 'a';
                }
            }
            
            plaintext += decryptedChar;
        } else {
            plaintext += c;
        }
    }
    
    return plaintext;
}