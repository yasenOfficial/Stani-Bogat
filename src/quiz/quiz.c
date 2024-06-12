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

QuizQuestion *find_question_by_index(int index)
{
    QuizQuestion *current = head;
    int current_index = 1;

    while (current && current_index < index)
    {
        current = current->next;
        current_index++;
    }

    return current_index == index ? current : NULL;
}

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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

    char buffer[256];
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
                // Process and write the new question data
                fprintf(temp_file, "%s\n", newText);
            }
            else if (line_count <= 4) // Options lines
            {
                // Process and write the new options
                fprintf(temp_file, "%s\n", newOptions[line_count - 1]);
            }
            else if (line_count == 5) // Correct answer line
            {
                // Process and write the new correct answer
                fprintf(temp_file, "%d\n", newCorrect_index);
            }
            else if (line_count == 6) // Difficulty line
            {
                // Process and write the new difficulty
                fprintf(temp_file, "%d\n", new_difficulty);

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
        perror("Ne moge da se otvori fail za zapis");
        return;
    }

    QuizQuestion *current = head;
    while (current)
    {
        fprintf(file, "%s\n", current->question_text);
        for (int i = 0; i < 4; i++)
        {
            fprintf(file, "%s\n", current->options[i]);
        }
        fprintf(file, "%d\n", current->correct_option_index);
        fprintf(file, "%d\n", current->difficulty);
        current = current->next;
    }

    fclose(file);
}

void load_questions_from_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Ne moge da se otvori fail za chetene");
        return;
    }

    cleanup_quiz();

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file))
    {
        QuizQuestion *new_question = (QuizQuestion *)malloc(sizeof(QuizQuestion));
        if (!new_question)
        {
            perror("Ne moje da se zaredi vuprosa v pametta");
            fclose(file);
            return;
        }

        buffer[strcspn(buffer, "\n")] = '\0';
        new_question->question_text = strdup(buffer);

        for (int i = 0; i < 4; i++)
        {
            fgets(buffer, sizeof(buffer), file);
            buffer[strcspn(buffer, "\n")] = '\0';
            new_question->options[i] = strdup(buffer);
        }

        fscanf(file, "%d\n", &new_question->correct_option_index);
        fscanf(file, "%d\n", &new_question->difficulty);

        new_question->next = head;
        head = new_question;
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
