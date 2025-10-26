#include "permutation.h"
#include "utils.h"
#include <string>
#include <vector>
#include <algorithm>
#include <utility>
#include <cctype>

using namespace std;


//алгоритм шифрования:
//замена пробелов на подчеркивания для сохранения структуры текста
//создание таблицы и запись текста построчно
//определение порядка столбцов на основе ключа (сортировка)
//чтение таблицы по столбцам в порядке, определенном ключом
string encryptPermutationText(const string& text, const string& key) {
    if (text.empty()) return "";
    string processed = replaceSpacesWithUnderscores(text);//замена пробеловз
    string upperKey;
    for (unsigned char c : key) if (isalpha(c)) upperKey += toupper(c); //подготовка ключа: оставляем только буквы и преобразуем в верхний регистр
    if (upperKey.empty()) return processed;

    //создаем порядок столбцов на основе ключа (символ + позиция)
    vector<pair<char, int>> keyWithIndex;
    for (int i = 0; i < (int)upperKey.size(); ++i) {
        keyWithIndex.push_back({upperKey[i], i});
    }
    
    //сортируем ключ для получения порядка перестановки
    sort(keyWithIndex.begin(), keyWithIndex.end(), 
         [](const pair<char,int>& a, const pair<char,int>& b) {
             return a.first == b.first ? a.second < b.second : a.first < b.first;
         });
    
    //создаем порядок столбцов для шифрования
    vector<int> columnOrder(upperKey.size());
    for (int i = 0; i < (int)keyWithIndex.size(); ++i) {
        columnOrder[keyWithIndex[i].second] = i;
    }

    int cols = (int)upperKey.size(); //количество столбцов = длина ключа
    int rows = (processed.size() + cols - 1) / cols; //вычисляем необходимое количество строк
    
    //создаем таблицу (заполняем "_")
    vector<vector<char>> table(rows, vector<char>(cols, '_'));
    
    //заполняем таблицу по строкам (по диагонали с левого верхнего угла)
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int pos = i * cols + j;
            if (pos < (int)processed.size()) {
                table[i][j] = processed[pos];
            }
        }
    }
    
    //читаем по столбцам в порядке columnOrder
    string ciphertext;
    ciphertext.reserve(rows * cols);
    
    //читаем столбцы в порядке: сначала столбец с наименьшим номером в новой нумерации
    for (int colIndex = 0; colIndex < cols; ++colIndex) {
        //находим исходный столбец, который должен идти на позиции colIndex
        int originalCol = -1;
        for (int i = 0; i < cols; ++i) {
            if (columnOrder[i] == colIndex) {
                originalCol = i;
                break;
            }
        }
        
        //читаем столбец сверху вниз
        for (int i = 0; i < rows; ++i) {
            ciphertext += table[i][originalCol];
        }
    }
    
    return ciphertext;
}

//алгоритм дешифрования - обратный процесс шифрованию
string decryptPermutationText(const string& ciphertext, const string& key) {
    if (ciphertext.empty()) return "";
    string upperKey;
    for (unsigned char c : key) if (isalpha(c)) upperKey += toupper(c);
    if (upperKey.empty()) return ciphertext;

    //создаем порядок столбцов на основе ключа
    vector<pair<char, int>> keyWithIndex;
    for (int i = 0; i < (int)upperKey.size(); ++i) {
        keyWithIndex.push_back({upperKey[i], i});
    }
    
    sort(keyWithIndex.begin(), keyWithIndex.end(), 
         [](const pair<char,int>& a, const pair<char,int>& b) {
             return a.first == b.first ? a.second < b.second : a.first < b.first;
         });
    
    vector<int> columnOrder(upperKey.size());
    for (int i = 0; i < (int)keyWithIndex.size(); ++i) {
        columnOrder[keyWithIndex[i].second] = i;
    }

    int cols = (int)upperKey.size();
    int rows = (ciphertext.size() + cols - 1) / cols;
    
    //создаем пустую таблицу
    vector<vector<char>> table(rows, vector<char>(cols, '_'));
    
    //заполняем таблицу по столбцам в порядке columnOrder
    int cipherIndex = 0;
    for (int colIndex = 0; colIndex < cols; ++colIndex) {
        //находим исходный столбец, который должен идти на позиции colIndex
        int originalCol = -1;
        for (int i = 0; i < cols; ++i) {
            if (columnOrder[i] == colIndex) {
                originalCol = i;
                break;
            }
        }
        
        //записываем столбец сверху вниз
        for (int i = 0; i < rows; ++i) {
            if (cipherIndex < (int)ciphertext.size()) {
                table[i][originalCol] = ciphertext[cipherIndex++];
            }
        }
    }
    
    //читаем таблицу по строкам
    string plaintext;
    plaintext.reserve(rows * cols);
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            plaintext += table[i][j];
        }
    }
    
    //убираем лишние подчеркивания в конце
    size_t lastChar = plaintext.find_last_not_of('_');
    if (lastChar != string::npos) {
        plaintext = plaintext.substr(0, lastChar + 1);
    }
    
    return restoreUnderscoresToSpaces(plaintext); //возвращаем без "заглушек" в виде "_"
}