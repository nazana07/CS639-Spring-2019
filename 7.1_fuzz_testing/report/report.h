#define MAXENTRIES	100000
#define MAXLINESIZE	1024
#define MAXARGS		16

#define FIELDSIZE	64

#define TOTALSIZE 100

#define SEL	0
#define HEL	1
#define X_C	2
#define DAY	3
#define NGT	4
#define ACT	5
#define HDD	6
#define SIM	7
#define DUL	8
#define PIC	9
#define SIC	10
#define GAS_GALS	11
#define GAS_DOLS	12
#define OIL_QTS	13
#define OIL_DOLS	14
#define TOTAL	15

struct fields
{
    char *fieldname;
    int   fieldindex;
    int   fieldselect;
    char *spreadcolumn;
};

struct daterec
{
    int	date_day;
    int	date_month;
    int	date_year;
};

struct entry
{
    struct daterec e_date;
    char   	   e_type[16];
    char	   e_ident[16];
    char	   e_fromto[FIELDSIZE];
    int		   e_lngs;
    int		   e_nightlngs;
    int		   e_apprchs;
    double	   e_times[TOTAL+1];
    int		   e_endofpage;
};
