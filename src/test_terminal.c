#include <stdio.h>
#include "map.h"
#include "terminal.h"
#include <string.h>

typedef struct {
  int level;
  int coins;
  int hp;
  int maxhp;
  int pw;
  int maxpw;
  int ac;
  int xplevel;
  int xppoints;
  int moves;
} status_t;

status_t get_status(terminal_t* term) {
  status_t s;
  window_t* win = term->windows[WIN_STATUS];
  char* status = win->ascii + 24*win->ncols;
  status[win->ncols-1] = 0; // make this line a propoer null ended C string
  printf("status:%s\n",status);
  sscanf(status," Dlvl:%d $:%d HP:%d(%d) Pw:%d(%d) AC:%d Xp:%d/%d T:%d ",
	 &s.level, &s.coins, &s.hp, &s.maxhp, &s.pw, &s.maxpw, &s.ac, &s.xplevel,&s.xppoints,&s.moves);
  return s;
}

void print_status(status_t* s) {
  printf("level     : %d\n",s->level);
  printf("coins     : %d\n",s->coins);
  printf("health    : %d of %d\n",s->hp, s->maxhp);
  printf("power     : %d of %d\n",s->pw, s->maxpw);
  printf("xp. level : %d\n",s->xplevel);
  printf("xp. points: %d\n",s->xppoints);
  printf("game moves: %d\n",s->moves);
} 

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
      printf("\nMAP\n");
      map_dump(term->map,stdout);
      // reset windows??
      status_t s = get_status(term);
      print_status(&s);
      term->data_has_ended = 0;
      frame_number++;
    }
  }
  terminal_free(term);
  return 0;
}
