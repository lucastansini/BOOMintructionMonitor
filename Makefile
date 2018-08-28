CC=gcc
SOURCEFILE=src/instMonitor.c
FLAGS=-o
OUTPUT=instMonitor

all:
		$(CC) $(FLAGS) $(OUTPUT) $(SOURCEFILE)
