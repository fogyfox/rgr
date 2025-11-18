#ifndef CIPHER_VIGENERE_H
#define CIPHER_VIGENERE_H

#include <string>
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

// Экспортируемые функции
__attribute__((visibility("default")))
string encryptVigenere(const string& plaintext, const string& key, bool useCyrillic = true);

__attribute__((visibility("default")))
string decryptVigenere(const string& ciphertext, const string& key, bool useCyrillic = true);

__attribute__((visibility("default")))
string encryptVigenereBinary(const string& data, const string& key);

__attribute__((visibility("default")))
string decryptVigenereBinary(const string& data, const string& key);
#ifdef __cplusplus
}
#endif

#endif