#include "vigenere.h"
#include "utils.h"
#include <string>
#include <algorithm>
#include <stdexcept>

using namespace std;

//для корректной работы с кириллицей в Windows-1251
//определяет позицию кириллического символа в алфавите
//алфавит: А-Е, Ё, Ж-Я (всего 33 буквы)
//возвращает: 0-32 для кириллических букв, -1 для остальных
int getCyrillicPosition(char c) {
    unsigned char uc = static_cast<unsigned char>(c);
    
    if (uc >= 0xC0 && uc <= 0xC5) { //А-Е
        return uc - 0xC0;
    } else if (uc == 0xA8) { //Ё
        return 6;
    } else if (uc >= 0xC6 && uc <= 0xDF) { //Ж-Я
        return uc - 0xC0 + 1;
    } else if (uc >= 0xE0 && uc <= 0xE5) { //а-е
        return uc - 0xE0;
    } else if (uc == 0xB8) { //ё
        return 6;
    } else if (uc >= 0xE6 && uc <= 0xFF) { //ж-я
        return uc - 0xE0 + 1;
    }
    return -1;
}

//получает кириллический символ по позиции в алфавите
//pos: позиция в алфавите (0-32)
//isLower: true для строчной буквы, false для прописной
char getCyrillicFromPosition(int pos, bool isLower = false) {
    char base = isLower ? 0xE0 : 0xC0; // а или А
    
    if (pos >= 0 && pos <= 5) {
        return base + pos;
    } else if (pos == 6) {
        return isLower ? 0xB8 : 0xA8; // ё или Ё
    } else if (pos >= 7 && pos <= 32) {
        return base + pos - 1;
    }
    return '?';
}

//подготавливает ключ: оставляет только буквы и преобразует в верхний регистр
//удаляет все не-буквенные символы и приводит к единому регистру
string prepareKey(const string& key) {
    string preparedKey;
    
    for (char c : key) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (isCyrillic(c)) {
            //преобразуем в верхний регистр для кириллицы
            if (uc >= 0xE0 && uc <= 0xFF) { //строчная кириллица
                if (uc == 0xB8) { // ё
                    preparedKey += 0xA8; //Ё
                } else {
                    preparedKey += uc - 0x20; //в верхний регистр
                }
            } else {
                preparedKey += c; //уже верхний регистр
            }
        } else if (isLatin(c)) {
            preparedKey += toupper(c);
        }
    }
    return preparedKey;
}

//расширяет ключ для кириллических символов текста
//создает строку той же длины, что и текст, где на позициях кириллических символов
string expandKeyForCyrillic(const string& text, const string& key) {
    string expandedKey(text.length(), ' ');
    
    int keyIndex = 0;
    for (size_t i = 0; i < text.length(); i++) {
        if (isCyrillic(text[i])) {
            expandedKey[i] = key[keyIndex % key.length()];
            keyIndex++;
        }
    }
    return expandedKey;
}

//расширяет ключ для латинских символов текста
string expandKeyForLatin(const string& text, const string& key) {
    string expandedKey(text.length(), ' ');
    
    int keyIndex = 0;
    for (size_t i = 0; i < text.length(); i++) {
        if (isLatin(text[i])) {
            expandedKey[i] = key[keyIndex % key.length()];
            keyIndex++;
        }
    }
    return expandedKey;
}


