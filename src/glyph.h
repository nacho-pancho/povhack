#ifndef GLYPH_H
#define GLYPH_H

#include <stdint.h>

typedef struct {
  uint16_t code;  // same as tiledata codes
  uint16_t flags; // taken directly from tiledata
  char ascii;     // plain ASCII character in console
  char color;     // color code (16 values) 
  char dx;        // horizontal displacement w.r.t. previous map
  char dy;        // horizontal displacement w.r.t. previous map
  char tele  ;    // jumped abruptly
} glyph_t;        // nethack pixel :)

void reset_glyph(glyph_t* g);

glyph_t create_glyph(int gcode, int gflags, int gchar, int gstyle);

#endif
