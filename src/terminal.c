#include <ctype.h>
#include "terminal.h"

typedef struct {
  uint16_t par[7];
  uint8_t npar;
  uint8_t code;
} cmd_t;

#define ESC_CHAR '\033'
#define BEGIN_CHAR '['

static void print_cmd(cmd_t* cmd) {
  printf("ESC [ ");
  for (int i = 0; i < cmd->npar-1; i++) {
    printf("%d ; ",cmd->par[i]);
  }
  printf("%d ",cmd->par[cmd->npar-1]);
  putchar(cmd->code);
  putchar('\n');
}

static void cursor_up(frame_t* f,cmd_t* cmd) {
  printf("up: ");
  print_cmd(cmd);
  if (f->cursor_y > 0) f->cursor_y--;
}

static void cursor_down(frame_t* f,cmd_t* cmd) {
  printf("down: ");
  print_cmd(cmd);
  if (f->cursor_y < f->nrows-1) f->cursor_y++;
}

static void cursor_right(frame_t* f,cmd_t* cmd) {
  printf("right: ");
  print_cmd(cmd);
  if (f->cursor_x < f->ncols-1) f->cursor_x++;
}

static void cursor_left(frame_t* f,cmd_t* cmd) {
  printf("left: ");
  print_cmd(cmd);
  if (f->cursor_x > 0) f->cursor_x--;
}

static void cursor_goto(frame_t* f,cmd_t* cmd) {
  printf("goto: ");
  print_cmd(cmd);
  if (cmd->npar < 1) {
    f->cursor_y = 0;
    f->cursor_x = 0;
  } else if (cmd->npar < 2) {
    f->cursor_y = cmd->par[0] < 24 ? cmd->par[0] : 24;
    f->cursor_x = 0;
  } else {
    f->cursor_y = cmd->par[0] < 24 ? cmd->par[0] : 24;
    f->cursor_x = cmd->par[1] < 79 ? cmd->par[1] : 79;
  }
}

static void erase_display(frame_t* f,cmd_t* cmd) {
  printf("erase in display: ");
  print_cmd(cmd);
  if ((cmd->npar) == 0)
    cmd->par[0] = 0;
  switch (cmd->par[0]) {
  case 0: // erase from cursor to end
    printf("erase from cursor to end of display\n");
    break;
  case 1: // erase from begin to cursor
    printf("erase from beginning of display to cursor\n");
    break;
  case 2: // erase all
    printf("erase all\n");
    break;
  case 3: // erase saved lines
    printf("unhandled: erase saved display\n");
    break;
  }
}

static void erase_line(frame_t* f,cmd_t* cmd) {
  printf("erase in line: ");
  print_cmd(cmd);
  if ((cmd->npar) == 0)
    cmd->par[0] = 0;
  switch (cmd->par[0]) {
  case 0: // erase from cursor to end of line
    printf("erase line from cursor to end of line\n");
    break;
  case 1: // erase from begin of line to cursor
    printf("erase line from beginning to cursor\n");
    break;
  case 2: // erase all line
    printf("erase all\n");
    break;
  case 3: // erase saved lines
    printf("unhandled: erase saved lines\n");
    break;
  }
}

static void set_style(frame_t* f,cmd_t* cmd) {
  printf("set style: ");
  print_cmd(cmd);
  switch (cmd->par[0]) {
  case 0: // reset style
    f->current_glyph.color = 0;
    break;
  case 1: // bold
    f->current_glyph.color |= 0x08;
    break;
  case 22: // normal intensity
    f->current_glyph.color &= 0x07;
    break;
  case 23: // 
    f->current_glyph.color &= 0x07;
    break;
  case 30: case 31: case 32: case 33: //foreground color (8 colors)
  case 34: case 35: case 36: case 37:    
    f->current_glyph.color = f->current_glyph.color & 0xf8;
    f->current_glyph.color |= (cmd->par[0] - 30);
    break;
    // 40-47 are background color
  default:
    break;
  }
}

