#include <string.h>
#include "glyph.h"

void reset_glyph(glyph_t* g) {
  memset(g,0,sizeof(glyph_t));
}
