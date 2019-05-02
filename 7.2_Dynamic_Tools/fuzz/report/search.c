#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "report.h"
#include "countchar.h"

#define LARGENUMBER 99999

extern struct entry logbook[MAXENTRIES];
extern int count;

extern struct fields fieldtable[];
int dateselect;
int identselect;
int typeselect;
int fromtoselect;
int lngselect;
int aprselect;

FILE *outfile;

struct daterec startdate;
struct daterec enddate;

int typematch;
char typelist [MAXARGS][MAXLINESIZE];

/*extern*/ void ProcessDate (char [], struct daterec *);
void Spread_String (FILE *, char *, int, char *);
void Spread_Int (FILE *, char *, int, int);
void Spread_Double (FILE *, char *, int, double);
void Spread_Total (FILE *, char *, int, int);
int Count (char *, char);

int identmatch;
char identlist [MAXARGS][MAXLINESIZE];
int GetArgs (char [], char [MAXARGS][MAXLINESIZE]);
void PrintHeader ();
void PrintSubTotals (char *, int, int, double []);
int RangeCmd (int , char [MAXARGS][MAXLINESIZE]);
int SelectCmd (int , char [MAXARGS][MAXLINESIZE]);
int UnselectCmd (int , char [MAXARGS][MAXLINESIZE]);
int ReportCmd (int , char [MAXARGS][MAXLINESIZE]);
int SpreadCmd (int , char [MAXARGS][MAXLINESIZE]);
int CurrentCmd (int , char [MAXARGS][MAXLINESIZE]);
int RecentCmd (int , char [MAXARGS][MAXLINESIZE]);
int MonthlyCmd ();
int TotalCmd ();
int ListCmd ();
int InitCmd ();
int HelpCmd ();
void PrintDate (FILE *, struct daterec);
char *gets(char *);


void
InitSearch ()
{
    startdate.date_year = 0;
    enddate.date_year = LARGENUMBER;

    typematch = 0;
    identmatch = 0;

    /*------don't reset fields...use unselect for that
     
    for (i = 0; fieldtable[i].fieldname != NULL; i++)
	fieldtable[i].fieldselect = 0;

    dateselect = 0;
    identselect = 0;
    typeselect = 0;
    fromtoselect = 0;
    lngselect = 0;
    aprselect = 0;

    */
}



void
Search ()
{
    char inputline [MAXLINESIZE];
    char arglist [MAXARGS][MAXLINESIZE];
    int numargs;
    int rv;
    int len;
    int i;
    int max;
    char c;

    InitSearch ();

    fprintf (stderr, "> ");
    fflush (stderr);
    while (gets (inputline))
    {
	len = strlen(inputline);
	for (i=0; i< len; i++) {
	    countchar(inputline[i]);
	}

	numargs = GetArgs (inputline, arglist);

	rv = 0;
	if (numargs > 0)
	{
	    if (!strcmp (arglist[0], "range"))
		rv = RangeCmd (numargs, arglist);
	    else if (!strcmp (arglist[0], "select"))
		rv = SelectCmd (numargs, arglist);
	    else if (!strcmp (arglist[0], "unselect"))
		rv = UnselectCmd (numargs, arglist);
	    else if (!strcmp (arglist[0], "list"))
		rv = ListCmd ();
	    else if (!strcmp (arglist[0], "monthly"))
		rv = MonthlyCmd ();
	    else if (!strcmp (arglist[0], "total"))
		rv = TotalCmd ();
	    else if (!strcmp (arglist[0], "report"))
		rv = ReportCmd (numargs, arglist);
	    else if (!strcmp (arglist[0], "spread"))
		rv = SpreadCmd (numargs, arglist);
	    else if (!strcmp (arglist[0], "init"))
		rv = InitCmd ();
	    else if (!strcmp (arglist[0], "help"))
		rv = HelpCmd ();
	    else if (!strcmp (arglist[0], "current"))
		rv = CurrentCmd (numargs, arglist);
	    else if (!strcmp (arglist[0], "recent"))
		rv = RecentCmd (numargs, arglist);
	    else if (!strcmp (arglist[0], "quit"))
		break;
	    else if (!strcmp (arglist[0], "bye"))
		break;
	    else if (!strcmp (arglist[0], "die"))
		break;
	    else if (!strcmp (arglist[0], ""))
		continue;
	    else
		fprintf (stderr, "'%s' is unknown command\n", arglist[0]);
	}
	fprintf (stderr, "> ");
	fflush (stderr);
    }

    max = 0;
    for (c='a'; c<='z'; c++) {
	i = chartotal(c);
	if (i > max) max = i;
    }
    fprintf (stderr, "Max char count = %d.\n", max);
    fprintf (stderr, "Session done.\n");
}



