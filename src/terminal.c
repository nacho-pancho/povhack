#include <ctype.h>
#include "terminal.h"
#include "logging.h"

//#define DEBUG_TERM

//
// NetHack TileData command info
//
#define NH_CMD_START_GLYPH    0
#define NH_CMD_END_GLYPH      1
#define NH_CMD_SELECT_WINDOW  2
#define NH_CMD_END_FRAME      3

#define is_start_glyph(v) (((v)->cmd == VT_CMD_TILEDATA) && ((v)->par[1] == NH_CMD_START_GLYPH))
#define is_end_glyph(v) (((v)->cmd == VT_CMD_TILEDATA) && ((v)->par[1] == NH_CMD_END_GLYPH))
#define is_end_map(v) (((v)->cmd == VT_CMD_TILEDATA) && ((v)->par[1] == NH_CMD_END_FRAME))

terminal_t* terminal_init() {
  terminal_t* out = (terminal_t*) malloc(sizeof(terminal_t));
  out->current_window = 0;
  for (int w = 0; w < NWINDOWS; ++w) {
    const int ncols = w != WIN_MAP ? 120: 80;
    const int nrows = 25;
    out->windows[w] = window_init(nrows,ncols);
  }
  out->map = map_init(25,80);
  terminal_reset(out);
  return out;
}

void terminal_free(terminal_t* f) {
  map_free(f->map);
  for (int w = 0; w < NWINDOWS; ++w) {
    window_free(f->windows[w]);
  }
  free(f);
}

void terminal_reset(terminal_t* f) {
  f->current_window = 0;
  f->current_glyph_code = 0;
  f->current_glyph_flags = 0;
  f->data_has_ended = 0;
  f->is_glyph = 0;
  for (int w = 0; w < NWINDOWS; ++w) {
    window_reset(f->windows[w]);
  }
  map_reset(f->map);
}

void terminal_copy(terminal_t* dst, const terminal_t* src) {
  dst->current_window      = src->current_window;
  dst->current_glyph_code  = src->current_glyph_code;
  dst->current_glyph_flags = src->current_glyph_flags;
  dst->data_has_ended      = src->data_has_ended;
  dst->is_glyph            = src->is_glyph;
  for (int w = 0; w < NWINDOWS; ++w) {
    window_copy(dst->windows[w],dst->windows[w]);
  }
  map_copy(dst->map,src->map);
}

typedef struct {
  int16_t par[7];
  int16_t npar;
  int16_t code;
  char string[32]; // long enough!
  int16_t len;
} cmd_t;

#define ESC_CHAR '\033'
#define BEGIN_CHAR '['

#define CW(t) ((t)->windows[(t)->current_window])

static void print_cmd(cmd_t* cmd) {
  if (!level_xdebug())
    return;
  printf("ESC [ ");
  for (int i = 0; i < cmd->npar; i++) {
    printf("%d ",cmd->par[i]);
    if (i < (cmd->npar-1)) {
      putchar(';');
    }
  }
  putchar(cmd->code);
  putchar('\n');
}

static void cursor_up(terminal_t* f,cmd_t* cmd) {
 
  xdebug("up: ");
    print_cmd(cmd);

  window_t* w = CW(f);
  if (w->cy > 0) w->cy--;
}

static void cursor_down(terminal_t* f,cmd_t* cmd) {

  xdebug("down: ");
  print_cmd(cmd);

  window_t* w = CW(f);
  if (w->cy < (w->nrows-1)) w->cy++;
}

static void cursor_right(terminal_t* f,cmd_t* cmd) {

  xdebug("right: ");
  print_cmd(cmd);

  window_t* w = CW(f);
  if (w->cx < (w->ncols-1)) w->cx++;
}

static void cursor_left(terminal_t* f,cmd_t* cmd) {

  xdebug("left: ");
  print_cmd(cmd);

  window_t* w = CW(f);
  if (w->cx > 0) w->cx--;
}

