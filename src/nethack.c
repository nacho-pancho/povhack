#include <ctype.h>

static int is_monster(char c) {
  char sym[] = {'@','~','&','\'',':',';',0};
  if (isalpha(c)) return 1;
  for (int i = 0; sym[i] != 0; ++i) {
    if (c == sym[i]) return 1;
  }
  return 0;
}

static int is_structure(char c) {
  char sym[] = {'|','-','+',0};
  for (int i = 0; sym[i] != 0; ++i) {
    if (c == sym[i]) return 1;
  }
  return 0;
}
