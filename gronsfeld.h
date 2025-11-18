#ifndef CIPHER_GRONSFELD_H
#define CIPHER_GRONSFELD_H

#include <string>
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((visibility("default")))
string encryptGronsfeld(const string& plaintext, const string& keyStr, bool useCyrillic = true);

__attribute__((visibility("default")))
string decryptGronsfeld(const string& ciphertext, const string& keyStr, bool useCyrillic = true);

__attribute__((visibility("default")))
string encryptGronsfeldBinary(const string& data, const string& key);

__attribute__((visibility("default")))
string decryptGronsfeldBinary(const string& data, const string& key);

#ifdef __cplusplus
}
#endif

#endif