static void tiledata(frame_t* f,cmd_t* cmd) {
  printf("tiledata: ");
  print_cmd(cmd);
  int subcmd = cmd->par[0];
  int code = cmd->par[1];
  int flags = cmd->par[2];
  if (subcmd == '0') { // begin glyph
    f->current_glyph.flags = flags;
    f->current_glyph.code =  code;
  } else if (subcmd == '2') { // end frame!
    // do something important here!
  }
}

static void unhandled_command(frame_t* f,cmd_t* cmd) {
  printf("unhandled command:");
  print_cmd(cmd);
}

    
static int apply_cmd(frame_t* f, cmd_t* cmd) {
  switch(cmd->code) {
  case 'A':
    cursor_up(f,cmd);
    break;
  case 'B':
    cursor_down(f,cmd);
    break;
  case 'C':
    cursor_right(f,cmd);
    break;
  case 'D':
    cursor_left(f,cmd);
    break;
    //  case 'E':
    //    cursor_begin_line_down(f,cmd);
    //    break;
    //  case 'F':
    //    cursor_begin_line_up(f,cmd);
    //    break;
    //  case 'G':
    //    cursor_goto_col(f,cmd);
    //    break;
  case 'H':
    cursor_goto(f,cmd);
    break;
  case 'J':
    erase_display(f,cmd);
    break;
  case 'K':
    erase_line(f,cmd);
    break;
  case 'm':
    set_style(f,cmd);
    break;  
  case 'z':
    tiledata(f,cmd);
    break;  
  default: // unhandled
    unhandled_command(f,cmd);
    return 0; 
  }
  return 0;
}

static void advance_cursor(frame_t* f) {
  f->cursor_x++;
  if (f->cursor_x >= f->ncols) {
    f->cursor_x = 0;
    f->cursor_y++;
    if (f->cursor_y >= f->nrows) {
      f->cursor_y = 0;
    }
  }
}

#include <assert.h>

static void frame_write_glyph(frame_t* f, int c) {
  assert(f->current_layer < NLAYERS);
  assert(f->cursor_y >= 0);
  assert(f->cursor_x >= 0);
  assert(f->cursor_y < NH_ROWS);
  assert(f->cursor_x < NH_COLS);
  f->current_glyph.ascii = c;
  f->layers[f->current_layer][f->cursor_x+f->cursor_y*f->ncols] = f->current_glyph;
}

void frame_write_char(frame_t* f, int c) {
  if (c < 0) // EOF
    return;

  static cmd_t cmd;
  static int c1 = 0, c0 = 0;
  static int within_command = 0;
  static int parval;

  if (c < 0) return;
  // update state
  c1 = c0; c0 = c;
  
  if (!within_command) {
    if (c0 == ESC_CHAR) {
      return;
    } if ((c1 == ESC_CHAR) && (c0 == BEGIN_CHAR)) {
      // begin command
      within_command = 1;
      cmd.code = 0;
      cmd.npar = 0;
      parval = 0;
    } else {
      // not a command,  print the char in the corresponding layer
      if (c < 33) 
	printf("write char 0x%2x\n",c);
      else
	printf("write char  %c\n",c);
      frame_write_glyph(f,c);
      advance_cursor(f);      
    }
  } else { // within command
      if ( isalpha(c) ) {
	// end of command
	cmd.par[cmd.npar++] = parval; 
	cmd.code = c;
	within_command = 0;
	// apply command
	apply_cmd(f,&cmd);
      } else if (c == ';') {
	// finished a new parameter of the command
	cmd.par[cmd.npar++] = parval;
	parval = 0;
      } else if ( isdigit(c)) {
	// new  parameter digit
	parval = parval*10 + c-'0'; // update param value
      } else if ( c == '?' ) {
	// ignore extended command starting with [?
      } else {
	printf("unable to interpret sequence. ESC %c\n",c);
      }
  }
}
