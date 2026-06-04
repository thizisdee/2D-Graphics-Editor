#include <stdio.h>

#define ROWS 20
#define COLS 50

char canvas[ROWS][COLS];

void initCanvas()
{
    int i,j;

    for(i=0;i<ROWS;i++)
    {
        for(j=0;j<COLS;j++)
        {
            canvas[i][j]='_';
        }
    }
}

void drawRectangle(int row,int col,int width,int height)
{
    int i;

    for(i=col;i<col+width;i++)
    {
        canvas[row][i]='*';
        canvas[row+height-1][i]='*';
    }

    for(i=row;i<row+height;i++)
    {
        canvas[i][col]='*';
        canvas[i][col+width-1]='*';
    }
}

void displayCanvas()
{
    int i,j;

    for(i=0;i<ROWS;i++)
    {
        for(j=0;j<COLS;j++)
        {
            printf("%c",canvas[i][j]);
        }
        printf("\n");
    }
}
int main()
{
    int choice;

    initCanvas();

    while(1)
    {
        printf("\n\n----- MENU -----\n");
        printf("1. Display Canvas\n");
        printf("2. Draw Rectangle\n");
        printf("3. Exit\n");

        printf("Enter choice: ");
        scanf("%d",&choice);

        switch(choice)
        {
            case 1:
                displayCanvas();
                break;

            case 2:
            {
                int r,c,w,h;

                printf("Enter row column width height: ");
                scanf("%d%d%d%d",&r,&c,&w,&h);

                drawRectangle(r,c,w,h);
                break;
            }

            case 3:
                return 0;

            default:
                printf("Invalid Choice!");
        }
    }
}
