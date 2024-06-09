#include "quiz.h"
#include <stdlib.h>
#include <stdio.h>

static QuizQuestion *head = NULL;

void initialize_quiz() {
    head = NULL;
}

void add_question_to_file(const char *filename, char *text, int difficulty, char **options, int correct_index) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        perror("Ne moge da se otvori fail za zapis");
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
        perror("Ne moge da se otvori fail za chetene");
        return;
    }

    FILE *temp_file = fopen("temp.txt", "w");
    if (!temp_file) {
        perror("Ne moge da se otvori vremenen fail za zapis");
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

            printf("Vuvedete nov text za vuprosa: ");
            fgets(new_text, sizeof(new_text), stdin);
            new_text[strcspn(new_text, "\n")] = '\0';  // Премахване на новия ред

            for (int i = 0; i < 4; i++) {
                printf("Vuvedete nov otgovor %d: ", i + 1);
                fgets(new_options[i], sizeof(new_options[i]), stdin);
                new_options[i][strcspn(new_options[i], "\n")] = '\0';  // Премахване на новия ред
            }

            printf("Vuvedete nomer na pravilniq otgovor (1-4): ");
            scanf("%d", &new_correct_index);
            new_correct_index--;  // Правим индекса 0-базиран

            printf("Vuvedete nivo na trudnost (1-10): ");
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
        perror("Ne moge da se otvori fail za zapis");
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
        perror("Ne moge da se otvori fail za chetene");
        return;
    }

    cleanup_quiz();

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        QuizQuestion *new_question = (QuizQuestion*)malloc(sizeof(QuizQuestion));
        if (!new_question) {
            perror("Ne moje da se zaredi vuprosa v pametta");
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

// Ne sum siguren dali ni trqbwat gornite dve funkcii ^^^

void print_questions(const char *filename, bool print_answers, bool print_difficulty) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Ne moge da se otvori fail za chetene");
        return;
    }

    char buffer[256];
    int index = 1;
    while (fgets(buffer, sizeof(buffer), file)) {
        printf("Vupros %d: %s", index++, buffer);
        for (int i = 0; i < 4; i++) {
            fgets(buffer, sizeof(buffer), file);
            printf("  Otgovor %d: %s", i + 1, buffer);
        }
        fgets(buffer, sizeof(buffer), file);
        int correct_option;
        sscanf(buffer, "%d", &correct_option);
        printf("  Veren otgovor: %d\n", correct_option);
        
        fgets(buffer, sizeof(buffer), file);
        int difficulty;
        sscanf(buffer, "%d", &difficulty);
        printf("  Trudnost: %d\n\n", difficulty);
    }

    fclose(file);
}



int main() {
    // Инициализиране на системата за викторина
    initialize_quiz();

    // Добавяне на някои въпроси към викторината
    char *options1[] = {"Optsiya 1", "Optsiya 2", "Optsiya 3", "Optsiya 4"};
    add_question_to_file("quiz_questions.txt", "Vupros 1", 5, options1, 2);

    char *options2[] = {"Optsiya A", "Optsiya B", "Optsiya C", "Optsiya D"};
    add_question_to_file("quiz_questions.txt", "Vupros 2", 7, options2, 1);

    // Извеждане на всички въпроси във викторината
    printf("Nachalni Vuprosi:\n");
    print_questions("quiz_questions.txt", true, true);

    // Редактиране на въпрос във викторината
    edit_question_in_file("quiz_questions.txt", 1);

    // Извеждане на актуализираните въпроси във викторината
    printf("\nAktualizirani Vuprosi:\n");
    print_questions("quiz_questions.txt", true, true);

    // Запазване на актуализираните въпроси във файл
    save_questions_to_file("quiz_questions_updated.txt");

    // Зареждане на въпросите от актуализирания файл
    load_questions_from_file("quiz_questions_updated.txt");

    // Извеждане на заредените въпроси
    printf("\nZaredeni Vuprosi:\n");
    print_questions("quiz_questions_updated.txt", true, true);

    // Почистване на системата за викторина
    cleanup_quiz();

    return 0;
}
