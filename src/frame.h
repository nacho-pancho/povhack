#ifndef FRAME_H
#define FRAME_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "glyph.h"

#define NH_COLS 80
#define NH_ROWS 24

typedef struct {
  // meta info
  char valid; // valid game frame (may be message only frame)
  int16_t number; // frame number
  uint16_t ncols; // size 
  uint16_t nrows; // size
  int16_t hero_x; // current position of hero
  int16_t hero_y; // current position of hero

  //
  // thera are 5 layers that make up the scene
  //
  glyph_t* dungeon; // fixed stuff: floor, walls, ladders,.
  glyph_t* objects; // objects lying around 
  glyph_t* monsters; // mosters that move
  glyph_t* effects; // especial effects like arrows, explosions, etc.
  glyph_t* text;    // text shown to the player
  
  char message[NH_COLS+1];
  char status1[NH_COLS+1];
  char status2[NH_COLS+1];
} frame_t;

frame_t* frame_init();

glyph_t* frame_get_dungeon(frame_t* f, int x, int y);
glyph_t* frame_get_object(frame_t* f, int x, int y);
glyph_t* frame_get_monster(frame_t* f, int x, int y);
glyph_t* frame_get_effect(frame_t* f, int x, int y);
glyph_t* frame_get_text(frame_t* f, int x, int y);

void frame_free(frame_t* f);
void frame_reset(frame_t* f);
void frame_copy(frame_t* dst, const frame_t* src);
void frame_dump(frame_t* frame, FILE* out);
void frame_write(frame_t* frame, FILE* out);
int frame_changed(const frame_t* a, const frame_t* b);
int frame_valid(frame_t* f);

#endif
