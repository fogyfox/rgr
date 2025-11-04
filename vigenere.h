#ifndef CIPHER_VIGENERE_H
#define CIPHER_VIGENERE_H

#include <string>
using namespace std;


string encryptVigenere(const string& plaintext, const string& key, bool useCyrillic = true);
string decryptVigenere(const string& ciphertext, const string& key, bool useCyrillic = true);

#endif