#ifndef CIPHER_GRONSFELD_H
#define CIPHER_GRONSFELD_H

#include <string>

std::string encryptGronsfeld(const std::string& plaintext, const std::string& keyStr, bool useCyrillic = true);
std::string decryptGronsfeld(const std::string& ciphertext, const std::string& keyStr, bool useCyrillic = true);

#endif