#include <stdio.h>
#include <tty.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <argp.h>
#include "logging.h"
#include "frame.h"
#include "options.h"
#include "vt100.h"

//
// NetHack TileData command info
//
#define NH_CMD_START_GLYPH    0
#define NH_CMD_END_GLYPH      1
#define NH_CMD_SELECT_WINDOW  2
#define NH_CMD_END_FRAME      3

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



//
// Nethack uses colors 0-7 combined with bold to form its 16 color palette
// 

void detect_motion(frame_t* frame, const frame_t* prev_frame);
vt_command_t* intercept_command(int c);
void apply_style(vt_command_t* style_cmd, glyph_t* pglyph);
void frame_to_pov(const frame_t* frame, const config_t* cfg, FILE* outf );
//
// macros to avoid stupid errors
// 

//------------------------------------------------------------

int main ( int argc, char * argv[] ) {
    config_t cfg = parse_opt ( argc, argv );
    
    char ofname[128];
    FILE * inf = NULL;
    
    printf ( "POVHACK\n" );
    //
    //
    inf = fopen ( cfg.input_file, "r" );
    if (!inf) {
      fprintf(stderr,"could ont open file %s for reading.\n",cfg.input_file);
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
	    
	    snprintf(ofname,127,"%s_%06d.frm",cfg.output_prefix,nframes);
	    fout = fopen(ofname,"w");
	    frame_write(frame,fout);
	    fclose(fout);
	    // save frame
	    snprintf(ofname,127,"%s_%06d.pov",cfg.output_prefix,nframes);
	    fout = fopen(ofname,"w");
	    frame_to_pov(frame,&cfg,fout);
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

void frame_to_pov(const frame_t* frame, const config_t* cfg, FILE* outf ) {
    const double h = 1;
    fprintf ( outf, "#include \"%s\"\n", cfg->tileset_file );
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
