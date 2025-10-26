#ifndef CIPHER_PERMUTATION_H
#define CIPHER_PERMUTATION_H

#include <string>

using namespace std;

string encryptPermutationText(const string& text, const string& key);
string decryptPermutationText(const string& ciphertext, const string& key);

#endif