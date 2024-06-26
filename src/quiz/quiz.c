#include "quiz.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../encrypt_decrypt/encrypt_decrypt.h"

static QuizQuestion *head = NULL;

extern const int KEY_SIZE;
extern unsigned char encryption_key[];

unsigned char *debug_encrypt(const char *text, const unsigned char *encryption_key, const char *mode)
{
    size_t length = strlen(text);
    unsigned char *result = (unsigned char *)malloc(length + 1); // Allocate memory za  text + '\0'
    if (!result)
    {
        perror("Memory allocation failed");
        return NULL;
    }

    // Debug print: Input text and length
    // printf("Input text: %s\n", text);
    // printf("Text length: %zu\n", length);

    if (strcmp(mode, "encryption") == 0)
    {
        unsigned char *encrypted = (unsigned char *)malloc(length);
        if (!encrypted)
        {
            perror("Memory allocation failed");
            free(result);
            return NULL;
        }

        xor_encrypt_decrypt((const unsigned char *)text, encrypted, length, encryption_key);

        // Debug print: Encrypted data in hex
        // printf("Encrypted data (hex): ");
        // for (size_t i = 0; i < length; ++i)
        // {
        //     printf("%02X ", encrypted[i]);
        // }
        // printf("\n");

        memcpy(result, encrypted, length);
        result[length] = '\0';

        free(encrypted);
    }
    else if (strcmp(mode, "decryption") == 0)
    {
        unsigned char *decryption = (unsigned char *)malloc(length);
        if (!decryption)
        {
            perror("Memory allocation failed");
            free(result);
            return NULL;
        }

        xor_encrypt_decrypt((const unsigned char *)text, decryption, length, encryption_key);


        // Debug print: Decrypted data in hex
        // printf("Decrypted data (hex): ");
        // for (size_t i = 0; i < length; ++i)
        // {
        //     printf("%02X ", decrypted[i]);
        // }
        // printf("\n");

        memcpy(result, decryption, length);
        result[length] = '\0';

        free(decryption);
    }
    else
    {
        printf("Invalid mode\n");
        free(result);
        return NULL;
    }

    // Debug print: Final result in hex
    // printf("Final result (hex): ");
    // for (size_t i = 0; i < length; ++i)
    // {
    //     printf("%02X ", result[i]);
    // }
    // printf("\n");

    return result;
}

void initialize_quiz()
{
    head = NULL;
}

void add_question_to_file(const char *filename, char *text, uint8_t difficulty, char **options, uint8_t correct_index)
{
    QuizQuestion *new_question = (QuizQuestion *)malloc(sizeof(QuizQuestion));
    if (!new_question)
    {
        perror("Ne moje da se zaredi vuprosa v pametta");
        return;
    }

    new_question->question_text = strdup(text);
    if (!new_question->question_text)
    {
        perror("Memory allocation failed");
        free(new_question);
        return;
    }

    for (int i = 0; i < 4; i++)
    {
        new_question->options[i] = strdup(options[i]);
        if (!new_question->options[i])
        {
            perror("Memory allocation failed");
            free(new_question->question_text);
            free(new_question);
            return;
        }
    }

    new_question->correct_option_index = correct_index;
    new_question->difficulty = difficulty;
    new_question->next = head;
    head = new_question;

    FILE *file = fopen(filename, "a");
    if (!file)
    {
        perror("Ne moge da se otvori fail za zapis");
        return;
    }

    unsigned char *encrypted_text = debug_encrypt(text, encryption_key, "encryption");
    if (!encrypted_text)
    {
        fclose(file);
        return;
    }
    fprintf(file, "%s\n", encrypted_text);
    free(encrypted_text);

    for (int i = 0; i < 4; i++)
    {
        unsigned char *encrypted_option = debug_encrypt(options[i], encryption_key, "encryption");
        if (!encrypted_option)
        {
            fclose(file);
            return;
        }
        fprintf(file, "%s\n", encrypted_option);
        free(encrypted_option); // Free memory allocated for encrypted_option
    }

    unsigned char encrypted_correct_index[sizeof(correct_index)];
    xor_encrypt_decrypt((const unsigned char *)&correct_index, encrypted_correct_index, sizeof(correct_index), encryption_key);
    // printf("Correct index: %d\n", correct_index);
    // printf("Encryptiran text: ");
    // for (size_t i = 0; i < sizeof(correct_index); i++)
    // {
    //     printf("%02X ", encrypted_correct_index[i]);
    // }
    // printf("\n");

    fprintf(file, "%s\n", encrypted_correct_index);
    free(encrypted_correct_index);

    unsigned char encrypted_difficulty[sizeof(difficulty)];
    xor_encrypt_decrypt((const unsigned char *)&difficulty, encrypted_difficulty, sizeof(difficulty), encryption_key);

    fprintf(file, "%s\n", encrypted_difficulty);
    free(encrypted_difficulty);

    fclose(file);
}

