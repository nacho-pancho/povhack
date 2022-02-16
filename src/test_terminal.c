#include <stdio.h>
#include <string.h>
#include "map.h"
#include "terminal.h"
#include "status.h"

int main(int argc, char* argv[]) {
  terminal_t* term = terminal_init();
  int frame_number = 0;
  while (!feof(stdin)) {
    int c = fgetc(stdin);
    terminal_put(term,c);
    if (term->data_has_ended) {
      // dump current windows
      status_t s = get_status(term);
      printf("\n====================\n");
      printf("END OF DATA: FRAME %4d TIME %4d\n",frame_number,s.moves);
      printf("====================\n");
      print_status(&s);
      for (int w = 1; w <= WIN_INVEN; ++w) {
	printf("\nWINDOW NUMBER %4d\n",w);
	window_dump(term->windows[w],stdout);
      }
      printf("\nMAP\n");
      map_dump(term->map,stdout);
      // reset windows??
      term->data_has_ended = 0;
      frame_number++;
    }
  }
  terminal_free(term);
  return 0;
}
