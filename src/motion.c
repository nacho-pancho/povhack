#include "map.h"
#include "nethack.h"

void detect_motion(map_t* map, map_t* prev_map) {

  // motion is stored in a printable char: there are 25 possible movements
  // written using letters from A to Y
  // the character is computed as:  'A' + 5*(2+dy) + (2+dx)
  // where -2 <= dx,dy <= 2
  for (int y = 1; y < 22; ++y) {
    for (int x = 0; x < 80; ++x) {
      glyph_t* target = map_get_monster(map,x,y);
      int tg = target->code;
      int found = 0;
      if (!target->code) {
	target->dx = target->dy = target->tele = 0;
	continue;
      }
      //
      // we search for matching glyphs
      // this is more robust than chars
      //
      const glyph_t* prev = map_get_monster(prev_map,x,y);
      if (tg == prev->code) {
	target->dx = 0;
	target->dy = 0;
	target->tele = 0;
	continue;
      }
      
      // radius 1
      int dy0 = y > 0 ?  -1: 0;
      int dy1 = y < 21 ? +1: 0;
      int dx0 = x > 0 ?  -1: 0;
      int dx1 = x < 79 ? +1: 0;
      for (int dy = dy0; !found && (dy <= dy1); ++dy) {
	for (int dx = dx0; (dx <= dx1); ++dx) {
	  prev = map_get_monster(prev_map,x+dx,y+dy);
	  if (tg == prev->code) {
	    target->dy = -dy;
	    target->dx = -dx;
	    target->tele = 0;
	    found = 1;
	    break;
	  }
	}
      }
      if (found)
	continue;

      // radius 2
      dy0 = y > 1 ?  -2: 0;
      dy1 = y < 20 ? +2: 0;
      dx0 = x > 1 ?  -2: 0;
      dx1 = x < 78 ? +2: 0;
      for (int dy = dy0; !found && (dy <= dy1); ++dy) {
	for (int dx = dx0; (dx <= dx1); ++dx) {
	  prev = map_get_monster(prev_map,x+dx,y+dy);
	  if (tg == prev->code) {
	    target->dy = -dy;
	    target->dx = -dx;
	    target->tele = 0;
	    found = 1;
	    break;
	  }
	}
      } // radius 2 search
      //
      // where is this guy?
      // likely teleported
      if (!found) {
	//printf("not found! teleported?\n");
	target->tele = 1;
      }
    } // for each col
  } // for each row
} // end search
