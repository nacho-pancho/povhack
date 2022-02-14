#include <string.h>
#include <stdio.h>
#include "frame.h"

glyph_t* frame_get_dungeon(frame_t* f, int x, int y) {
  return f->dungeon + y*NH_COLS + x;
}

glyph_t* frame_get_object(frame_t* f, int x, int y) {
  return f->objects + y*NH_COLS + x;
}
glyph_t* frame_get_monster(frame_t* f, int x, int y) {
  return f->monsters + y*NH_COLS + x;
}
glyph_t* frame_get_effect(frame_t* f, int x, int y) {
  return f->effects + y*NH_COLS + x;
}
glyph_t* frame_get_text(frame_t* f, int x, int y) {
  return f->text + y*NH_COLS + x;
}

//------------------------------------------------------------

frame_t* frame_init() {
  frame_t* out = (frame_t*) malloc(sizeof(frame_t));
  out->ncols = NH_COLS;
  out->nrows = NH_ROWS;
  out->dungeon  = out->layers[DUNGEON_LAYER] =
    (glyph_t*) malloc(sizeof(glyph_t)*NH_COLS*NH_ROWS);
  out->objects  = out->layers[OBJECTS_LAYER] =
    (glyph_t*) malloc(sizeof(glyph_t)*NH_COLS*NH_ROWS);
  out->monsters = out->layers[MONSTERS_LAYER] =
    (glyph_t*) malloc(sizeof(glyph_t)*NH_COLS*NH_ROWS);
  out->effects  = out->layers[EFFECTS_LAYER] =
    (glyph_t*) malloc(sizeof(glyph_t)*NH_COLS*NH_ROWS);
  out->text     = out->layers[TEXT_LAYER] =
    (glyph_t*) malloc(sizeof(glyph_t)*NH_COLS*NH_ROWS);
  frame_reset(out);
  return out;
}

//------------------------------------------------------------

void frame_reset(frame_t* f) {
  for (int l = 0; l < NLAYERS; ++l) {
    memset(f->layers[l], 0, sizeof(glyph_t)*NH_COLS*NH_ROWS);
  }

  f->message[0] = 0;
  f->status1[0] = 0;
  f->status2[0] = 0;
  f->hero_x = -1;
  f->hero_y = -1;
  f->cursor_x = 0;
  f->cursor_y = 0;
  f->current_layer = TEXT_LAYER;
  reset_glyph(&f->current_glyph);
}

//------------------------------------------------------------

void frame_free(frame_t* f) {
  free(f->dungeon);
  free(f->objects);
  free(f->monsters);
  free(f->effects);
  free(f->text);
  free(f);
}

//------------------------------------------------------------

void frame_copy(frame_t* dst, const frame_t* src) {
  dst->hero_x = src->hero_x;
  dst->hero_y = src->hero_y;
  dst->number = src->number;
  dst->valid  = src->valid;
  dst->nrows  = src->nrows;
  dst->ncols  = src->ncols;
  dst->current_layer = src->current_layer;
  strcpy(dst->message,src->message);
  strcpy(dst->status1,src->status1);
  strcpy(dst->status1,src->status2);
  
  for (int l = 0; l < NLAYERS; ++l) {
    memcpy(dst->layers[l],src->layers[l],sizeof(glyph_t)*NH_ROWS*NH_COLS);
  }
}

//------------------------------------------------------------

int frame_valid(frame_t* f) {
  const int x = f->hero_x;
  const int y = f->hero_y;
  const glyph_t* g = frame_get_monster(f,x,y);
  return (g->code != 0); 
}

//------------------------------------------------------------

int frame_changed(const frame_t* a, const frame_t* b) {
  if (a->hero_x != b->hero_x) return 1;
  if (a->hero_y != b->hero_y) return 1;
  
  for (int l = 0; l < NLAYERS; ++l) {
    if (memcmp(a->layers[l],b->layers[l],sizeof(glyph_t)*NH_COLS*NH_ROWS)) return 1;
  }
  return 0;
}

//------------------------------------------------------------

void frame_write(frame_t* frame, FILE* out) {
  for (int y = 0; y < NH_ROWS; y++) {
    for (int x = 0; x < NH_COLS; x++) {
      glyph_t* g;
      fprintf(out,"%d %d ",x,y);
      g = frame_get_dungeon(frame,x,y);
      fprintf(out,"%d %d %d %d ",
	      g->code,g->flags,g->ascii,g->color);
      g = frame_get_object(frame,x,y);
      fprintf(out,"%d %d %d %d ",
	      g->code,g->flags,g->ascii,g->color);
      g = frame_get_monster(frame,x,y);
      fprintf(out,"%d %d %d %d %d %d ",
	      g->code,g->flags,g->ascii,g->color,g->dx,g->dy);
      g = frame_get_effect(frame,x,y);
      fprintf(out,"%d %d %d %d ",
	      g->code,g->flags,g->ascii,g->color);
      g = frame_get_text(frame,x,y);
      fprintf(out,"%u %u\n",
	      g->ascii,g->color);
    }
  }
}

#if 0
//------------------------------------------------------------

static void hruler(FILE* out) {
  fputc(' ',out);
  for (int j = 0; j < NH_COLS; j++) {
    fputc(j % 10 ? '-' : ('0'+j/10),out);
  }
  fputc('\n',out);
}

//------------------------------------------------------------

static void vruler(int i, FILE* out) {
  fputc(i % 10 ? '|' : ('0'+i/10),out);
}
#endif
//------------------------------------------------------------

void frame_dump(frame_t* frame, FILE* out) {
#if 0
  if (!out) return;
  char movkey[3][3] = { {'y','k','u'}, {'h','.','l'}, {'b','j','n'} }; 
  // ascii
  glyph_t* g = frame->glyphs;
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
  g = frame->glyphs;
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
  g = frame->glyphs;
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
  g = frame->glyphs;
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
  g = frame->glyphs;
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
  fprintf(out,"valid:%s\n",frame->valid? "YES": "NO");
  fprintf(out,"num:%d\n",frame->number);
  fprintf(out,"msg:%s\n",frame->message);
  fprintf(out,"st1:%s\n",frame->status1);
  fprintf(out,"st2:%s\n",frame->status2);
  fprintf(out,"hero at %2d, %2d\n",frame->hero_x,frame->hero_y);
#endif  
}
