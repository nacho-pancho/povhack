#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "map.h"
#include "util.h"
#include "logging.h"

glyph_t* map_get(map_t* f, int l, int x, int y) {
  return f->layers[l] + y*f->ncols + x;
}

glyph_t* map_get_dungeon(map_t* f, int x, int y) {
  return f->dungeon + y*f->ncols + x;
}

glyph_t* map_get_object(map_t* f, int x, int y) {
  return f->objects + y*f->ncols + x;
}
glyph_t* map_get_monster(map_t* f, int x, int y) {
  return f->monsters + y*f->ncols + x;
}
glyph_t* map_get_effect(map_t* f, int x, int y) {
  return f->effects + y*f->ncols + x;
}

//------------------------------------------------------------

map_t* map_init(int nr, int nc) {
  map_t* out = (map_t*) malloc(sizeof(map_t));
  out->ncols = nc;
  out->nrows = nr;
  const int nbytes = out->ncols*out->nrows*sizeof(glyph_t);
  out->dungeon  = out->layers[DUNGEON_LAYER]  = (glyph_t*) malloc(nbytes);
  out->objects  = out->layers[OBJECTS_LAYER]  = (glyph_t*) malloc(nbytes);
  out->monsters = out->layers[MONSTERS_LAYER] = (glyph_t*) malloc(nbytes);
  out->effects  = out->layers[EFFECTS_LAYER]  = (glyph_t*) malloc(nbytes);
  map_reset(out);
  return out;
}

//------------------------------------------------------------

void map_reset(map_t* f) {
  for (int l = 0; l < NLAYERS; ++l) {
    memset(f->layers[l], 0, sizeof(glyph_t)*f->ncols*f->nrows);
  }

  f->hero_x = -1;
  f->hero_y = -1;
}

//------------------------------------------------------------

void map_free(map_t* f) {
  free(f->dungeon);
  free(f->objects);
  free(f->monsters);
  free(f->effects);
  free(f);
}

//------------------------------------------------------------

void map_copy(map_t* dst, const map_t* src) {
  dst->hero_x = src->hero_x;
  dst->hero_y = src->hero_y;
  dst->number = src->number;
  // dangerous! this only works because maps don't change size
  dst->nrows  = src->nrows;
  dst->ncols  = src->ncols;
  
  for (int l = 0; l < NLAYERS; ++l) {
    memcpy(dst->layers[l],src->layers[l],sizeof(glyph_t)*src->nrows*src->ncols);
  }
}


//------------------------------------------------------------

int is_empty(glyph_t* g) {
  return (g->code == 0);
}

//------------------------------------------------------------

int is_monster(glyph_t* g) {
  char c = g->ascii;
  char sym[] = {'@','~','&','\'',':',';',0};
  if (isalpha(c)) return 1;
  for (int i = 0; sym[i] != 0; ++i) {
    if (c == sym[i]) return 1;
  }
  return 0;
}

//------------------------------------------------------------

int is_structure(glyph_t* g) {
  char sym[] = {'.','|','-','+','#','<','>','_','}','{','^',0};
  char c = g->ascii;
  for (int i = 0; sym[i] != 0; ++i) {
    if (c == sym[i]) return 1;
  }
  // other less clear-cut
  // " web
  // \ throne
  // | grave
  return 0;
}

//------------------------------------------------------------

int is_effect(glyph_t* g) {
  return 0;
}

//------------------------------------------------------------
int is_object(glyph_t* g) {
  char sym[] = {'"','[','0','`','$','%','*','!','=','?','(','/',')',0};
  char c = g->ascii;
  for (int i = 0; sym[i] != 0; ++i) {
    if (c == sym[i]) return 1;
  }
  // less clear-cut, requires style info
  // + spellbook as long as it's not orange
  // _ iron chain
  // 
  return 0;
}

//------------------------------------------------------------
int is_door(map_t* map, int x, int y) {
  // an unbroken door is a white | or - character
  // open doors
  glyph_t* g = map_get_dungeon(map,x,y);
  if ((g->ascii == '-') && ((g->color & 0x07) == 3)) {
    return V_DOOR_OPEN;
  }
  if ((g->ascii == '|') && ((g->color & 0x07) == 3)) {
    return H_DOOR_OPEN;
  }
  
  // closed doors
  if ((g->ascii == '+') && ((g->color & 0x07) == 3)) {
    if (is_wall(map,x,y-1) || is_wall(map,x,y+1)) {
      return V_DOOR_CLOSED;
    } else if (is_wall(map,x-1,y) || is_wall(map,x+1,y)) {
      return H_DOOR_CLOSED;
    } else {
      // cannot rely on walls; try floor
      char n = map_get_dungeon(map,x,y-1)->ascii;
      char s = map_get_dungeon(map,x,y+1)->ascii;
      char w = map_get_dungeon(map,x-1,y)->ascii;
      char e = map_get_dungeon(map,x+1,y)->ascii;
      if ((n == '#') || (s == '#')) {
	return H_DOOR_CLOSED;
      } else if ((e == '#') || (w == '#')) {
	return V_DOOR_CLOSED;
      } else {
	warn("cannot determine orientation of door n=%c s=%c w=%c e=%c\n", n, s, w, e);
	return NO_DOOR;
      }
    }
  }
  // broken door
  if (g->ascii == '.') {
    if (is_wall(map,x,y-1) && is_wall(map,x,y+1)) {
      return V_DOOR_BROKEN;
    } else if (is_wall(map,x-1,y) && is_wall(map,x+1,y)) {
      return H_DOOR_BROKEN;
    } else {
      return NO_DOOR;
    }
  }
  return NO_DOOR;
}