void edit_question_in_file(const char *filename, int question_number, const char *newText, uint8_t new_difficulty, const char *newOptions[], uint8_t newCorrect_index)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Ne moge da se otvori fail za chetene");
        return;
    }

    FILE *temp_file = fopen("temp.txt", "w");
    if (!temp_file)
    {
        perror("Ne moge da se otvori vremenniqt fail za pisane");
        fclose(file);
        return;
    }

    char buffer[255];
    int line_count = 0;
    int current_question = 1;
    bool editing_question = false; // Flag to indicate if currently editing the specified question

    while (fgets(buffer, sizeof(buffer), file))
    {
        if (current_question == question_number) // If this is the question to edit
        {
            if (line_count == 0) // First line of the question
            {
                editing_question = true;
                // Encrypt the new question text
                unsigned char *encrypted_text = debug_encrypt(newText, encryption_key, "encryption");
                if (!encrypted_text)
                {
                    fclose(file);
                    fclose(temp_file);
                    return;
                }
                fprintf(temp_file, "%s\n", encrypted_text);
                free(encrypted_text);
            }
            else if (line_count <= 4) // Options lines
            {
                // Encrypt and write the new options
                unsigned char *encrypted_option = debug_encrypt(newOptions[line_count - 1], encryption_key, "encryption");
                if (!encrypted_option)
                {
                    fclose(file);
                    fclose(temp_file);
                    return;
                }
                fprintf(temp_file, "%s\n", encrypted_option);
                free(encrypted_option);
            }
            else if (line_count == 5) // Correct answer line
            {
                // Encrypt and write the new correct answer
                unsigned char encrypted_correct_index[sizeof(newCorrect_index)];
                xor_encrypt_decrypt((const unsigned char *)&newCorrect_index, encrypted_correct_index, sizeof(newCorrect_index), encryption_key);
                fwrite(encrypted_correct_index, sizeof(unsigned char), sizeof(newCorrect_index), temp_file);
                fwrite("\n", sizeof(char), 1, temp_file); // Write newline
            }
            else if (line_count == 6) // Difficulty line
            {
                // Write new difficulty to the file
                unsigned char encrypted_difficulty[sizeof(new_difficulty)];
                xor_encrypt_decrypt((const unsigned char *)&new_difficulty, encrypted_difficulty, sizeof(new_difficulty), encryption_key);
                fwrite(encrypted_difficulty, sizeof(unsigned char), sizeof(new_difficulty), temp_file);
                fwrite("\n", sizeof(char), 1, temp_file); // Write newline

                editing_question = false; // Finished editing this question
            }
        }
        else
        {
            // Copy non-question lines as they are
            fprintf(temp_file, "%s", buffer);
        }

        line_count++;
        if (line_count == 7)
        {
            line_count = 0;
            current_question++;
        }
    }

    fclose(file);
    fclose(temp_file);

    remove(filename);
    rename("temp.txt", filename);
}

void cleanup_quiz()
{
    QuizQuestion *current = head;
    while (current)
    {
        QuizQuestion *next = current->next;
        free(current->question_text);
        for (int i = 0; i < 4; i++)
        {
            free(current->options[i]);
        }
        free(current);
        current = next;
    }
    head = NULL;
}

void save_questions_to_file(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        perror("Failed to open file for writing");
        return;
    }

    QuizQuestion *current = head;
    while (current)
    {
        unsigned char *encrypted_text = debug_encrypt(current->question_text, encryption_key, "encryption");
        fprintf(file, "%s\n", encrypted_text);
        free(encrypted_text);

        for (int i = 0; i < 4; i++)
        {
            unsigned char *encrypted_option = debug_encrypt(current->options[i], encryption_key, "encryption");
            fprintf(file, "%s\n", encrypted_option);
            free(encrypted_option);
        }

        unsigned char encrypted_correct_index[sizeof(uint8_t)];
        xor_encrypt_decrypt((const unsigned char *)&current->correct_option_index, encrypted_correct_index, sizeof(uint8_t), encryption_key);
        fprintf(file, "%02X\n", encrypted_correct_index[0]);

        unsigned char encrypted_difficulty[sizeof(uint8_t)];
        xor_encrypt_decrypt((const unsigned char *)&current->difficulty, encrypted_difficulty, sizeof(uint8_t), encryption_key);
        fprintf(file, "%02X\n", encrypted_difficulty[0]);

        current = current->next;
    }

    fclose(file);
}

