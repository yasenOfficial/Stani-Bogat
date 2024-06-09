#ifndef QUIZ_H
#define QUIZ_H

#define MAX_DIFFICULTY 10 
#define MAX_QUESTIONS 1000

#include <time.h>
#include <stdbool.h>

typedef struct quiz_question {
    char *question_text;
    char *options[4];
    int correct_option_index;
    int difficulty;
    struct quiz_question *next;
} QuizQuestion;

/*
Имаше няколко опции за реализация на тази задача.
Linked list
BST
Graph

Избрахме linked list, защото е най-лесно да се добавят нови въпроси и да се търси въпрос по трудност. 
BST и Graph също биха били добри опции, но не биха донесли значително подобрение на функционалността на програмата (overkill са), 
тъй като не се изисква сложно търсене на въпроси.

Ще използваме масив от linked lists, като всеки индекс на масива съответства на определено ниво на трудност, 
което съдържа свързан списък с въпроси за това ниво. Тази структура помага за ефективна категоризация и извличане.
*/

void initialize_quiz();
void add_question_to_file(const char *filename, char *text, int difficulty, char **options, int correct_index);
void edit_question_in_file(const char *filename, int question_number);
void save_questions_to_file(const char *filename);
void load_questions_from_file(const char *filename);
void cleanup_quiz();
void print_questions();



#endif
