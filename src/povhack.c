#include <stdio.h>
#include <tty.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 
// general ANSI/VT100 command
// ESC[par1;par2;...;parn<letter>
//
#define VT_CMD_TILEDATA      'z'

typedef struct {
  uint16_t par[7];
  short npar;
  char cmd;
} vt_command_t;

//
// NetHack TileData command info
//

#define NH_CMD_START_GLYPH    0
#define NH_CMD_END_GLYPH      1
#define NH_CMD_SELECT_WINDOW  2
#define NH_CMD_END_FRAME      3

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

vt_command_t* detect_command(int c, vt_command_t* cmd);

/**
 * returns 0 if no command was detected
 * otherwise returns a pointer to the second argument
 */
vt_command_t* detect_command(int c, vt_command_t* cmd) {
  static int c1 = 0, c0 = 0;
  static int within_command = 0;
  static int parval;
  // update state
  c1 = c0; c0 = c;
  if (!within_command) {
    if ((c1 == ESC_CHAR) && (c0 == BEGIN_CHAR)) {
      within_command = 1;
      cmd->npar = 0;
      parval = 0;
    }
    return 0;
  } else { // within command
    if ( isalpha(c) ) { // end of command
      cmd->par[cmd->npar++] = parval; 
      cmd->cmd = c;
      within_command = 0;
      return cmd;      
    } else if (c == ';') {
      cmd->par[cmd->npar++] = parval; 
      parval = 0;
      return 0;
    } else if ( isdigit(c)) { 
      parval = parval*10 + c-'0'; // update param value
      return 0;
    } else {
      fprintf(stderr,"not a valid ANSI command.! Offending character is %c (%d)\n",c,c);
      return 0;
    }
  }
}


/**
 * returns 0 if the command is not a NH command
 * otherwise returns a pointer to the second argument
 */
nh_command_t* nethack_command(vt_command_t* vtc, nh_command_t* nhc) {
  if (vtc->cmd != VT_CMD_TILEDATA) { // not a NH command 
    return 0;
  }
  // first NH param is always present and always 1
  // second NH param is the actualNH command
  nhc->cmd = vtc->par[1]; 
  if (nhc->cmd == NH_CMD_START_GLYPH) {
    nhc->glyph = vtc->par[2]; // param 2 is glyph
    nhc->flags = vtc->par[3]; // param 3 is flags
  } 
  return nhc;
}

void tty_copyto(tty dest, tty src);

typedef struct {
  uint16_t glyph; // same as tiledata codes
  uint16_t flags; // taken directly from tiledata
  char vtcolor; // VT color code (16 values) 
  char vtflags; // bold, etc.
  char dx; // horizontal displacement w.r.t. previous frame
  char dy; // horizontal displacement w.r.t. previous frame
} haxel_t; // nethack pixel :)

typedef struct {
  haxel_t* haxels;
  uint16_t ncols;
  uint16_t nrows;
  uint16_t hero_i; // row where our hero is located
  uint16_t hero_j; // col. ...
} frame_t;

/**
 * modifies scene information based on a particular nethack command
 * for example mark the status (flags) of some glyph 
 * e.g.: if we get a 'corpse' flag, we may draw the 3D glyph as lying on the ground
 */
void apply_command(const nh_command_t* nhc, frame_t* frame);

/**
 * gathers drawing information based on current and previous frame
 * including the glyphs to be drawn, their estimated movement, heading, etc.
 */
void gather_scene_info(const tty prev_vt, const tty vt, frame_t* frame);

#define NH_COLS 80
#define NH_ROWS 21

//
//char get_direction ( int cx, int cy );
//
#define vtget( t, i, j ) ( ( t )->scr[ ( i ) * NH_COLS + ( j ) ] )
#define frget( t, i, j ) ( ( t )->haxels[ ( i ) * NH_COLS + ( j ) ] )

int main ( int argc, char * argv[] ) {
    const char * ifname = NULL, * tfname, * ofprefix = NULL;
    char ofname[128];
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
    // prepare TTY info
    //
    tty vt = tty_init ( 80, 24, 1 ); // 80x21, not resizable
    tty prev_vt = tty_init ( 80, 24, 1 ); // 80x21, not resizable
    tty_reset ( vt );
    tty_reset ( prev_vt );
    
    inf = fopen ( ifname, "r" );
    
    vt_command_t vtc;
    nh_command_t nhc;
    int nframes = 0;
    while ( !feof ( inf ) ) {
        int c = fgetc ( inf );
	if ( detect_command(c,&vtc) ) {
	  if ( nethack_command(&vtc,&nhc) ) {
		  // oh yes
	  }
	}
	//detect_motion(prev_vt, vt, motion);
        //write_frame ( vt, ofname, tfname );
	// update frames (terminals)
	tty_copyto(prev_vt,vt);
        tty_write ( vt, ( char* ) &c, 1 );
    }
    printf ( "processed %d frames\n", nframes );
    fclose ( inf );
    tty_free ( vt );
    tty_free ( prev_vt );
    exit ( 0 );
}


void tty_copyto(tty dest, tty src) {
  attrchar* dest_scr = dest->scr;
  memcpy(dest,src,sizeof(struct tty));
  // recover pointer to buffer
  dest->scr = dest_scr;
  memcpy(dest->scr,src->scr,sizeof(attrchar)*src->sx*src->sy);
}


