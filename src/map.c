#include <string.h>
#include <stdio.h>
#include "map.h"

glyph_t* map_get_map(map_t* f, int x, int y) {
  return f->map + y*NH_COLS + x;
}

glyph_t* map_get_object(map_t* f, int x, int y) {
  return f->objects + y*NH_COLS + x;
}
glyph_t* map_get_monster(map_t* f, int x, int y) {
  return f->monsters + y*NH_COLS + x;
}
glyph_t* map_get_effect(map_t* f, int x, int y) {
  return f->effects + y*NH_COLS + x;
}

//------------------------------------------------------------

map_t* map_init() {
  map_t* out = (map_t*) malloc(sizeof(map_t));
  out->ncols = NH_COLS;
  out->nrows = NH_ROWS;
  out->map  = out->layers[DUNGEON_LAYER] =
    (glyph_t*) malloc(sizeof(glyph_t)*NH_COLS*NH_ROWS);
  out->objects  = out->layers[OBJECTS_LAYER] =
    (glyph_t*) malloc(sizeof(glyph_t)*NH_COLS*NH_ROWS);
  out->monsters = out->layers[MONSTERS_LAYER] =
    (glyph_t*) malloc(sizeof(glyph_t)*NH_COLS*NH_ROWS);
  out->effects  = out->layers[EFFECTS_LAYER] =
    (glyph_t*) malloc(sizeof(glyph_t)*NH_COLS*NH_ROWS);
  map_reset(out);
  return out;
}

//------------------------------------------------------------

void map_reset(map_t* f) {
  for (int l = 0; l < NLAYERS; ++l) {
    memset(f->layers[l], 0, sizeof(glyph_t)*NH_COLS*NH_ROWS);
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
  dst->nrows  = src->nrows;
  dst->ncols  = src->ncols;
  
  for (int l = 0; l < NLAYERS; ++l) {
    memcpy(dst->layers[l],src->layers[l],sizeof(glyph_t)*NH_ROWS*NH_COLS);
  }
}

//------------------------------------------------------------

int map_valid(map_t* f) {
  const int x = f->hero_x;
  const int y = f->hero_y;
  const glyph_t* g = map_get_monster(f,x,y);
  return (g->code != 0); 
}

//------------------------------------------------------------

int map_changed(const map_t* a, const map_t* b) {
  if (a->hero_x != b->hero_x) return 1;
  if (a->hero_y != b->hero_y) return 1;
  
  for (int l = 0; l < NLAYERS; ++l) {
    if (memcmp(a->layers[l],b->layers[l],sizeof(glyph_t)*NH_COLS*NH_ROWS)) return 1;
  }
  return 0;
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

void map_write(map_t* map, FILE* out) {
  for (int y = 0; y < NH_ROWS; y++) {
    for (int x = 0; x < NH_COLS; x++) {
    }
  }
}

//------------------------------------------------------------

void map_dump(map_t* map, FILE* out) {
#if 0
  if (!out) return;
  char movkey[3][3] = { {'y','k','u'}, {'h','.','l'}, {'b','j','n'} }; 
  // ascii
  glyph_t* g = map->glyphs;
  fprintf(out,"chars:\n");
  hruler(out);
  for (int i = 0; i < NH_ROWS; i++) {
    vruler(i,out);
    for (int j = 0; j < NH_COLS; j++, g++) {
      fputc(g->ascii,out);
    }
    vruler(i,out);
    fputc('\n',out);
  }
  fprintf(out,"movement:\n");
  g = map->glyphs;
  hruler(out);
  for (int i = 0; i < NH_ROWS; i++) {
    vruler(i,out);
    for (int j = 0; j < NH_COLS; j++, g++) {
      const int dx =  g->dx > 0 ? 1: (g->dx < 0 ? -1 : 0);
      const int dy =  g->dy > 0 ? 1: (g->dy < 0 ? -1 : 0);
      fputc(movkey[1+dy][1+dx],out);
    }
    vruler(i,out);
    fputc('\n',out);
  }
  // style
  fprintf(out,"color:\n");
  g = map->glyphs;
  hruler(out);
  for (int i = 0; i < NH_ROWS; i++) {
    vruler(i,out);
    for (int j = 0; j < NH_COLS; j++, g++) {
      int color = g->color;
      fputc(color > 9? 'A'+ color: '0' + color,out);
    }
    vruler(i,out);
    fputc('\n',out);
  }
  // glyph (only first 4 bits)
  g = map->glyphs;
  fprintf(out,"glyphs:\n");
  hruler(out);
  for (int i = 0; i < NH_ROWS; i++) {
    vruler(i,out);
    for (int j = 0; j < NH_COLS; j++, g++) {
      int code = g->code & 0x000f;
      fputc(code > 9? 'A'+ code: '0' + code,out);
    }
    vruler(i,out);
    fputc('\n',out);
  }  
  // flags (only first 4 bits
  g = map->glyphs;
  fprintf(out,"flags:\n");
  hruler(out);
  for (int i = 0; i < NH_ROWS; i++) {
    vruler(i,out);
    for (int j = 0; j < NH_COLS; j++, g++) {
      int flags = g->flags & 0x000f;
      fputc(flags > 9? 'A'+ flags: '0' + flags,out);
    }
    vruler(i,out);
    fputc('\n',out);
  }  
  hruler(out);
  fprintf(out,"valid:%s\n",map->valid? "YES": "NO");
  fprintf(out,"num:%d\n",map->number);
  fprintf(out,"msg:%s\n",map->message);
  fprintf(out,"st1:%s\n",map->status1);
  fprintf(out,"st2:%s\n",map->status2);
  fprintf(out,"hero at %2d, %2d\n",map->hero_x,map->hero_y);
#endif  
}
