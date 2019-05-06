#include <stdio.h>
#include <stdlib.h>

int *cnttable;

void initcharcount ()
{
    cnttable = (int *)malloc(sizeof(int) * 100000);
}

void countchar (unsigned char c) // c used to be signed. If c > 127 => c is negative
{
    int i;

    i = (c*10000+7) % 100000; // c is used to ca
    cnttable[i]++; // 
    /* Debug: printf ("c=%d (%x), cnttable[%d]=%d\n", (int)c, (int)c, i, cnttable[i]); */
    return;
}

int chartotal (char c)
{
    int i;

    i = (c*10000+7) % 100000;
    return (cnttable[i]);
}
