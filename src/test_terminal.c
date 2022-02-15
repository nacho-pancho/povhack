#include <stdio.h>
#include "map.h"
#include "terminal.h"
#include <string.h>

int get_game_time(terminal_t* term) {
  window_t* win = term->windows[WIN_STATUS];
  char* status = win->ascii + 24*win->ncols;
  status[win->ncols-1] = 0; // make this line a propoer null ended C string
  char* off = strstr(status,"T:");
  if (!off) { return -1; }
  else {
    int pepe;
    sscanf(off+2,"%d ",&pepe);
    return pepe;
  }
}

int main(int argc, char* argv[]) {
  terminal_t* term = terminal_init();
  int frame_number = 0;
  while (!feof(stdin)) {
    int c = fgetc(stdin);
    terminal_put(term,c);
    if (term->data_has_ended) {
      // dump current windows
      int game_time = get_game_time(term);
      printf("\n====================\n");
      printf("END OF DATA: FRAME %4d TIME %4d\n",frame_number,game_time);
      printf("====================\n");
      for (int w = 1; w <= WIN_INVEN; ++w) {
	printf("\nWINDOW NUMBER %4d\n",w);
	window_dump(term->windows[w],stdout);
      }
      // reset windows??      
      term->data_has_ended = 0;
      frame_number++;
    }
  }
  terminal_free(term);
  return 0;
}
