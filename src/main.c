#include "encrypt_decrypt.h"

void generate_key(unsigned char *key) {
    // For simplicity, we use a hardcoded key. In a real application, generate a secure random key.
    const char *hardcoded_key = "simplekey123456"; // Must be 16 bytes for this example
    memcpy(key, hardcoded_key, KEY_SIZE);
}

int main() {
    unsigned char key[KEY_SIZE];

    generate_key(key);

    // Encrypt and decrypt files
    encrypt("plaintext.txt", "encrypted.dat", key);
    decrypt("encrypted.dat", "decrypted.txt", key);

    return 0;
}
