#include "terminal.h"
#include "status.h"
#include <string.h>
#include <stdio.h>

status_t get_status(terminal_t* term) {
  status_t s = {0,0,0,0,0,0,0,0,0};
  window_t* win = term->windows[WIN_STATUS];
  char* status = win->ascii + 24*win->ncols;
  status[win->ncols-1] = 0; // make this line a propoer null ended C string
  sscanf(status," Dlvl:%d $:%d HP:%d(%d) Pw:%d(%d) AC:%d Xp:%d/%d T:%d ",
	 &s.level, &s.coins, &s.hp, &s.maxhp, &s.pw, &s.maxpw, &s.ac, &s.xplevel,&s.xppoints,&s.moves);
  return s;
}

void print_status(status_t* s, FILE* out) {
  fprintf(out,"level     : %d\n",s->level);
  fprintf(out,"coins     : %d\n",s->coins);
  fprintf(out,"health    : %d of %d\n",s->hp, s->maxhp);
  fprintf(out,"power     : %d of %d\n",s->pw, s->maxpw);
  fprintf(out,"xp. level : %d\n",s->xplevel);
  fprintf(out,"xp. points: %d\n",s->xppoints);
  fprintf(out,"game moves: %d\n",s->moves);
} 
