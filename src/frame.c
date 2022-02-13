#include <string.h>
#include <stdio.h>
#include "frame.h"

glyph_t* frame_get(frame_t* f, int x, int y) {
  return f->glyphs + y*NH_COLS + x;
}

//------------------------------------------------------------

frame_t* frame_init() {
  frame_t* out = (frame_t*) malloc(sizeof(frame_t));
  out->ncols = NH_COLS;
  out->nrows = NH_ROWS;
  out->glyphs = (glyph_t*) malloc(sizeof(glyph_t)*NH_COLS*NH_ROWS);
  out->hero_x = 0;
  out->hero_y = 0;
  out->number = 0;
  out->valid = 0;
  return out;
}

//------------------------------------------------------------

void frame_reset(frame_t* f) {
  for (int i = 0; i < NH_COLS*NH_ROWS; ++i) {
    f->glyphs[i].ascii = 0x20; // space
    f->glyphs[i].flags = 0;
    f->glyphs[i].code = 0;
    f->glyphs[i].color = 0;
    f->glyphs[i].dx    = 0;
    f->glyphs[i].dy    = 0;
    f->glyphs[i].tele  = 1;
    f->message[0] = 0;
    f->status1[0] = 0;
    f->status2[0] = 0;
    f->hero_x = -1;
    f->hero_y = -1;
  }
}

//------------------------------------------------------------

void frame_free(frame_t* f) {
  free(f->glyphs);
  free(f);
}

//------------------------------------------------------------

void frame_copy(frame_t* dst, const frame_t* src) {
  // override everything BUT the dynamic pointer!
  glyph_t* dst_ptr = dst->glyphs;
  memcpy(dst,src,sizeof(frame_t));
  dst->glyphs = dst_ptr;
  memcpy(dst->glyphs,src->glyphs,sizeof(glyph_t)*NH_ROWS*NH_COLS);
}

//------------------------------------------------------------

int frame_valid(frame_t* f) {
  const int x = f->hero_x;
  const int y = f->hero_y;
  const glyph_t* g = frame_get(f,x,y);
  printf("valid?: x=%d y=%d a=%d c=%d\n",x,y,g->ascii,g->code);
  return (g->code != 0); 
}

//------------------------------------------------------------

int frame_changed(const frame_t* a, const frame_t* b) {
  if (a->hero_x != b->hero_x) return 1;
  if (a->hero_y != b->hero_y) return 1;
  return memcmp(a->glyphs+NH_COLS,b->glyphs+NH_COLS,sizeof(glyph_t)*NH_COLS*(NH_ROWS-4));
}

//------------------------------------------------------------

void frame_write(frame_t* frame, FILE* out) {
  glyph_t* g = frame->glyphs;
  for (int y = 0; y < NH_ROWS; y++) {
    for (int x = 0; x < NH_COLS; x++, g++) {
      fprintf(out,"%d %d %u %u %d %d %d %d\n",x,y,
	      g->code,g->flags,g->ascii,g->color,g->dx,g->  dy);
    }
  }
}


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

//------------------------------------------------------------

void frame_dump(frame_t* frame, FILE* out) {
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
}