void load_questions(const char *filename, char options[4][100], char *text, int *correct_option, int *difficulty, bool *found, int desired_difficulty)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Ne moge da se otvori fail za chetene");
        return;
    }

    char buffer[256];
    int questions_with_desired_difficulty = 0;

    // Count questions with the desired difficulty
    while (fgets(buffer, sizeof(buffer), file))
    {
        size_t length = strlen(buffer);
        buffer[length - 1] = '\0';
        unsigned char *decrypted = debug_encrypt(buffer, encryption_key, "decryption");
        strcpy(text, decrypted);

        for (int i = 0; i < 4; i++)
        {
            fgets(buffer, sizeof(buffer), file);
            size_t length = strlen(buffer);
            buffer[length - 1] = '\0';
            unsigned char *decrypted_option = debug_encrypt(buffer, encryption_key, "decryption");
            strcpy(options[i], (char *)decrypted_option);
            free(decrypted_option);
        }

        fgets(buffer, sizeof(buffer), file);
        unsigned char correct_index[sizeof(uint8_t)];
        sscanf(buffer, "%s", correct_index);
        xor_encrypt_decrypt(correct_index, (unsigned char *)&correct_index, sizeof(uint8_t), encryption_key);
        *correct_option = (int)correct_index[0];

        fgets(buffer, sizeof(buffer), file);
        unsigned char diff[sizeof(uint8_t)];
        sscanf(buffer, "%s", diff);
        xor_encrypt_decrypt(diff, (unsigned char *)&diff, sizeof(uint8_t), encryption_key);
        *difficulty = (int)diff[0];

        if (*difficulty == desired_difficulty)
        {
            questions_with_desired_difficulty++;
        }
    }

    // Check if there are questions with desired difficulty
    if (questions_with_desired_difficulty == 0)
    {
        fclose(file);
        *found = false;
        return;
    }

    // Generate a random number to select a question
    srand(time(NULL));
    int random_question = rand() % questions_with_desired_difficulty;

    // Reset file pointer to the beginning of the file
    fseek(file, 0, SEEK_SET);

    // Read questions again and find the randomly selected one
    int count = 0;
    while (fgets(buffer, sizeof(buffer), file))
    {
        size_t length = strlen(buffer);
        buffer[length - 1] = '\0';
        unsigned char *decrypted = debug_encrypt(buffer, encryption_key, "decryption");
        strcpy(text, decrypted);

        for (int i = 0; i < 4; i++)
        {
            fgets(buffer, sizeof(buffer), file);
            size_t length = strlen(buffer);
            buffer[length - 1] = '\0';
            unsigned char *decrypted_option = debug_encrypt(buffer, encryption_key, "decryption");
            strcpy(options[i], (char *)decrypted_option);
            free(decrypted_option);
        }

        fgets(buffer, sizeof(buffer), file);
        unsigned char correct_index[sizeof(uint8_t)];
        sscanf(buffer, "%s", correct_index);
        xor_encrypt_decrypt(correct_index, (unsigned char *)&correct_index, sizeof(uint8_t), encryption_key);
        *correct_option = (int)correct_index[0];

        fgets(buffer, sizeof(buffer), file);
        unsigned char diff[sizeof(uint8_t)];
        sscanf(buffer, "%s", diff);
        xor_encrypt_decrypt(diff, (unsigned char *)&diff, sizeof(uint8_t), encryption_key);
        *difficulty = (int)diff[0];

        if (*difficulty == desired_difficulty && count == random_question)
        {
            *found = true;
            break;
        }
        else if (*difficulty == desired_difficulty)
        {
            count++;
        }
    }

    fclose(file);
}

