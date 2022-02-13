#include <stdio.h>
#include <tty.h>
#include <stdlib.h>
#include <string.h>
//#include <ctype.h>
#include <math.h>
//#include <argp.h>
#include "logging.h"
#include "frame.h"
#include "options.h"
#include "vt100.h"
#include "nethack.h"


void detect_motion(frame_t* frame, frame_t* prev_frame);

void apply_style(vt_command_t* style_cmd, glyph_t* pglyph);

/**
 * most important function of this program
 */
void frame_to_pov(frame_t* prev_frame,
		  frame_t* curr_frame,
		  const config_t* cfg,
		  double time,
		  FILE* outf );
//
// macros to avoid stupid errors
// 

int get_game_time(const char* status);

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
      fprintf(stderr,"could not open file %s for reading.\n",cfg.input_file);
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
    
    
    int game_frame = 0;
    int video_frame = 0;
    int prev_time = -1;
    while ( !feof ( inf ) ) {
      //
      // intercept NetHack output
      //
      int c = fgetc ( inf );
      vt_command_t* vtc = intercept_command(c);
      if (vtc) {
	if (is_start_glyph(vtc)) {
	  int y = vt->cy;
	  int x = vt->cx;
	  // get glyph code and flags
	  glyph_t* g = frame_get(frame,x,y);
	  g->code = vtc->par[2]; // ESC [ 1; 0; code; flags z
	  g->flags = vtc->par[3]; // ESC [ 1; 0; code; flags z
	} else if (is_end_frame(vtc)) {
	  tty_to_frame(vt,frame);
	  printf("END FRAME %d\n",game_frame);
	  int time = get_game_time(frame->status2);
	  printf("TIME %d\n",time);
	  if (time != prev_time) {	    
	    prev_time = time;
	    // capture hero position
	    frame->hero_y = vt->cy;
	    frame->hero_x = vt->cx;
	    int valid = frame_valid(frame);
	    if (!valid) {
	      printf("FRAME is not valid!\n");
	    } else {
	      frame->number = game_frame;
	      // paste  vt100 data to frame
	      // detect changes
	      // detect motion
	      detect_motion(frame,prev_frame);
	      //printf("*** vt dump:\n");
	      //tty_dump(vt);
	  
	      FILE* fout = NULL;
	      snprintf(ofname,127,"%s_%06d.dbg",cfg.output_prefix,game_frame);
	      fout = fopen(ofname,"w");
	      frame_dump(frame,fout);
	      fclose(fout);
	  
	      snprintf(ofname,127,"%s_%06d.frm",cfg.output_prefix,game_frame);
	      fout = fopen(ofname,"w");
	      frame_write(frame,fout);
	      fclose(fout);
	      // save frame
	      for (int f = 1; f <= cfg.subframes; ++f) {
		const double T = (double) f / (double)cfg.subframes;
		printf("video frame %10d\n",video_frame);
		snprintf(ofname,127,"%s_%06d.pov",cfg.output_prefix,video_frame);
		fout = fopen(ofname,"w");
		frame_to_pov(prev_frame,frame,&cfg,T, fout);
		fclose(fout);
		video_frame++;
	      }
	    }
	    // save current frame as previous frame
	    frame_copy(prev_frame,frame);
	    // reset current frame
	    frame_reset(frame);
	    // advance frame
	    game_frame++;
	  } // actually changed time
	} // end frame
      } // end is a vt command
      //
      // write to in-memory virtual terminal
      //
      tty_write ( vt, ( char* ) &c, 1 );
    } // while
    
    printf ( "processed %d game frames\n", game_frame );
    printf ( "wrote %d video frames\n", video_frame );
    fclose ( inf );
    tty_free ( vt );
    frame_free ( prev_frame );
    frame_free (frame);
    exit ( 0 );
}

