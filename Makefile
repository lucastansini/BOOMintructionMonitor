CC=gcc
SOURCEFILE=src/instMonitor.c
FLAGS=-Wall -o
OUTPUT=instMonitor

all:
		$(CC) $(FLAGS) $(OUTPUT) $(SOURCEFILE)

clean:
		rm $(OUTPUT)
