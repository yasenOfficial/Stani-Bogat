#include "menu.h"
#include "../quiz/quiz.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#define GETCH() getch()

#define OPTION_COUNT 5
#define QUESTION_COUNT 10

void option(unsigned char c, int *otg, int count)
{
    if (c == 224 || c == 0)
    {
        c = GETCH();
        if (c == 72)
        { // Стрелка нагоре
            (*otg)--;
            if (*otg < 1)
                *otg = count;
        }
        else if (c == 80)
        { // Стрелка надолу
            (*otg)++;
            if (*otg > count)
                *otg = 1;
        }
    }
}

void info_questions(char *question_text, char **options, int *correct_index, int *difficulty) {
    // Prompt for the question text
    printf("Vuvedete tekst na vuprosa: ");
    if (fgets(question_text, 256, stdin) == NULL || is_empty_string(question_text)) {
        printf("Greshka: Tekstut na vuprosa ne moje da bude prazen.\n");
        return;
    }
    question_text[strcspn(question_text, "\n")] = '\0'; // Remove trailing newline

    // Prompt for the options
    for (int i = 0; i < 4; i++) {
        char option[256];
        printf("Vuvedete otgovor %d: ", i + 1);
        if (fgets(option, sizeof(option), stdin) == NULL || is_empty_string(option)) {
            printf("Greshka: Otgovor %d ne moje da bude prazen.\n", i + 1);
            i--; // Retry the same option
            continue;
        }
        option[strcspn(option, "\n")] = '\0'; // Remove trailing newline
        options[i] = strdup(option); // Duplicate the string into options
    }

    // Prompt for the correct answer index
    printf("Vuvedete nomera na pravilniya otgovor (1-4): ");
    if (scanf("%d", correct_index) != 1 || *correct_index < 1 || *correct_index > 4) {
        printf("Greshka: Nomerut na pravilniya otgovor trqbva da bude mejdu 1 i 4.\n");
        while (getchar() != '\n'); // Clear the input buffer
        return;
    }

    // Prompt for the difficulty
    printf("Vuvedete trudnostta na vuprosa (1-10): ");
    if (scanf("%d", difficulty) != 1 || *difficulty < 1 || *difficulty > 10) {
        printf("Greshka: Trudnostta trqbva da bude mejdu 1 i 10.\n");
        while (getchar() != '\n'); // Clear the input buffer
        return;
    }

    // Clear the input buffer
    while (getchar() != '\n');
}


// Jokeri
void joker5050(char correct, int *options);
void jokerObadise(char correct, int *options, int difficulty);
void jokerPublika(char correct, int *options, int difficulty);

void show_question(int otg, int *options, int joker5050_used, int jokerObadise_used, int jokerPublika_used)
{
    system("cls");

    printf("Vupros:\n");

    if (otg == 1 && options[0])
        printf(">1. Otgovor A\n");
    else if (options[0])
        printf(" 1. Otgovor A\n");
    else
        printf(" 1. ---\n");
    if (otg == 2 && options[1])
        printf(">2. Otgovor B\n");
    else if (options[1])
        printf(" 2. Otgovor B\n");
    else
        printf(" 2. ---\n");
    if (otg == 3 && options[2])
        printf(">3. Otgovor C\n");
    else if (options[2])
        printf(" 3. Otgovor C\n");
    else
        printf(" 3. ---\n");
    if (otg == 4 && options[3])
        printf(">4. Otgovor D\n");
    else if (options[3])
        printf(" 4. Otgovor D\n");
    else
        printf(" 4. ---\n");
    if (otg == 5)
        printf(">5. Jokeri\n");
    else
        printf(" 5. Jokeri\n");

    printf("Dostupni Jokeri: ");
    if (!joker5050_used)
        printf("50/50 ");
    if (!jokerObadise_used)
        printf("Obadise ");
    if (!jokerPublika_used)
        printf("Publika ");
    printf("\n");
}

int is_empty_string(const char *str)
{
    return str == NULL || str[0] == '\0' || str[0] == '\n';
}

