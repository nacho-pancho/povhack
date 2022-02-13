#include <stdio.h>
#include <tty.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "logging.h"
#include "options.h"
#include "vt100.h"
#include "frame.h"
#include "nethack.h"
#include "motion.h"

//------------------------------------------------------------
/**
 * most important function of this program
 */
void frame_to_pov(frame_t* prev_frame,
		  frame_t* curr_frame,
		  const config_t* cfg,
		  double time,
		  FILE* outf );

//------------------------------------------------------------

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
	    // catpure hero position
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
