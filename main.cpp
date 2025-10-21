#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <utility>
#include <fstream>
#include <limits>


using namespace std;

// Функция для настройки кодировки

// Заменяем пробелы на подчёркивания
string replaceSpacesWithUnderscores(const string& text) {
    string result = text;
    for (char& c : result) {
        if (c == ' ') c = '_';
    }
    return result;
}

// Восстанавливаем подчёркивания в пробелы
string restoreUnderscoresToSpaces(const string& text) {
    string result = text;
    for (char& c : result) {
        if (c == '_') c = ' ';
    }
    return result;
}

// Функция для шифрования перестановкой с ключом (оптимизированная для больших файлов)
string encryptPermutation(const string& text, const string& key) {
    if (text.empty()) return "";

    // Заменяем пробелы на подчёркивания
    string processedText = replaceSpacesWithUnderscores(text);

    // Обрабатываем ключ
    string upperKey;
    for (size_t i = 0; i < key.size(); ++i) {
        char c = key[i];
        if (isalpha(c)) {
            upperKey += toupper(c);
        }
    }

    if (upperKey.empty()) return processedText;

    // Создаем вектор пар (символ ключа, исходная позиция)
    vector<pair<char, int>> keyChars;
    for (int i = 0; i < (int)upperKey.size(); ++i) {
        keyChars.push_back(make_pair(upperKey[i], i));
    }

    // Сортируем по алфавиту
    sort(keyChars.begin(), keyChars.end(),
        [](const pair<char, int>& a, const pair<char, int>& b) {
            if (a.first == b.first) {
                return a.second < b.second;
            }
            return a.first < b.first;
        });

    // Создаем порядок перестановки столбцов
    vector<int> order(keyChars.size());
    for (int i = 0; i < (int)keyChars.size(); ++i) {
        order[keyChars[i].second] = i;
    }

    // Определяем размеры таблицы
    int cols = (int)upperKey.size();
    int rows = ((int)processedText.size() + cols - 1) / cols;

    // Оптимизация: используем reserve для предварительного выделения памяти
    string ciphertext;
    ciphertext.reserve(processedText.size() + rows * 2); // + буфер для форматирования

    // Прямое вычисление без создания полной таблицы в памяти
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int originalCol = -1;
            // Находим исходный столбец для текущей позиции
            for (int k = 0; k < cols; ++k) {
                if (order[k] == j) {
                    originalCol = k;
                    break;
                }
            }

            int pos = i * cols + originalCol;
            if (pos < (int)processedText.size()) {
                ciphertext += processedText[pos];
            }
            else {
                ciphertext += '_';
            }
        }
    }

    // Форматируем в группы по 5 символов (только для отображения)
    string result;
    result.reserve(ciphertext.size() + ciphertext.size() / 5 + 1);

    int count = 0;
    for (size_t i = 0; i < ciphertext.size(); ++i) {
        if (count > 0 && count % 5 == 0) {
            result += ' ';
        }
        result += ciphertext[i];
        count++;
    }

    return result;
}