int menu()
{
    int joker5050_used = 0, jokerObadise_used = 0, jokerPublika_used = 0;

    unsigned char c;
    int op = 1;
    srand(time(NULL));

    do
    {
        system("cls");

        printf("***************************\n");
        printf("*     STANI BOGAT         *\n");
        printf("***************************\n");

        if (op == 1)
            printf(">1. Zapochni igra\n");
        else
            printf(" 1. Zapochni igra\n");
        if (op == 2)
            printf(">2. Pravila na igrata\n");
        else
            printf(" 2. Pravila na igrata\n");
        if (op == 3)
            printf(">3. Dobavqne na now vupros\n");
        else
            printf(" 3. Dobavqne na now vupros\n");
        if (op == 4)
            printf(">4. Redaktirane na vupros\n");
        else
            printf(" 4. Redaktirane na vupros\n");
        if (op == 5)
            printf(">5. Izhod\n");
        else
            printf(" 5. Izhod\n");

        c = GETCH();

        // Проверка за Ctrl + C (ASCII код 3)
        if (c == 3)
        {
            printf("\nProgramata e spryana s Ctrl + C\n");
            break;
        }

        if (c == 13) // Enter
        {
            if (op == 1)
            {
                printf("Igrata zapochna!\n");
                printf("Natisni kojto i da e klavish za da produljish...\n\n");

                GETCH(); // Ralchev da pishe kod
                // Select random question
                // press j for menu with jokers
                int question_difficulty = 9;   // примерна трудност, може да се променя според въпроса
                int options[4] = {1, 1, 1, 1}; // всички отговори са налични
                int otg = 1;
                char correct_answer = 'A'; // примерен отговор, може да се променя според въпроса
                do
                {
                    show_question(otg, options, joker5050_used, jokerObadise_used, jokerPublika_used);
                    c = GETCH();
                    option(c, &otg, OPTION_COUNT);

                    if (c == 3)
                    {
                        printf("\nProgramata e spryana s Ctrl + C\n");
                        return 0;
                    }

                    if (c == 13)
                    {
                        if (otg == 1 && options[0])
                        {
                            printf("Izbra otgovor A\n");
                            if (correct_answer == 'A')
                                printf("Pravilen otgovor!\n");
                            else
                                printf("Greshen otgovor!\n");
                            GETCH();
                        }
                        else if (otg == 2 && options[1])
                        {
                            printf("Izbra otgovor B\n");
                            if (correct_answer == 'B')
                                printf("Pravilen otgovor!\n");
                            else
                                printf("Greshen otgovor!\n");
                            GETCH();
                        }
                        else if (otg == 3 && options[2])
                        {
                            printf("Izbra otgovor C\n");
                            if (correct_answer == 'C')
                                printf("Pravilen otgovor!\n");
                            else
                                printf("Greshen otgovor!\n");
                            GETCH();
                        }
                        else if (otg == 4 && options[3])
                        {
                            printf("Izbra otgovor D\n");
                            if (correct_answer == 'D')
                                printf("Pravilen otgovor!\n");
                            else
                                printf("Greshen otgovor!\n");
                            GETCH();
                        }
                        else if (otg == 5)
                        {
                            int joker_op = 1;
                            do
                            {
                                system("cls");
                                printf("Izberete Joker:\n");
                                if (joker_op == 1 && !joker5050_used)
                                    printf(">1. 50/50\n");
                                else if (!joker5050_used)
                                    printf(" 1. 50/50\n");
                                else
                                    printf(" 1. ---\n");
                                if (joker_op == 2 && !jokerObadise_used)
                                    printf(">2. Obadise na priyatel\n");
                                else if (!jokerObadise_used)
                                    printf(" 2. Obadise na priyatel\n");
                                else
                                    printf(" 2. ---\n");
                                if (joker_op == 3 && !jokerPublika_used)
                                    printf(">3. Pomosht ot publika\n");
                                else if (!jokerPublika_used)
                                    printf(" 3. Pomosht ot publika\n");
                                else
                                    printf(" 3. ---\n");
                                if (joker_op == 4)
                                    printf(">4. Vurni se\n");
                                else
                                    printf(" 4. Vurni se\n");

                                c = GETCH();
                                option(c, &joker_op, 4);

                                if (c == 3)
                                {
                                    printf("\nProgramata e spryana s Ctrl + C\n");
                                    return 0;
                                }

                                if (c == 13)
                                {
                                    if (joker_op == 1 && !joker5050_used)
                                    {
                                        joker5050('A', options); // Примерен верен отговор
                                        joker5050_used = 1;
                                    }
                                    else if (joker_op == 2 && !jokerObadise_used)
                                    {
                                        jokerObadise('A', options, question_difficulty); // Примерен верен отговор
                                        jokerObadise_used = 1;
                                    }
                                    else if (joker_op == 3 && !jokerPublika_used)
                                    {
                                        jokerPublika('A', options, question_difficulty); // Примерен верен отговор
                                        jokerPublika_used = 1;
                                    }
                                    else if (joker_op == 4)
                                    {
                                        break;
                                    }
                                }
                            } while (c != 13 || joker_op != 4);
                        }
                    }
                } while (c != 27); // Изход от играта с ESC
            }
            else if (op == 2)
            {
                printf("----------------------------------------------------------------------------------------------\n");
                printf("1. Imate 10 vyprosa s narastvashta trudnost.\n");
                printf("2. Vseki vypros ima 4 vyzmozhni otgovora, no samo edin e praven.\n");
                printf("3. Mozhete da izpolzvate tri jokera: 50:50, Pomosht ot publikata i Obadi se na priyatel.\n");
                printf("4. Za da spechelite golyamata nagrada, tryabva da otgovorite praveno na vsichki vyprosi.\n");
                printf("----------------------------------------------------------------------------------------------\n");

                printf("Natisni kojto i da e klavish za da produljish...\n\n");

                GETCH();
            }
            else if (op == 3)
            {
                printf("Dobavqne na nov vupros\n");

                char question_text[100];
                int difficulty = 0;
                int correct_index = 0;
                int options[4];

                info_questions(question_text, options, correct_index, difficulty);
     
                add_question_to_file("quiz_questions.txt", question_text, difficulty, options, correct_index);
                printf("Vuprosut e dobaven uspeshno.\n");

                continue; // Vrushtane v menuto sled dobavyane na vupros
            }
            else if (op == 4)
            {
                print_questions("quiz_questions.txt", true, true);

                int choice = 0;
                printf("Izberete vupros, koito shte redaktirate:\n");
                scanf("%d", &choice);

                int c;
                while ((c = getchar()) != '\n' && c != EOF);

                char new_question_text[100];
                int new_difficulty = 0;
                int new_correct_index = 0;
                int new_options[4];

                info_questions(new_question_text, new_options, new_correct_index, new_difficulty);

                edit_question_in_file("quiz_questions.txt", choice, new_question_text, new_difficulty, new_options, new_correct_index);
                GETCH();
            }
            else if (op == 5)
            {
                printf("Izhod\n");
                break;
            }
        }

        option(c, &op, OPTION_COUNT);

    } while (c != 27);

    return 0;
}

