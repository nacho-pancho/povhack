#ifndef GLYPH_H
#define GLYPH_H

#include <stdint.h>

typedef struct {
  uint16_t code;  // same as tiledata codes
  uint16_t flags; // taken directly from tiledata
  char ascii;     // plain ASCII character in console
  char color;     // color code (16 values) 
  char dx;        // horizontal displacement w.r.t. previous frame
  char dy;        // horizontal displacement w.r.t. previous frame
} glyph_t;        // nethack pixel :)

void reset_glyph(glyph_t* g);


#endif