void print_questions(const char *filename, bool print_answers, bool print_difficulty)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Ne moge da se otvori fail za chetene");
        return;
    }

    char buffer[256];
    int index = 1;
    while (fgets(buffer, sizeof(buffer), file))
    {

        size_t length = strlen(buffer); // Length of the decrypted string
        buffer[length - 1] = '\0';      // Remove newline character
        unsigned char *decrypted = debug_encrypt(buffer, encryption_key, "decryption");
        printf("Vupros %d: %s\n", index++, decrypted);

        for (int i = 0; i < 4; i++)
        {
            fgets(buffer, sizeof(buffer), file);
            size_t length = strlen(buffer); // Length of the decrypted string
            buffer[length - 1] = '\0';      // Remove newline character
            unsigned char *decrypted = debug_encrypt(buffer, encryption_key, "decryption");
            printf("  Otgovor %d: %s\n", i + 1, decrypted);
            free(decrypted); // Free decrypted buffer
        }

        fgets(buffer, sizeof(buffer), file);
        unsigned char correct_index[sizeof(uint8_t)];
        sscanf(buffer, "%s", correct_index);
        xor_encrypt_decrypt(correct_index, (unsigned char *)&correct_index, sizeof(uint8_t), encryption_key);
        if (print_answers)
        {
            printf("  Pravilen otgovor: %d\n", correct_index[0]);
        }

        if (print_difficulty)
        {
            fgets(buffer, sizeof(buffer), file);
            unsigned char difficulty[sizeof(uint8_t)];
            sscanf(buffer, "%s", difficulty);
            xor_encrypt_decrypt(difficulty, (unsigned char *)&difficulty, sizeof(uint8_t), encryption_key);
            printf("  Trudnost: %d\n", difficulty[0]);
        }
    }

    fclose(file);
}

void load_random_question(const char *filename, char options[4][100], char *text, int *correct_option, int *difficulty)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Ne moge da se otvori fail za chetene");
        return;
    }

    char buffer[256];
    int count = 0;

    // Count questions with the desired difficulty
    while (fgets(buffer, sizeof(buffer), file))
    {
        size_t length = strlen(buffer);
        buffer[length - 1] = '\0';
        unsigned char *decrypted = debug_encrypt(buffer, encryption_key, "decryption");
        strcpy(text, decrypted);

        for (int i = 0; i < 4; i++)
        {
            fgets(buffer, sizeof(buffer), file);
            size_t length = strlen(buffer);
            buffer[length - 1] = '\0';
            unsigned char *decrypted_option = debug_encrypt(buffer, encryption_key, "decryption");
            strcpy(options[i], (char *)decrypted_option);
            free(decrypted_option);
        }

        fgets(buffer, sizeof(buffer), file);
        unsigned char correct_index[sizeof(uint8_t)];
        sscanf(buffer, "%s", correct_index);
        xor_encrypt_decrypt(correct_index, (unsigned char *)&correct_index, sizeof(uint8_t), encryption_key);
        *correct_option = (int)correct_index[0];

        fgets(buffer, sizeof(buffer), file);
        unsigned char diff[sizeof(uint8_t)];
        sscanf(buffer, "%s", diff);
        xor_encrypt_decrypt(diff, (unsigned char *)&diff, sizeof(uint8_t), encryption_key);
        *difficulty = (int)diff[0];

        count++;
    }

    srand(time(NULL));
    int random_question = rand() % count;

    // Reset file pointer to the beginning of the file
    fseek(file, 0, SEEK_SET);

    // Read questions again and find the randomly selected one
    count = 0;
    while (fgets(buffer, sizeof(buffer), file))
    {
        size_t length = strlen(buffer);
        buffer[length - 1] = '\0';
        unsigned char *decrypted = debug_encrypt(buffer, encryption_key, "decryption");
        strcpy(text, decrypted);

        for (int i = 0; i < 4; i++)
        {
            fgets(buffer, sizeof(buffer), file);
            size_t length = strlen(buffer);
            buffer[length - 1] = '\0';
            unsigned char *decrypted_option = debug_encrypt(buffer, encryption_key, "decryption");
            strcpy(options[i], (char *)decrypted_option);
            free(decrypted_option);
        }

        fgets(buffer, sizeof(buffer), file);
        unsigned char correct_index[sizeof(uint8_t)];
        sscanf(buffer, "%s", correct_index);
        xor_encrypt_decrypt(correct_index, (unsigned char *)&correct_index, sizeof(uint8_t), encryption_key);
        *correct_option = (int)correct_index[0];

        fgets(buffer, sizeof(buffer), file);
        unsigned char diff[sizeof(uint8_t)];
        sscanf(buffer, "%s", diff);
        xor_encrypt_decrypt(diff, (unsigned char *)&diff, sizeof(uint8_t), encryption_key);
        *difficulty = (int)diff[0];

        if (count == random_question)
        {
            break;
        }
        else
        {
            count++;
        }
    }

    fclose(file);
}