#include "encrypt_decrypt.h"

// XOR encryption/decryption function
void xor_encrypt_decrypt(const unsigned char *input, unsigned char *output, size_t length, const unsigned char *key) {
    for (size_t i = 0; i < length; ++i) {
        output[i] = input[i] ^ key[i % KEY_SIZE];
    }
}

// Encrypt function
void encrypt(const char *input_filename, const char *output_filename, const unsigned char *key) {
    FILE *ifp = fopen(input_filename, "rb");
    FILE *ofp = fopen(output_filename, "wb");

    if (!ifp || !ofp) {
        perror("File open error");
        exit(EXIT_FAILURE);
    }

    unsigned char buffer[1024];
    unsigned char output[1024];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), ifp)) > 0) {
        xor_encrypt_decrypt(buffer, output, bytes_read, key);
        fwrite(output, 1, bytes_read, ofp);
    }

    fclose(ifp);
    fclose(ofp);
}

// Decrypt function
void decrypt(const char *input_filename, const char *output_filename, const unsigned char *key) {
    // XOR decryption is identical to encryption
    encrypt(input_filename, output_filename, key);
}
