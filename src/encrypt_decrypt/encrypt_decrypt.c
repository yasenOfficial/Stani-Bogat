#include "encrypt_decrypt.h"

const uint32_t A_LCG = 1664525;
const uint32_t C_LCG = 1013904223;
const uint32_t M = 0xFFFFFFFF; // 2^32

// защо тези стойности ^^?

// A_LCG -> Tази стойност е избрана, защото е добре познат множител в LCG алгоритмите

// Подобно на A_LCG, чрез емпирични тестове е установено, че C_LCG стойността работи добре при генериране на псевдослучайни числа

// В този случай M е зададено на 2^32, което е максималната стойност за 32-битово цяло число без знак. 
// Генерираните числа покриват целия спектър на 32-битово цяло число без знак.

// const unsigned char key[] = "my_secret_key_16";
// const int KEY_SIZE = sizeof(key) - 1;
// за тестване на с main() функцията

// X_0 се инициализира с ключа на шифъра.
uint32_t init_state(const unsigned char *key, size_t key_size) {
    uint32_t state = 0;
    for (size_t i = 0; i < key_size; ++i) {
        state = (state * 31 + key[i]) % M;
    }
    // Преминава през всеки байт от ключа, актуализирайки състоянието с просто тип hash изчисление (състояние = (състояние * 31 + ключ[i]) % M)
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
    // (Всяко следващо число се генерира от предишното, като се използва формулата X_{n+1} = (A * X_n + C) % M)
}

extern void xor_encrypt_decrypt(const unsigned char *input, unsigned char *output, size_t length, const unsigned char *key) {
    uint32_t encryptState = 0;

    for (int i = 0; i < KEY_SIZE; i++) {
        encryptState = encryptState * 31 + key[i];
    } 
    // Прави буквално почти същото като init_state, може би трябваше да се използва init_state, за да е по-чист кода

    for (size_t i = 0; i < length; ++i) { // Итерира през всеки байт от входните данни
        // На всеки KEY_SIZE байта encryptState състоянието се актуализира с помощта на функцията lcg_pseudo_random. 

        if (i % KEY_SIZE == 0) { 
            encryptState = lcg(&encryptState);
        }
        output[i] = input[i] ^ ((encryptState >> (8 * (i % 4))) & 0xFF);  
        
        // Този ред изпълнява операцията XOR. encryptState състоянието се измества надясно с кратно на 8 бита

        // & 0xFF: Това е побитова операция И с 0xFF, което е еквивалентно на 11111111 в двоичен код. 
        // Тази операция гарантира, че се запазва само (LSB) най-малко значимият байт (8 бита) от изместената стойност.
    }
}

// Testvame koda
// #include <stdio.h>
// #include <stdint.h>
// #include <stdlib.h>
// #include <string.h>

// int main() {



//     const unsigned char input[] = "Hello, world!";

//     unsigned char encrypted[sizeof(input)];
//     unsigned char decrypted[sizeof(input)];

//     xor_encrypt_decrypt(input, encrypted, sizeof(input), key);

//     printf("Encryptiran text: ");
//     for (size_t i = 0; i < sizeof(input); i++) {
//         printf("%02X ", encrypted[i]);
//     }
//     printf("\n");

//     xor_encrypt_decrypt(encrypted, decrypted, sizeof(input), key);

//     printf("Decryptiran text: ");
//     for (size_t i = 0; i < sizeof(input); i++) {
//         printf("%c", decrypted[i]);
//     }
//     printf("\n");

//     return 0;
// }

