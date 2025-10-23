#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <utility>
#include <fstream>
#include <limits>
#include <locale>

using namespace std;

// Функция для настройки кодировки
void setupEncoding() {
    setlocale(LC_ALL, "Russian");
}

// === ФУНКЦИИ ДЛЯ ШИФРА ВИЖЕНЕРА ===

// Функция для проверки, является ли символ латинской буквой
bool isLatin(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

// Функция для проверки, является ли символ кириллической буквой (Windows-1251)
bool isCyrillic(char c) {
    // Русские буквы в кодировке Windows-1251
    return (c >= -64 && c <= -1) || c == -72 || c == -88;
}

// Функция для приведения кириллической буквы к верхнему регистру (Windows-1251)
char toUpperCyrillic(char c) {
    // Строчные буквы в Windows-1251: -32..-1 (а-я), -88 (ё)
    // Прописные буквы в Windows-1251: -64..-33 (А-Я), -72 (Ё)
    if (c >= -32 && c <= -1) { // а-я
        return c - 32; // Преобразование строчной в прописную
    } else if (c == -88) { // ё
        return -72; // Ё
    }
    return c;
}

// Функция для приведения кириллической буквы к нижнему регистру (Windows-1251)
char toLowerCyrillic(char c) {
    if (c >= -64 && c <= -33) { // А-Я
        return c + 32; // Преобразование прописной в строчную
    } else if (c == -72) { // Ё
        return -88; // ё
    }
    return c;
}

// Функция для получения позиции кириллической буквы в алфавите
int getCyrillicPosition(char c) {
    // Позиции в алфавите для Windows-1251:
    // А-Е: -64..-59 (0-5)
    // Ё: -72 (6)
    // Ж-Я: -58..-33 (7-32)
    
    if (c >= -64 && c <= -59) { // А-Е
        return c + 64; // 0-5
    } else if (c == -72) { // Ё
        return 6;
    } else if (c >= -58 && c <= -33) { // Ж-Я
        return c + 65; // 7-32
    }
    return -1; // не кириллическая буква
}

// Функция для получения кириллической буквы по позиции
char getCyrillicFromPosition(int pos) {
    if (pos >= 0 && pos <= 5) { // А-Е
        return pos - 64;
    } else if (pos == 6) { // Ё
        return -72;
    } else if (pos >= 7 && pos <= 32) { // Ж-Я
        return pos - 65;
    }
    return '?';
}

// Функция для подготовки ключа
string prepareKey(const string& key, bool useCyrillic = false) {
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

// Функция для расширения ключа до длины текста
string expandKey(const string& text, const string& key, bool useCyrillic = false) {
    string expandedKey;
    expandedKey.reserve(text.length());
    
    int keyIndex = 0;
    for (char c : text) {
        if ((useCyrillic && isCyrillic(c)) || (!useCyrillic && isLatin(c))) {
            expandedKey += key[keyIndex % key.length()];
            keyIndex++;
        } else {
            expandedKey += c; // Сохраняем другие символы как есть
        }
    }
    return expandedKey;
}

// Функция шифрования Виженера
string encryptVigenere(const string& plaintext, const string& key, bool useCyrillic = false) {
    string ciphertext;
    ciphertext.reserve(plaintext.length());
    
    string preparedKey = prepareKey(key, useCyrillic);
    if (preparedKey.empty()) {
        throw invalid_argument("Ключ должен содержать хотя бы одну букву");
    }
    
    string expandedKey = expandKey(plaintext, preparedKey, useCyrillic);
    
    int alphabetSize = useCyrillic ? 33 : 26;
    
    for (size_t i = 0; i < plaintext.length(); i++) {
        char p = plaintext[i];
        
        if ((useCyrillic && isCyrillic(p)) || (!useCyrillic && isLatin(p))) {
            char k = expandedKey[i];
            char encryptedChar;
            
            if (useCyrillic) {
                int pPos = getCyrillicPosition(toUpperCyrillic(p));
                int kPos = getCyrillicPosition(k);
                
                if (pPos != -1 && kPos != -1) {
                    int encryptedPos = (pPos + kPos) % alphabetSize;
                    encryptedChar = getCyrillicFromPosition(encryptedPos);
                    
                    // Сохраняем регистр
                    if (p >= -32 && p <= -1) { // строчная буква
                        encryptedChar = toLowerCyrillic(encryptedChar);
                    }
                } else {
                    encryptedChar = p; // если не удалось определить позицию
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
            // Сохраняем другие символы как есть
            ciphertext += p;
        }
    }
    
    return ciphertext;
}

// Функция дешифрования Виженера
string decryptVigenere(const string& ciphertext, const string& key, bool useCyrillic = false) {
    string plaintext;
    plaintext.reserve(ciphertext.length());
    
    string preparedKey = prepareKey(key, useCyrillic);
    if (preparedKey.empty()) {
        throw invalid_argument("Ключ должен содержать хотя бы одну букву");
    }
    
    string expandedKey = expandKey(ciphertext, preparedKey, useCyrillic);
    
    int alphabetSize = useCyrillic ? 33 : 26;
    
    for (size_t i = 0; i < ciphertext.length(); i++) {
        char c = ciphertext[i];
        
        if ((useCyrillic && isCyrillic(c)) || (!useCyrillic && isLatin(c))) {
            char k = expandedKey[i];
            char decryptedChar;
            
            if (useCyrillic) {
                int cPos = getCyrillicPosition(toUpperCyrillic(c));
                int kPos = getCyrillicPosition(k);
                
                if (cPos != -1 && kPos != -1) {
                    int decryptedPos = (cPos - kPos + alphabetSize) % alphabetSize;
                    decryptedChar = getCyrillicFromPosition(decryptedPos);
                    
                    // Сохраняем регистр
                    if (c >= -32 && c <= -1) { // строчная буква
                        decryptedChar = toLowerCyrillic(decryptedChar);
                    }
                } else {
                    decryptedChar = c; // если не удалось определить позицию
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
            // Сохраняем другие символы как есть
            plaintext += c;
        }
    }
    
    return plaintext;
}

// Функция для выбора алфавита
bool selectAlphabet() {
    cout << "\nВыберите алфавит:\n";
    cout << "1 - Латинский\n";
    cout << "2 - Кириллический\n";
    cout << "Ваш выбор: ";
    
    int choice;
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    return (choice == 2); // true для кириллицы, false для латиницы
}

// === СУЩЕСТВУЮЩИЕ ФУНКЦИИ ПЕРЕСТАНОВКИ ===

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
    ciphertext.reserve(processedText.size() + rows * 2);

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
    ifstream in(fileName, ios::binary | ios::ate);
    if (!in.is_open()) {
        throw runtime_error("Ошибка: не удалось открыть файл '" + fileName + "'");
    }

    streamsize size = in.tellg();
    in.seekg(0, ios::beg);

    if (size > 100 * 1024 * 1024) {
        cout << "Предупреждение: файл очень большой (" << size / (1024 * 1024)
            << " MB). Обработка может занять время..." << endl;
    }

    const size_t BUFFER_SIZE = 64 * 1024;
    vector<char> buffer(BUFFER_SIZE);
    string content;
    content.reserve(size);

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

    const size_t BUFFER_SIZE = 64 * 1024;
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

// Заглушки для других методов шифрования
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

    string upperKey;
    for (char c : key) if (isalpha(c)) upperKey += toupper(c);
    if (upperKey.empty()) return data;

    int cols = (int)upperKey.size();
    int rows = (data.size() + cols - 1) / cols;

    vector<pair<char,int>> keyChars;
    for (int i = 0; i < (int)upperKey.size(); ++i) keyChars.push_back({upperKey[i], i});
    sort(keyChars.begin(), keyChars.end(), [](auto &a, auto &b){
        return a.first != b.first ? a.first < b.first : a.second < b.second;
    });

    vector<int> order(upperKey.size());
    for (int i = 0; i < (int)keyChars.size(); ++i) order[keyChars[i].second] = i;

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

int main() {
    setupEncoding();
    
    while (true) {
        cout << "\n=== КРИПТОГРАФИЧЕСКАЯ СИСТЕМА ===" << endl;
        cout << "0 - Выход" << endl;
        cout << "1 - Шифрование" << endl;
        cout << "2 - Дешифрование" << endl;

        int mode;
        cout << "\nВыберите действие: ";
        cin >> mode;

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

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
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (fileType < 1 || fileType > 2) {
            cout << "Неверный выбор типа файла." << endl;
            continue;
        }

        cout << "\nМетоды шифрования:" << endl;
        cout << "1 - Табличная перестановка с ключом" << endl;
        cout << "2 - Шифр Виженера" << endl;

        int method;
        cout << "Выберите метод: ";
        cin >> method;

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (method < 1 || method > 2) {
            cout << "Неверный выбор метода." << endl;
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
                    string encrypted;
                    
                    if (method == 1) {
                        // Перестановка
                        encrypted = encryptPermutation(text, key);
                        writeLargeFile(fileName + ".enc_perm.txt", encrypted);
                        cout << "Текст зашифрован (перестановка) и сохранён в " << fileName + ".enc_perm.txt" << endl;
                    } else {
                        // Виженер
                        bool useCyrillic = selectAlphabet();
                        encrypted = encryptVigenere(text, key, useCyrillic);
                        writeLargeFile(fileName + ".enc_vig.txt", encrypted);
                        cout << "Текст зашифрован (Виженер) и сохранён в " << fileName + ".enc_vig.txt" << endl;
                    }
                } else {
                    // Бинарный файл - только перестановка
                    if (method == 1) {
                        auto data = readBinaryFile(fileName);
                        auto encrypted = encryptPermutationBinary(data, key);
                        writeLargeFileBinary(fileName + ".enc", encrypted);

                        ofstream meta(fileName + ".meta");
                        meta << data.size();
                        meta.close();

                        cout << "Файл зашифрован (перестановка) и сохранён в " << fileName + ".enc" << endl;
                    } else {
                        cout << "Шифр Виженера не поддерживается для бинарных файлов." << endl;
                    }
                }
            } 
            else {
                // === ДЕШИФРОВАНИЕ ===
                if (fileType == 1) {
                    // Текстовый файл
                    string text = readLargeFile(fileName);
                    string decrypted;
                    
                    if (method == 1) {
                        // Перестановка
                        decrypted = decryptPermutation(text, key);
                        writeLargeFile(fileName + ".dec_perm.txt", decrypted);
                        cout << "Файл расшифрован (перестановка) и сохранён в " << fileName + ".dec_perm.txt" << endl;
                    } else {
                        // Виженер
                        bool useCyrillic = selectAlphabet();
                        decrypted = decryptVigenere(text, key, useCyrillic);
                        writeLargeFile(fileName + ".dec_vig.txt", decrypted);
                        cout << "Файл расшифрован (Виженер) и сохранён в " << fileName + ".dec_vig.txt" << endl;
                    }
                } else {
                    // Бинарный файл - только перестановка
                    if (method == 1) {
                        size_t origSize = 0;

                        string baseName = fileName.substr(0, fileName.find_last_of('.'));
                        ifstream meta(baseName + ".meta");
                        if (meta.is_open()) meta >> origSize;
                        meta.close();

                        auto data = readBinaryFile(fileName);
                        auto decrypted = decryptPermutationBinary(data, key, origSize);
                        writeLargeFileBinary(fileName + ".dec", decrypted);
                        cout << "Файл расшифрован (перестановка) и сохранён в " << fileName + ".dec" << endl;
                    } else {
                        cout << "Шифр Виженера не поддерживается для бинарных файлов." << endl;
                    }
                }
            }
        } 
        catch (const exception& e) {
            cout << "Ошибка: " << e.what() << endl;
        }
    }

    return 0;
}