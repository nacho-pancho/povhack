OBJ=nethack.o terminal.o frame.o glyph.o logging.o options.o  motion.o

.c.o:
	gcc -Wall -c $<

all: $(OBJ) povhack.c
	gcc -Wall -o povhack povhack.c $(OBJ) -ltty -lm

test:
	gcc -Wall -o test test_terminal.c terminal.c frame.c glyph.c

clean:
	rm -f *.o povhack
