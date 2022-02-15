#include "util.h"
//------------------------------------------------------------

void hruler(FILE* out, int ncols) {
  fputc(' ',out);
  for (int j = 0; j < ncols; j++) {
    fputc(j % 10 ? '-' : ('0'+j/10),out);
  }
  fputc('\n',out);
}

//------------------------------------------------------------

void vruler(int i, FILE* out) {
  fputc(i % 10 ? '|' : ('0'+i/10),out);
}