// Функция для дешифрования перестановкой с ключом (оптимизированная)
string decryptPermutation(const string& ciphertext, const string& key) {
    if (ciphertext.empty()) return "";

    // Удаляем пробелы из шифртекста
    string processedCiphertext;
    processedCiphertext.reserve(ciphertext.size());

    for (char c : ciphertext) {
        if (c != ' ') {
            processedCiphertext += c;
        }
    }

    // Обрабатываем ключ
    string upperKey;
    for (size_t i = 0; i < key.size(); ++i) {
        char c = key[i];
        if (isalpha(c)) {
            upperKey += toupper(c);
        }
    }

    if (upperKey.empty()) return processedCiphertext;

    // Создаем вектор пар (символ ключа, исходная позиция)
    vector<pair<char, int>> keyChars;
    for (int i = 0; i < (int)upperKey.size(); ++i) {
        keyChars.push_back(make_pair(upperKey[i], i));
    }

    // Сортируем по алфавиту
    sort(keyChars.begin(), keyChars.end(),
        [](const pair<char, int>& a, const pair<char, int>& b) {
            if (a.first == b.first) {
                return a.second < b.second;
            }
            return a.first < b.first;
        });

    // Создаем порядок перестановки столбцов
    vector<int> order(keyChars.size());
    for (int i = 0; i < (int)keyChars.size(); ++i) {
        order[keyChars[i].second] = i;
    }

    // Создаем обратный порядок
    vector<int> reverseOrder(order.size());
    for (int i = 0; i < (int)order.size(); ++i) {
        reverseOrder[order[i]] = i;
    }

    // Определяем размеры таблицы
    int cols = (int)upperKey.size();
    int rows = ((int)processedCiphertext.size() + cols - 1) / cols;

    // Восстанавливаем текст без создания полной таблицы
    string plaintext;
    plaintext.reserve(processedCiphertext.size());

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int pos = i * cols + reverseOrder[j];
            if (pos < (int)processedCiphertext.size()) {
                plaintext += processedCiphertext[pos];
            }
        }
    }

    // Удаляем заполняющие символы
    size_t lastChar = plaintext.find_last_not_of('_');
    if (lastChar != string::npos) {
        plaintext = plaintext.substr(0, lastChar + 1);
    }

    // Восстанавливаем пробелы
    return restoreUnderscoresToSpaces(plaintext);
}

// Функция для чтения больших файлов
string readLargeFile(const string& fileName) {
    ifstream in(fileName, ios::binary | ios::ate); // открываем в конце файла
    if (!in.is_open()) {
        throw runtime_error("Ошибка: не удалось открыть файл '" + fileName + "'");
    }

    // Получаем размер файла
    streamsize size = in.tellg();
    in.seekg(0, ios::beg);

    // Проверяем размер файла
    if (size > 100 * 1024 * 1024) { // больше 100 MB
        cout << "Предупреждение: файл очень большой (" << size / (1024 * 1024)
            << " MB). Обработка может занять время..." << endl;
    }

    // Читаем файл блоками для экономии памяти
    const size_t BUFFER_SIZE = 64 * 1024; // 64 KB блоки
    vector<char> buffer(BUFFER_SIZE);
    string content;
    content.reserve(size); // предварительное выделение памяти

    cout << "Чтение файла... Размер: " << size / 1024 << " KB" << endl;

    while (in.read(buffer.data(), BUFFER_SIZE)) {
        content.append(buffer.data(), in.gcount());
    }
    content.append(buffer.data(), in.gcount());

    in.close();
    return content;
}

// Функция для записи больших файлов
void writeLargeFile(const string& fileName, const string& content) {
    ofstream out(fileName, ios::binary);
    if (!out.is_open()) {
        throw runtime_error("Ошибка: не удалось создать файл '" + fileName + "'");
    }

    // Записываем блоками
    const size_t BUFFER_SIZE = 64 * 1024; // 64 KB блоки
    size_t totalWritten = 0;
    size_t totalSize = content.size();

    cout << "Запись файла... Размер: " << totalSize / 1024 << " KB" << endl;

    while (totalWritten < totalSize) {
        size_t toWrite = min(BUFFER_SIZE, totalSize - totalWritten);
        out.write(content.c_str() + totalWritten, toWrite);
        totalWritten += toWrite;
    }

    out.close();
    cout << "Текст сохранён в файл " << fileName
        << " (" << totalSize / 1024 << " KB)" << endl;
}

// Функция для показа прогресса
void showProgress(size_t current, size_t total, const string& operation) {
    if (total > 1000000) { // показываем прогресс только для больших файлов
        int percent = static_cast<int>((current * 100) / total);
        if (percent % 5 == 0) { // обновляем каждые 5%
            cout << operation << ": " << percent << "%\r";
            cout.flush();
        }
    }
}

// Заглушки для других методов шифрования
string encryptVigenere(const string& text, const string& key) {
    return "Шифр Вижинера ещё не реализован";
}

