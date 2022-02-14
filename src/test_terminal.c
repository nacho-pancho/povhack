#include <stdio.h>
#include "frame.h"
#include "terminal.h"

int main(int argc, char* argv[]) {
  frame_t* frame = frame_init();
  while (!feof(stdin)) {
    int c = fgetc(stdin);
    frame_write_char(frame,c);
  }
  frame_free(frame);
  return 0;
}
