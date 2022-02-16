#include <stdio.h>
#include <string.h>
#include "map.h"
#include "terminal.h"
#include "status.h"
#include "motion.h"

int main(int argc, char* argv[]) {
  terminal_t* term = terminal_init();
  int frame_number = 0;
  map_t* prev_map;
  prev_map = map_init(25,80);
  while (!feof(stdin)) {
    int c = fgetc(stdin);
    terminal_put(term,c);
    if (term->data_has_ended) {
      // dump current windows
      status_t s = get_status(term);
      printf("\n====================\n");
      printf("END OF DATA: FRAME %4d TIME %4d\n",frame_number,s.moves);
      printf("====================\n");
      print_status(&s,stdout);
      detect_motion(term->map,prev_map);
      for (int w = 1; w <= WIN_INVEN; ++w) {
	printf("\nWINDOW NUMBER %4d\n",w);
	window_dump(term->windows[w],stdout);
      }
      printf("\nMAP\n");
      map_dump(term->map,stdout);
      //
      // move on
      //
      map_copy(prev_map,term->map);
      term->data_has_ended = 0;
      frame_number++;
    }
  }
  terminal_free(term);
  map_free(prev_map);
  return 0;
}