//------------------------------------------------------------

int is_wall(map_t* map, int x, int y) {
  glyph_t* g = map_get_dungeon(map,x,y);
  // must be a white | or - character
  if ((g->color & 0x08) != 0)
    return NO_WALL;
  if (g->ascii == '-')
    return H_WALL;
  else if (g->ascii == '|')
    return V_WALL;
  else
    return NO_WALL;
}

//------------------------------------------------------------


int is_corner(map_t* map, int x, int y) {
  glyph_t* g = map_get_dungeon(map,x,y);
  // basic check
  if ((g->ascii != '-') || (g->color & 0x08) != 0) 
    return NO_CORNER;
  // 
  
  int nwall = is_wall(map,x,y-1);
  int swall = is_wall(map,x,y+1);
  if (!nwall && !swall) {
    return NO_CORNER;
  }
  // it is a corner
  // now try to determine orientation
  if (nwall && swall) { // all-sides corner
    return ALL_CORNER;
  } else if (swall) { // a wall to the south
    // either NW or NE corner
    if (is_wall(map,x+1,y) || is_door(map,x+1,y)) {
      return NW_CORNER;
    } else {
      return NE_CORNER;
    }
  } else { // a wall to the north
    // either SW or SE corner
    if (is_wall(map,x+1,y) || is_door(map,x+1,y)) {
      return SW_CORNER;
    } else {
      return SE_CORNER;
    }
  }
}

//------------------------------------------------------------
int infer_layer(glyph_t* g) {
  if (is_structure(g)) {
    return DUNGEON_LAYER;
  } else if (is_object(g)) {
    return OBJECTS_LAYER;
  } else if (is_monster(g)) {
    return MONSTERS_LAYER;
  } else if (is_effect(g)) {
    return EFFECTS_LAYER;
  } else {
    return DUNGEON_LAYER;
  }
}

//------------------------------------------------------------

void map_put(map_t* m, int x, int y, int gcode, int gflags, int gchar, int gcolor) {
  const int i = x + m->ncols*y;
  glyph_t g = create_glyph(gcode,gflags,gchar,gcolor);
  const int L = infer_layer(&g);
  // writing to one layer overwrites higher layers
  // so if we write a structure (e.g., floor), there is no longer a monster
  // or anything else there, so we erase whatever was in that position
  // at the lower layers
  //
  // on the other hand, if we write a monster, we don't want to destroy
  // the floor that was there before.
  //
  m->layers[L][i] = g;
   
  for (int l = L+1; l < NLAYERS; l++) {
    reset_glyph(&m->layers[l][i]);
  }
}

//------------------------------------------------------------

void map_set_hero_position(map_t* map, int x, int y) {
  map->hero_x = x;
  map->hero_y = y;
}

//------------------------------------------------------------

void map_dump(map_t* map, FILE* out) {
  for (int l = 0; l < NLAYERS; ++l) {
    fprintf(out,"LAYER %d\n",l);
    hruler(out,map->ncols);
    for (int y = 0; y < map->nrows; y++) {
      vruler(y,out);
      for (int x = 0; x < map->ncols; x++) {
	glyph_t* g = map_get(map,l,x,y);
	fputc(g->ascii >= 0x20 ? g->ascii :' ', out);
      }
      vruler(y,out);
      fputc('\n',out);
    }
    hruler(out,map->ncols);  
  }
}

//------------------------------------------------------------

void map_dump_motion(map_t* map, FILE* out) {
  hruler(out,map->ncols);
  for (int y = 0; y < map->nrows; y++) {
    vruler(y,out);
    for (int x = 0; x < map->ncols; x++) {
      glyph_t* g = map_get(map,MONSTERS_LAYER,x,y);
      if (g->code == 0) {
	fputc(' ',out);
      } else {
	const int dx = g->dx > 0 ? 1: (g->dx < 0 ? -1 : 0);
	const int dy = g->dy > 0 ? 1: (g->dy < 0 ? -1 : 0);
	const char sym[3][3] = { {'y','k','u'}, {'h','.','l'}, {'b','j','n'} };
	fputc(sym[dy+1][dx+1], out);
      }
    }
    vruler(y,out);
    fputc('\n',out);
  }
  hruler(out,map->ncols);  
}
