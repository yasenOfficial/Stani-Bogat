#include "encrypt_decrypt.h"

const uint32_t A_LCG = 1664525;
const uint32_t C_LCG = 1013904223;
const uint32_t M = 0xFFFFFFFF; // 2^32

// X_0 се инициализира с ключа на шифъра.
uint32_t init_state(const unsigned char *key, size_t key_size) {
    uint32_t state = 0;
    for (size_t i = 0; i < key_size; ++i) {
        state = (state * 31 + key[i]) % M;
    }
    return state;
}

// Линейен Конгруентен Генератор (LCG)
// https://www.geeksforgeeks.org/linear-congruence-method-for-generating-pseudo-random-numbers/
uint32_t lcg(uint32_t *state) {
    // X_{n+1} = (A * X_n + C) % M

    //където:
    //X_{n+1} е следващото генерирано число,
    //X_n е текущото генерирано число,
    //A е множителя,
    //C е константата на добавяне,
    //M е модулът.

    *state = (A_LCG * (*state) + C_LCG) % M;
    return *state;

    // X_0 се инициализира с ключа на шифъра.
    // Следващото число се генерира, като се умножи текущото с A, се добави константата C и се вземе остатъкът при деление на M.
}

// Мултипликативен Конгруентен Генератор (MCG)
// https://www.geeksforgeeks.org/multiplicative-congruence-method-for-generating-pseudo-random-numbers/
uint32_t mcg(uint32_t *state) {
    // X_{n+1} = (A * X_n) % M

    //където променливите са същи като при LCG.

    *state = (A_LCG * (*state)) % M;
    return *state;

    // X_0 се инициализира с ключа на шифъра.
    // Следващото число се генерира, като се умножи текущото с A, се добави константата C и се вземе остатъкът при деление на M.
}

extern void xor_encrypt_decrypt(const unsigned char *input, unsigned char *output, size_t length, const unsigned char *key) {
    uint32_t encryptState = 0;

    for (int i = 0; i < KEY_SIZE; i++) {
        encryptState = encryptState * 31 + key[i];
    }
    
    //Този цикъл хешира ключа за шифроване, за да инициализира encryptState състоянието.
    //Всеки байт от ключа се използва за модифициране на състоянието, като се гарантира, 
    //че различните ключове водят до различни начални състояния за encryptState. 
    //(31 e произволно число, което се използва за хеширане на ключа)
    

    for (size_t i = 0; i < length; ++i) { // Итерира през всеки байт от входните данни
        // На всеки KEY_SIZE байта encryptState състоянието се актуализира с помощта на функцията lcg_pseudo_random. 
        // Това означава, че на всеки KEY_SIZE байта се генерира нова случайна стойност, която влияе на следващия сегмент от ключа

        if (i % KEY_SIZE == 0) { 
            encryptState = lcg(&encryptState);
        }
        output[i] = input[i] ^ ((encryptState >> (8 * (i % 4))) & 0xFF);  
        
        // Този ред изпълнява операцията XOR. encryptState състоянието се измества надясно с кратно на 8 бита (контролирано от i % 4) 
        // Това помага за по-равномерното разпространение на влиянието на PRNG състоянието в данните.

        // & 0xFF: Това е побитова операция И с 0xFF, което е еквивалентно на 11111111 в двоичен код. 
        // Тази операция гарантира, че се запазва само най-малко значимият байт (8 бита) от изместената стойност.
    }
}

/* Testvame koda
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const unsigned char key[KEY_SIZE] = "my_secret_key_16";

    const unsigned char input[] = "Hello, world!";

    unsigned char encrypted[sizeof(input)];
    unsigned char decrypted[sizeof(input)];

    xor_encrypt_decrypt(input, encrypted, sizeof(input), key);

    printf("Encryptiran text: ");
    for (size_t i = 0; i < sizeof(input); i++) {
        printf("%02X ", encrypted[i]);
    }
    printf("\n");

    xor_encrypt_decrypt(encrypted, decrypted, sizeof(input), key);

    printf("Decryptiran text: ");
    for (size_t i = 0; i < sizeof(input); i++) {
        printf("%c", decrypted[i]);
    }
    printf("\n");

    return 0;
}

*/