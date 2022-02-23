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


clean:
	rm -f *.o povhack
