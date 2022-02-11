#ifndef VT100_H
#define VT100_H
#include <tty.h>
#include "frame.h"
// 
// general ANSI/VT100 command
// ESC[par1;par2;...;parn<letter>
//
#define VT_CMD_TILEDATA     'z'
#define VT_CMD_NONE          0

typedef struct {
  uint16_t par[7];
  uint8_t npar;
  uint8_t cmd;
} vt_command_t;

#define ESC_CHAR '\033'
#define BEGIN_CHAR '['

#define vtget( t, i, j ) ( ( t )->scr[ ( i ) * NH_COLS + ( j ) ] )

#define is_cmd(v) ( v != 0 && isalpha((v)->cmd))

vt_command_t* intercept_command(int c);

char vtattr_to_color(uint64_t attr);

void tty_dump(tty vt, FILE* out);

void tty_to_frame(tty vt, frame_t* frame);


#endif
