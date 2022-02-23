#ifndef WINDOW_H
#define WINDOW_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "nethack.h"


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
/**
 * for debugging, with extra stuff around
 */
void window_dump(window_t* window, FILE* out);
/**
 * for actual storage; each character is printed with two bytes: the ascii character, and the color as a hex '0-9A-F'
 */
void window_save(window_t* window, FILE* out);

#endif
