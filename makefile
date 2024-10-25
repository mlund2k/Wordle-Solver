wordle: lund_wordle.c
	gcc -g lund_wordle.c -o wordle
EXE = wordle
OBJECTS = $(SOURCES:%.c=%.0)
.PHONY: clean
clean:
	-rm -f $(EXE)
	-rm -f $(OBJECTS)
