#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "report.h"

#define TRUE	1

double atof ();

extern struct entry logbook[MAXENTRIES];
int GetField (char [], int *, char [], char []);
void ProcessDate (char [], struct daterec *);

struct fields fieldtable[] =
{
    { "sel", SEL, 0, "F" },
    { "hel", HEL, 0, "G" },
    { "x-c", X_C, 0, "H" },
    { "day", DAY, 0, "I" },
    { "ngt", NGT, 0, "J" },
    { "act", ACT, 0, "K" },
    { "hdd", HDD, 0, "L" },
    { "sim", SIM, 0, "M" },
    { "dul", DUL, 0, "N" },
    { "pic", PIC, 0, "O" },
    { "sic", SIC, 0, "P" },
    { "gas", GAS_GALS, 0, "Q" },
    { "gas$", GAS_DOLS, 0, "R" },
    { "oil", OIL_QTS, 0, "S" },
    { "oil$", OIL_DOLS, 0, "T" },
    { 0,    0,   0,  0 }
};


void
MarkLastOnPage (int idx)
{
    logbook[idx].e_endofpage = TRUE;
}


void
ProcessLine(char inputline[], int idx)
{
    int i = 0;
    int j;
    int len, rv;
    char value[FIELDSIZE];
    char fieldname[FIELDSIZE];
    int found;

    len = strlen(inputline) - 1;
    inputline [len] = '\000';

    while (i < len)
    {
	if ((rv = GetField (inputline, &i, fieldname, value)) < 0)
	    return;

	if (!strcmp (fieldname, "dte"))
	    ProcessDate (value, &(logbook[idx].e_date));
	else if (!strcmp (fieldname, "typ"))
	    strcpy (logbook[idx].e_type, value);
	else if (!strcmp (fieldname, "idt"))
	    strcpy (logbook[idx].e_ident, value);
	else if (!strcmp (fieldname, "f/t"))
	    strcpy (logbook[idx].e_fromto, value);
	else if (!strcmp (fieldname, "lng"))
	    logbook[idx].e_lngs = atoi (value);
	else if (!strcmp (fieldname, "apr"))
	    logbook[idx].e_apprchs = atoi (value);
	else {
	    j = 0;
	    found = 0;
	    while (fieldtable[j].fieldname != NULL)
	    {
		if (!strcmp (fieldname, fieldtable[j].fieldname)) {
		    logbook[idx].e_times[fieldtable[j].fieldindex] = atof (value);
		    found = 1;
		    break;
		}
		j++;
	    }
	    if (!found) {
		fprintf (stderr, "Unknown field '%s' on input:\n  %s\n",
		    fieldname, inputline);
		return;
	    }
	}
    }
    return;
}


int
GetField (char inputline[], int *idx, char fieldname[], char value[])
{
    int i;
    int j;

    i = *idx;

    /*---Bypass leading blankings                     */
    while (inputline[i] == ' ')
	i++;

    /*---Get field name (terminated by '='            */
    j = 0;
    while (inputline[i] != '=')
	fieldname[j++] = inputline[i++];
    fieldname[j] =  '\000';
    i++;

    /*---Get field value (terminated by ',' or 0       */
    j = 0;
    while ((inputline[i] != ',') && (inputline[i] != '\000'))
	value[j++] = inputline[i++];
    value[j] =  '\000';
    i++;
    
    *idx = i;
    return (0);
}


void
ProcessDate (char value[], struct daterec *date)
{
    int i;

    date->date_month = atoi (value);

    for (i=0; value[i] != '/'; i++);
    i++;
    date->date_day = atoi (&(value[i]));

    for (; value[i] != '/'; i++);
    i++;
    date->date_year = atoi (&(value[i]));
}

int
Count (char *s, char c)
{
    int rv = 0;
    int i;
    if (s != NULL)
    {
	for (i=0; s[i] != '\000'; i++)
	{
	    if (s[i] == c) rv++;
	}
    }
    return rv;
}
