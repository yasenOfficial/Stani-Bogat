#include "quiz.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <encrypt_decrypt.h>

static QuizQuestion *head = NULL;

extern encryption_key;

void write_encrypted(FILE *file, const char *data) {
    size_t len = strlen(data);
    unsigned char *encrypted = (unsigned char *)malloc(len + 1);
    xor_encrypt_decrypt((const unsigned char *)data, encrypted, len, encryption_key);
    encrypted[len] = '\0';
    fprintf(file, "%s\n", encrypted);
}

void read_and_decrypt(FILE *file, char *buffer, int bufsize) {
    fgets(buffer, bufsize, file);
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
        len--;
    }
    xor_encrypt_decrypt((const unsigned char *)buffer, (unsigned char *)buffer, len, encryption_key);
}

void initialize_quiz() {
    head = NULL;
}

void add_question_to_file(const char *filename, char *text, int difficulty, char **options, int correct_index) {
    QuizQuestion *new_question = (QuizQuestion*)malloc(sizeof(QuizQuestion));
    if (!new_question) {
        perror("Failed to allocate memory for the question");
        return;
    }

    new_question->question_text = strdup(text);
    for (int i = 0; i < 4; i++) {
        new_question->options[i] = strdup(options[i]);
    }
    new_question->correct_option_index = correct_index;
    new_question->difficulty = difficulty;
    new_question->next = head;
    head = new_question;

    FILE *file = fopen(filename, "a");
    if (!file) {
        perror("Failed to open file for appending");
        return;
    }

    write_encrypted(file, text);
    for (int i = 0; i < 4; i++) {
        write_encrypted(file, options[i]);
    }
    char line[256];
    snprintf(line, sizeof(line), "%d", correct_index);
    write_encrypted(file, line);
    snprintf(line, sizeof(line), "%d", difficulty);
    write_encrypted(file, line);

    fclose(file);
}

QuizQuestion* find_question_by_index(int index) {
    QuizQuestion *current = head;
    int current_index = 1;

    while (current && current_index < index) {
        current = current->next;
        current_index++;
    }

    return current_index == index ? current : NULL;
}

void edit_question_in_file(const char *filename, int question_number) {
    FILE *file = fopen(filename, "r+");
    if (!file) {
        perror("Failed to open file for reading and writing");
        return;
    }

    // (Rest of your existing edit_question_in_file code goes here, with modifications to use write_encrypted and read_and_decrypt as necessary)

    fclose(file);
}

void cleanup_quiz() {
    QuizQuestion *current = head;
    while (current) {
        QuizQuestion *next = current->next;
        free(current->question_text);
        for (int i = 0; i < 4; i++) {
            free(current->options[i]);
        }
        free(current);
        current = next;
    }
    head = NULL;
}

void save_questions_to_file(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    QuizQuestion *current = head;
    while (current) {
        write_encrypted(file, current->question_text);
        for (int i = 0; i < 4; i++) {
            write_encrypted(file, current->options[i]);
        }
        char line[256];
        snprintf(line, sizeof(line), "%d", current->correct_option_index);
        write_encrypted(file, line);
        snprintf(line, sizeof(line), "%d", current->difficulty);
        write_encrypted(file, line);
        current = current->next;
    }

    fclose(file);
}

void load_questions_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file for reading");
        return;
    }

    cleanup_quiz();

    // (Rest of your existing load_questions_from_file code goes here, with modifications to use write_encrypted and read_and_decrypt as necessary)

    fclose(file);
}

void print_questions(const char *filename, bool print_answers, bool print_difficulty) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file for reading");
        return;
    }

    // (Rest of your existing print_questions code goes here, with modifications to use write_encrypted and read_and_decrypt as necessary)

    fclose(file);
}
