#ifndef FRAME_H
#define FRAME_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "glyph.h"

#define NH_COLS 80
#define NH_ROWS 24

typedef struct {
  char valid; // valid game frame (may be message only frame)
  int16_t number; // frame number
  glyph_t* glyphs;
  uint16_t ncols;
  uint16_t nrows;
  int16_t hero_i; // row where our hero is located
  int16_t hero_j; // col. ...
  char message[NH_COLS+1];
  char status1[NH_COLS+1];
  char status2[NH_COLS+1];
} frame_t;

#define frget( t, i, j ) ( ( t )->glyphs[ ( i ) * NH_COLS + ( j ) ] )

frame_t* frame_init();
void frame_free(frame_t* f);
void frame_reset(frame_t* f);
void frame_copy(frame_t* dst, const frame_t* src);
void frame_dump(frame_t* frame, FILE* out);
void frame_write(frame_t* frame, FILE* out);
int frame_changed(const frame_t* a, const frame_t* b);
int frame_valid(const frame_t* f);

#endif
