#include "permutation.h"
#include "utils.h"
#include <string>
#include <vector>
#include <algorithm>
#include <utility>
#include <cctype>

using namespace std;

//считывание символов с учётом UTF-8
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

//функция для получения числового ключа из строки
vector<int> getNumericKey(const string& key) {
    vector<int> numericKey;
    for (char c : key) {
        numericKey.push_back(static_cast<unsigned char>(c));
    }
    return numericKey;
}

//функция для создания порядка столбцов на основе числового ключа
vector<int> createColumnOrder(const vector<int>& numericKey) {
    vector<pair<int, int>> keyWithIndex;
    for (size_t i = 0; i < numericKey.size(); i++) {
        keyWithIndex.push_back({numericKey[i], i});
    }
    
    //сортируем по значениям ключа
    sort(keyWithIndex.begin(), keyWithIndex.end(), 
         [](const pair<int, int>& a, const pair<int, int>& b) {
             return a.first == b.first ? a.second < b.second : a.first < b.first;
         });
    
    vector<int> columnOrder(keyWithIndex.size());
    for (size_t i = 0; i < keyWithIndex.size(); i++) {
        columnOrder[keyWithIndex[i].second] = i;
    }
    
    return columnOrder;
}

//шифрование бинарных данных
string encryptPermutationBinary(const string& data, const string& key) {
    if (data.empty() || key.empty()) return data;
    
    vector<int> numericKey = getNumericKey(key);
    vector<int> columnOrder = createColumnOrder(numericKey);
    
    int cols = numericKey.size();
    int rows = (data.length() + cols - 1) / cols;
    
    //создаем таблицу
    vector<vector<char>> table(rows, vector<char>(cols, 0));
    
    //заполняем таблицу
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            size_t index = i * cols + j;
            if (index < data.length()) {
                table[i][j] = data[index];
            } else {
                table[i][j] = 0; //заполнитель
            }
        }
    }
    
    //читаем по столбцам в порядке ключа
    string result;
    for (int colIndex = 0; colIndex < cols; colIndex++) {
        int originalCol = -1;
        for (int i = 0; i < cols; i++) {
            if (columnOrder[i] == colIndex) {
                originalCol = i;
                break;
            }
        }
        
        for (int i = 0; i < rows; i++) {
            result += table[i][originalCol];
        }
    }
    
    return result;
}

//дешифрование бинарных данных
string decryptPermutationBinary(const string& data, const string& key) {
    if (data.empty() || key.empty()) return data;
    
    vector<int> numericKey = getNumericKey(key);
    vector<int> columnOrder = createColumnOrder(numericKey);
    
    int cols = numericKey.size();
    int rows = data.length() / cols;
    
    if (rows * cols != data.length()) {
        throw ("Некорректная длина зашифрованных данных");
    }
    
    //создаем таблицу для записи
    vector<vector<char>> table(rows, vector<char>(cols, 0));
    
    //заполняем таблицу по столбцам в порядке ключа
    size_t dataIndex = 0;
    for (int colIndex = 0; colIndex < cols; colIndex++) {
        int originalCol = -1;
        for (int i = 0; i < cols; i++) {
            if (columnOrder[i] == colIndex) {
                originalCol = i;
                break;
            }
        }
        
        for (int i = 0; i < rows; i++) {
            if (dataIndex < data.length()) {
                table[i][originalCol] = data[dataIndex++];
            }
        }
    }
    
    //читаем построчно
    string result;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (i * cols + j < data.length()) {
                result += table[i][j];
            }
        }
    }
    
    //удаляем нулевые байты в конце (заполнители)
    while (!result.empty() && result.back() == 0) {
        result.pop_back();
    }
    
    return result;
}

//шифрование текста
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
    size_t effectiveLength = countEffectiveChars(processed);
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

//дешифрование текста
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
    size_t effectiveLength = countEffectiveChars(ciphertext);
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