int get_game_time(const char* status) {
  char* off = strstr(status,"T:");
  if (!off) { return -1; }
  else {
    int pepe;
    sscanf(off+2,"%d ",&pepe);
    return pepe;
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
#include <ctype.h>

int is_monster(char c) {
  char sym[] = {'@','~','&','\'',':',';',0};
  if (isalpha(c)) return 1;
  for (int i = 0; sym[i] != 0; ++i) {
    if (c == sym[i]) return 1;
  }
  return 0;
}

int is_structure(char c) {
  char sym[] = {'|','-','+',0};
  for (int i = 0; sym[i] != 0; ++i) {
    if (c == sym[i]) return 1;
  }
  return 0;
}

//------------------------------------------------------------

void detect_motion(frame_t* frame, frame_t* prev_frame) {

  // motion is stored in a printable char: there are 25 possible movements
  // written using letters from A to Y
  // the character is computed as:  'A' + 5*(2+dy) + (2+dx)
  // where -2 <= dx,dy <= 2
  for (int y = 1; y < 22; ++y) {
    for (int x = 0; x < 80; ++x) {
      glyph_t* target = frame_get(frame,x,y);
      char tc = target->ascii;
      int tg = target->code;

      int found = 0;
      // these don't move
      if (!is_monster(tc)) {
	target->dx = 0;
	target->dy = 0;
	target->tele = 0;
	continue;
      }
      //printf("motion of character %c (glyph %d) at i=%d j=%d :",tc,tg, i,j);
      //
      // we search for matching glyphs
      // this is more robust than chars
      //
      const glyph_t* prev = frame_get(prev_frame,x,y);
      if (tg == prev->code) {
	target->dx = 0;
	target->dy = 0;
	target->tele = 0;
	//printf("none.\n");
	continue;
      }
      
      // radius 1
      int dy0 = y > 0 ?  -1: 0;
      int dy1 = y < 21 ? +1: 0;
      int dx0 = x > 0 ?  -1: 0;
      int dx1 = x < 79 ? +1: 0;
      for (int dy = dy0; !found && (dy <= dy1); ++dy) {
	for (int dx = dx0; (dx <= dx1); ++dx) {
	  prev = frame_get(prev_frame,x+dx,y+dy);
	  if (tg == prev->code) {
	    target->dy = -dy;
	    target->dx = -dx;
	    target->tele = 0;
	    found = 1;
	    break;
	  }
	}
      }
      if (found)
	continue;

      // radius 2
      dy0 = y > 1 ?  -2: 0;
      dy1 = y < 20 ? +2: 0;
      dx0 = x > 1 ?  -2: 0;
      dx1 = x < 78 ? +2: 0;
      for (int dy = dy0; !found && (dy <= dy1); ++dy) {
	for (int dx = dx0; (dx <= dx1); ++dx) {
	  prev = frame_get(prev_frame,x+dx,y+dy);
	  if (tg == prev->code) {
	    target->dy = -dy;
	    target->dx = -dx;
	    target->tele = 0;
	    found = 1;
	    break;
	  }
	}
      } // radius 2 search
      //
      // where is this guy?
      // likely teleported
      if (!found) {
	//printf("not found! teleported?\n");
	target->tele = 1;
      }
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


void frame_to_pov(frame_t* prev_frame,
		  frame_t* curr_frame,
		  const config_t* cfg,
		  double T,
		  FILE* outf ) {

  fprintf ( outf, "#version 3.7;\n" );
  fprintf ( outf, "#declare GameFrame = %d;\n", curr_frame->number );
  fprintf ( outf, "#declare GameSubFrame = %f;\n", T );
  fprintf ( outf, "#include \"%s\"\n", cfg->tileset_file );

  for ( int y1 = 1 ; ( y1 < 22 ) ; ++y1 ) {
        for ( int x1 = 0 ; x1 < NH_COLS ; ++x1 ) {
	  // current info on this glyph
	  glyph_t* g1      = frame_get(curr_frame,x1,y1);
	  double hx = curr_frame->hero_x;
	  double hy = curr_frame->hero_y;
	  uint16_t flags = g1->flags;
	  char     chr   = g1->ascii;
	  char     color = g1->color;
	  int is_hero = (x1 == hx) && (y1 == hy);
	  if (chr == ' ') { // nothing here
		  continue;
	  }
	  if (chr != '>') { // if ladder down, don't put a floor
 	    fprintf( outf, "object { Floor ");
	    fprintf( outf, " translate < %2d, %2d,  0 > }\n", x1, y1);
	  }
	  // ceiling
	  if (chr != '<') { // if ladder up, don't put a ceiling
 	    fprintf( outf, "object { Ceiling ");
	    fprintf( outf, " translate < %2d, %2d,  0 > }\n", x1, y1);
	  }
	  // thing	 
	  fprintf ( outf, "object { Tiles[%d][%d]\n", chr, color);	  
	  if (is_hero) { fprintf(outf," no_shadow\n"); }
	  if (!is_monster(chr)) {
	    // monsters don't move
	    fprintf( outf, " translate < %2d, %2d,  0>\n}\n", x1, y1);
	  }  else {
	    //
	    // monsters can move
	    //
	    const double dx1 = g1->dx;
	    const double dy1 = g1->dy;
	    
	    // previous info on this glyph
	    // is located at its previous position (given by displacement)
	    const int x0 = x1 - g1->dx;
	    const int y0 = y1 - g1->dy;
	    const glyph_t* g0 = frame_get(prev_frame,x0,y0);
	    double dx0, dy0;
	    double dx, dy;
	    double angle;
	    double x, y;
	    if (g0->code != g1->code) {
	      // not same glyph!
	      printf("not same glyph!\n");
	      dx0 = dx1;
	      dy0 = dy1;
	      x = (1.0-T)*x0 + T*x1;
	      y = (1.0-T)*y0 + T*y1;	    
	      dx = (1.0-T)*dx0 + T*dx1;
	      dy = (1.0-T)*dy0 + T*dy1;	    
	      angle = 0; // 90 - (180.0/M_PI)*atan2f(dy, dx);
	    } else {
	      dx0 = g0->dx;
	      dy0 = g0->dy;
	      x = (1.0-T)*x0 + T*x1;
	      y = (1.0-T)*y0 + T*y1;	    
	      dx = (1.0-T)*dx0 + T*dx1;
	      dy = (1.0-T)*dy0 + T*dy1;	    
	      angle = (180.0/M_PI)*atan2f(dy, dx) - 90;
	    }
	    //printf("glyph %c %03d: T=%6.3f curr=(%d,%d) prev=(%2d,%2d) dcurr=(%6.3f,%6.3f) dprev=(%6.3f,%6.3f) dinst=(%6.3f,%6.3f) angle=%7.2f\n",g1->ascii, g1->code,
	    //	   T,x1,y1,x0,y0,dx0,dy0,dx1,dy1,dx,dy,angle);
	    // interpolated speed is only for computing the rotation angle
	    fprintf( outf, " rotate %5f*z\n", angle );
	    // when T < 1, we are showing the frames between the previous and
	    // the current. Thus we need to displace the glyph *backwards* 
	    // according to current speed and position
	    fprintf( outf, " translate < %7.3f, %7.3f,  0>\n}\n", x, y);
	    if (is_hero) { // is this our hero??
	      fprintf ( outf, "light_source { GlobalLight translate < %7.3f, %7.3f,  0 > }\n", x, y );
	      fprintf ( outf, "light_source { LocalLight  translate < %7.3f, %7.3f,  0 > }\n", x, y);
	      fprintf ( outf, "camera {\n" );
	      fprintf ( outf, "  perspective\n" );
	      fprintf ( outf, "  right (1920/1080)*x\n" );
	      fprintf ( outf, "  sky z\n" );
	      fprintf ( outf, "  location < %7.3f, %7.3f, CameraHeight> \n", x, y+5);
	      fprintf ( outf, "  look_at  < %7.3f, %7.3f, 0.5>\n", x, y );
	      fprintf ( outf, "}\n" );
	    }
	  }
        }
    }

}
