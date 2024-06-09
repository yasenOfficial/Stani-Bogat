#include "quiz.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static QuizQuestion *head = NULL;

void initialize_quiz() {
    head = NULL;
}

void add_question_to_file(const char *filename, char *text, int difficulty, char **options, int correct_index) {
    QuizQuestion *new_question = (QuizQuestion*)malloc(sizeof(QuizQuestion));
    if (!new_question) {
        perror("Ne moje da se zaredi vuprosa v pametta");
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
    int line_count = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        if (line_count == 0 && current_question == question_number) {
            char new_text[256];
            char new_options[4][256];
            int new_correct_index;
            int new_difficulty;

            printf("Vuvedete nov text za vuprosa: ");
            fgets(new_text, sizeof(new_text), stdin);
            new_text[strcspn(new_text, "\n")] = '\0';

            for (int i = 0; i < 4; i++) {
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

            fprintf(temp_file, "%s\n", new_text);
            for (int i = 0; i < 4; i++) {
                fprintf(temp_file, "%s\n", new_options[i]);
            }
            fprintf(temp_file, "%d\n", new_correct_index);
            fprintf(temp_file, "%d\n", new_difficulty);

            for (int i = 0; i < 6; i++) {
                fgets(buffer, sizeof(buffer), file);
            }

            current_question++;
            line_count = 0;
        } else {
            fprintf(temp_file, "%s", buffer);
            line_count++;
            if (line_count == 6) {
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

        buffer[strcspn(buffer, "\n")] = '\0';
        new_question->question_text = strdup(buffer);

        for (int i = 0; i < 4; i++) {
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
        if (print_answers) {
            printf("  Veren otgovor: %d\n", correct_option + 1);
        }

        fgets(buffer, sizeof(buffer), file);
        int difficulty;
        sscanf(buffer, "%d", &difficulty);
        if (print_difficulty) {
            printf("  Trudnost: %d\n\n", difficulty);
        }
    }

    fclose(file);
}

int main() {
    // Initialize the quiz system
    initialize_quiz();

    // Add some questions to the quiz
    char *options1[] = {"Optsiya 1", "Optsiya 2", "Optsiya 3", "Optsiya 4"};
    add_question_to_file("quiz_questions.txt", "Vupros 1", 5, options1, 2);

    char *options2[] = {"Optsiya A", "Optsiya B", "Optsiya C", "Optsiya D"};
    add_question_to_file("quiz_questions.txt", "Vupros 2", 7, options2, 1);

    // Print all questions in the quiz
    printf("Nachalni Vuprosi:\n");
    print_questions("quiz_questions.txt", true, true);

    // Edit a question in the quiz
    edit_question_in_file("quiz_questions.txt", 1);

    // Print the updated questions in the quiz
    printf("\nAktualizirani Vuprosi:\n");
    print_questions("quiz_questions.txt", true, true);

    // Save the updated questions to a file
    save_questions_to_file("quiz_questions_updated.txt");

    // Load the questions from the updated file
    load_questions_from_file("quiz_questions_updated.txt");

    // Print the loaded questions
    printf("\nZaredeni Vuprosi:\n");
    print_questions("quiz_questions_updated.txt", true, true);

    // Cleanup the quiz system
    cleanup_quiz();

    return 0;
}
