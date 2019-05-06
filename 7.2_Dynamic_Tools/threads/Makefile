CC = gcc
WARNING_FLAGS = -Wall -Wextra
EXE = prodcomm
OPT = -g -O0 -fno-omit-frame-pointer --sanitize=thread
SCAN_BUILD_DIR = scan-build-out

all: main.o queue.o
	$(CC) $(CCFLAGS) $(OPT) -o $(EXE) main.o queue.o -lpthread

main.o: main.c queue.h
	$(CC) $(CCFLAGS) $(WARNING_FLAGS) $(OPT) -c main.c

queue.o: queue.c queue.h
	$(CC) $(CCFLAGS) $(WARNING_FLAGS) $(OPT) -c queue.c

clean:
	rm -f $(EXE) *.o