string decryptVigenere(const string& ciphertext, const string& key) {
    return "Дешифрование Вижинера ещё не реализовано";
}

string encryptGronsfeld(const string& text, const string& key) {
    return "Шифр Гронсфельда ещё не реализован";
}

string decryptGronsfeld(const string& ciphertext, const string& key) {
    return "Дешифрование Гронсфельда ещё не реализовано";
}

vector<unsigned char> readBinaryFile(const string& fileName) {
    ifstream in(fileName, ios::binary | ios::ate);
    if (!in.is_open()) {
        throw runtime_error("Ошибка: не удалось открыть файл '" + fileName + "'");
    }

    streamsize size = in.tellg();
    in.seekg(0, ios::beg);

    vector<unsigned char> buffer(size);
    if (!in.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw runtime_error("Ошибка чтения файла");
    }

    in.close();
    return buffer;
}

vector<unsigned char> encryptPermutationBinary(const vector<unsigned char>& data, const string& key) {
    if (data.empty()) return {};

    // Обработка ключа (только буквы, в верхний регистр)
    string upperKey;
    for (char c : key) if (isalpha(c)) upperKey += toupper(c);
    if (upperKey.empty()) return data;

    int cols = (int)upperKey.size();
    int rows = (data.size() + cols - 1) / cols;

    // Создаём порядок столбцов
    vector<pair<char,int>> keyChars;
    for (int i = 0; i < (int)upperKey.size(); ++i) keyChars.push_back({upperKey[i], i});
    sort(keyChars.begin(), keyChars.end(), [](auto &a, auto &b){
        return a.first != b.first ? a.first < b.first : a.second < b.second;
    });

    vector<int> order(upperKey.size());
    for (int i = 0; i < (int)keyChars.size(); ++i) order[keyChars[i].second] = i;

    // Шифруем данные без добавления лишних байтов
    vector<unsigned char> encrypted(data.size());
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int originalCol = -1;
            for (int k = 0; k < cols; ++k) if (order[k] == j) { originalCol = k; break; }

            int pos = i * cols + originalCol;
            int encPos = i * cols + j;
            if (pos < (int)data.size() && encPos < (int)data.size()) {
                encrypted[encPos] = data[pos];
            }
        }
    }

    return encrypted;
}


vector<unsigned char> decryptPermutationBinary(const vector<unsigned char>& data, const string& key, size_t originalSize) {
    if (data.empty()) return {};

    string upperKey;
    for (char c : key) if (isalpha(c)) upperKey += toupper(c);
    if (upperKey.empty()) return data;

    int cols = (int)upperKey.size();
    int rows = (data.size() + cols - 1) / cols;

    vector<pair<char,int>> keyChars;
    for (int i = 0; i < cols; ++i)
        keyChars.push_back({upperKey[i], i});
    sort(keyChars.begin(), keyChars.end(), [](auto &a, auto &b) {
        return a.first != b.first ? a.first < b.first : a.second < b.second;
    });

    vector<int> order(cols);
    for (int i = 0; i < cols; ++i)
        order[keyChars[i].second] = i;

    vector<int> reverseOrder(cols);
    for (int i = 0; i < cols; ++i)
        reverseOrder[order[i]] = i;

    vector<unsigned char> decrypted(data.size());
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int pos = i * cols + reverseOrder[j];
            int decPos = i * cols + j;
            if (pos < (int)data.size() && decPos < (int)data.size())
                decrypted[pos] = data[decPos];
        }
    }

    // Убираем лишние нули и мусор
    if (originalSize < decrypted.size())
        decrypted.resize(originalSize);
    else {
        while (!decrypted.empty() && decrypted.back() == 0)
            decrypted.pop_back();
    }

    return decrypted;
}



void writeLargeFileBinary(const string& fileName, const vector<unsigned char>& data) {
    ofstream out(fileName, ios::binary);
    if (!out.is_open()) {
        throw runtime_error("Ошибка: не удалось создать файл '" + fileName + "'");
    }
    out.write(reinterpret_cast<const char*>(data.data()), data.size());
    out.close();
}

