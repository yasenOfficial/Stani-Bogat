#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "encrypt_decrypt/encrypt_decrypt.h"
#include "menu/menu.h"
#include "quiz/quiz.h"

unsigned char encryption_key[] = "This is a key";
const int KEY_SIZE = sizeof(encryption_key) - 1;

int main() {
    initialize_quiz();

    char *options1[] = {"Optsiya 1", "Optsiya 2", "Optsiya 3", "Optsiya 4"};
    add_question_to_file("quiz_questions.txt", "Vupros 1", 5, options1, 2);

    char *options2[] = {"Optsiya A", "Optsiya B", "Optsiya C", "Optsiya D"};
    add_question_to_file("quiz_questions.txt", "Vupros 2", 7, options2, 1);

    printf("\nNachalni Vuprosi:\n");
    print_questions("quiz_questions.txt", true, true);

    edit_question_in_file("quiz_questions.txt", 1);

    printf("\nAktualizirani Vuprosi:\n");
    print_questions("quiz_questions.txt", true, true);

    // save_questions_to_file("quiz_questions_updated.txt");

    // load_questions_from_file("quiz_questions_updated.txt");

    // printf("\nZaredeni Vuprosi:\n");
    // print_questions("quiz_questions_updated.txt", true, true);

    cleanup_quiz();

    return 0;
}