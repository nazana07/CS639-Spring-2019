#include <stdio.h>
#include <stdlib.h>

int *cnttable;

void initcharcount ()
{
    cnttable = (int *)malloc(sizeof(int) * 256);
}

void countchar (char c)
{
    int i;

    i = (c+7) % 256;
    cnttable[i]++;
    /* Debug: printf ("c=%d (%x), cnttable[%d]=%d\n", (int)c, (int)c, i, cnttable[i]); */
    return;
}

int chartotal (char c)
{
    int i;

    i = (c+7) % 256;
    return (cnttable[i]);
}