int
GetArgs (char inputline[], char arglist[MAXARGS][MAXLINESIZE])
{
    int i, j;
    int argno = 0;

    i = 0;
    j = 0;
    while (inputline[i] == ' ') i++;

    while (inputline[i] != '\000')
    {
	if (inputline[i] == ' ')
	{
	    arglist[argno][j] = '\000';
	    while (inputline[i] == ' ') i++;
	    argno++;
	    j = 0;
	}
	arglist[argno][j++] = inputline[i++];
    }
    arglist[argno][j] = '\000';
    argno++;
    return (argno);
}


void
PrintSubTotals (char *s, int lngs, int appr, double l[])
{
    int j = 0;

    fprintf (outfile, "\n%-17s", s);
    fputs ("       ", outfile);
    fputs ("                         ", outfile);
    fprintf (outfile, " %6d", lngs);
    fprintf (outfile, " %4d", appr);
    while (fieldtable[j].fieldname != NULL)
	fprintf (outfile, " %6.1f", l[j++]);
    fputs ("\n", outfile);
}


void
PrintHeader ()
{
    int j = 0;

    fputs ("Date       ", outfile);
    fputs ("Type    ", outfile);
    fputs ("Ident  ", outfile);
    fputs ("From/to                  ", outfile);
    fputs (" lngs", outfile);
    fputs (" aprs", outfile);

    while (fieldtable[j].fieldname != NULL)
	fprintf (outfile, "%7s", fieldtable[j++].fieldname);
    fputs ("\n", outfile);
}


void
InitSubTotals (l)
    double l[];
{
    int i;

    for (i = 0; i < TOTALSIZE; i++)
	l[i] = 0.0;
}


int
InRange (i)
    int i;
{
    int j, k;
    int typeflag = 0;
    int identflag = 0;

    if ((
	(logbook[i].e_date.date_year > startdate.date_year)
	||
	((logbook[i].e_date.date_year == startdate.date_year) &&
	 (logbook[i].e_date.date_month > startdate.date_month))
	||
	((logbook[i].e_date.date_year == startdate.date_year) &&
	 (logbook[i].e_date.date_month == startdate.date_month) &&
	 (logbook[i].e_date.date_day >= startdate.date_day))
       )
       &&
       (
	(logbook[i].e_date.date_year < enddate.date_year)
	||
	((logbook[i].e_date.date_year == enddate.date_year) &&
	 (logbook[i].e_date.date_month < enddate.date_month))
	||
	((logbook[i].e_date.date_year == enddate.date_year) &&
	 (logbook[i].e_date.date_month == enddate.date_month) &&
	 (logbook[i].e_date.date_day <= enddate.date_day))
       ))
    {
	if (typematch)
	{
	    for (j = 0; j < typematch; j++)
		if (!strcmp (logbook[i].e_type, typelist[j]))
		{
		    typeflag = 1;
		    break;
		}
	} else
	    typeflag = 1;

	if (identmatch)
	{
	    for (k = 0; k < identmatch; k++)
		if (!strcmp (logbook[i].e_ident, identlist[k]))
		{
		    identflag = 1;
		    break;
		}
	} else
	    identflag = 1;
    }
    return (identflag && typeflag);
}

void
PrintRange ()
{
    int i;

    /*-----Print ranging dates-------------------------------------------*/

    if (startdate.date_year != 0) {
	printf ("starting date = ");
	PrintDate (stdout, startdate);
    }

    if (enddate.date_year != LARGENUMBER) {
	if (startdate.date_year != 0)
	    printf (",  ");
	printf ("ending date = ");
	PrintDate (stdout, enddate);
    }

    if ((startdate.date_year != 0) || (enddate.date_year != LARGENUMBER))
	printf ("\n");

    /*-----Print ranging types-------------------------------------------*/
    if (typematch > 0) {
	printf ("types =");
	for (i = 0; i < typematch; i++)
	    printf (" %s", typelist[i]);
	printf ("\n");
    }

    if (identmatch > 0) {
	printf ("idents =");
	for (i = 0; i < identmatch; i++)
	    printf (" %s", identlist[i]);
	printf ("\n");
    }
    printf ("\n");
}


