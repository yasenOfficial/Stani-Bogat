#ifndef ENCRYPT_DECRYPT_H
#define ENCRYPT_DECRYPT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KEY_SIZE 16

void encrypt(const char *input_filename, const char *output_filename, const unsigned char *key);
void decrypt(const char *input_filename, const char *output_filename, const unsigned char *key);

#endif // ENCRYPT_DECRYPT_H
