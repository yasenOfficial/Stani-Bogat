#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <locale.h>

int main()
{
    setlocale(LC_ALL, "");
    printf("***************************\n");
    printf("*     STANI BOGAT         *\n");
    printf("***************************\n");

    unsigned char c;
    int op = 1;
    do
    {
        system("cls");

        if (op == 1)
            printf(">1. Zapochni igra\n");
        else
            printf(" 1. Zapochni igra\n");
        if (op == 2)
            printf(">2. Pravila na igrata\n");
        else
            printf(" 2. Pravila na igrata\n");

        if (op == 3)
            printf(">3. Izhod\n");
        else
            printf(" 3. Izhod\n");

        c = getch();

        // Проверка за Ctrl + C (ASCII код 3)
        if (c == 3) {
            printf("\nProgramata e spryana s Ctrl + C\n");
            break;
        }

        if (c == 13)
        {
            if(op == 2) {
                printf("1. Imate 10 vyprosa s narastvashta trudnost.\n");
                printf("2. Vseki vypros ima 4 vyzmozhni otgovora, no samo edin e praven.\n");
                printf("3. Mozhete da izpolzvate tri jokera: 50:50, Pomosht ot publikata i Obadi se na priyatel.\n");
                printf("4. Za da spechelite golyamata nagrada, tryabva da otgovorite praveno na vsichki vyprosi.\n\n");
            }
            printf("\nYou have chosen option: %d", op);
            break;
        }

        if (c == 224 || c == 0)
        {
            c = getch();
            if (c == 72)
            {
                op--;
                if (op < 1)
                    op = 6;
            }
            else if (c == 80)
            {
                op++;
                if (op > 6)
                    op = 1;
            }
        }

    } while (c != 27);

    return 0;
}