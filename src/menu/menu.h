#ifndef MENU_H
#define MENU_H

#include <stdio.h>
#include <stdlib.h>

#define OPTION_COUNT 5
#define QUESTION_COUNT 10

void option(unsigned char c, int *otg, int count);
void joker5050(char correct, int *options);
void jokerObadise(char correct, int question_difficulty);
void jokerPublika(char correct, int question_difficulty);

int menu();

#endif 

