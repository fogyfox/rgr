#include <iostream>
#include <string>
#include <fstream>
#include <limits>
#include <vector>

#include "permutation.h"
#include "vigenere.h"
#include "gronsfeld.h"
#include "utils.h"

using namespace std;

//для текстовых файлов
void encryptTextFile(const string& inputFile, const string& outputFile, const string& key, int method) {
    ifstream in(inputFile);
    ofstream out(outputFile);
    if (!in) throw runtime_error("Не удалось открыть файл " + inputFile);
    if (!out) throw runtime_error("Не удалось создать файл " + outputFile);

    //чтение файла построчно с сохранением переносов строк
    string text;
    string line;
    while (getline(in, line)) {
        text += line + "\n";
    }
    if (!text.empty()) text.pop_back(); //удаляем последний \n

    string result;
    switch (method) {
        case 1: 
            result = encryptPermutationText(text, key);
            break;
        case 2: 
            result = encryptVigenere(text, key, true);
            break;
        case 3: 
            result = encryptGronsfeld(text, key, true);
            break;
    }

    out << result;
}

void decryptTextFile(const string& inputFile, const string& outputFile, const string& key, int method) {
    ifstream in(inputFile);
    ofstream out(outputFile);
    if (!in) throw runtime_error("Не удалось открыть файл " + inputFile);
    if (!out) throw runtime_error("Не удалось создать файл " + outputFile);

    string text;
    string line;
    while (getline(in, line)) {
        text += line + "\n";
    }
    if (!text.empty()) text.pop_back(); //удаляем последний \n

    string result;
    switch (method) {
        case 1: 
            result = decryptPermutationText(text, key);
            break;
        case 2: 
            result = decryptVigenere(text, key, true);
            break;
        case 3: 
            result = decryptGronsfeld(text, key, true);
            break;
    }

    out << result;
}

//для бинарных файлов
void encryptBinaryFile(const string& inputFile, const string& outputFile, const string& key, int method) {
    ifstream in(inputFile, ios::binary);
    ofstream out(outputFile, ios::binary);
    if (!in) throw runtime_error("Не удалось открыть файл " + inputFile);
    if (!out) throw runtime_error("Не удалось создать файл " + outputFile);
    if (key.empty()) throw runtime_error("Ключ не должен быть пустым");

    //буфер для чтения/записи блоков данных
    vector<char> buffer(8192);
    size_t keyLen = key.size();
    size_t pos = 0; //количество фактически прочитанных байт

    while (in) {
        in.read(buffer.data(), buffer.size());
        streamsize n = in.gcount();
        //шифрование каждого байта в блоке
        for (streamsize i = 0; i < n; ++i) {
            unsigned char b = static_cast<unsigned char>(buffer[i]);
            unsigned char k = static_cast<unsigned char>(key[(pos + i) % keyLen]);

            switch (method) {
                case 1: //Табличная перестановка - XOR
                    buffer[i] = b ^ k; 
                    break;
                case 2: // Виженер - сложение по модулю 256
                    buffer[i] = (b + k) % 256; 
                    break;
                case 3: // Гронсфельд - сложение с цифровым ключом
                    //Для бинарных файлов используем только цифры из ключа
                    unsigned char digit_key = 0;
                    for (char key_char : key) {
                        if (key_char >= '0' && key_char <= '9') {
                            digit_key = key_char - '0';
                            break;
                        }
                    }
                    buffer[i] = (b + digit_key) % 256; 
                    break;
            }
        }
        pos += n;
        out.write(buffer.data(), n); //запись зашифрованного блока
    }
}

void decryptBinaryFile(const string& inputFile, const string& outputFile, const string& key, int method) {
    ifstream in(inputFile, ios::binary);
    ofstream out(outputFile, ios::binary);
    if (!in) throw runtime_error("Не удалось открыть файл " + inputFile);
    if (!out) throw runtime_error("Не удалось создать файл " + outputFile);
    if (key.empty()) throw runtime_error("Ключ не должен быть пустым");

    vector<char> buffer(8192);
    size_t keyLen = key.size();
    size_t pos = 0;

    while (in) {
        in.read(buffer.data(), buffer.size());
        streamsize n = in.gcount();
        for (streamsize i = 0; i < n; ++i) {
            unsigned char b = static_cast<unsigned char>(buffer[i]);
            unsigned char k = static_cast<unsigned char>(key[(pos + i) % keyLen]);

            switch (method) {
                case 1: //Табличное шифрование - XOR
                    buffer[i] = b ^ k; 
                    break;
                case 2: //Виженер - вычитание по модулю 256
                    buffer[i] = (b - k + 256) % 256; 
                    break;
                case 3: //Гронсфельд - вычитание с цифровым ключом (берётся только первая цифра из ключа)
                    unsigned char digit_key = 0;
                    for (char key_char : key) {
                        if (key_char >= '0' && key_char <= '9') {
                            digit_key = key_char - '0';
                            break;
                        }
                    }
                    buffer[i] = (b - digit_key + 256) % 256; 
                    break;
            }
        }
        pos += n;
        out.write(buffer.data(), n); //запись
    }
}

