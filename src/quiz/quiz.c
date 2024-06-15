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

void load_questions_from_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open file for reading");
        return;
    }

    cleanup_quiz();

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file))
    {
        QuizQuestion *new_question = (QuizQuestion *)malloc(sizeof(QuizQuestion));
        if (!new_question)
        {
            perror("Memory allocation failed");
            fclose(file);
            return;
        }

        buffer[strcspn(buffer, "\n")] = '\0';
        unsigned char *decrypted_text = debug_encrypt(buffer, encryption_key, "decryption");
        new_question->question_text = strdup((char *)decrypted_text);
        free(decrypted_text);

        for (int i = 0; i < 4; i++)
        {
            if (!fgets(buffer, sizeof(buffer), file)) {
                perror("Unexpected end of file while reading options");
                free(new_question);
                fclose(file);
                return;
            }
            buffer[strcspn(buffer, "\n")] = '\0';
            unsigned char *decrypted_option = debug_encrypt(buffer, encryption_key, "decryption");
            new_question->options[i] = strdup((char *)decrypted_option);
            free(decrypted_option);
        }

        if (!fgets(buffer, sizeof(buffer), file)) {
            perror("Unexpected end of file while reading correct option index");
            free(new_question);
            fclose(file);
            return;
        }
        buffer[strcspn(buffer, "\n")] = '\0';
        unsigned char decrypted_correct_index[sizeof(uint8_t)];
        xor_encrypt_decrypt((const unsigned char *)buffer, decrypted_correct_index, sizeof(uint8_t), encryption_key);
        new_question->correct_option_index = *(uint8_t *)decrypted_correct_index;

        if (!fgets(buffer, sizeof(buffer), file)) {
            perror("Unexpected end of file while reading difficulty");
            free(new_question);
            fclose(file);
            return;
        }
        buffer[strcspn(buffer, "\n")] = '\0';
        unsigned char decrypted_difficulty[sizeof(uint8_t)];
        xor_encrypt_decrypt((const unsigned char *)buffer, decrypted_difficulty, sizeof(uint8_t), encryption_key);
        new_question->difficulty = *(uint8_t *)decrypted_difficulty;

        new_question->next = head;
        head = new_question;
    }

    fclose(file);
}

void load_questions(const char *filename,char options[4][100], char* text,int* correct_option, int* Idifficulty, bool* found )
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Ne moge da se otvori fail za chetene");
        *found = false;
        return;
    }
    if(found == true){
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), file))
        {
            size_t length = strlen(buffer); // Length of the decrypted string
            buffer[length - 1] = '\0';      // Remove newline character
            unsigned char *decrypted = debug_encrypt(buffer, encryption_key, "decryption");
            strcpy(text, decrypted);

            for (int i = 0; i < 4; i++)
            {
                fgets(buffer, sizeof(buffer), file);
                size_t length = strlen(buffer); // Length of the decrypted string
                buffer[length - 1] = '\0';      // Remove newline character
                unsigned char *decrypted = debug_encrypt(buffer, encryption_key, "decryption");
                strcpy(options[i], (char *)decrypted);
                free(decrypted); // Free decrypted buffer
            }

            fgets(buffer, sizeof(buffer), file);
            unsigned char correct_index[sizeof(uint8_t)];
            sscanf(buffer, "%s", correct_index);
            xor_encrypt_decrypt(correct_index, (unsigned char *)&correct_index, sizeof(uint8_t), encryption_key);
            *correct_option = (int)correct_index[0];

        

            
                fgets(buffer, sizeof(buffer), file);
                unsigned char difficulty[sizeof(uint8_t)];
                sscanf(buffer, "%s", difficulty);
                xor_encrypt_decrypt(difficulty, (unsigned char *)&difficulty, sizeof(uint8_t), encryption_key);
                *Idifficulty = (int)difficulty[0];

            

            
        }
        fclose(file);
    }
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

void find_question_by_difficulty(uint8_t difficulty, char **text, char ***options, uint8_t *correct_index, bool *has_found) {
    QuizQuestion *current = head;
    QuizQuestion *questions_with_difficulty[10]; // Assuming maximum 10 questions per difficulty
    int count = 0;

    // Find all questions with the given difficulty
    while (current != NULL) {
        if (current->difficulty == difficulty) {
            questions_with_difficulty[count++] = current;
        }
        current = current->next;
    }

    if (count == 0) {
        *has_found = false;
        *text = NULL;
        *options = NULL;
        *correct_index = 0;
        return;
    }

    // Randomly select one question from the found questions
    int random_index = rand() % count;
    QuizQuestion *selected_question = questions_with_difficulty[random_index];

    // Set return values
    *has_found = true;

    // Decrypt and set the question text
    unsigned char *decrypted_text = debug_encrypt(selected_question->question_text, encryption_key, "decryption");
    if (decrypted_text == NULL) {
        *has_found = false;
        return;
    }
    *text = strdup((char *)decrypted_text);
    free(decrypted_text);

    if (*text == NULL) {
        *has_found = false;
        return;
    }

    // Decrypt and set the correct option index
    unsigned char decrypted_correct_index[sizeof(uint8_t)];
    xor_encrypt_decrypt((const unsigned char *)&selected_question->correct_option_index, decrypted_correct_index, sizeof(uint8_t), encryption_key);
    *correct_index = *(uint8_t *)decrypted_correct_index;

    // Allocate memory for options array
    *options = (char **)malloc(4 * sizeof(char *));
    if (*options == NULL) {
        perror("Memory allocation failed");
        *has_found = false;
        free(*text);
        *text = NULL;
        *correct_index = 0;
        return;
    }

    // Decrypt and copy options
    for (int i = 0; i < 4; i++) {
        unsigned char *decrypted_option = debug_encrypt(selected_question->options[i], encryption_key, "decryption");
        if (decrypted_option == NULL) {
            perror("Decryption failed");
            *has_found = false;
            free(*text);
            for (int j = 0; j < i; j++) {
                free((*options)[j]);
            }
            free(*options);
            *text = NULL;
            *options = NULL;
            *correct_index = 0;
            return;
        }
        (*options)[i] = strdup((char *)decrypted_option);
        free(decrypted_option);
        if ((*options)[i] == NULL) {
            perror("Memory allocation failed");
            *has_found = false;
            free(*text);
            for (int j = 0; j < i; j++) {
                free((*options)[j]);
            }
            free(*options);
            *text = NULL;
            *options = NULL;
            *correct_index = 0;
            return;
        }
    }
}