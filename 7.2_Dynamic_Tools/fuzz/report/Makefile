#
# Program to generate report from my attr/value file format
#
CC = gcc
OPT = -O0
OPTLEVEL = -g
PROFILE = -fno-omit-frame-pointer --sanitize=address
SCAN_BUILD_DIR = scan-build-out

report:	report.o subrs.o search.o spread_subrs.o countchar.o
	$(CC) $(OPT) $(OPTLEVEL) $(PROFILE) report.o subrs.o search.o spread_subrs.o countchar.o -o report

report.o:	report.c report.h
	$(CC) -g -Wextra -Wall -pedantic $(OPT) $(OPTLEVEL) $(PROFILE) -c report.c

spread_subrs.o:	spread_subrs.c report.h
	$(CC) -g -Wextra -Wall $(OPT) $(OPTLEVEL) $(PROFILE) -c spread_subrs.c

subrs.o:	subrs.c report.h
	$(CC) -g -Wextra -Wall $(OPT) $(OPTLEVEL) $(PROFILE) -c subrs.c

search.o:	search.c report.h
	$(CC) -g $(OPT) $(OPTLEVEL) $(PROFILE) -c search.c

countchar.o:	countchar.c report.h
	$(CC) -g $(OPT) $(OPTLEVEL) $(PROFILE) -c countchar.c

clean:
	rm -f report *.o

scan-build: clean
	scan-build -o $(SCAN_BUILD_DIR) make

scan-view: scan-build
	firefox -new-window $(SCAN_BUILD_DIR)/*/index.html
