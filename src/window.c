#include <string.h>
#include <stdio.h>
#include "window.h"
#include "util.h"

//------------------------------------------------------------

char* window_get_ascii(window_t* f, int x, int y) {
  return f->ascii + y*f->ncols + x;
}

//------------------------------------------------------------

char* window_get_color(window_t* f, int x, int y) {
  return f->color + y*f->ncols + x;
}


//------------------------------------------------------------

window_t* window_init() {
  window_t* out = (window_t*) malloc(sizeof(window_t));
  out->ncols = MAX_WIN_COLS;
  out->nrows = MAX_WIN_ROWS;
  const int numel = out->ncols*out->nrows;
  out->ascii = malloc(sizeof(char)*numel);
  out->color = malloc(sizeof(char)*numel);
  window_reset(out);
  return out;
}

//------------------------------------------------------------

void window_reset(window_t* f) {
  const int numel = f->ncols*f->nrows;
  memset(f->ascii, 0x20, sizeof(char)*numel);
  memset(f->color, 0, sizeof(char)*numel);
  f->cx = 0;
  f->cy = 0;
  f->cc = 0; 
}

//------------------------------------------------------------

void window_free(window_t* f) {
  free(f->ascii);
  free(f->color);
  free(f);
}

//------------------------------------------------------------

void window_copy(window_t* dst, const window_t* src) {
  const int dst_len = dst->ncols*dst->nrows;
  const int src_len = src->ncols*src->nrows;
  dst->nrows  = src->nrows;
  dst->ncols  = src->ncols;
  if (dst_len != src_len) {
    dst->ascii = (char*) realloc(dst->ascii,src_len);
    dst->color = (char*) realloc(dst->color,src_len);
  }
  memcpy(dst->ascii,src->ascii,sizeof(char)*src_len);
  memcpy(dst->color,src->color,sizeof(char)*src_len);
}

//------------------------------------------------------------

void window_put(window_t* window, int c) {
  const int i = window->cx + window->ncols*window->cy;
  window->ascii[i] = c;
  window->color[i] = window->cc;
}

//------------------------------------------------------------

void window_dump(window_t* window, FILE* out) {
  hruler(out,window->ncols);
  for (int y = 0, i = 0; y < window->nrows; y++) {
    vruler(y,out);
    for (int x = 0; x < window->ncols; x++, i++) {
      fputc(window->ascii[i] >= ' '? window->ascii[i]:'.',out);
    }
    vruler(y,out);
    fputc('\n',out);
  }
  hruler(out,window->ncols);
}

