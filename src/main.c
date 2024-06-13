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

int main()
{
    initialize_quiz();
    const char *newoptions[] = {"1", "2", "3", "4"};

    const char *options1[] = {"35", "36", "37", "38"};
    add_question_to_file("quiz_questions.txt", "How old is Andrew Tate?", 5, options1, 3);

    const char *options2[] = {"A", "B", "C", "D"};
    add_question_to_file("quiz_questions.txt", "ABCD Vupros", 7, options2, 1);

    // menu();
    
    printf("\nNachalni Vuprosi:\n");
    print_questions("quiz_questions.txt", true, true);

    // Edit the first question
    edit_question_in_file("quiz_questions.txt", 1, "How old is Brad Pitt?", 5, newoptions, 1);

    printf("\nAktualizirani Vuprosi:\n");
    print_questions("quiz_questions.txt", true, true);

    cleanup_quiz();

    return 0;
}
