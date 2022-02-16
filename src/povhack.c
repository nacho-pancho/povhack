#include <stdio.h>
#include <tty.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "logging.h"
#include "options.h"
#include "vt100.h"
#include "map.h"
#include "nethack.h"
#include "motion.h"
#include "terminal.h"
#include "status.h"

//------------------------------------------------------------
/**
 * most important function of this program
 */
void map_to_pov(terminal_t* term,
		int frame,
		double time,
		const config_t* cfg,
		FILE* outf );


//------------------------------------------------------------

int main ( int argc, char * argv[] ) {
    config_t cfg = parse_opt ( argc, argv );
    
    char ofname[128];
    FILE * fin = NULL;
    
    printf ( "POVHACK\n" );
    //
    //
    fin = fopen ( cfg.input_file, "r" );
    if (!fin) {
      fprintf(stderr,"could not open file %s for reading.\n",cfg.input_file);
      exit(1);
    }

    terminal_t* term = terminal_init();
    int frame_number = 0;
    map_t* prev_map;
    prev_map = map_init(25,80);
    while (!feof(fin)) {
      int c = fgetc(fin);
      terminal_put(term,c);
      if (term->data_has_ended) {
	//
	// dump current windows
	//
	status_t s = get_status(term);
	printf("\n====================\n");
	printf("END OF DATA: FRAME %4d TIME %4d\n",frame_number,s.moves);
	printf("====================\n");
	print_status(&s);
	detect_motion(term->map,prev_map);
	for (int w = 1; w <= WIN_INVEN; ++w) {
	  printf("\nWINDOW NUMBER %4d\n",w);
	  window_dump(term->windows[w],stdout);
	}
	printf("\nMAP\n");
	map_dump(term->map,stdout);
	//
	// generate POV file
	//
	snprintf(ofname,127,"%s_%06d.pov",cfg.output_prefix,frame_number);
	FILE* fout = fopen(ofname,"w");
	if (!fout) exit(1);
	double T = 1.0;	
	map_to_pov(term,frame_number,T,&cfg,fout);
	fclose(fout);
	//
	// move on
	//
	map_copy(prev_map,term->map);
	term->data_has_ended = 0;
	frame_number++;
      }
    }

    fclose ( fin );
    terminal_free(term);
    map_free(prev_map);
    exit ( 0 );
}


//------------------------------------------------------------

void map_to_pov(terminal_t* term,
		int frame,
		double time,
		const config_t* cfg,
		FILE* outf ) {
  
  fprintf ( outf, "#version 3.7;\n" );
  fprintf ( outf, "#declare GameFrame = %d;\n", frame );
  fprintf ( outf, "#declare GameSubFrame = %f;\n", time );
  fprintf ( outf, "#include \"%s\"\n", cfg->tileset_file );
#if 0
  
  for ( int y1 = 1 ; ( y1 < 22 ) ; ++y1 ) {
        for ( int x1 = 0 ; x1 < curr_map->ncols ; ++x1 ) {
	  // current info on this glyph
	  glyph_t* g1      = 0; // map_get(curr_map,x1,y1);
	  double hx = curr_map->hero_x;
	  double hy = curr_map->hero_y;
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
	    const glyph_t* g0 = 0; // map_get(prev_map,x0,y0);
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
	    // when T < 1, we are showing the maps between the previous and
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
#endif
}
