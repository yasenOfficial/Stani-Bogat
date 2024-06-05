#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <locale.h>

#define OPTION_COUNT 3

int main()
{
    setlocale(LC_ALL, "");
    
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
        if (op == 3)printf(">3. Izhod\n");else printf(" 3. Izhod\n");

        c = getch();

        // Проверка за Ctrl + C (ASCII код 3)
        if (c == 3) {
            printf("\nProgramata e spryana s Ctrl + C\n");
            break;
        }

        if (c == 13) // Enter
        {
            if(op == 1){
                printf("Zapochna igra\n");
                break; // Ralchev da pishe kod
                // Select random question
                // press j for menu with jokers
            }else if(op == 2) {
                printf("----------------------------------------------------------------------------------------------\n");
                printf("1. Imate 10 vyprosa s narastvashta trudnost.\n");
                printf("2. Vseki vypros ima 4 vyzmozhni otgovora, no samo edin e praven.\n");
                printf("3. Mozhete da izpolzvate tri jokera: 50:50, Pomosht ot publikata i Obadi se na priyatel.\n");
                printf("4. Za da spechelite golyamata nagrada, tryabva da otgovorite praveno na vsichki vyprosi.\n");
                printf("----------------------------------------------------------------------------------------------\n");

                printf("Natisni kojto i da e klavish za da produljish...\n\n");

                getch(); 

            }else if(op == 3) {
                printf("Izhod\n");
                break;
            }
        }

        if (c == 224 || c == 0)
        {
            c = getch();
            if (c == 72)
            {
                op--;
                if (op < 1)
                    op = OPTION_COUNT;
            }
            else if (c == 80)
            {
                op++;
                if (op > OPTION_COUNT)
                    op = 1;
            }
        }

    } while (c != 27);

    return 0;
}