static void cursor_goto(terminal_t* f,cmd_t* cmd) {

  xdebug("goto: ");
  print_cmd(cmd);

  window_t* w = CW(f);
  if (cmd->npar == 0) {
    w->cy = 0;
    w->cx = 0;
  } else if (cmd->npar == 2) {
    w->cy = cmd->par[0] < w->nrows ? cmd->par[0] : w->nrows-1;
    w->cx = cmd->par[1] < w->ncols ? cmd->par[1] : w->ncols-1;
  } else {
    warn("invalid goto command\n");
  }
}

static void erase_win(window_t* w, int start, int end) {
  for (int i = start; i < end; ++i) {
    w->ascii[i] = 0x20;
    w->color[i] = 0x00;
  }
}

static void erase_map(map_t* f, int start, int end) {
  for (int i = start; i < end; ++i) {
    for (int l = 0; l < NLAYERS; ++l) {
      reset_glyph(&f->layers[l][i]);
    }
  }
}

static void erase_display(terminal_t* f,cmd_t* cmd) {

  xdebug("erase in display: ");
  print_cmd(cmd);

  if ((cmd->npar) == 0)
    cmd->par[0] = 0;
  window_t* w = CW(f);
  const int i0 = 0;
  const int ic = w->cx+w->ncols*w->cy;
  const int i1 = w->nrows*w->ncols;

  switch (cmd->par[0]) {
  case 0: // erase from cursor to end

    xdebug("erase from cursor to end of display\n");
    
    erase_win(w,ic,i1);
    if (f->current_window == WIN_MAP) {
      erase_map(f->map,ic,i1);
    }
    break;
  case 1: // erase from begin to cursor

    xdebug("erase from beginning of display to cursor\n");

    erase_win(w,i0,ic);
    if (f->current_window == WIN_MAP) {
      erase_map(f->map,i0,ic);
    }
    break;
  case 2: // erase all

    xdebug("erase all\n");

    erase_win(w,i0,i1);
    w->cx = w->cy = w->cc = 0;
    if (f->current_window == WIN_MAP) {
      erase_map(f->map,i0,i1);
    }
    break;
  case 3: // erase saved lines
    return;
  default:
    return;
  }
}

static void erase_line(terminal_t* f,cmd_t* cmd) {

  xdebug("erase in line: ");
  print_cmd(cmd);

  if ((cmd->npar) == 0)
    cmd->par[0] = 0;

  window_t* w = CW(f);
  const int i0 = w->cy*w->ncols;
  const int ic = i0 + w->cx;
  const int i1 = i0 + w->ncols;
  switch (cmd->par[0]) {
  case 0: // erase from cursor to end of line

    xdebug("erase line from cursor to end of line\n");

    erase_win(w,ic,i1);
    if (f->current_window == WIN_MAP) {
      erase_map(f->map,ic,i1);
    }
    break;
  case 1: // erase from begin of line to cursor
    erase_win(w,i0,ic);
    if (f->current_window == WIN_MAP) {
      erase_map(f->map,i0,ic);
    }
    break;
  case 2: // erase all line

    xdebug("erase all\n");

    erase_win(w,i0,i1);
    if (f->current_window == WIN_MAP) {
      erase_map(f->map,i0,i1);
    }
    break;
  case 3: // erase saved lines
    return;
  default:
    return;
  }
}

static void set_style(terminal_t* f,cmd_t* cmd) {

  xdebug("set style: ");
  print_cmd(cmd);

  window_t* w = CW(f);
  switch (cmd->par[0]) {
  case 0: // reset style
    w->cc = 0;
    break;
  case 1: // bold
    w->cc |= 0x08;
    break;
  case 22: // normal intensity
    w->cc &= 0x07;
    break;
  case 23: // 
    w->cc &= 0x07;
    break;
  case 30: case 31: case 32: case 33: //foreground color (8 colors)
  case 34: case 35: case 36: case 37:    
    w->cc = (w->cc & 0xf8) | (cmd->par[0] - 30);
    break;
    // 40-47 are background color
  default:
    break;
  }
}

