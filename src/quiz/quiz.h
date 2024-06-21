#ifndef QUIZ_H
#define QUIZ_H

#define MAX_DIFFICULTY 10 
#define MAX_QUESTIONS 100

extern const int KEY_SIZE;
extern unsigned char encryption_key[];

#include <time.h>
#include <stdbool.h>
#include <stdint.h>


typedef struct quiz_question {
    char *question_text;
    char *options[4];
    int correct_option_index;
    int difficulty;
    struct quiz_question *next;
} QuizQuestion;



extern void initialize_quiz();
extern void add_question_to_file(const char *filename, char *text, uint8_t difficulty, char **options, uint8_t correct_index);
extern void edit_question_in_file(const char *filename, int question_number, const char *newText, uint8_t new_difficulty, const char *newOptions[], uint8_t newCorrect_index);
extern void save_questions_to_file(const char *filename);
extern void cleanup_quiz();
extern void load_questions(const char *filename,char options[4][100], char* text,int* correct_option, int* Idifficulty, bool* found, int desired_difficulty);
extern void print_questions(const char *filename, bool print_answers, bool print_difficulty);
extern void load_random_question(const char *filename, char options[4][100], char *text, int *correct_option, int *difficulty);

#endif
