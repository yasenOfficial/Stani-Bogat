#include "quiz.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "../encrypt_decrypt/encrypt_decrypt.h"

/*
# Step 1: Compile quiz.c
gcc -c quiz.c -o quiz.o

# Step 2: Compile encrypt_decrypt.c
gcc -c encrypt_decrypt.c -o encrypt_decrypt.o

# Step 3: Link quiz.o and encrypt_decrypt.o
gcc quiz.o encrypt_decrypt.o -o quiz_program
*/
bool debug = true;

static QuizQuestion *head = NULL;

extern encryption_key;

void write_encrypted(FILE *file, const char *data)
{
    size_t len = strlen(data);
    unsigned char *encrypted = (unsigned char *)malloc(len + 1);
    xor_encrypt_decrypt((const unsigned char *)data, encrypted, len, encryption_key);
    encrypted[len] = '\0';
    fprintf(file, "%s\n", encrypted);
}

void read_and_decrypt(FILE *file, char *buffer, int bufsize)
{
    fgets(buffer, bufsize, file);
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n')
    {
        buffer[len - 1] = '\0';
        len--;
    }
    xor_encrypt_decrypt((const unsigned char *)buffer, (unsigned char *)buffer, len, encryption_key);
}

void initialize_quiz()
{
    head = NULL;
}