vector<unsigned char> encryptPermutationBinary(const vector<unsigned char>& data, const string& key);
vector<unsigned char> decryptPermutationBinary(const vector<unsigned char>& data, const string& key, size_t originalSize);



int main() {
    while (true) {
        cout << "\n=== КРИПТОГРАФИЧЕСКАЯ СИСТЕМА ===" << endl;
        cout << "0 - Выход" << endl;
        cout << "1 - Шифрование" << endl;
        cout << "2 - Дешифрование" << endl;

        int mode;
        cout << "\nВыберите действие: ";
        cin >> mode;

        cin.clear();
        cin.ignore(10000, '\n');

        if (mode == 0) {
            cout << "Завершение программы..." << endl;
            break;
        }

        if (mode < 1 || mode > 2) {
            cout << "Неверный выбор. Попробуйте снова." << endl;
            continue;
        }

        cout << "\nТип файла:" << endl;
        cout << "1 - Текстовый файл (.txt)" << endl;
        cout << "2 - Бинарный файл (.jpg, .png, .pdf и т.п.)" << endl;

        int fileType;
        cout << "Выберите тип: ";
        cin >> fileType;

        cin.clear();
        cin.ignore(10000, '\n');

        if (fileType < 1 || fileType > 2) {
            cout << "Неверный выбор типа файла." << endl;
            continue;
        }

        cout << "\nМетоды шифрования:" << endl;
        cout << "1 - Табличная перестановка с ключом" << endl;

        int method;
        cout << "Выберите метод: ";
        cin >> method;

        cin.clear();
        cin.ignore(10000, '\n');

        if (method != 1) {
            cout << "Пока доступен только метод перестановки." << endl;
            continue;
        }

        string fileName, key;
        cout << "\nВведите имя файла: ";
        getline(cin, fileName);
        cout << "Введите ключ: ";
        getline(cin, key);

        try {
            if (mode == 1) {
                // === ШИФРОВАНИЕ ===
                if (fileType == 1) {
                    // Текстовый файл
                    string text = readLargeFile(fileName);
                    string encrypted = encryptPermutation(text, key);
                    writeLargeFile(fileName + ".enc.txt", encrypted);
                    cout << "Текст зашифрован и сохранён в " << fileName + ".enc.txt" << endl;
                } else {
                    // Бинарный файл
                    auto data = readBinaryFile(fileName);
                    auto encrypted = encryptPermutationBinary(data, key);
                    writeLargeFileBinary(fileName + ".enc", encrypted);

                    // сохраняем исходный размер для восстановления
                    ofstream meta(fileName + ".meta");
                    meta << data.size();
                    meta.close();

                    cout << "Файл зашифрован и сохранён в " << fileName + ".enc" << endl;
                }
            } 
            else {
                // === ДЕШИФРОВАНИЕ ===
                if (fileType == 1) {
                    // Текстовый файл
                    string text = readLargeFile(fileName);
                    string decrypted = decryptPermutation(text, key);
                    writeLargeFile(fileName + ".dec.txt", decrypted);
                    cout << "Файл расшифрован и сохранён в " << fileName + ".dec.txt" << endl;
                } else {
                    // Бинарный файл
                    size_t origSize = 0;

                    // читаем мета-информацию (размер исходного файла)
                    string baseName = fileName.substr(0, fileName.find_last_of('.'));
                    ifstream meta(baseName + ".meta");
                    if (meta.is_open()) meta >> origSize;
                    meta.close();

                    auto data = readBinaryFile(fileName);
                    auto decrypted = decryptPermutationBinary(data, key, origSize);
                    writeLargeFileBinary(fileName + ".dec", decrypted);
                    cout << "Файл расшифрован и сохранён в " << fileName + ".dec" << endl;
                }
            }
        } 
        catch (const exception& e) {
            cout << "Ошибка: " << e.what() << endl;
        }
    }

    return 0;
}

