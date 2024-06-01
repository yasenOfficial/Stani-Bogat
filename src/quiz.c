#include "quiz.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define MAX_QUESTIONS 1000

static int num_questions = 0;

void initialize_quiz() {
    srand(time(NULL));  // Seed the random number generator
    num_questions = 0; 
}
