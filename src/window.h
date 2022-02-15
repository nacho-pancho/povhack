#ifndef WINDOW_H
#define WINDOW_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_WIN_COLS 120
#define MAX_WIN_ROWS 25

#define NWINDOWS 7
#define WIN_MESSAGE_0 0 // used at the beginning and when damage is inflicted?
#define WIN_MESSAGE   1 // most of the messages 
#define WIN_STATUS    2
#define WIN_MAP       3
#define WIN_MENU      4
#define WIN_DIALOG    5 // more like dialog

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

window_t* window_init();
void window_free(window_t* f);
void window_reset(window_t* f);
void window_copy(window_t* dst, const window_t* src);

void window_put(window_t* window, int c);

void window_dump(window_t* window, FILE* out);

#endif