void detect_motion(tty prev_vt, tty vt, char* motion) {
  memset(motion,32,80*24*sizeof(char));
  // motion is stored in a printable char: there are 25 possible movements
  // written using letters from A to Y
  // the character is computed as:  'A' + 5*(2+dy) + (2+dx)
  // where -2 <= dx,dy <= 2
  for (int i = 1; i < 22; ++i) {
    for (int j = 0; j < 80; ++j) {
      attrchar target = vtget(vt,i,j);
      attrchar prev   = vtget(prev_vt,i,j);
      int found = 0, di_found = -10, dj_found = -10;
      if ((target.ch == ' ') || 
          (target.ch == '#') || 
	  (target.ch == '.') || 
	  (target.ch == '|') || 
	  (target.ch == '-') || 
	  (target.ch == '>') || 
	  (target.ch == '<') || 
	  (target.ch == '{') || 
	  (target.ch == '+'))  {
	motion[i*80+j] = '.';
	continue;
      } 
      if (target.ch == prev.ch) {
	di_found = dj_found = 0;
	found = 1;
      }
      // first radius
      int di0 = i > 0 ?  -1: 0;
      int di1 = i < 21 ? +1: 0;
      int dj0 = j > 0 ?  -1: 0;
      int dj1 = j < 79 ? +1: 0;
      if (!found) {
	for (int di = -di0; !found && (di <= di1); ++di) {
          for (int dj = -dj0; (dj <= dj1); ++dj) {
            prev = vtget(prev_vt,i+di,j+dj);
	    if (target.ch == prev.ch) {
	      di_found = di;
	      dj_found = dj;
	      found = 1;
	      break;
	    }
	  }
        }	
      }
      // wasteful and redundant (we already cheched radius 1), but easy
      if (!found) {
        di0 = i > 1 ?  -2: 0;
        di1 = i < 20 ? +2: 0;
        dj0 = j > 1 ?  -2: 0;
        dj1 = j < 78 ? +2: 0;
        for (int di = di0; !found && (di <= di1); ++di) {
          for (int dj = dj0; (dj <= dj1); ++dj) {
            prev = vtget(prev_vt,i+di,j+dj);
	    if (target.ch == prev.ch) {
	      di_found = di;
	      dj_found = dj;
	      found = 1;
	      break;
	    }
          }
        }
      }
      if (!found) {      
	di_found = dj_found = 0;
      }
      motion[i*80+j] = 'A' + 5*(2+di_found) + (2+dj_found); // didn't move
    }
  }
}

void infer_direction(char dir, int* pdirx, int* pdiry, int* pangle) {
    // infer direction of character
    //
    int dirx, diry;
    int angle = 0;
    switch ( dir ) {
    case 'h':
        dirx = -1;
        diry =  0;
        angle = -90;
        break;
    case 'j':
        dirx =  0;
        diry =  1;
        angle = 180;
        break;
    case 'k':
        dirx =  0;
        diry = -1;
        angle = 0;
        break;
    case 'l':
        dirx =  1;
        diry =  0;
        angle = 90;
        break;
    case 'y':
        dirx = -1;
        diry = -1;
        angle = -45;
        break;
    case 'u':
        dirx =  1;
        diry = -1;
        angle = 45;
        break;
    case 'b':
        dirx = -1;
        diry =  1;
        angle = -135;
        break;
    case 'n':
        dirx =  1;
        diry =  1;
        angle = 135;
        break;
    case '.':
        dirx =  1;
        diry = -1;
        break;
    }
    *pdirx = dirx;
    *pdiry = diry;
    *pangle = angle;
}

void write_pov_frame(const frame_t* frame, const char* ofname, const char* tfname) {
    const int h = 2;
    FILE* outf;
    outf = fopen ( ofname, "w" );
    fprintf ( outf, "#include \"%s\"\n", tfname );

    for ( int i = 0 ; ( i < 21 ) ; ++i ) {
        for ( int j = 0 ; j < 80 ; ++j ) {
	  char chr = 0, col = 0, bol = 0;
          fprintf ( outf, "object { Tiles[%d][%d][%d] translate %d*x+%d*z }\n", chr, col, bol, j, ( 20 - i ) );
        }
    }
    int cx = frame->hero_j;
    int cy = frame->hero_i;
    haxel_t hero_data = frget(frame,frame->hero_i,frame->hero_j);
    int dirx = hero_data.dx;
    int diry = hero_data.dy;
    fprintf ( outf, "light_source { GlobalLight translate %d*x+%d*z }\n", cx, 21 - cy );
    fprintf ( outf, "light_source { LocalLight  translate %f*x+%f*z }\n", cx + 0.1 * dirx, 21 - cy - 0.1 * diry );
    fprintf ( outf, "camera {\n" );
    fprintf ( outf, "  perspective\n" );
    fprintf ( outf, "  right (1920/1080)*x\n" );
    fprintf ( outf, "  sky y\n" );
    fprintf ( outf, "  location <%d,%d,%d> \n", cx - h * dirx, 2 * h, 21 - cy + h * diry );
    fprintf ( outf, "  look_at  <%d,0.5,%d>\n", cx, 21 - cy );
    fprintf ( outf, "}\n" );

    fclose ( outf );
}

