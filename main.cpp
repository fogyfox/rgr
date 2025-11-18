#include <iostream>
#include <unistd.h> 
#include <string>
#include <fstream>
#include <limits>
#include <vector>
#include <dlfcn.h>

#include "utils.h"

using namespace std;

//действия меню
enum class MenuAction {
    EXIT = 0,
    ENCRYPT_TEXT = 1,
    ENCRYPT_FILE = 2,
    DECRYPT_FILE = 3
};

//выборы шифра
enum class CipherMethod {
    PERMUTATION = 1,
    VIGENERE = 2,
    GRONSFELD = 3
};

//типы файла
enum class FileType {
    TEXT = 1,
    BINARY = 2
};

//структура для хранения функций шифрования
struct CipherFunctions {
    string (*encryptText)(const string&, const string&, bool);
    string (*decryptText)(const string&, const string&, bool);
    string (*encryptBinary)(const string&, const string&);
    string (*decryptBinary)(const string&, const string&);
    void* libraryHandle;
    
    CipherFunctions() : encryptText(nullptr), decryptText(nullptr), encryptBinary(nullptr), decryptBinary(nullptr), libraryHandle(nullptr) {}
};

//функция для загрузки библиотеки
CipherFunctions loadCipherLibrary(CipherMethod method) {
    CipherFunctions funcs;
    const char* libraryName = nullptr;
    
    switch (method) {
        case CipherMethod::PERMUTATION:
            libraryName = "./libpermutation.so";
            break;
        case CipherMethod::VIGENERE:
            libraryName = "./libvigenere.so";
            break;
        case CipherMethod::GRONSFELD:
            libraryName = "./libgronsfeld.so";
            break;
        default:
            cout << "Неизвестный метод шифрования" << endl;
            return funcs;
    }
    
    if (!libraryName) {
        return funcs;
    }
    
    void* handle = dlopen(libraryName, RTLD_LAZY);
    if (!handle) {
        cout << "Ошибка загрузки библиотеки " << libraryName << ": " << dlerror() << endl;
        cout << "Текущая директория: " << get_current_dir_name() << endl;
        return funcs;
    }
    
    funcs.libraryHandle = handle;
    
    //загружаем функции
    switch (method) {
        case CipherMethod::PERMUTATION:
            funcs.encryptText = reinterpret_cast<string(*)(const string&, const string&, bool)>(dlsym(handle, "encryptPermutationText"));
            funcs.decryptText = reinterpret_cast<string(*)(const string&, const string&, bool)>(dlsym(handle, "decryptPermutationText"));
            funcs.encryptBinary = reinterpret_cast<string(*)(const string&, const string&)>(dlsym(handle, "encryptPermutationBinary"));
            funcs.decryptBinary = reinterpret_cast<string(*)(const string&, const string&)>(dlsym(handle, "decryptPermutationBinary"));
            break;
        case CipherMethod::VIGENERE:
            funcs.encryptText = reinterpret_cast<string(*)(const string&, const string&, bool)>(dlsym(handle, "encryptVigenere"));
            funcs.decryptText = reinterpret_cast<string(*)(const string&, const string&, bool)>(dlsym(handle, "decryptVigenere"));
            funcs.encryptBinary = reinterpret_cast<string(*)(const string&, const string&)>(dlsym(handle, "encryptVigenereBinary"));
            funcs.decryptBinary = reinterpret_cast<string(*)(const string&, const string&)>(dlsym(handle, "decryptVigenereBinary"));
            break;
        case CipherMethod::GRONSFELD:
            funcs.encryptText = reinterpret_cast<string(*)(const string&, const string&, bool)>(dlsym(handle, "encryptGronsfeld"));
            funcs.decryptText = reinterpret_cast<string(*)(const string&, const string&, bool)>(dlsym(handle, "decryptGronsfeld"));
            funcs.encryptBinary = reinterpret_cast<string(*)(const string&, const string&)>(dlsym(handle, "encryptGronsfeldBinary"));
            funcs.decryptBinary = reinterpret_cast<string(*)(const string&, const string&)>(dlsym(handle, "decryptGronsfeldBinary"));
            break;
    }
    
    //проверяем ошибки загрузки функций
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        cout << "Ошибка загрузки функций из библиотеки " << libraryName << ": " << dlsym_error << endl;
        dlclose(handle);
        funcs = CipherFunctions();
    }
    
    return funcs;
}

//функция для выгрузки библиотеки
void unloadCipherLibrary(CipherFunctions& funcs) {
    if (funcs.libraryHandle) {
        dlclose(funcs.libraryHandle);
        funcs.libraryHandle = nullptr;
    }
    funcs.encryptText = nullptr;
    funcs.decryptText = nullptr;
    funcs.encryptBinary = nullptr;
    funcs.decryptBinary = nullptr;
}