//алгоритм шифрования:
//подготовка ключа
//создание расширенных ключей отдельно для кириллицы и латиницы
//для каждого символа текста сдвиг выполняется по формуле: (pos_text + pos_key) mod alphabet_size
string encryptVigenere(const string& plaintext, const string& key, bool useCyrillic) {
    string ciphertext;
    ciphertext.reserve(plaintext.length());
    
    string preparedKey = prepareKey(key);
    if (preparedKey.empty()) {
        throw invalid_argument("Ключ должен содержать хотя бы одну букву");
    }
    
    string expandedKeyCyrillic = expandKeyForCyrillic(plaintext, preparedKey);
    string expandedKeyLatin = expandKeyForLatin(plaintext, preparedKey);
    
    for (size_t i = 0; i < plaintext.length(); i++) {
        unsigned char c = static_cast<unsigned char>(plaintext[i]);
        
        //шифрование кириллицы
        if (isCyrillic(plaintext[i])) {
            char k = expandedKeyCyrillic[i];
            int pPos = getCyrillicPosition(plaintext[i]);//позиция символа текста
            int kPos = getCyrillicPosition(k);//позиция символа ключа
            
            if (pPos != -1 && kPos != -1) {
                int encryptedPos = (pPos + kPos) % 33; //основная формула Виженера: (P + K) mod 33
                bool isLower = (c >= 0xE0 && c <= 0xFF) || c == 0xB8;
                char encryptedChar = getCyrillicFromPosition(encryptedPos, isLower);
                ciphertext += encryptedChar;
            } else {
                ciphertext += plaintext[i]; //если не удалось определить позицию
            }
        }
        //шифрование латиницы
        else if (isLatin(plaintext[i])) {
            char k = expandedKeyLatin[i];
            char base;
            int alphabetSize = 26;
            
            //определяем базовый символ для регистра
            if (isupper(plaintext[i])) {
                base = 'A';
            } else {
                base = 'a';
            }
            
            int keyShift;
            if (isCyrillic(k)) {
                keyShift = getCyrillicPosition(k) % 26; //если ключ кириллический, используем его позицию по модулю 26
            } else {
                keyShift = k - 'A'; //латинский ключ: A=0, B=1, ..., Z=25
            }
            
            char encryptedChar = base + (plaintext[i] - base + keyShift) % alphabetSize; //формула Виженера для латиницы: (P + K) mod 26
            ciphertext += encryptedChar;
        }
        //остальные символы
        else {
            ciphertext += plaintext[i];
        }
    }
    
    return ciphertext;
}


//алгоритм дешифрования:
//алгоритм тот же, только сдвиг опреляется по формуле: (pos_cipher - pos_key + 33(или 26)) mod 33 (или 26)
string decryptVigenere(const string& ciphertext, const string& key, bool useCyrillic) {
    string plaintext;
    plaintext.reserve(ciphertext.length());
    
    string preparedKey = prepareKey(key);
    if (preparedKey.empty()) {
        throw invalid_argument("Ключ должен содержать хотя бы одну букву");
    }
    
    string expandedKeyCyrillic = expandKeyForCyrillic(ciphertext, preparedKey);
    string expandedKeyLatin = expandKeyForLatin(ciphertext, preparedKey);
    
    for (size_t i = 0; i < ciphertext.length(); i++) {
        unsigned char c = static_cast<unsigned char>(ciphertext[i]);
        
        //дешифрование кирилицы
        if (isCyrillic(ciphertext[i])) {
            char k = expandedKeyCyrillic[i];
            int cPos = getCyrillicPosition(ciphertext[i]);
            int kPos = getCyrillicPosition(k);
            
            if (cPos != -1 && kPos != -1) {
                int decryptedPos = (cPos - kPos + 33) % 33; //обратная формула Виженера: (C - K + 33) mod 33
                bool isLower = (c >= 0xE0 && c <= 0xFF) || c == 0xB8;
                char decryptedChar = getCyrillicFromPosition(decryptedPos, isLower);
                plaintext += decryptedChar;
            } else {
                plaintext += ciphertext[i];
            }
        }
        //дешифруем латиницу
        else if (isLatin(ciphertext[i])) {
            char k = expandedKeyLatin[i];
            char base;
            int alphabetSize = 26;
            
            if (isupper(ciphertext[i])) {
                base = 'A';
            } else {
                base = 'a';
            }
            
            int keyShift;
            if (isCyrillic(k)) {
                keyShift = getCyrillicPosition(k) % 26;
            } else {
                keyShift = k - 'A';
            }
            
            char decryptedChar = base + (ciphertext[i] - base - keyShift + alphabetSize) % alphabetSize; //обратная формула для латиницы: (C - K + 26) mod 26
            plaintext += decryptedChar;
        }
        //остальные символы
        else {
            plaintext += ciphertext[i];
        }
    }
    
    return plaintext;
}