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

//------------------------------------------------------------
/**
 * most important function of this program
 */
void map_to_pov(map_t* prev_map,
		  map_t* curr_map,
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
    map_t* map, *prev_map;
    map = map_init();
    prev_map = map_init();
    map_reset(map);
    map_reset(prev_map);
    
    
    int game_map = 0;
    int video_map = 0;
    int prev_time = -1;
    while ( !feof ( inf ) ) {
    } // while
    
    printf ( "processed %d game maps\n", game_map );
    printf ( "wrote %d video maps\n", video_map );
    fclose ( inf );
    tty_free ( vt );
    map_free ( prev_map );
    map_free (map);
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

void map_to_pov(map_t* prev_map,
		  map_t* curr_map,
		  const config_t* cfg,
		  double T,
		  FILE* outf ) {

  fprintf ( outf, "#version 3.7;\n" );
  fprintf ( outf, "#declare GameFrame = %d;\n", curr_map->number );
  fprintf ( outf, "#declare GameSubFrame = %f;\n", T );
  fprintf ( outf, "#include \"%s\"\n", cfg->tileset_file );

  for ( int y1 = 1 ; ( y1 < 22 ) ; ++y1 ) {
        for ( int x1 = 0 ; x1 < NH_COLS ; ++x1 ) {
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

}