int
InitCmd ()
{
    InitSearch ();
    fprintf (stderr, "Ranges initialized\n");
    return (0);
}


int
HelpCmd ()
{
    fprintf (stderr, "Commands are:\n");
    fprintf (stderr, "    range dte [start|stop] <date>  -- Restrict list to date range\n");
    fprintf (stderr, "    range typ <type>    -- Restrict list to aircraft type\n");
    fprintf (stderr, "    init                -- Initial field and range selections\n");
    fprintf (stderr, "    list                -- Print selected rows and entries\n");
    fprintf (stderr, "    select [<field>]*   -- Select fields to be listed\n");
    fprintf (stderr, "    unselect [<field>]* -- Delete fields to be listed\n");
    fprintf (stderr, "    report [<filename>] -- Print in report format\n");
    fprintf (stderr, "    spread [<filename>] -- Output logbook in SC format\n");
    fprintf (stderr, "    current             -- Print date and totals for IFR currency\n");
    fprintf (stderr, "    recent              -- list recent IFR flights\n");
    fprintf (stderr, "    [quit|bye|die]\n");
    return (0);
}


int
RecentCmd (int argno, char arglist[MAXARGS][MAXLINESIZE])
{
    int i, j;
    double totalACT = 0.0;
    double totalHDD = 0.0;

    if (argno != 1) {
	fprintf (stderr, "%s: wrong number of parameters\n", arglist[0]);
	return (-1);
    }

    /*--- Scan backwards to find at least 6 hours IFR time ---*/

    i = count;
    while ((i >= 0) && ((totalHDD + totalACT) < 6.0))
    {
	i--;
	totalACT += logbook[i].e_times[ACT];
	totalHDD += logbook[i].e_times[HDD];
    }

    /*--- Scan forward now and print these entries        ---*/

    printf ("   Recent IFR as of: ");
    PrintDate (stdout, logbook[i].e_date);
    printf ("\n");

    printf ("   %7.1f IFR hours (%7.1f actual, %7.1f hooded)\n",
	    totalACT + totalHDD, totalACT, totalHDD);

    for (; i <= count; i++)
	if (logbook[i].e_times[ACT] + logbook[i].e_times[HDD])
	{
	    if (dateselect) {
		printf ("dat=");
		PrintDate (stdout, logbook[i].e_date);
		printf ("  ");
	    }
	    if (identselect)
		printf ("idt=%s  ", logbook[i].e_ident);
	    if (typeselect)
		printf ("typ=%s  ", logbook[i].e_type);
	    if (fromtoselect)
		printf ("f/t=%s  ", logbook[i].e_fromto);
	    if (lngselect)
		printf ("lng=%d  ", logbook[i].e_lngs);
	    if (lngselect)
		printf ("apr=%d  ", logbook[i].e_apprchs);

	    j = 0;
	    while (fieldtable[j].fieldname != NULL)
	    {
		if (fieldtable[j].fieldselect)
		    printf ("%s=%3.1f  ",
			fieldtable[j].fieldname,
			logbook[i].e_times[fieldtable[j].fieldindex]);
		j++;
	    }
	    printf ("\n");
	}
    return (0);
}


int
CurrentCmd (int argno, char arglist[MAXARGS][MAXLINESIZE])
{
    int i;
    double totalACT = 0.0;
    double totalHDD = 0.0;

    if (argno != 1) {
	fprintf (stderr, "%s: wrong number of parameters\n", arglist[0]);
	return (-1);
    }

    i = count;
    while ((i >= 0) && ((totalHDD + totalACT) < 6.0))
    {
	i--;
	totalACT += logbook[i].e_times[ACT];
	totalHDD += logbook[i].e_times[HDD];
    }

    if ((totalHDD + totalACT) < 6.0) {
	printf ("   Only %7.1f IFR hours (%7.1f actual, %7.1f hooded) in log book\n",
		totalACT + totalHDD, totalACT, totalHDD);
    } else {
	printf ("   As of: ");
	PrintDate (stdout, logbook[i].e_date);
	printf ("\n");
	printf ("   %7.1f IFR hours (%7.1f actual, %7.1f hooded);\n",
		totalACT + totalHDD, totalACT, totalHDD);
	printf ("   on this this date %7.1f actual, %7.1f hooded\n",
	    logbook[i].e_times[ACT], logbook[i].e_times[HDD]);
		
    }
    return (0);
}



