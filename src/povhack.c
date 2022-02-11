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


void detect_motion(frame_t* frame, const frame_t* prev_frame);

void apply_style(vt_command_t* style_cmd, glyph_t* pglyph);

/**
 * most important function of this program
 */
void frame_to_pov(const frame_t* prev_frame,
		  const frame_t* curr_frame,
		  const config_t* cfg,
		  double time,
		  FILE* outf );
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
	  frame->number = game_frame;
	  // copy vt100 data to frame
	  tty_to_frame(vt,frame);
	  // detect changes
	  if (frame_changed(frame,prev_frame)) {
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
	    for (int i = 1; i <= cfg.subframes; ++i) {
	      const double T = (double) i / (double)cfg.subframes;
	      snprintf(ofname,127,"%s_%06d.pov",cfg.output_prefix,video_frame);
	      fout = fopen(ofname,"w");
	      frame_to_pov(prev_frame,frame,&cfg,T, fout);
	      fclose(fout);
	      video_frame++;
	    }
	    // save as previous frame
	    game_frame++;
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
    printf ( "processed %d game frames\n", game_frame );
    printf ( "wrote %d video frames\n", video_frame );
    fclose ( inf );
    tty_free ( vt );
    frame_free ( prev_frame );
    frame_free (frame);
    exit ( 0 );
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
  char monsters[] = {'~','&','\'',':',';','@',0};
  if (isalpha(c)) return 1;
  for (int i = 0; monsters[i] != 0; ++i) {
    if (c == monsters[i]) return 1;
  }
  return 0;
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
      //
      // we search for matching glyphs
      // this is more robust than chars
      //
      if (tg == prev->code) {
	target->dx = 0;
	target->dy = 0;
	target->tele = 0;
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
	    target->tele = 0;
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
	    target->tele = 0;
	    found = 1;
	    break;
	  }
	}
      } // radius 2 search
      //
      // where is this guy?
      // likely teleported
      target->tele = 1;
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

//static char unmovable[] = {' ','#','.','|','-','+','%','>','<','{','_','}','\\','/','^','[',']','0','`','$','%','*','(',')','?','!',0};

//#define NH_STRUCTURE
//#define NH_DUNGEON
//#define NH_ITEM


void frame_to_pov(const frame_t* prev_frame,
		  const frame_t* curr_frame,
		  const config_t* cfg,
		  double T,
		  FILE* outf ) {

  fprintf ( outf, "#declare GameFrame = %d;\n", curr_frame->number );
  fprintf ( outf, "#declare GameSubFrame = %f;\n", T );
  fprintf ( outf, "#include \"%s\"\n", cfg->tileset_file );

  static double prev_hero_x, prev_hero_y;
  for ( int i = 1 ; ( i < 22 ) ; ++i ) {
        for ( int j = 0 ; j < NH_COLS ; ++j ) {
	  glyph_t* g0    = &frget(prev_frame,i,j);
	  glyph_t* g1    = &frget(curr_frame,i,j);
	  uint16_t flags = g1->flags;
	  char     chr   = g1->ascii;
	  char     color = g1->color;
	  // floor
	  double cx = j;
	  double cy = 20 - i;
	  // it's our hero!
	  if (chr != '>') { // if ladder down, don't put a floor
 	    fprintf( outf, "object { Floor ");
	    fprintf( outf, " translate <%5f,  0,%5f> }\n", cx, cy);
	  }
	  // ceiling
	  if (chr != '<') { // if ladder up, don't put a ceiling
 	    fprintf( outf, "object { Ceiling ");
	    fprintf( outf, " translate <%5f,  0,%5f> }\n", cx, cy);
	  }
	  // thing	 
          fprintf ( outf, "object { Tiles[%d][%d]\n", chr, color);
	  if (!is_monster(chr)) {
	    // monsters don't move
	    fprintf( outf, " translate <%5f,  0,%5f>\n}\n", cx, cy);
	  }  else {
	    //
	    // monsters can move
	    //
	    const double dx0 = g0->dx;
	    const double dy0 = g0->dy;
	    const double dx1 = g1->dx;
	    const double dy1 = g1->dy;

	    // interpolated speed is only for computing the rotation angle
	    const double dx = (1.0-T)*dx0 + T*dx1;
	    const double dy = (1.0-T)*dy0 + T*dy1;
	    
	    double angle = 90-(180.0/M_PI)*atan2f(dy, dx);
	    fprintf( outf, " rotate %5f*y\n", angle );
	    // when T < 1, we are showing the frames between the previous and
	    // the current. Thus we need to displace the glyph *backwards* 
	    // according to current speed and position
	    if (!g1->tele) {
	      // smooth movement
	      cx -= (1.0-T)*dx1;
	      cy -= (1.0-T)*dy1;
	    } else {
	      // the guy teleported
	      angle = 0;
	    }
	    fprintf( outf, " translate <%5f,  0,%5f>\n}\n", cx, cy);
	    if ((i == curr_frame->hero_i) && (j == curr_frame->hero_j)) { // is this our hero??
	      double camx, camy;
	      if (!g1->tele) {
		camx = prev_hero_x;
		camy = prev_hero_y;
	      } else {
		// the guy likely teleported
		// default camera a little behind the guy
		camx = cx;
		camy = cy - 0.2;
	      }
	      prev_hero_x = cx;
	      prev_hero_y = cy;
	      //
	      // this is our hero!
	      // lights and cameras please
	      //
	      fprintf ( outf, "light_source { GlobalLight translate %5f*x+%5f*z }\n", camx, camy );
	      fprintf ( outf, "light_source { LocalLight  translate %5f*x+%5f*z }\n", cx, cy);
	      fprintf ( outf, "camera {\n" );
	      fprintf ( outf, "  perspective\n" );
	      fprintf ( outf, "  right (1920/1080)*x\n" );
	      fprintf ( outf, "  sky y\n" );
	      fprintf ( outf, "  location <%5f,CameraHeight,%5f> \n", camx, camy);
	      fprintf ( outf, "  look_at  <%5f,CameraHeight,%5f>\n", cx,  cy );
	      fprintf ( outf, "}\n" );
	    }
	  }
        }
    }

}