//функции для работы с текстовыми файлами
void encryptTextFile(const string& inputFile, const string& outputFile, const string& key, 
                    CipherFunctions& cipherFuncs) {
    ifstream in(inputFile);
    ofstream out(outputFile);
    if (!in) throw runtime_error("Не удалось открыть файл " + inputFile);
    if (!out) throw runtime_error("Не удалось создать файл " + outputFile);

    if (!cipherFuncs.encryptText) {
        throw runtime_error("Шифр недоступен");
    }

    string text;
    string line;
    while (getline(in, line)) {
        text += line + "\n";
    }
    if (!text.empty()) text.pop_back();

    string result = cipherFuncs.encryptText(text, key, true);
    out << result;
}

void decryptTextFile(const string& inputFile, const string& outputFile, const string& key, 
                    CipherFunctions& cipherFuncs) {
    ifstream in(inputFile);
    ofstream out(outputFile);
    if (!in) throw runtime_error("Не удалось открыть файл " + inputFile);
    if (!out) throw runtime_error("Не удалось создать файл " + outputFile);

    if (!cipherFuncs.decryptText) {
        throw runtime_error("Шифр недоступен");
    }

    string text;
    string line;
    while (getline(in, line)) {
        text += line + "\n";
    }
    if (!text.empty()) text.pop_back();

    string result = cipherFuncs.decryptText(text, key, true);
    out << result;
}