int
RangeCmd (int argno, char arglist[MAXARGS][MAXLINESIZE])
{
    /*if ((argno != 4) && (argno != 3)) {
	fprintf (stderr, "%s: wrong number of parameters\n", arglist[0]);
	return (-1);
    }*/

    if (!strcmp (arglist[1], "dte"))
    {
	if (!strcmp (arglist[2], "start"))
	    ProcessDate (arglist[3], &startdate);
	else if (!strcmp (arglist[2], "stop"))
	    ProcessDate (arglist[3], &enddate);
	else
	{
	    fprintf (stderr, "range dat: 3rd parm should be start/stop\n");
	    return (-1);
	}
    }
    else if (!strcmp (arglist[1], "typ"))
    {
	if (typematch >= MAXARGS)
	    fprintf (stderr, "range typ: too many patterns\n");
	else
	    strcpy (typelist[typematch++], arglist[2]);
    }
    else if (!strcmp (arglist[1], "idt"))
    {
	if (identmatch >= MAXARGS)
	    fprintf (stderr, "range idt: too many patterns\n");
	else
	    strcpy (identlist[identmatch++], arglist[2]);
    } else {
	fprintf (stderr, "range: can't use that field\n");
	return (-1);
    }

    return (0);
}



int
SelectCmd (int argno, char arglist[MAXARGS][MAXLINESIZE])
{
    int i;
    int parm;
    int found;

    if (argno < 2) {
	fprintf (stderr, "%s: wrong number of parameters\n", arglist[0]);
	return (-1);
    }

    for (parm = 1; parm < argno; parm++)
    {
	if (!strcmp (arglist[parm], "dte"))
	    dateselect = 1;
	else if (!strcmp (arglist[parm], "typ"))
	    typeselect = 1;
	else if (!strcmp (arglist[parm], "idt"))
	    identselect = 1;
	else if (!strcmp (arglist[parm], "f/t"))
	    fromtoselect = 1;
	else if (!strcmp (arglist[parm], "lng"))
	    lngselect = 1;
	else if (!strcmp (arglist[parm], "apr"))
	    aprselect = 1;
	else
	{
	    i = 0;
	    found = 0;
	    while (fieldtable[i].fieldname != NULL)
	    {
		if (!strcmp (arglist[parm], fieldtable[i].fieldname)) {
		    fieldtable[i].fieldselect = 1;
		    found = 1;
		    break;
		}
		i++;
	    }
	    if (!found) {
		fprintf (stderr, "%s: bad field name `%s'\n", arglist[0], arglist[parm]);
		return (-1);
	    }
	}
    }
    return (0);
}



int
UnselectCmd (int argno, char arglist[MAXARGS][MAXLINESIZE])
{
    int i;
    int parm;
    int found;

    if (argno < 2) {
	fprintf (stderr, "%s: wrong number of parameters\n", arglist[0]);
	return (-1);
    }

    for (parm = 1; parm < argno; parm++)
    {
	if (!strcmp (arglist[parm], "dte"))
	    dateselect = 0;
	else if (!strcmp (arglist[parm], "typ"))
	    typeselect = 0;
	else if (!strcmp (arglist[parm], "idt"))
	    identselect = 0;
	else if (!strcmp (arglist[parm], "f/t"))
	    fromtoselect = 0;
	else if (!strcmp (arglist[parm], "lng"))
	    lngselect = 0;
	else if (!strcmp (arglist[parm], "apr"))
	    aprselect = 0;
	else
	{
	    i = 0;
	    found = 0;
	    while (fieldtable[i].fieldname != NULL)
	    {
		if (!strcmp (arglist[parm], fieldtable[i].fieldname)) {
		    fieldtable[i].fieldselect = 0;
		    found = 1;
		    break;
		}
		i++;
	    }
	    if (!found) {
		fprintf (stderr, "%s: bad field name\n", arglist[0]);
		return (-1);
	    }
	}
    }
    return (0);
}


