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
    
    // ВОТ ИСПРАВЛЕНИЕ: используем тот же подход, что и в getCyrillicPositionUTF8
    vector<string> alphabetLower = getCyrillicAlphabet(false);
    vector<string> alphabetUpper = getCyrillicAlphabet(true);
    
    // Определяем, в каком регистре был оригинальный символ
    for (size_t i = 0; i < alphabetLower.size(); i++) {
        if (alphabetLower[i] == originalChar) {
            return alphabetLower[pos];
        }
        if (alphabetUpper[i] == originalChar) {
            return alphabetUpper[pos];
        }
    }
    
    // Если не нашли (маловероятно), возвращаем из нижнего регистра
    return alphabetLower[pos];
}

// Подготавливает ключ - оставляет только буквы
string prepareKey(const string& key) {
    string preparedKey;
    
    for (size_t i = 0; i < key.length(); ) {
        if (isCyrillicUTF8(key, i)) {
            // Для кириллицы добавляем оба байта как есть
            preparedKey += key[i];
            preparedKey += key[i + 1];
            i += 2;
        } else if (isLatin(key[i])) {
            preparedKey += key[i]; // сохраняем оригинальный регистр
            i++;
        } else {
            i++;
        }
    }
    
    return preparedKey;
}

// Создает расширенный ключ той же длины, что и текст
string expandKey(const string& text, const string& key) {
    if (key.empty()) return "";
    
    string expandedKey;
    size_t keyPos = 0;
    
    for (size_t i = 0; i < text.length(); ) {
        if (isCyrillicUTF8(text, i)) {
            //для кириллического символа в тексте берем кириллический символ из ключа
            if (keyPos < key.length()) {
                if (isCyrillicUTF8(key, keyPos)) {
                    expandedKey += key[keyPos];
                    expandedKey += key[keyPos + 1];
                    keyPos += 2;
                } else {
                    //если в ключе латиница, используем ее как есть
                    expandedKey += key[keyPos];
                    expandedKey += ' '; //заполнитель
                    keyPos++;
                }
            } else {
                keyPos = 0;
                if (isCyrillicUTF8(key, keyPos)) {
                    expandedKey += key[keyPos];
                    expandedKey += key[keyPos + 1];
                    keyPos += 2;
                } else {
                    expandedKey += key[keyPos];
                    expandedKey += ' ';
                    keyPos++;
                }
            }
            i += 2;
        } else if (isLatin(text[i])) {
            //для латинского символа берем один символ из ключа
            if (keyPos < key.length()) {
                expandedKey += key[keyPos];
                keyPos++;
            } else {
                keyPos = 0;
                expandedKey += key[keyPos];
                keyPos++;
            }
            i++;
        } else {
            //для не-буквенных символов не используем ключ
            expandedKey += ' ';
            i++;
        }
        
        if (keyPos >= key.length()) {
            keyPos = 0;
        }
    }
    
    return expandedKey;
}

// Получает числовой сдвиг из символа ключа
int getShiftFromKeyChar(const string& keyChar, size_t pos) {
    if (isCyrillicUTF8(keyChar, pos)) {
        int posCyr = getCyrillicPositionUTF8(keyChar, pos);
        return posCyr != -1 ? posCyr : 0;
    } else if (isLatin(keyChar[pos])) {
        return toupper(keyChar[pos]) - 'A';
    }
    return 0;
}

// Основная функция шифрования
string encryptVigenere(const string& plaintext, const string& key, bool useCyrillic) {
    if (plaintext.empty()) return plaintext;
    
    string preparedKey = prepareKey(key);
    if (preparedKey.empty()) {
        throw invalid_argument("Ключ должен содержать хотя бы одну букву");
    }
    
    string expandedKey = expandKey(plaintext, preparedKey);
    string ciphertext;
    
    size_t i = 0;
    while (i < plaintext.length()) {
        // Шифрование кириллицы
        if (useCyrillic && isCyrillicUTF8(plaintext, i)) {
            string cyrChar = plaintext.substr(i, 2);
            int textPos = getCyrillicPositionUTF8(plaintext, i);
            
            if (textPos != -1) {
                // Получаем сдвиг из ключа
                int shift = getShiftFromKeyChar(expandedKey, i);
                
                // Выполняем шифрование
                int newPos = (textPos + shift) % 33;
                
                // Определяем регистр исходного символа
                bool isLower = (static_cast<unsigned char>(cyrChar[0]) == 0xD1);
                string encryptedChar = getCyrillicFromPositionUTF8(newPos, cyrChar);
                ciphertext += encryptedChar;
            } else {
                ciphertext += cyrChar;
            }
            i += 2;
        }
        // Шифрование латиницы
        else if (isLatin(plaintext[i])) {
            char currentChar = plaintext[i];
            char base = isupper(currentChar) ? 'A' : 'a';
            
            // Получаем сдвиг из ключа
            int shift = getShiftFromKeyChar(expandedKey, i);
            
            // Выполняем шифрование
            char encryptedChar = base + (currentChar - base + shift) % 26;
            ciphertext += encryptedChar;
            
            i++;
        }
        // Остальные символы
        else {
            ciphertext += plaintext[i];
            i++;
        }
    }
    
    return ciphertext;
}

// Основная функция дешифрования
string decryptVigenere(const string& ciphertext, const string& key, bool useCyrillic) {
    if (ciphertext.empty()) return ciphertext;
    
    string preparedKey = prepareKey(key);
    if (preparedKey.empty()) {
        throw invalid_argument("Ключ должен содержать хотя бы одну букву");
    }
    
    string expandedKey = expandKey(ciphertext, preparedKey);
    string plaintext;
    
    size_t i = 0;
    while (i < ciphertext.length()) {
        // Дешифрование кириллицы
        if (useCyrillic && isCyrillicUTF8(ciphertext, i)) {
            string cyrChar = ciphertext.substr(i, 2);
            int textPos = getCyrillicPositionUTF8(ciphertext, i);
            
            if (textPos != -1) {
                // Получаем сдвиг из ключа
                int shift = getShiftFromKeyChar(expandedKey, i);
                
                // Выполняем дешифрование
                int newPos = (textPos - shift + 33) % 33;
                
                // ВОТ ИСПРАВЛЕНИЕ: определяем регистр по ЗАШИФРОВАННОМУ тексту, но инвертируем логику
                // Если зашифрованный текст в нижнем регистре - значит оригинал был в верхнем, и наоборот
                bool isLower = (static_cast<unsigned char>(cyrChar[0]) == 0xD1);
                // Инвертируем регистр для дешифрования
                isLower = !isLower;
                
                string decryptedChar = getCyrillicFromPositionUTF8(newPos, cyrChar);
                plaintext += decryptedChar;
            } else {
                plaintext += cyrChar;
            }
            i += 2;
        }
        // Дешифрование латиницы
        else if (isLatin(ciphertext[i])) {
            char currentChar = ciphertext[i];
            char base = isupper(currentChar) ? 'A' : 'a';
            
            // Получаем сдвиг из ключа
            int shift = getShiftFromKeyChar(expandedKey, i);
            
            // Выполняем дешифрование
            char decryptedChar = base + (currentChar - base - shift + 26) % 26;
            plaintext += decryptedChar;
            
            i++;
        }
        // Остальные символы
        else {
            plaintext += ciphertext[i];
            i++;
        }
    }
    
    return plaintext;
}