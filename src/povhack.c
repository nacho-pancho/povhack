#include <stdio.h>
#include <tty.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define NH_COLS 80
#define NH_ROWS 24

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

//
// NetHack TileData command info
//
#define NH_CMD_START_GLYPH    0
#define NH_CMD_END_GLYPH      1
#define NH_CMD_SELECT_WINDOW  2
#define NH_CMD_END_FRAME      3

#define is_cmd(v) ( v != 0 && isalpha((v)->cmd))
#define is_style_cmd(v) (((v)->cmd == 'm')) // officially known as 'Select Graphic Rendition' (SGR)
#define is_nh_cmd(v) (((v).cmd == VT_CMD_TILEDATA))
#define is_start_glyph(v) (((v)->cmd == VT_CMD_TILEDATA) && ((v)->par[1] == NH_CMD_START_GLYPH))
#define is_end_glyph(v) (((v)->cmd == VT_CMD_TILEDATA) && ((v)->par[1] == NH_CMD_END_GLYPH))
#define is_end_frame(v) (((v)->cmd == VT_CMD_TILEDATA) && ((v)->par[1] == NH_CMD_END_FRAME))

#define FLAG_CORPSE        0x0001
#define FLAG_INVISIBLE     0x0002
#define FLAG_DETECTED      0x0004
#define FLAG_PET           0x0008
#define FLAG_RIDDEN        0x0010
#define FLAG_STATUE        0x0020
#define FLAG_PILE          0x0040
#define FLAG_LAVA_HILIGHT  0x0080
#define FLAG_ICE_HILIGHT   0x0100
#define FLAG_OUT_OF_SIGHT  0x0200
#define FLAG_UNEXPLORED    0x0400
#define FLAG_FEMALE        0x0800
#define FLAG_BADCOORDS     0x1000


typedef struct {
  uint16_t glyph;
  uint16_t flags;
  char cmd;
} nh_command_t;

#define ESC_CHAR '\033'
#define BEGIN_CHAR '['

// these are the attribute flags defined in tty.h
// 
//#define VT100_ATTR_COLOR_MASK 0x3ffffff
//#define VT100_ATTR_COLOR_TYPE 0x3000000
//#define VT100_COLOR_OFF 0
//#define VT100_COLOR_16  1
//#define VT100_COLOR_256 2
//#define VT100_COLOR_RGB 3
//#define VT100_ATTR_BOLD         0x04000000
//#define VT100_ATTR_DIM          0x08000000
//#define VT100_ATTR_ITALIC       0x10000000
//#define VT100_ATTR_UNDERLINE    0x20000000
//#define VT100_ATTR_STRIKE       0x40000000
//#define VT100_ATTR_INVERSE      0x0400000000000000
//#define VT100_ATTR_BLINK        0x0800000000000000
//#define VT100_ATTR_CJK          0x8000000000000000

//
// Nethack uses colors 0-7 combined with bold to form its 16 color palette
// 
typedef struct {
  uint16_t code;  // same as tiledata codes
  uint16_t flags; // taken directly from tiledata
  char ascii;     // plain ASCII character in console
  char color;     // color code (16 values) 
  char dx;        // horizontal displacement w.r.t. previous frame
  char dy;        // horizontal displacement w.r.t. previous frame
} glyph_t;        // nethack pixel :)

void reset_glyph(glyph_t* g) {
  memset(g,0,sizeof(glyph_t));
}

char vtattr_to_color(uint64_t attr) {
  return (attr & VT100_ATTR_BOLD) ?  (attr & 0x07) : (attr & 0x07) + 8;
}

typedef struct {
  char valid; // valid game frame (may be message only frame)
  int16_t number; // frame number
  glyph_t* glyphs;
  uint16_t ncols;
  uint16_t nrows;
  int16_t hero_i; // row where our hero is located
  int16_t hero_j; // col. ...
  char message[NH_COLS+1];
  char status1[NH_COLS+1];
  char status2[NH_COLS+1];
} frame_t;


frame_t* frame_init();
void frame_free(frame_t* f);
void frame_reset(frame_t* f);
void frame_copy(frame_t* dst, const frame_t* src);
void frame_dump(frame_t* frame, FILE* out);
void frame_write(frame_t* frame, FILE* out);
int frame_changed(const frame_t* a, const frame_t* b);

