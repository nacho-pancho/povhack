#include <stdio.h>
#include "map.h"
#include "terminal.h"

int main(int argc, char* argv[]) {
  terminal_t* term = terminal_init();
  while (!feof(stdin)) {
    int c = fgetc(stdin);
    terminal_put(term,c);
    if (term->data_has_ended) {
      // dump current windows
      printf("\n====================\n");
      printf("END OF DATA\n");
      printf("====================\n");
      for (int w = 0; w < NWINDOWS; ++w) {
	printf("\nWINDOW NUMBER %4d\n",w);
	window_dump(term->windows[w],stdout);
      }
      term->data_has_ended = 0;
    }
  }
  terminal_free(term);
  return 0;
}
