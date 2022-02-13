#include <ctype.h>
#include "terminal.h"

typedef struct {
  uint16_t par[7];
  uint8_t npar;
  uint8_t code;
} cmd_t;

#define ESC_CHAR '\033'
#define BEGIN_CHAR '['

cmd_t* intercept_command(int c) {
  static cmd_t cmd;
  static int c1 = 0, c0 = 0;
  static int within_command = 0;
  static int parval;
  // update state
  if (c < 0) return 0;
  c1 = c0; c0 = c;
  if (!within_command) {
    if ((c1 == ESC_CHAR) && (c0 == BEGIN_CHAR)) {
      within_command = 1;
      cmd.code = 0;
      cmd.npar = 0;
      parval = 0;
    }
    return 0;
  } else { // within command
    if ( isalpha(c) ) { // end of command
      cmd.par[cmd.npar++] = parval; 
      cmd.code = c;
      within_command = 0;
      return &cmd;
    } else if (c == ';') {
      cmd.par[cmd.npar++] = parval;
      parval = 0;
      return 0;
    } else if ( isdigit(c)) { 
      parval = parval*10 + c-'0'; // update param value
      return 0;
    } else if (c == '?') {
      // ignore extended command
      within_command = 0;
      return 0;
    } else {
      fprintf(stderr,"not a valid ANSI command.! Offending character is %c (%d), context c1 %c\n",c,c,c1);
      within_command = 0;
      return 0;
    }
  }
}


void frame_write_char(frame_t* f, char c) {
  cmd_t* cmd = intercept_command(c);
  if (cmd) {
  } else {
    
  }
}
