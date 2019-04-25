#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "report.h"
#include "countchar.h"

extern void Search ();
extern void MarkLastOnPage (int);
extern void ProcessLine (char [], int);
void PrintDate (FILE *, struct daterec);

struct entry logbook[MAXENTRIES];
int count = 0;

int
main(int argc, char *argv[])
{
	FILE *logfile;
	char inputline[MAXLINESIZE];
	int filenum = 1;

	initcharcount();
	count = 0;
	if (argc < 2)
	{
	    fprintf (stderr, "usage: %s filename(s)\n", argv[0]);
	    exit (-1);
	};

	/* Interate through each input log file ...  */

	while (filenum < argc)
	{
	    if ((logfile = fopen (argv[filenum], "r")) == NULL)
	    {
		fprintf (stderr, "%s: cannot open '%s'\n", argv[0],
			argv[filenum]);
		exit (-1);
	    }
	    fprintf (stderr, "file '%s'open\n", argv[filenum]);

	    while (fgets (inputline, MAXLINESIZE, logfile) != NULL)
	    {
		if (!strcmp (inputline, "*page\n"))
		{
		    if (count > 0)
			MarkLastOnPage (count-1);
		}
		else
		{
		    ProcessLine (inputline, count);
		    count++;
		    if (count > MAXENTRIES) {
			fprintf (stderr, "%s: logbook has more than %d entries\n",
				argv[0], MAXENTRIES);
			exit (-1);
		    }
		}
	    }
	    filenum++;
	}
	printf ("%d input log entries read\n", count);

	printf ("First entry date:");
	PrintDate (stdout, logbook[0].e_date);
	printf ("\n");

	printf ("Last entry date:");
	PrintDate (stdout, logbook[count-1].e_date);
	printf ("\n");

	Search ();
	return 0;
}
