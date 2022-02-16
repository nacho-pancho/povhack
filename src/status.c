#include "terminal.h"
#include "status.h"
#include <string.h>

status_t get_status(terminal_t* term) {
  status_t s;
  window_t* win = term->windows[WIN_STATUS];
  char* status = win->ascii + 24*win->ncols;
  status[win->ncols-1] = 0; // make this line a propoer null ended C string
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