//для бинарных файлов
void encryptBinaryFile(const string& inputFile, const string& outputFile, const string& key, CipherFunctions& cipherFuncs) {
    ifstream in(inputFile, ios::binary);
    ofstream out(outputFile, ios::binary);
    if (!in) throw runtime_error("Не удалось открыть файл " + inputFile);
    if (!out) throw runtime_error("Не удалось создать файл " + outputFile);
    if (key.empty()) throw runtime_error("Ключ не должен быть пустым");

    //если есть специальная бинарная функция, используем её
    if (cipherFuncs.encryptBinary) {
        string content((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
        string result = cipherFuncs.encryptBinary(content, key);
        out.write(result.data(), result.size());
    }
    //для Виженера и Гронсфельда используем специальную бинарную обработку
    else if (cipherFuncs.encryptText) {
        string content((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
        
        //для бинарных данных отключаем кириллицу и используем простое сложение по модулю 256
        string result;
        if (cipherFuncs.encryptText) {
            //создаем простой шифр для бинарных данных
            size_t keyLen = key.size();
            for (size_t i = 0; i < content.length(); ++i) {
                unsigned char b = static_cast<unsigned char>(content[i]);
                unsigned char k = static_cast<unsigned char>(key[i % keyLen]);
                result += static_cast<char>((b + k) % 256);
            }
        }
        out.write(result.data(), result.size());
    }
    else {
        throw runtime_error("Шифр недоступен для бинарных данных");
    }
}

void decryptBinaryFile(const string& inputFile, const string& outputFile, const string& key, CipherFunctions& cipherFuncs) {
    ifstream in(inputFile, ios::binary);
    ofstream out(outputFile, ios::binary);
    if (!in) throw runtime_error("Не удалось открыть файл " + inputFile);
    if (!out) throw runtime_error("Не удалось создать файл " + outputFile);
    if (key.empty()) throw runtime_error("Ключ не должен быть пустым");

    //если есть специальная бинарная функция, используем её
    if (cipherFuncs.decryptBinary) {
        string content((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
        string result = cipherFuncs.decryptBinary(content, key);
        out.write(result.data(), result.size());
    }
    //для Виженера и Гронсфельда используем специальную бинарную обработку
    else if (cipherFuncs.decryptText) {
        string content((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
        
        //для бинарных данных отключаем кириллицу и используем простое вычитание по модулю 256
        string result;
        if (cipherFuncs.decryptText) {
            size_t keyLen = key.size();
            for (size_t i = 0; i < content.length(); ++i) {
                unsigned char b = static_cast<unsigned char>(content[i]);
                unsigned char k = static_cast<unsigned char>(key[i % keyLen]);
                result += static_cast<char>((b - k + 256) % 256);
            }
        }
        out.write(result.data(), result.size());
    }
    else {
        throw runtime_error("Шифр недоступен для бинарных данных");
    }
}

//шифрование текста
string encryptText(const string& text, const string& key, CipherFunctions& cipherFuncs) {
    if (!cipherFuncs.encryptText) {
        throw runtime_error("Шифр недоступен");
    }
    return cipherFuncs.encryptText(text, key, true);
}

//сохранение текста в файл
void saveTextToFile(const string& text, const string& filename) {
    ofstream file(filename);
    if (!file) throw runtime_error("Не удалось создать файл " + filename);
    file << text;
    cout << "Текст успешно сохранён в файл: " << filename << endl;
}

int main() {
    cout << "=== КРИПТОГРАФИЧЕСКАЯ СИСТЕМА ===" << endl;
    cout << "Проверка компонентов..." << endl;
    
    while(true) {
        cout << "\n=== КРИПТОГРАФИЧЕСКАЯ СИСТЕМА ===" << endl;
        cout << "0 - Выход\n";
        cout << "1 - Шифрование текста" << endl;
        cout << "2 - Шифрование файла" << endl;
        cout << "3 - Дешифрование файла" << endl;
        
        // Цикл для проверки выбора действия
        int actionInput;
        while (true) {
            cout << "Выберите действие: ";
            cin >> actionInput;
            
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Неверный ввод" << endl;
                continue;
            }
            
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            if (actionInput >= 0 && actionInput <= 3) {
                break;
            } else {
                cout << "Неверный выбор" << endl;
            }
        }
        
        MenuAction action = static_cast<MenuAction>(actionInput);
        
        if (action == MenuAction::EXIT) break;

        //цикл для проверки выбора шифра
        cout << "Выберите шифр:\n";
        cout << "1 - Табличная перестановка с ключевым словом" << endl;
        cout << "2 - Шифр Виженера" << endl;
        cout << "3 - Шифр Гронсфельда" << endl;
        
        int cipherInput;
        while (true) {
            cout << "Выбор: ";
            cin >> cipherInput;
            
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Неверный ввод" << endl;
                continue;
            }
            
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            if (cipherInput >= 1 && cipherInput <= 3) {
                break;
            } else {
                cout << "Неверный выбор шифра" << endl;
            }
        }
        
        CipherMethod cipher = static_cast<CipherMethod>(cipherInput);

        //загружаем библиотеку выбранного шифра
        CipherFunctions cipherFuncs = loadCipherLibrary(cipher);
        
        if (!cipherFuncs.encryptText || !cipherFuncs.decryptText) {
            cout << "ОШИБКА: Данный шифр недоступен. Библиотека не найдена или повреждена." << endl;;
            cout << "Убедитесь, что файл библиотеки присутствует в текущей директории." << endl;
            continue;
        }

        //ввод ключа
        cout << "Введите ключ: "; 
        string key; 
        getline(cin, key);

        try {
            if (action == MenuAction::ENCRYPT_TEXT) {
                cout << "Введите текст: "; 
                string text; 
                getline(cin, text);
                
                string result = encryptText(text, key, cipherFuncs);
                cout << "Зашифрованный текст:\n" << result << endl;
                
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
                } else {
                    cout << "Результат не сохранён..." << endl;
                }
            } else {
                //работа с файлами
                cout << "Выберите тип файла:" << endl;;
                cout << "1 - Текстовый файл" << endl;;
                cout << "2 - Бинарный файл" << endl;;
                
                //цикл для проверки выбора типа файла
                int fileTypeInput;
                while (true) {
                    cout << "Выбор: ";
                    cin >> fileTypeInput;
                    
                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Неверный ввод." << endl;
                        continue;
                    }
                    
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    
                    if (fileTypeInput == 1 || fileTypeInput == 2) {
                        break;
                    } else {
                        cout << "Неверный выбор типа файла." << endl;
                    }
                }
                
                FileType fileType = static_cast<FileType>(fileTypeInput);
                
                cout << "Введите имя входного файла: "; 
                string inFile; 
                getline(cin, inFile);
                
                cout << "Введите имя выходного файла: "; 
                string outFile; 
                getline(cin, outFile);

                if (action == MenuAction::ENCRYPT_FILE) {
                    if (fileType == FileType::TEXT) {
                        encryptTextFile(inFile, outFile, key, cipherFuncs);
                        cout << "Текстовый файл успешно зашифрован: " << outFile << endl;;
                    } else {
                        encryptBinaryFile(inFile, outFile, key, cipherFuncs);
                        cout << "Бинарный файл успешно зашифрован: " << outFile << endl;;
                    }
                } else {
                    if (fileType == FileType::TEXT) {
                        decryptTextFile(inFile, outFile, key, cipherFuncs);
                        cout << "Текстовый файл успешно расшифрован: " << outFile << endl;;
                    } else {
                        decryptBinaryFile(inFile, outFile, key, cipherFuncs);
                        cout << "Бинарный файл успешно расшифрован: " << outFile << endl;;
                    }
                }
            }
        } catch (const exception& e) { 
            cout << "Ошибка: " << e.what() << endl; 
        }
        
        //выгружаем библиотеку после использования
        unloadCipherLibrary(cipherFuncs);
    }
    
    cout << "Выход из программы." << endl;;
    return 0;
}