int
ReportCmd (int argno, char arglist[MAXARGS][MAXLINESIZE])
{
    int i, j;
    int page = 1;
    double linetotal[TOTALSIZE];
    double grandtotal[TOTALSIZE];
    int linelngs=0, grandlngs=0;
    int lineaprs=0, grandaprs=0;
    char pageno[16];

    if (argno == 1)
	outfile = stdout;
    else if (argno == 2)
    {
	if ((outfile = fopen (arglist[1], "w")) == NULL)
	    return (-1);
    } else {
	fprintf (stderr, "%s: too many parameters\n", arglist[0]);
	return (-1);
    }

    InitSubTotals (grandtotal);
    InitSubTotals (linetotal);
    PrintRange ();
    PrintHeader ();
    for (i=0; i < count; i++)
    {
	if (InRange (i))
	{
	    PrintDate (outfile, logbook[i].e_date);
	    fputs (" ", outfile);
	    fprintf (outfile, "%-8.8s", logbook[i].e_type);
	    fprintf (outfile, "%-7.7s", logbook[i].e_ident);
	    fprintf (outfile, "%-25.25s", logbook[i].e_fromto);
	    fprintf (outfile, "%5d", logbook[i].e_lngs);
	    fprintf (outfile, "%5d", logbook[i].e_apprchs);
	    linelngs += logbook[i].e_lngs;
	    grandlngs += logbook[i].e_lngs;
	    lineaprs += logbook[i].e_apprchs;
	    grandaprs += logbook[i].e_apprchs;

	    j = 0;
	    while (fieldtable[j].fieldname != NULL)
	    {
		fprintf (outfile, " %6.1f",
		    logbook[i].e_times[fieldtable[j].fieldindex]);
		linetotal[j]  += logbook[i].e_times[fieldtable[j].fieldindex];
		grandtotal[j] += logbook[i].e_times[fieldtable[j].fieldindex];
		j++;
	    }
	    fputs ("\n", outfile);

	    if ((logbook[i].e_endofpage) || (i == count-1))
	    {
		sprintf (pageno, "PAGE %3d", page);
		PrintSubTotals (pageno, linelngs, lineaprs, linetotal);
		InitSubTotals (linetotal);
		linelngs = 0;
		lineaprs = 0;
		PrintSubTotals ("TOTAL", grandlngs, grandaprs, grandtotal);
		if (i != count-1)
		{
		    fputs ("\n", outfile);
		    page++;
		    PrintHeader ();
		}
	    }
	}
    }
    PrintSubTotals ("GRAND TOTAL", grandlngs, grandaprs, grandtotal);
    return (0);
}


#define FIRSTROW 4

int
SpreadCmd (int argno, char arglist[MAXARGS][MAXLINESIZE])
{
    int i, j;
    int row;

    if (argno == 1)
	outfile = stdout;
    else if (argno == 2)
    {
	if ((outfile = fopen (arglist[1], "w")) == NULL)
	    return (-1);
    } else {
	fprintf (stderr, "%s: too many parameters\n", arglist[0]);
	return (-1);
    }

    row = FIRSTROW;
    for (i=0; i < count; i++)
    {
	if (InRange (i))
	{
	    fprintf (outfile, "leftstring %s%d = \"", "A", row);
	    PrintDate (outfile, logbook[i].e_date);
	    fputs ("\"\n", outfile);
	    Spread_String (outfile, "B", row, logbook[i].e_type);
	    Spread_String (outfile, "C", row, logbook[i].e_ident);
	    Spread_String (outfile, "D", row, logbook[i].e_fromto);
	    Spread_Int    (outfile, "E", row, logbook[i].e_lngs);

	    j = 0;
	    while (fieldtable[j].fieldname != NULL)
	    {
		if (logbook[i].e_times[fieldtable[j].fieldindex] != 0.0)
		    Spread_Double (outfile,
				   fieldtable[j].spreadcolumn,
				   row,
				   logbook[i].e_times[fieldtable[j].fieldindex]
				  );
		j++;
	    }
	    row++;
	}
    }
    row++;

    /*----Now, put out entries to put totals at the end of the sheet----*/
    Spread_Total (outfile, "E", row, FIRSTROW);
    j = 0;
    while (fieldtable[j].fieldname != NULL)
    {
	Spread_Total  (outfile, fieldtable[j].spreadcolumn, row, FIRSTROW);
	j++;
    }

    return (0);
}


