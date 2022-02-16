#ifndef FRAME_H
#define FRAME_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "glyph.h"

#define NLAYERS 4
#define DUNGEON_LAYER 0
#define OBJECTS_LAYER 1
#define MONSTERS_LAYER 2
#define EFFECTS_LAYER 3

typedef struct {
  int16_t number; // map number
  uint16_t ncols; // size 
  uint16_t nrows; // size
  
  int16_t hero_x; // current position of hero
  int16_t hero_y; // current position of hero
  //
  // thera are 5 layers that make up the scene
  //
  glyph_t* layers[NLAYERS];

  glyph_t* dungeon; // fixed stuff: floor, walls, ladders,.
  glyph_t* objects; // objects lying around 
  glyph_t* monsters; // mosters that move
  glyph_t* effects; // especial effects like arrows, explosions, etc.
} map_t;

map_t* map_init(int nr, int nc);

glyph_t* map_get_dungeon(map_t* f, int x, int y);
glyph_t* map_get_object(map_t* f, int x, int y);
glyph_t* map_get_monster(map_t* f, int x, int y);
glyph_t* map_get_effect(map_t* f, int x, int y);

void map_free(map_t* f);
void map_reset(map_t* f);
void map_copy(map_t* dst, const map_t* src);
void map_dump(map_t* map, FILE* out);
void map_put(map_t* map, int x, int y, int gcode, int gflags, int gchar, int gcolor);
void map_write(map_t* map, FILE* out);
void map_set_hero_position(map_t* map, int x, int y);
int map_changed(const map_t* a, const map_t* b);
int map_valid(map_t* f);

#endif
