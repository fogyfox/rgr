#ifndef CIPHER_VIGENERE_H
#define CIPHER_VIGENERE_H

#include <string>

std::string encryptVigenere(const std::string& plaintext, const std::string& key, bool useCyrillic = true);
std::string decryptVigenere(const std::string& ciphertext, const std::string& key, bool useCyrillic = true);

#endif