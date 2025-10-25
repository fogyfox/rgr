#ifndef CIPHER_PERMUTATION_H
#define CIPHER_PERMUTATION_H

#include <string>

std::string encryptPermutationText(const std::string& text, const std::string& key);
std::string decryptPermutationText(const std::string& ciphertext, const std::string& key);

#endif