int
ListCmd ()
{
    int i, j;

    PrintRange ();
    for (i=0; i < count; i++)
    {
	if (InRange (i))
	{
	    if (dateselect) {
		printf ("dat=");
		PrintDate (stdout, logbook[i].e_date);
		printf ("  ");
	    }
	    if (identselect)
		printf ("idt=%s  ", logbook[i].e_ident);
	    if (typeselect)
		printf ("typ=%s  ", logbook[i].e_type);
	    if (fromtoselect)
		printf ("f/t=%s  ", logbook[i].e_fromto);
	    if (lngselect)
		printf ("lng=%d  ", logbook[i].e_lngs);
	    if (aprselect)
		printf ("apr=%d  ", logbook[i].e_apprchs);

	    j = 0;
	    while (fieldtable[j].fieldname != NULL)
	    {
		if (fieldtable[j].fieldselect)
		{
		    if (!strcmp(fieldtable[j].fieldname, "gas$") ||
		        !strcmp(fieldtable[j].fieldname, "oil$"))
		    {
			printf ("%s=%6.2f  ",
			    fieldtable[j].fieldname,
			    logbook[i].e_times[fieldtable[j].fieldindex]);
		    } else {
			printf ("%s=%3.1f  ",
			    fieldtable[j].fieldname,
			    logbook[i].e_times[fieldtable[j].fieldindex]);
		    }
		}
		j++;
	    }
	    printf ("\n");
	}
    }
    return (0);
}


void
PrintDate (FILE *fp, struct daterec date)
{
    fprintf (fp, "%2d/%2d/%4d", date.date_month, date.date_day, date.date_year);
}


int
TotalCmd ()
{
    int i, j, k;
    int totallngs = 0;
    int totalaprs = 0;
    int totallegs = 0;
    double totals[TOTAL+1];

    PrintRange ();
    for (i=0; i <= TOTAL; i++)
	totals[i] = 0;

    for (i=0; i < count; i++)
    {
	if (InRange (i))
	{
	    totallngs += logbook[i].e_lngs;
	    totalaprs += logbook[i].e_apprchs;
	    totallegs += Count(logbook[i].e_fromto,'-');
	    j = 0;
	    while (fieldtable[j].fieldname != NULL)
	    {
		k = fieldtable[j].fieldindex;
		totals[k] += logbook[i].e_times[k];
		j++;
	    }
	}
    }

    if (lngselect)
	printf ("lngs=%d\n", totallngs);
    if (aprselect)
	printf ("aprs=%d\n", totalaprs);
    printf ("legs=%d\n", totallegs);

    j = 0;
    while (fieldtable[j].fieldname != NULL)
    {
	if (fieldtable[j].fieldselect)
	{
	    k = fieldtable[j].fieldindex;
	    if (!strcmp(fieldtable[j].fieldname, "gas$") ||
		!strcmp(fieldtable[j].fieldname, "oil$"))
	    {
		printf ("%s=%6.2f\n", fieldtable[j].fieldname, totals[k]);
	    } else {
		printf ("%s=%3.1f\n", fieldtable[j].fieldname, totals[k]);
	    }
	}
	j++;
    }
    return (0);
}


int
MonthlyCmd ()
{
    int i, j, k;
    int totallngs;
    int totalaprs;
    double totals[TOTAL+1];

    PrintRange ();

    totallngs = 0;
    totalaprs = 0;
    for (i=0; i <= TOTAL; i++)
	totals[i] = 0;

    for (i=0; i < count; i++)
    {
	if (InRange (i))
	{
	    totallngs += logbook[i].e_lngs;
	    j = 0;
	    while (fieldtable[j].fieldname != NULL)
	    {
		k = fieldtable[j].fieldindex;
		totals[k] += logbook[i].e_times[k];
		j++;
	    }
	}
    }

    if (lngselect)
	printf ("lngs=%d\n", totallngs);
    if (aprselect)
	printf ("lngs=%d\n", totalaprs);

    j = 0;
    while (fieldtable[j].fieldname != NULL)
    {
	if (fieldtable[j].fieldselect)
	{
	    k = fieldtable[j].fieldindex;
	    printf ("%s=%3.1f\n", fieldtable[j].fieldname, totals[k]);
	}
	j++;
    }
    return (0);
}
