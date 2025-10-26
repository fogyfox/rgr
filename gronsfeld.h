#ifndef CIPHER_GRONSFELD_H
#define CIPHER_GRONSFELD_H

#include <string>

using namespace std;

string encryptGronsfeld(const string& plaintext, const string& keyStr, bool useCyrillic = true);
string decryptGronsfeld(const string& ciphertext, const string& keyStr, bool useCyrillic = true);

#endif