void joker5050(char correct, int *options)
{
    int removed = 0;
    while (removed < 2)
    {
        int r = rand() % 4;
        if ((r == 0 && correct != 'A' && options[0]) ||
            (r == 1 && correct != 'B' && options[1]) ||
            (r == 2 && correct != 'C' && options[2]) ||
            (r == 3 && correct != 'D' && options[3]))
        {
            options[r] = 0;
            removed++;
        }
    }
    printf("50/50 joker izpolzvan!\\n");
    GETCH();
}

void jokerObadise(char correct, int *options, int difficulty)
{
    int probability;
    if (difficulty <= 3)
    {
        probability = 80;
    }
    else if (difficulty <= 6)
    {
        probability = 60;
    }
    else
    {
        probability = 30;
    }

    int chance = rand() % 100;
    char friend_answer;

    char available_options[2];
    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        if (options[i])
        {
            available_options[count++] = 'A' + i;
        }
    }

    if (chance < probability)
    {
        friend_answer = correct;
    }
    else
    {
        do
        {
            friend_answer = available_options[rand() % count];
        } while (friend_answer == correct);
    }

    printf("Prijatelqt vi smqta che verniqt otgovor e: %c\n", friend_answer);
    GETCH();
}

void jokerPublika(char correct, int *options, int difficulty)
{
    int probability;
    if (difficulty <= 3)
    {
        probability = 80;
    }
    else if (difficulty <= 6)
    {
        probability = 60;
    }
    else
    {
        probability = 30;
    }

    int votes[4] = {0, 0, 0, 0};
    int total_votes = 100;

    char available_options[2];
    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        if (options[i])
        {
            available_options[count++] = 'A' + i;
        }
    }

    if (rand() % 100 < probability)
    {
        votes[correct - 'A'] = rand() % (total_votes / 2) + (total_votes / 2);
    }
    else
    {
        votes[correct - 'A'] = rand() % (total_votes / 4);
    }
    total_votes -= votes[correct - 'A'];

    for (int i = 0; i < 4; i++)
    {
        if (options[i] && i != correct - 'A')
        {
            votes[i] = rand() % total_votes;
            total_votes -= votes[i];
        }
    }
    votes[correct - 'A'] += total_votes;

    printf("Publikata glasuva taka:\n");
    printf("A: %d%%\n", votes[0]);
    printf("B: %d%%\n", votes[1]);
    printf("C: %d%%\n", votes[2]);
    printf("D: %d%%\n", votes[3]);
    GETCH();
}
