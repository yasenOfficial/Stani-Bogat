#include "menu.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <conio.h>
    #define GETCH() getch()
#else
    #define GETCH() getchar()
#endif

void option(unsigned char c, int *otg){
    if (c == 224 || c == 0) {
        c = GETCH();
        if (c == 72) { // Стрелка нагоре
            (*otg)--;
            if (*otg < 1)
                *otg = OPTION_COUNT;
        } else if (c == 80) { // Стрелка надолу
            (*otg)++;
            if (*otg > OPTION_COUNT)
                *otg = 1;
        }
    }
}

//Jokeri
void joker5050();
void jokerObadise();
void jokerPublika();
int menu()
{
    int joker5050 = 0, jokerObadise = 0, jokerPublika = 0;
    
    unsigned char c;
    int op = 1;
    do
    {
        system("cls");

        printf("***************************\n");
        printf("*     STANI BOGAT         *\n");
        printf("***************************\n");

        if (op == 1)printf(">1. Zapochni igra\n");else printf(" 1. Zapochni igra\n");
        if (op == 2)printf(">2. Pravila na igrata\n");else printf(" 2. Pravila na igrata\n");
        if (op == 3)printf(">3. Dobavqne na now vupros\n");else printf(" 3. Dobavqne na now vupros\n");
        if (op == 4)printf(">4. Redaktirane na vupros\n");else printf(" 4. Redaktirane na vupros\n");
        if (op == 5)printf(">5. Izhod\n");else printf(" 5. Izhod\n");

        c = GETCH();

        // Проверка за Ctrl + C (ASCII код 3)
        if (c == 3) {
            printf("\nProgramata e spryana s Ctrl + C\n");
            break;
        }

        if (c == 13) // Enter
        {
            if(op == 1){
                printf("Igrata zapochna!\n");
                printf("Natisni kojto i da e klavish za da produljish...\n\n");

                GETCH();  // Ralchev da pishe kod
                // Select random question
                // press j for menu with jokers
                int otg = 1;
                do{
                    int i = 0;
                    system("cls");
                    printf("Vupros %d:\n", i + 1);

                    if (otg == 1)printf(">1. Otgovor A\n");else printf(" 1. Otgovor A\n");
                    if (otg == 2)printf(">2. Otgovor B\n");else printf(" 2. Otgovor B\n");
                    if (otg == 3)printf(">3. Otgovor C\n");else printf(" 3. Otgovor C\n");
                    if (otg == 4)printf(">4. Otgovor D\n");else printf(" 4. Otgovor D\n");
                    if (otg == 5)printf(">5. Jokeri\n");else printf(" 5. Jokeri\n");

                    c = GETCH();

                    option(c, &otg);

                    if (c == 3) {
                        printf("\nProgramata e spryana s Ctrl + C\n");
                        return 0;
                    }

                    if(otg == 1) printf("Izbra otgovor A\n");
                    else if(otg == 2) printf("Izbra otgovor B\n");
                    else if(otg == 3) printf("Izbra otgovor C\n");
                    else if(otg == 4) printf("Izbra otgovor D\n");
                    else if(otg == 5) printf("Izbra joker:");

                }while (c != 27);
                
            }else if(op == 2) {
                printf("----------------------------------------------------------------------------------------------\n");
                printf("1. Imate 10 vyprosa s narastvashta trudnost.\n");
                printf("2. Vseki vypros ima 4 vyzmozhni otgovora, no samo edin e praven.\n");
                printf("3. Mozhete da izpolzvate tri jokera: 50:50, Pomosht ot publikata i Obadi se na priyatel.\n");
                printf("4. Za da spechelite golyamata nagrada, tryabva da otgovorite praveno na vsichki vyprosi.\n");
                printf("----------------------------------------------------------------------------------------------\n");

                printf("Natisni kojto i da e klavish za da produljish...\n\n");

                GETCH(); 

            }else if(op == 3) {
                printf("Dobavqne na vupros\n");
                break;

            }else if(op == 4) {
                printf("Redaktirane na vupros\n");
                break;

            }else if(op == 5) {
                printf("Izhod\n");
                break;
            }
        }

    option(c, &op);

    } while (c != 27);

    return 0;
}