void tty_to_frame(tty vt, frame_t* frame);


void tty_dump(tty vt);

void detect_motion(frame_t* frame, const frame_t* prev_frame);
vt_command_t* intercept_command(int c);
void apply_style(vt_command_t* style_cmd, glyph_t* pglyph);

void frame_to_pov(const frame_t* frame, FILE* outf, const char* tfname);
//
// macros to avoid stupid errors
// 
#define vtget( t, i, j ) ( ( t )->scr[ ( i ) * NH_COLS + ( j ) ] )
#define frget( t, i, j ) ( ( t )->glyphs[ ( i ) * NH_COLS + ( j ) ] )

//------------------------------------------------------------

int main ( int argc, char * argv[] ) {
    const char * ifname = NULL, * tfname, * ofprefix = NULL;
    char ofname[128];
    tfname = "tileset.pov";
    FILE * inf = NULL;
    
    if ( argc < 3 ) {
        fprintf ( stderr, "usage: %s [-t tileset.pov] <frame.tty> <frame.pov>\n", argv[ 0 ] );
        exit ( 1 );
    }
    if ( ( argv[ 1 ][ 0 ] == '-' ) && ( argv[ 1 ][ 1 ] == 't' ) ) {
        tfname = argv[ 2 ];
        argv += 2;
        argc -= 2;
    }
    if ( argc < 3 ) {
        fprintf ( stderr, "usage: %s [-t tileset.pov] <frame.tty> <frame.pov>\n", argv[ 0 ] );
        exit ( 1 );
    }
    ifname = argv[ 1 ];
    ofprefix = argv[ 2 ];
    printf ( "POVHACK\n" );
    printf ( "=======\n" );
    printf ( "tiles : %s\n", tfname );
    printf ( "input : %s\n", ifname );
    printf ( "output: %s\n", ofprefix );
    printf ( "=======\n" );
    //
    //
    inf = fopen ( ifname, "r" );
    if (!inf) {
      fprintf(stderr,"could ont open file %s for reading.\n",ifname);
      exit(1);
    }
    //
    // prepare TTY info
    //
    tty vt = tty_init ( 80, 24, 0 ); // 80x24, not resizable
    tty_reset ( vt );
    //
    // our info
    //
    frame_t* frame, *prev_frame;
    frame = frame_init();
    prev_frame = frame_init();
    frame_reset(frame);
    frame_reset(prev_frame);
    
    
    int nframes = 0;
    while ( !feof ( inf ) ) {
      //
      // intercept NetHack output
      //
      int c = fgetc ( inf );
      vt_command_t* vtc = intercept_command(c);
      if (vtc) {
	//printf("command %c, %d args: ",vtc->cmd,vtc->npar);
	//for (int i = 0; i < vtc->npar; i++) {
	//  printf(" %d",vtc->par[i]);
	//}
	if (is_start_glyph(vtc)) {
	  //printf("i=%d,j=%d: start glyph %d %d\n",
	  //	 vt->cy,vt->cx,vtc->par[2],vtc->par[3]);
	  int i = vt->cy;
	  int j = vt->cx;
	  // get glyph code and flags
	  glyph_t* g = &frame->glyphs[i*NH_COLS+j];
	  g->code = vtc->par[2]; // ESC [ 1; 0; code; flags z
	  g->flags = vtc->par[3]; // ESC [ 1; 0; code; flags z
	}  else if (is_end_frame(vtc)) {
	  // capture hero position
	  frame->hero_i = vt->cy;
	  frame->hero_j = vt->cx;
	  frame->number = nframes;
	  // copy vt100 data to frame
	  tty_to_frame(vt,frame);
	  // detect changes
	  if (frame_changed(frame,prev_frame)) {
	    // detect motion
	    detect_motion(frame,prev_frame);
	    //printf("*** vt dump:\n");
	    //tty_dump(vt);
	    FILE* fout;
	    
	    printf("*** frame dump:\n");
	    //snprintf(ofname,127,"%s_%06d.dbg",ofprefix,nframes);
	    //fout = fopen(ofname,"w");
	    frame_dump(frame,fout);
	    //fclose(fout);
	    
	    snprintf(ofname,127,"%s_%06d.frm",ofprefix,nframes);
	    fout = fopen(ofname,"w");
	    frame_write(frame,fout);
	    fclose(fout);
	    // save frame
	    snprintf(ofname,127,"%s_%06d.pov",ofprefix,nframes);
	    fout = fopen(ofname,"w");
	    frame_to_pov(frame,fout,tfname);
	    fclose(fout);
	    // save as previous frame
	    nframes++;
	  } else {
	  }
	  // reset frame
	  frame_copy(prev_frame,frame);
	  frame_reset(frame);
	}
      }
      //
      // write to in-memory virtual terminal
      //
      tty_write ( vt, ( char* ) &c, 1 );
    }
    printf ( "processed %d frames\n", nframes );
    fclose ( inf );
    tty_free ( vt );
    frame_free ( prev_frame );
    frame_free (frame);
    exit ( 0 );
}

