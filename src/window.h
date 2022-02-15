#ifndef WINDOW_H
#define WINDOW_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define NWINDOWS      10

#define WIN_BASE      0 // special 'base' window, for 'absolute screen' positioning
#define WIN_MESSAGE   1 // most of the messages 
#define WIN_STATUS    2 // status
#define WIN_MAP       3 // these don't get absolute positioning, apparently
#define WIN_INVEN     4 // inventory

typedef struct {
  int nrows;
  int ncols;
  char* ascii; // characters
  char* color; // their style (only color for now)
  int cx; // cursor x
  int cy; // cursor y
  int cc; // current style (color)
} window_t;


char* window_get_ascii(window_t* f, int x, int y);
char* window_get_color(window_t* f, int x, int y);

window_t* window_init(int nr, int nc);
void window_free(window_t* f);
void window_reset(window_t* f);
void window_copy(window_t* dst, const window_t* src);

void window_put(window_t* window, int c);

void window_dump(window_t* window, FILE* out);

#endif
