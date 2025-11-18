#ifndef CIPHER_PERMUTATION_H
#define CIPHER_PERMUTATION_H

#include <string>
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((visibility("default")))
string encryptPermutationText(const string& text, const string& key);

__attribute__((visibility("default")))
string decryptPermutationText(const string& ciphertext, const string& key);

__attribute__((visibility("default")))
string encryptPermutationBinary(const string& data, const string& key);

__attribute__((visibility("default")))
string decryptPermutationBinary(const string& data, const string& key);

#ifdef __cplusplus
}
#endif

#endif