//------------------------------------------------------------

/**
 * returns 0 if no command was detected
 * otherwise returns a pointer to the second argument
 */
vt_command_t* intercept_command(int c) {
  static vt_command_t cmd;
  static int c1 = 0, c0 = 0;
  static int within_command = 0;
  static int parval;
  // update state
  c1 = c0; c0 = c;
  if (!within_command) {
    if ((c1 == ESC_CHAR) && (c0 == BEGIN_CHAR)) {
      within_command = 1;
      cmd.cmd = 0;
      cmd.npar = 0;
      parval = 0;
    }
    return 0;
  } else { // within command
    if ( isalpha(c) ) { // end of command
      cmd.par[cmd.npar++] = parval; 
      cmd.cmd = c;
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

//#define VT100_ATTR_COLOR_MASK 0x3ffffff
//#define VT100_ATTR_COLOR_TYPE 0x3000000
//#define VT100_COLOR_OFF 0
//#define VT100_COLOR_16  1
//#define VT100_COLOR_256 2
//#define VT100_COLOR_RGB 3
//#define VT100_ATTR_BOLD         0x04000000
//#define VT100_ATTR_DIM          0x08000000
//#define VT100_ATTR_ITALIC       0x10000000
//#define VT100_ATTR_UNDERLINE    0x20000000
//#define VT100_ATTR_STRIKE       0x40000000
//#define VT100_ATTR_INVERSE      0x0400000000000000
//#define VT100_ATTR_BLINK        0x0800000000000000
//#define VT100_ATTR_CJK          0x8000000000000000

//------------------------------------------------------------


void apply_style(vt_command_t* vtc, glyph_t* glyph) {
  uint64_t style_cmd = vtc->par[0];
  
  switch (style_cmd) {
  case 0: // reset style
    glyph->color = 0;
    break;
  case 1: // bold
    glyph->color |= 0x08;
    break;
  case 22: // normal intensity
    glyph->color &= 0x07;
    break;
  case 23: // 
    glyph->color &= 0x07;
    break;
  case 30: case 31: case 32: case 33: //foreground color (8 colors)
  case 34: case 35: case 36: case 37:    
    glyph->color = glyph->color & 0xf8;
    glyph->color |= (style_cmd - 30);
    break;
    // 40-47 are background color
  default:
    break;
  }
}

//------------------------------------------------------------


void detect_motion(frame_t* frame, const frame_t* prev_frame) {

  // motion is stored in a printable char: there are 25 possible movements
  // written using letters from A to Y
  // the character is computed as:  'A' + 5*(2+dy) + (2+dx)
  // where -2 <= dx,dy <= 2
  for (int i = 1; i < 22; ++i) {
    for (int j = 0; j < 80; ++j) {
      glyph_t* target = &frget(frame,i,j);
      const glyph_t* prev   = &frget(prev_frame,i,j);
      int tc = target->ascii;
      int tg = target->code;
      target->dx = target->dy = 0;

      int found = 0;
      // these don't move
      if ((tc == ' ') || 
          (tc == '#') || 
	  (tc == '.') || 
	  (tc == '|') || 
	  (tc == '-') || 
	  (tc == '>') || 
	  (tc == '<') || 
	  (tc == '{') || 
	  (tc == '+') ||
	  (tg == 0))  {
	continue;
      }
      //
      // we search for matching glyphs
      // this is more robust than chars
      //
      if (tg == prev->code) {
	continue;
      }
      
      // radius 1
      int di0 = i > 0 ?  -1: 0;
      int di1 = i < 21 ? +1: 0;
      int dj0 = j > 0 ?  -1: 0;
      int dj1 = j < 79 ? +1: 0;
      for (int di = -di0; !found && (di <= di1); ++di) {
	for (int dj = -dj0; (dj <= dj1); ++dj) {
	  prev = &frget(prev_frame,i+di,j+dj);
	  if (tg == prev->code) {
	    target->dy = -di;
	    target->dx = -dj;
	    found = 1;
	    break;
	  }
	}
      }
      if (found)
	continue;

      // radius 2
      di0 = i > 1 ?  -2: 0;
      di1 = i < 20 ? +2: 0;
      dj0 = j > 1 ?  -2: 0;
      dj1 = j < 78 ? +2: 0;
      for (int di = di0; !found && (di <= di1); ++di) {
	for (int dj = dj0; (dj <= dj1); ++dj) {
	  prev = &frget(prev_frame,i+di,j+dj);
	  if (tg == prev->code) {
	    target->dy = -di;
	    target->dx = -dj;
	    found = 1;
	    break;
	  }
	}
      } // radius 2 search
    } // for each col
  } // for each row
} // end search


//------------------------------------------------------------

static void print_hruler() {
  putchar(' ');
  for (int j = 0; j < NH_COLS; j++) {
    putchar(j % 10 ? '-' : ('0'+j/10));
  }
  putchar('\n');
}

//------------------------------------------------------------

static void print_vruler(int i) {
  putchar(i % 10 ? '|' : ('0'+i/10));
}

//------------------------------------------------------------

void tty_dump(tty vt) {
  attrchar* ac = vt->scr;
  printf("chars:\n");
  print_hruler();
  for (int i = 0; i < NH_ROWS; i++) {
    print_vruler(i);
    for (int j = 0; j < NH_COLS; j++, ac++) {
      putchar(ac->ch);
    }
    print_vruler(i);
    putchar('\n');
  }
  printf("color:\n");
  ac = vt->scr;
  print_hruler();
  for (int i = 0; i < NH_ROWS; i++) {
    print_vruler(i);
    for (int j = 0; j < NH_COLS; j++, ac++) {
      int color = vtattr_to_color(ac->attr);
      putchar(color > 9? 'A'+ color: '0' + color);
    }
    print_vruler(i);
    putchar('\n');
  }  
  print_hruler();
  printf("cursor at i=%d j=%d\n",vt->cy,vt->cx);
}

//------------------------------------------------------------

void frame_dump(frame_t* frame, FILE* outf) {
  char movkey[3][3] = { {'y','k','u'}, {'h','.','l'}, {'b','j','n'} }; 
  // ascii
  glyph_t* g = frame->glyphs;
  printf("chars:\n");
  print_hruler();
  for (int i = 0; i < NH_ROWS; i++) {
    print_vruler(i);
    for (int j = 0; j < NH_COLS; j++, g++) {
      putchar(g->ascii);
    }
    print_vruler(i);
    putchar('\n');
  }
  printf("movement:\n");
  g = frame->glyphs;
  print_hruler();
  for (int i = 0; i < NH_ROWS; i++) {
    print_vruler(i);
    for (int j = 0; j < NH_COLS; j++, g++) {
      const int dx =  g->dx > 0 ? 1: (g->dx < 0 ? -1 : 0);
      const int dy =  g->dy > 0 ? 1: (g->dy < 0 ? -1 : 0);
      putchar(movkey[1+dy][1+dx]);
    }
    print_vruler(i);
    putchar('\n');
  }
  // style
  printf("color:\n");
  g = frame->glyphs;
  print_hruler();
  for (int i = 0; i < NH_ROWS; i++) {
    print_vruler(i);
    for (int j = 0; j < NH_COLS; j++, g++) {
      int color = g->color;
      putchar(color > 9? 'A'+ color: '0' + color);
    }
    print_vruler(i);
    putchar('\n');
  }
  // glyph (only first 4 bits)
  g = frame->glyphs;
  printf("glyphs:\n");
  print_hruler();
  for (int i = 0; i < NH_ROWS; i++) {
    print_vruler(i);
    for (int j = 0; j < NH_COLS; j++, g++) {
      int code = g->code & 0x000f;
      putchar(code > 9? 'A'+ code: '0' + code);
    }
    print_vruler(i);
    putchar('\n');
  }  
  // flags (only first 4 bits
  g = frame->glyphs;
  printf("flags:\n");
  print_hruler();
  for (int i = 0; i < NH_ROWS; i++) {
    print_vruler(i);
    for (int j = 0; j < NH_COLS; j++, g++) {
      int flags = g->flags & 0x000f;
      putchar(flags > 9? 'A'+ flags: '0' + flags);
    }
    print_vruler(i);
    putchar('\n');
  }  
  print_hruler();
  printf("valid:%s\n",frame->valid? "YES": "NO");
  printf("num:%d\n",frame->number);
  printf("msg:%s\n",frame->message);
  printf("st1:%s\n",frame->status1);
  printf("st2:%s\n",frame->status2);
  printf("hero at i=%d j=%d\n",frame->hero_i,frame->hero_j);
}

//------------------------------------------------------------

void frame_write(frame_t* frame, FILE* out) {
  glyph_t* g = frame->glyphs;
  for (int i = 0; i < NH_ROWS; i++) {
    for (int j = 0; j < NH_COLS; j++, g++) {
      fprintf(out,"%d %d %u %u %d %d %d %d\n",i,j,
	      g->code,g->flags,g->ascii,g->color,g->dx,g->  dy);
    }
  }
}

//------------------------------------------------------------

frame_t* frame_init() {
  frame_t* out = (frame_t*) malloc(sizeof(frame_t));
  out->ncols = NH_COLS;
  out->nrows = NH_ROWS;
  out->glyphs = (glyph_t*) malloc(sizeof(glyph_t)*NH_COLS*NH_ROWS);
  out->hero_i = 0;
  out->hero_j = 0;
  out->number = 0;
  out->valid = 0;
  return out;
}

//------------------------------------------------------------

void frame_reset(frame_t* f) {
  for (int i = 0; i < NH_COLS*NH_ROWS; ++i) {
    f->glyphs[i].ascii = 0x20; // space
    f->glyphs[i].flags = 0;
    f->glyphs[i].code = 0;
    f->glyphs[i].color = 0;
    f->glyphs[i].dx    = 0;
    f->glyphs[i].dy    = 0;
    f->message[0] = 0;
    f->status1[0] = 0;
    f->status2[0] = 0;
    f->hero_i = -1;
    f->hero_j = -1;
  }
}

//------------------------------------------------------------

void frame_free(frame_t* f) {
  free(f->glyphs);
  free(f);
}

//------------------------------------------------------------

void frame_copy(frame_t* dst, const frame_t* src) {
  // override everything BUT the dynamic pointer!
  glyph_t* dst_ptr = dst->glyphs;
  memcpy(dst,src,sizeof(frame_t));
  dst->glyphs = dst_ptr;
  memcpy(dst->glyphs,src->glyphs,sizeof(glyph_t)*NH_ROWS*NH_COLS);
}

int frame_changed(const frame_t* a, const frame_t* b) {
  if (a->hero_i != b->hero_i) return 1;
  if (a->hero_j != b->hero_j) return 1;
  return memcmp(a->glyphs+NH_COLS,b->glyphs+NH_COLS,sizeof(glyph_t)*NH_COLS*(NH_ROWS-4));
}

//------------------------------------------------------------

void tty_to_frame(tty vt, frame_t* frame) {
  //
  // copy top message and status bars
  //
  for (int j = 0; j < NH_COLS; j++) {
    frame->message[j] = vtget(vt,0,j).ch;
    frame->status1[j] = vtget(vt,22,j).ch;
    frame->status2[j] = vtget(vt,23,j).ch;
  }
  frame->message[NH_COLS] = 0;
  frame->status1[NH_COLS] = 0;
  frame->status2[NH_COLS] = 0;
  //
  // translate vt attrchars to glyphs
  // we only overwrite character and code
  // (we check that vt character and frame character are the same
  // on a given position)
  for (int i = 0; i < NH_ROWS; i++) {
    for (int j = 0; j < NH_COLS; j++) {
      attrchar ac = vtget(vt,i,j);
      glyph_t* gl = &frget(frame,i,j);
      gl->ascii = ac.ch;
      gl->color = vtattr_to_color(ac.attr);
    }
  }  
}

//------------------------------------------------------------

//#define FLAG_CORPSE        0x0001
//#define FLAG_INVISIBLE     0x0002
//#define FLAG_DETECTED      0x0004
//#define FLAG_PET           0x0008
//#define FLAG_RIDDEN        0x0010
//#define FLAG_STATUE        0x0020
//#define FLAG_PILE          0x0040
//#define FLAG_LAVA_HILIGHT  0x0080
//#define FLAG_ICE_HILIGHT   0x0100
//#define FLAG_OUT_OF_SIGHT  0x0200
//#define FLAG_UNEXPLORED    0x0400
//#define FLAG_FEMALE        0x0800
//#define FLAG_BADCOORDS     0x1000

void frame_to_pov(const frame_t* frame, FILE* outf, const char* tfname) {
    const double h = 1;
    fprintf ( outf, "#include \"%s\"\n", tfname );
    static double prev_dx = 0, prev_dy = 0;
    for ( int i = 1 ; ( i < 22 ) ; ++i ) {
        for ( int j = 0 ; j < NH_COLS ; ++j ) {
	  glyph_t* g = &frget(frame,i,j);
	  uint16_t flags = g->flags;
	  char chr = g->ascii;
          fprintf ( outf, " object { Tiles[%d][%d] ", chr, g->color);
	  // perhaps rotate
	  if ((g->dy != 0) || (g->dx != 0)) {
	    double angle = 90-(180.0/M_PI)*atan2f(g->dy, g->dx);
	    fprintf( outf, " rotate %f*y ", angle );
	  }
	  fprintf( outf, " translate <%3d,  0,%3d> }\n", j, (20 - i));
	  if (chr != '>') { // if ladder down, don't put a floor
 	    fprintf( outf, " object {Floor ");
	    fprintf( outf, " translate <%3d,  0,%3d> }\n", j, (20 - i));
	  }
        }
    }
    double cx = frame->hero_j;
    double cy = frame->hero_i;
    glyph_t hero_data = frget(frame,frame->hero_i,frame->hero_j);
    double dx = hero_data.dx;
    double dy = hero_data.dy;
    if ((dx == 0) && (dy == 0)) {
      prev_dx *= 0.5;
      prev_dy *= 0.5;
      dx = prev_dx;
      dy = prev_dy;
    } else {
      prev_dx = dx;
      prev_dy = dy;
    }
    fprintf ( outf, "light_source { GlobalLight translate %f*x+%f*z }\n", cx, 20 - cy );
    //fprintf ( outf, "sphere { <0,1,0>,0.1 pigment {color Green} finish {ambient 1} no_shadow translate %d*x+%d*z }\n", cx, 20 - cy );
    fprintf ( outf, "light_source { LocalLight  translate %f*x+%f*z }\n", cx + 0.1 * dx, 20 - cy - 0.1 * dy );
    //fprintf ( outf, "sphere { <0,1,0>,0.1 pigment {color Blue}  finish {ambient 1} no_shadow translate %f*x+%f*z }\n", cx + 0.1 * dx, 20 - cy - 0.1 * dy );
    fprintf ( outf, "camera {\n" );
    fprintf ( outf, "  perspective\n" );
    fprintf ( outf, "  right (1920/1080)*x\n" );
    fprintf ( outf, "  sky y\n" );
    fprintf ( outf, "  location <%f,%f,%f> \n", cx - h * dx, 2 * h, 20 - cy + h * dy );
    fprintf ( outf, "  look_at  <%f,0.5,%f>\n", cx, 20 - cy );
    fprintf ( outf, "}\n" );

}