/**
 * signals that a screen has finished drawing and
 * the user has control
 */ 
static void end_data(terminal_t* f) {

  window_t* w = CW(f);
  if (f->current_window == WIN_MAP) {
    map_set_hero_position(f->map,w->cx,w->cy);
  }
  f->data_has_ended = 1;
  f->is_glyph = 0;
}

static void tiledata(terminal_t* f,cmd_t* cmd) {

  xdebug("tiledata: ");
  print_cmd(cmd);
  
  int subcmd = cmd->par[1];
  int code = cmd->par[2];
  int flags = cmd->par[3];
  if (subcmd == 0) { // begin glyph

    xdebug("begin glyph\n");
  
    f->is_glyph = 1;
    f->current_glyph_code = code;
    f->current_glyph_flags = flags;
  } else if (subcmd == 1) { // end glyph
    f->is_glyph = 0;

    xdebug("end glyph\n");
  
  } else if (subcmd == 2) { // switch to window

    xdebug("select window %d\n",cmd->par[2]);
    //if (cmd->par[2] > 0) // window 0 is some kind of 'global' window
    f->current_window = cmd->par[2];
  } else if (subcmd == 3) { // end map!
    end_data(f);
  }
}

static void unhandled_command(terminal_t* f,cmd_t* cmd) {
  warn("unhandled command: %s \n",cmd->string);
  print_cmd(cmd);
}

    
static int apply_cmd(terminal_t* f, cmd_t* cmd) {
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

#include <assert.h>


void terminal_put(terminal_t* f, int c) {
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
      cmd.len = 2;
      cmd.string[0] = 'E';
      cmd.string[1] = '[';
      parval = -1; // no parameters yet
    } else {
      // not a command,  print the char in the corresponding layer
      if (f->is_glyph) {
	// we rely on the map window for status
	// the map is not a window
	if (f->current_window != WIN_MAP) {
	  xdebug("should be in map window!");
	} else {
	  // must be map window to work
	  window_t* w = CW(f);
	  // coordinates are drawn from window
	  const int x = w->cx;
	  const int y = w->cy;
	  // char from current char
	  const char gchar = c;
	  // flags from previous 'start glyph' command
	  const uint16_t gflags = f->current_glyph_flags;
	  const uint16_t gcode  = f->current_glyph_code;
	  const uint16_t gstyle = w->cc;

	  xdebug("write glyph code=0x%04x flags=0x%04x char=%c style=0x%02x\n",
		 gcode,gflags,gchar,gstyle);
	  
	  map_put(f->map,x,y,gcode,gflags,gchar,gstyle);
	}	
      }
      //
      // put char
      // this may be a regular char, or a special char
      // if it is a special char, it might move the cursor
      window_t* w = CW(f);

      xdebug("put char %c\n",c);

      window_put(w,c);
    }
  } else { // within command
    cmd.string[cmd.len++] = c;
      if ( isalpha(c) ) {
	// end of command
	if (parval >=0 ) // trailing parameter value
	  cmd.par[cmd.npar++] = parval;  // 
	cmd.code = c;
	within_command = 0;
	// apply command
	cmd.string[cmd.len] = 0;
	apply_cmd(f,&cmd);
      } else if (c == ';') {
	// finished a new parameter of the command, a new parameter comes
	cmd.par[cmd.npar] = parval;
	cmd.npar++;
	parval = 0;
      } else if ( isdigit(c)) {
	if (parval < 0) // begin a numeric parameter
	  parval = 0; 
	// new  parameter digit
	parval = parval*10 + c-'0'; // update param value
      } else if ( c == '?' ) {
	// ignore extended command starting with [?
      } else {
	cmd.string[cmd.len] = 0;
	warn("unable to interpret command:. ESC %s\n",cmd.string);
      }
  }
}
