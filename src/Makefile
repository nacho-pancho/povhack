OBJ= \
	motion.o \
	status.o \
	terminal.o \
	window.o \
	map.o \
	glyph.o \
	options.o \
	logging.o \
	util.o

all: povhack test

.c.o:
	gcc -Wall -g -c $<

povhack: $(OBJ) povhack.c Makefile
	gcc -Wall -o povhack povhack.c $(OBJ) -ltty -lm


test: test_terminal.c $(OBJ) Makefile
	gcc -Wall -O0 -g -o test test_terminal.c $(OBJ)

clean:
	rm -f *.o povhack