void add_question_to_file(const char *filename, char *text, int difficulty, char **options, int correct_index)
{

    QuizQuestion *new_question = (QuizQuestion *)malloc(sizeof(QuizQuestion));
    if (!new_question)
    {
        perror("Ne moje da se zaredi vuprosa v pametta");
        return;
    }

    new_question->question_text = strdup(text);
    for (int i = 0; i < 4; i++)
    {
        new_question->options[i] = strdup(options[i]);
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
    if (debug)
    {
        fprintf(file, "%s\n", text);
    }
    else
    {
        write_encrypted(file, text);
    }
    for (int i = 0; i < 4; i++)
    {
        if (debug)
        {
            fprintf(file, "%s\n", options[i]);
        }
        else
        {
            write_encrypted(file, options[i]);
        }
    }
    if (debug)
    {
        fprintf(file, "%d\n", correct_index);
    }
    else
    {
        write_encrypted(file, (const char *)&correct_index);
    }
    if (debug)
    {
        fprintf(file, "%d\n", difficulty);
    }
    else
    {
        write_encrypted(file, (const char *)&difficulty);
    }

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

// i dont think this works either
void edit_question_in_file(const char *filename, int question_number)
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
        perror("Ne moge da se otvori vremenen fail za zapis");
        fclose(file);
        return;
    }

    char buffer[256];
    int current_question = 1;
    int line_count = 0;

    QuizQuestion *question_to_edit = find_question_by_index(question_number);
    if (!question_to_edit)
    {
        printf("Nqma vupros s takuv nomer\n");
        fclose(file);
        fclose(temp_file);
        return;
    }

    while (fgets(buffer, sizeof(buffer), file))
    {
        if (line_count == 0 && current_question == question_number)
        {
            char new_text[256];
            char new_options[4][256];
            int new_correct_index;
            int new_difficulty;

            printf("Vuvedete nov text za vuprosa: ");
            fgets(new_text, sizeof(new_text), stdin);
            new_text[strcspn(new_text, "\n")] = '\0';

            for (int i = 0; i < 4; i++)
            {
                printf("Vuvedete nov otgovor %d: ", i + 1);
                fgets(new_options[i], sizeof(new_options[i]), stdin);
                new_options[i][strcspn(new_options[i], "\n")] = '\0';
            }

            printf("Vuvedete nomer na pravilniq otgovor (1-4): ");
            scanf("%d", &new_correct_index);
            new_correct_index--;

            printf("Vuvedete nivo na trudnost (1-10): ");
            scanf("%d", &new_difficulty);
            getchar();

            // Update the dynamic memory
            free(question_to_edit->question_text);
            question_to_edit->question_text = strdup(new_text);

            for (int i = 0; i < 4; i++)
            {
                free(question_to_edit->options[i]);
                question_to_edit->options[i] = strdup(new_options[i]);
            }

            question_to_edit->correct_option_index = new_correct_index;
            question_to_edit->difficulty = new_difficulty;

            // Write to temporary file
            if (debug)
            {
                fprintf(temp_file, "%s\n", new_text);
            }
            else
            {
                write_encrypted(temp_file, new_text);
            }
            for (int i = 0; i < 4; i++)
            {
                if (debug)
                {
                    fprintf(temp_file, "%s\n", new_options[i]);
                }
                else
                {
                    write_encrypted(temp_file, new_options[i]);
                }
            }
            if (debug)
            {
                fprintf(temp_file, "%d\n", new_correct_index);
            }
            else
            {
                write_encrypted(temp_file, (const char *)&new_correct_index);
            }
            if (debug)
            {
                fprintf(temp_file, "%d\n", new_difficulty);
            }
            else
            {
                write_encrypted(temp_file, (const char *)&new_difficulty);
            }

            for (int i = 0; i < 6; i++)
            {
                fgets(buffer, sizeof(buffer), file);
            }

            current_question++;
            line_count = 0;
        }
        else
        {
            fprintf(temp_file, "%s", buffer);
            line_count++;
            if (line_count == 6)
            {
                line_count = 0;
                current_question++;
            }
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

// print questions doesnt work
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
        if (debug)
        {
            printf("\n\nVupros %d: %s", index++, buffer);
        }
        else
        {
            // read_and_decrypt(file, buffer, sizeof(buffer));
            xor_encrypt_decrypt((const unsigned char *)buffer, (unsigned char *)buffer, sizeof(buffer), encryption_key);
            printf("\n\nVupros %d: %s\n", index++, buffer);
        }

        for (int i = 0; i < 4; i++)
        {
            fgets(buffer, sizeof(buffer), file);
            if (debug)
            {
                printf("  Otgovor %d: %s", i + 1, buffer);
            }
            else
            {
                read_and_decrypt(file, buffer, sizeof(buffer));
                printf("  Otgovor %d: %s\n", i + 1, buffer);
            }
        }
        fgets(buffer, sizeof(buffer), file);
        int correct_option;
        sscanf(buffer, "%d", &correct_option);
        if (print_answers)
        {
            if (debug)
            {
                printf("  Pravilen otgovor: %d\n", correct_option);
            }
            else
            {
                xor_encrypt_decrypt((const unsigned char *)&correct_option, (unsigned char *)&correct_option, sizeof(correct_option), encryption_key);
                printf("  Pravilen otgovor: %d\n", correct_option);
            }
        }

        fgets(buffer, sizeof(buffer), file);
        int difficulty;
        sscanf(buffer, "%d", &difficulty);
        if (print_difficulty)
        {
            if (debug)
            {
                printf("  Trudnost: %d\n", difficulty);
            }
            else
            {
                xor_encrypt_decrypt((const unsigned char *)&difficulty, (unsigned char *)&difficulty, sizeof(difficulty), encryption_key);
                printf("  Trudnost: %d\n", difficulty);
            }
        }
    }

    fclose(file);
}

// int main()
// {
//     // Initialize the quiz system
//     initialize_quiz();

//     // Add some questions to the quiz
//     char *options1[] = {"35", "36", "37", "38"};
//     add_question_to_file("quiz_questions.txt", "How old is andrew tate", 5, options1, 3);

//     char *options2[] = {"A", "B", "C", "D"};
//     add_question_to_file("quiz_questions.txt", "Tup vupros", 10, options2, 1);

//     // Print all questions in the quiz
//     printf("Nachalni Vuprosi:\n");
//     print_questions("quiz_questions.txt", true, true);

//     // Edit a question in the quiz
//     edit_question_in_file("quiz_questions.txt", 1);

//     // Print the updated questions in the quiz
//     printf("\nAktualizirani Vuprosi:\n");
//     print_questions("quiz_questions.txt", true, true);

//     // Save the updated questions to a file
//     // save_questions_to_file("quiz_questions_updated.txt");

//     // // Load the questions from the updated file
//     // load_questions_from_file("quiz_questions_updated.txt");

//     // // Print the loaded questions
//     // printf("\nZaredeni Vuprosi:\n");
//     // print_questions("quiz_questions_updated.txt", true, true);

//     // Cleanup the quiz system
//     cleanup_quiz();

//     return 0;
// }