//сохранение текста в файл
void saveTextToFile(const string& text, const string& filename) {
    ofstream file(filename);
    if (!file) throw runtime_error("Не удалось создать файл " + filename);
    file << text;
    cout << "Текст успешно сохранён в файл: " << filename << "\n";
}

//интерфейс
int main() {
    while(true) {
        cout << "\n=== КРИПТОГРАФИЧЕСКАЯ СИСТЕМА ===\n";
        cout << "0 - Выход\n";
        cout << "1 - Шифрование текста\n";
        cout << "2 - Шифрование файла\n";
        cout << "3 - Дешифрование файла\n";
        cout << "Выберите действие: ";
        
        int action; 
        cin >> action; 
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (action == 0) break; //выход из программы
        if (action < 1 || action > 3) {
            cout << "Неверный выбор. Попробуйте снова.\n";
            continue;
        }

        //выбор шифра
        cout << "Выберите шифр:\n";
        cout << "1 - Табличная перестановка с ключевым словом\n";
        cout << "2 - шифр Виженера\n";
        cout << "3 - шифр Гронсфельда\n";
        cout << "Выбор: ";
        
        int cipher; 
        cin >> cipher; 
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (cin.fail() || cipher < 1 || cipher > 3) {
            cout << "Неверный выбор шифра. Попробуйте снова.\n";
            if (cin.fail()) {
                cin.clear();
            }
            continue;
        }

        //ввод ключа
        cout << "Введите ключ: "; 
        string key; 
        getline(cin, key);

        try {
            if (action == 1) { //работа с текстом
                cout << "Введите текст: "; 
                string text; 
                getline(cin, text);
                string result;
                
                if (action == 1) { //шифрование текста
                    switch (cipher) {
                        case 1: 
                            result = encryptPermutationText(text, key);
                            break;
                        case 2: 
                            result = encryptVigenere(text, key, true);
                            break;
                        case 3: 
                            result = encryptGronsfeld(text, key, true);
                            break;
                    }
                    cout << "Зашифрованный текст:\n" << result << "\n";
                    
                    //предложение сохранить в файл
                    cout << "Хотите сохранить результат в файл? (y/n): ";
                    char saveChoice;
                    cin >> saveChoice;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    
                    if (saveChoice == 'y' || saveChoice == 'Y') {
                        cout << "Введите имя файла для сохранения: ";
                        string filename;
                        getline(cin, filename);
                        saveTextToFile(result, filename);
                    } else{
                        cout << "Результат не сохранён..." << endl;
                    
                    }
                }
            } else { //работа с файлами
                cout << "Выберите тип файла:\n";
                cout << "1 - Текстовый файл\n";
                cout << "2 - Бинарный файл\n";
                cout << "Выбор: ";
                int fileType;
                cin >> fileType;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                cout << "Введите имя входного файла: "; 
                string inFile; 
                getline(cin, inFile);
                
                cout << "Введите имя выходного файла: "; 
                string outFile; 
                getline(cin, outFile);

                if (action == 2) { //шифрование файла
                    if (fileType == 1) {
                        encryptTextFile(inFile, outFile, key, cipher);
                        cout << "Текстовый файл успешно зашифрован: " << outFile << "\n";
                    } else {
                        encryptBinaryFile(inFile, outFile, key, cipher);
                        cout << "Бинарный файл успешно зашифрован: " << outFile << "\n";
                    }
                } else { //дешифрование файла
                    if (fileType == 1) {
                        decryptTextFile(inFile, outFile, key, cipher);
                        cout << "Текстовый файл успешно расшифрован: " << outFile << "\n";
                    } else {
                        decryptBinaryFile(inFile, outFile, key, cipher);
                        cout << "Бинарный файл успешно расшифрован: " << outFile << "\n";
                    }
                }
            }
        } catch (const exception& e) { 
            cout << "Ошибка: " << e.what() << "\n"; 
        }
    }
    return 0;
}