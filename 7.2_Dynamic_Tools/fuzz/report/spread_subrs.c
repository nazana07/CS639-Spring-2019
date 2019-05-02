#include <stdio.h>
#include "report.h"

extern struct entry logbook[MAXENTRIES];
extern int count;

extern struct fields fieldtable[];

void
Spread_String (FILE *outfile, char *column, int row, char *data)
{
    fprintf (outfile, "leftstring %s%d=\"%s\"\n", column, row, data);
}


void
Spread_Int (FILE *outfile, char *column, int row, int data)
{
    fprintf (outfile, "let %s%d=%d\n", column, row, data);
}


void
Spread_Double (FILE *outfile, char *column, int row, double data)
{
    fprintf (outfile, "let %s%d=%6.1f\n", column, row, data);
}


void
Spread_Total (FILE *outfile, char *column, int row, int first)
{
    fprintf (outfile, "let %s%d=@sum(%s%d:%s%d)\n", column, row,
						    column, first,
						    column, row-1);
}
