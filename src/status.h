#ifndef STATUS_H
#define STATUS_H

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

status_t get_status(terminal_t* term);

void print_status(status_t* s);

#endif
