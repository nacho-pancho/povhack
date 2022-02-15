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

window_t* window_init(int nr, int nc) {
  window_t* out = (window_t*) malloc(sizeof(window_t));
  out->ncols = nc;
  out->nrows = nr;
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

static void advance_cursor(window_t* w) {
  w->cx++;
  if (w->cx >= w->ncols) {
    w->cx = 0;
    w->cy++;
    if (w->cy >= w->nrows) {
      w->cy = 0;
    }
  }
}

void window_put(window_t* w, int c) {
  const int i = w->cx + w->ncols*w->cy;
  if (c >= 0x20) {
    w->ascii[i] = c;
    w->color[i] = w->cc;
    advance_cursor(w);
  } else {
    switch (c) {      
    case '\b': // backspace
      if (w->cx > 0) w->cx--;
      break;
    case '\t': // tab
      while (w->cx % 8) w->cx++;
      if (w->cx >= w->ncols) w->cx = 0;
      break;
    case '\n': // newline
      w->cx = 0;
      if (w->cy < (w->ncols - 1))
	w->cy++;
      break;
    case '\r': // carriage return
      w->cx = 0;
      break;
    case '\a': // bell
      break;
    default:
      printf("unhandled special ASCII char 0x%02x\n",c);
      break;
    }
  }
}


//------------------------------------------------------------

void window_dump(window_t* window, FILE* out) {
  hruler(out,window->ncols);
  for (int y = 0, i = 0; y < window->nrows; y++) {
    vruler(y,out);
    for (int x = 0; x < window->ncols; x++, i++) {
      fputc(window->ascii[i] >= 0x20 ? window->ascii[i] :' ', out);
    }
    vruler(y,out);
    fputc('\n',out);
  }
  hruler(out,window->ncols);
}

