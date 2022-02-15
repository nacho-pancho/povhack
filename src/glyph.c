#include <string.h>
#include "glyph.h"

void reset_glyph(glyph_t* g) {
  memset(g,0,sizeof(glyph_t));
}

glyph_t create_glyph(int gcode, int gflags, int gchar, int gstyle) {
  glyph_t g;
  g.code = gcode;
  g.flags = gflags;
  g.ascii = gchar;
  g.color = gstyle;
  g.dx    = 0;
  g.dy    = 0;
  g.tele  = 0;
  return g;
}
