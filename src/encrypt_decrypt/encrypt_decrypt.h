#ifndef ENCRYPT_DECRYPT_H
#define ENCRYPT_DECRYPT_H

    #include <stdint.h>

    #define KEY_SIZE 16 

    const uint32_t A_LCG = 1664525;
    const uint32_t C_LCG = 1013904223;
    const uint32_t M = 0xFFFFFFFF; // 2^32 

    uint32_t init_state(const unsigned char *key, size_t key_size);
    uint32_t lcg(uint32_t *state);
    uint32_t mcg(uint32_t *state);

    void xor_encrypt_decrypt(const unsigned char *input, unsigned char *output, size_t length, const unsigned char *key);

#endif 
