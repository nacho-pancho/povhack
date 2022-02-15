#include <string.h>
#include <stdio.h>
#include "map.h"

glyph_t* map_get_map(map_t* f, int x, int y) {
  return f->map + y*f->ncols + x;
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
  out->map  = out->layers[DUNGEON_LAYER]      = (glyph_t*) malloc(nbytes);
  out->objects  = out->layers[OBJECTS_LAYER]  = (glyph_t*) malloc(nbytes);
  out->monsters = out->layers[MONSTERS_LAYER] = (glyph_t*) malloc(nbytes);
  out->effects  = out->layers[EFFECTS_LAYER] =  (glyph_t*) malloc(nbytes);
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
  free(f->map);
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
int infer_layer(glyph_t g) {
  return DUNGEON_LAYER;
}

void map_put(map_t* m, int x, int y, int gcode, int gflags, int gchar, int gcolor) {
  const int i = x + m->ncols*y;
  glyph_t g = create_glyph(gcode,gflags,gchar,gcolor);
  const int L = infer_layer(g);
  m->layers[L][i] = g;
}

//------------------------------------------------------------

void map_set_hero_position(map_t* map, int x, int y) {
  map->hero_x = x;
  map->hero_y = y;
}

//------------------------------------------------------------
