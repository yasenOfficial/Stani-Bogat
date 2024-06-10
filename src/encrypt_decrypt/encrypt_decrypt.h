#ifndef ENCRYPT_DECRYPT_H
#define ENCRYPT_DECRYPT_H

#include <stdint.h>
#include <stddef.h> 

#define KEY_SIZE 16 

extern const uint32_t A_LCG;
extern const uint32_t C_LCG;
extern const uint32_t M;

uint32_t init_state(const unsigned char *key, size_t key_size);
uint32_t lcg(uint32_t *state);
uint32_t mcg(uint32_t *state);

extern void xor_encrypt_decrypt(const unsigned char *input, unsigned char *output, size_t length, const unsigned char *key);

#endif 
