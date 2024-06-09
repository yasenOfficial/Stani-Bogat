#include "quiz.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define MAX_QUESTIONS 1000

static QuizQuestion *head = NULL;

void initialize_quiz() {
    head = NULL;
}

void add_question_to_file(const char *filename, char *text, int difficulty, char **options, int correct_index) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        perror("Не може да се отвори файл за запис");
        return;
    }

    fprintf(file, "%s\n", text);
    for (int i = 0; i < 4; i++) {
        fprintf(file, "%s\n", options[i]);
    }
    fprintf(file, "%d\n", correct_index);
    fprintf(file, "%d\n", difficulty);

    fclose(file);
}

void edit_question_in_file(const char *filename, int question_number) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Не може да се отвори файл за четене");
        return;
    }

    FILE *temp_file = fopen("temp.txt", "w");
    if (!temp_file) {
        perror("Не може да се отвори временен файл за запис");
        fclose(file);
        return;
    }

    char buffer[256];
    int current_question = 1;
    int editing = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        if (current_question == question_number) {
            editing = 1;
            char new_text[256];
            char new_options[4][256];
            int new_correct_index;
            int new_difficulty;

            printf("Въведете нов текст на въпроса: ");
            fgets(new_text, sizeof(new_text), stdin);
            new_text[strcspn(new_text, "\n")] = '\0';  // Премахване на новия ред

            for (int i = 0; i < 4; i++) {
                printf("Въведете нов отговор %d: ", i + 1);
                fgets(new_options[i], sizeof(new_options[i]), stdin);
                new_options[i][strcspn(new_options[i], "\n")] = '\0';  // Премахване на новия ред
            }

            printf("Въведете нов номер на верния отговор (1-4): ");
            scanf("%d", &new_correct_index);
            new_correct_index--;  // Правим индекса 0-базиран

            printf("Въведете ново ниво на трудност (1-10): ");
            scanf("%d", &new_difficulty);
            getchar();  // Премахване на новия ред

            fprintf(temp_file, "%s\n", new_text);
            for (int i = 0; i < 4; i++) {
                fprintf(temp_file, "%s\n", new_options[i]);
            }
            fprintf(temp_file, "%d\n", new_correct_index);
            fprintf(temp_file, "%d\n", new_difficulty);

            // Пропускане на старите редове на въпроса
            for (int i = 0; i < 6; i++) {
                fgets(buffer, sizeof(buffer), file);
            }

            current_question++;
            continue;
        }

        fprintf(temp_file, "%s", buffer);
        if (strchr(buffer, '\n') && ++editing % 6 == 0) {
            current_question++;
        }
    }

    fclose(file);
    fclose(temp_file);

    // Замяна на оригиналния файл с редактирания временен файл
    remove(filename);
    rename("temp.txt", filename);
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
        perror("Не може да се отвори файл за запис");
        return;
    }

    QuizQuestion *current = head;
    while (current) {
        fprintf(file, "%s\n", current->question_text);
        for (int i = 0; i < 4; i++) {
            fprintf(file, "%s\n", current->options[i]);
        }
        fprintf(file, "%d\n", current->correct_option_index);
        fprintf(file, "%d\n", current->difficulty);
        current = current->next;
    }

    fclose(file);
}

void load_questions_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Не може да се отвори файл за четене");
        return;
    }

    cleanup_quiz();

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        QuizQuestion *new_question = (QuizQuestion*)malloc(sizeof(QuizQuestion));
        if (!new_question) {
            perror("Не може да се задели памет за въпрос");
            fclose(file);
            return;
        }

        buffer[strcspn(buffer, "\n")] = '\0';  // Премахване на новия ред
        new_question->question_text = strdup(buffer);

        for (int i = 0; i < 4; i++) {
            fgets(buffer, sizeof(buffer), file);
            buffer[strcspn(buffer, "\n")] = '\0';  // Премахване на новия ред
            new_question->options[i] = strdup(buffer);
        }

        fscanf(file, "%d\n", &new_question->correct_option_index);
        fscanf(file, "%d\n", &new_question->difficulty);

        new_question->next = head;
        head = new_question;
    }

    fclose(file);
}

void print_questions() {
    QuizQuestion *current = head;
    int index = 1;
    while (current) {
        printf("Въпрос %d: %s\n", index++, current->question_text);
        for (int i = 0; i < 4; i++) {
            printf("  Отговор %d: %s\n", i + 1, current->options[i]);
        }
        printf("  Верен отговор: %d\n", current->correct_option_index + 1);
        printf("  Трудност: %d\n\n", current->difficulty);
        current = current->next;
    }
}