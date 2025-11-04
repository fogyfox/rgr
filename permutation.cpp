#include "permutation.h"
#include "utils.h"
#include <string>
#include <vector>
#include <algorithm>
#include <utility>
#include <cctype>

using namespace std;


string getCharAt(const string& text, size_t& index) {
    if (index >= text.length()) return "";
    
    if (isCyrillicUTF8(text, index)) {
        string result = text.substr(index, 2);
        index += 2;
        return result;
    } else {
        string result(1, text[index]);
        index++;
        return result;
    }
}

string encryptPermutationText(const string& text, const string& key) {
    if (text.empty()) return "";
    
    string processed = replaceSpacesWithUnderscores(text);
    string upperKey;
    
    //подготовка ключа - оставляем только буквы
    for (size_t i = 0; i < key.length(); ) {
        if (isCyrillicUTF8(key, i)) {
            upperKey += key[i];
            upperKey += key[i + 1];
            i += 2;
        } else if (isalpha(key[i])) {
            upperKey += toupper(key[i]);
            i++;
        } else {
            i++;
        }
    }
    
    if (upperKey.empty()) return processed;

    //создаем порядок столбцов
    vector<pair<string, int>> keyWithIndex;
    for (size_t i = 0; i < upperKey.length(); ) {
        string keyChar;
        if (isCyrillicUTF8(upperKey, i)) {
            keyChar = upperKey.substr(i, 2);
            keyWithIndex.push_back({keyChar, keyWithIndex.size()});
            i += 2;
        } else {
            keyChar = string(1, upperKey[i]);
            keyWithIndex.push_back({keyChar, keyWithIndex.size()});
            i++;
        }
    }
    
    sort(keyWithIndex.begin(), keyWithIndex.end(), 
         [](const pair<string, int>& a, const pair<string, int>& b) {
             return a.first == b.first ? a.second < b.second : a.first < b.first;
         });
    
    vector<int> columnOrder(keyWithIndex.size());
    for (size_t i = 0; i < keyWithIndex.size(); ++i) {
        columnOrder[keyWithIndex[i].second] = i;
    }

    int cols = keyWithIndex.size();
    size_t effectiveLength = countEffectiveChars(processed); // Используем функцию из utils
    int rows = (effectiveLength + cols - 1) / cols;
    
    //создаем таблицу
    vector<vector<string>> table(rows, vector<string>(cols, "_"));
    
    //заполняем таблицу
    size_t textIndex = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (textIndex < processed.length()) {
                table[i][j] = getCharAt(processed, textIndex);
            }
        }
    }
    
    //читаем по столбцам
    string ciphertext;
    for (int colIndex = 0; colIndex < cols; ++colIndex) {
        int originalCol = -1;
        for (int i = 0; i < cols; ++i) {
            if (columnOrder[i] == colIndex) {
                originalCol = i;
                break;
            }
        }
        
        for (int i = 0; i < rows; ++i) {
            ciphertext += table[i][originalCol];
        }
    }
    
    return ciphertext;
}

string decryptPermutationText(const string& ciphertext, const string& key) {
    if (ciphertext.empty()) return "";
    
    string upperKey;
    for (size_t i = 0; i < key.length(); ) {
        if (isCyrillicUTF8(key, i)) {
            upperKey += key[i];
            upperKey += key[i + 1];
            i += 2;
        } else if (isalpha(key[i])) {
            upperKey += toupper(key[i]);
            i++;
        } else {
            i++;
        }
    }
    
    if (upperKey.empty()) return ciphertext;

    vector<pair<string, int>> keyWithIndex;
    for (size_t i = 0; i < upperKey.length(); ) {
        string keyChar;
        if (isCyrillicUTF8(upperKey, i)) {
            keyChar = upperKey.substr(i, 2);
            keyWithIndex.push_back({keyChar, keyWithIndex.size()});
            i += 2;
        } else {
            keyChar = string(1, upperKey[i]);
            keyWithIndex.push_back({keyChar, keyWithIndex.size()});
            i++;
        }
    }
    
    sort(keyWithIndex.begin(), keyWithIndex.end(), 
         [](const pair<string, int>& a, const pair<string, int>& b) {
             return a.first == b.first ? a.second < b.second : a.first < b.first;
         });
    
    vector<int> columnOrder(keyWithIndex.size());
    for (size_t i = 0; i < keyWithIndex.size(); ++i) {
        columnOrder[keyWithIndex[i].second] = i;
    }

    int cols = keyWithIndex.size();
    size_t effectiveLength = countEffectiveChars(ciphertext); // Используем функцию из utils
    int rows = (effectiveLength + cols - 1) / cols;
    
    vector<vector<string>> table(rows, vector<string>(cols, "_"));
    
    size_t cipherIndex = 0;
    for (int colIndex = 0; colIndex < cols; ++colIndex) {
        int originalCol = -1;
        for (int i = 0; i < cols; ++i) {
            if (columnOrder[i] == colIndex) {
                originalCol = i;
                break;
            }
        }
        
        for (int i = 0; i < rows; ++i) {
            if (cipherIndex < ciphertext.length()) {
                table[i][originalCol] = getCharAt(ciphertext, cipherIndex);
            }
        }
    }
    
    string plaintext;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            plaintext += table[i][j];
        }
    }
    
    size_t lastChar = plaintext.find_last_not_of('_');
    if (lastChar != string::npos) {
        plaintext = plaintext.substr(0, lastChar + 1);
    }
    
    return restoreUnderscoresToSpaces(plaintext);
}