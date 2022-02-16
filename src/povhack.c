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

static void put_floor(int x, int y, FILE*outf) {
  fprintf( outf, "object { Floor ");
  fprintf( outf, " translate < %2d, %2d,  0 > ", x, y);
  fprintf( outf, " texture {FloorTexture} } \n");
}


static void put_corner(int x, int y, int angle, FILE* outf) {
  put_floor(x,y,outf);
  fprintf( outf, "object { Corner rotate %d*z ", angle); 
  fprintf( outf, " translate < %2d, %2d,  0 > ", x, y);
  fprintf( outf, " texture {WallTexture} } \n");
}


static void put_wall(int x, int y, int angle, FILE* outf) {
  put_floor(x,y,outf);
  fprintf( outf, "object { Wall rotate %d*z ", angle); 
  fprintf( outf, " translate < %2d, %2d,  0 > ", x, y);
  fprintf( outf, " texture {WallTexture} } \n");
}

static void put_door(int x, int y, int angle, FILE* outf) {
  put_floor(x,y,outf);
  fprintf( outf, "object { Door rotate %d*z ", angle); 
  fprintf( outf, " translate < %2d, %2d,  0 > }\n", x, y);
}

static void put_ladder_up(int x, int y, FILE* outf) {
  put_floor(x,y,outf);
  fprintf( outf, "object { Ladder ");
  fprintf( outf, " translate < %2d, %2d,  0 > }\n", x, y);
}

static void put_ladder_down(int x, int y, FILE* outf) {
  // no floor
  fprintf( outf, "object { Ladder ");
  fprintf( outf, " translate < %2d, %2d,  0 > }\n", x, y);
}

static void put_altar(int x, int y, FILE* outf) {
  put_floor(x,y,outf);
  fprintf( outf, "object { Trap ");
  fprintf( outf, " translate < %2d, %2d,  0 > }\n", x, y);
}

static void put_fountain(int x, int y, FILE* outf) {
  put_floor(x,y,outf);
  fprintf( outf, "object { Fountain ");
  fprintf( outf, " translate < %2d, %2d,  0 > }\n", x, y);
}

static void put_water(int x, int y, FILE* outf) {
  put_floor(x,y,outf);
  fprintf( outf, "object { Water ");
  fprintf( outf, " translate < %2d, %2d,  0 > }\n", x, y);
}

static void put_trap(int x, int y, FILE* outf) {
  fprintf( outf, "object { Trap ");
  fprintf( outf, " translate < %2d, %2d,  0 > }\n", x, y);
}

void map_to_pov(terminal_t* term,
		int frame,
		double time,
		const config_t* cfg,
		FILE* outf ) {
  
  fprintf ( outf, "#version 3.7;\n" );
  fprintf ( outf, "#declare GameFrame = %d;\n", frame );
  fprintf ( outf, "#declare GameSubFrame = %f;\n", time );
  fprintf ( outf, "#include \"%s\"\n", cfg->tileset_file );
  //
  // we write one layer at a time
  //
  map_t* map = term->map;
  //
  // structure layer: this one is handled using fixed objects
  // and additional logic to guess the orientation of objects
  // such as open doors, corners, etc.
  for ( int y1 = 1 ; ( y1 < 22 ) ; ++y1 ) {
    for ( int x1 = 0 ; x1 < map->ncols ; ++x1 ) {
      glyph_t* g = map_get_dungeon(map,x1,y1);
      switch(g->ascii) {
      case '.':
	put_floor(x1,y1,outf);
	break;
      case '|':
	put_wall(x1,y1,90,outf);
	break;
      case '-':
	if (y1 > 0) {
	  glyph_t* up = map_get_dungeon(map,x1,y1-1);
	  if (up->ascii == '|') {
	    // some this is a corner
	    if (x1 > 0) {
	      glyph_t* left = map_get_dungeon(map,x1-1,y1);
	      if (left->ascii == '-') {
		// southeast corner
		put_corner(x1,y1,-90,outf);
	      } else {
	        // southwest corner
		put_corner(x1,y1,0,outf);
	      }
	    } else {
	        // southwest corner
		put_corner(x1,y1,0,outf);
	      // southwest corner
	    }
	  } else {
	    // horizontal wall
	    put_wall(x1,y1,0,outf);
	  }
	} else if (y1 < (map->nrows-1)) {
	  glyph_t* down = map_get_dungeon(map,x1,y1+1);
	  if (down->ascii == '|') {
	    // this is a corner
	    if (x1 > 0) {
	      glyph_t* left = map_get_dungeon(map,x1-1,y1);
	      if (left->ascii == '-') {
	        // northeast corner
		put_corner(x1,y1,180,outf);
	      } else {
	        // northwest corner
		put_corner(x1,y1,90,outf);
	      }
	    } else {
	      // northwest corner
	      put_corner(x1,y1,90,outf);
	    }
	  } else {
	    // horizontal wall
	    put_wall(x1,y1,90,outf);
	  }
	}
	break;
      case '+':
	{
	  glyph_t dummy;
	  dummy.ascii = 0;
	  dummy.code  = 0;
	  // check surrounding walls for orientation
	  glyph_t* n = y1 > 0 ? map_get_dungeon(map,x1,y1-1) : &dummy;
	  glyph_t* s = y1 < (map->nrows-1) ? map_get_dungeon(map,x1,y1+1) : &dummy;
	  glyph_t* w = x1 > 0 ? map_get_dungeon(map,x1-1,y1) : &dummy;
	  glyph_t* e = x1 < (map->ncols-1) ? map_get_dungeon(map,x1+1,y1) : &dummy;
	  if ((n->ascii == '|') || (s->ascii == '|')) {
	    // vertical door for sure
	    put_door(x1,y1,90,outf);
	  } else if ((e->ascii == '-') || (w->ascii == '-')) {
	    // horizontal door
	    put_door(x1,y1,0,outf);
	  } else {
	    // unhandled door orientation?	    
	    put_door(x1,y1,45,outf);
	  }
	}
	break;
      case '#':
	put_floor(x1,y1,outf);
	break;
      case '<':
	put_ladder_up(x1,y1,outf);
	break;
      case '>':
	put_ladder_down(x1,y1,outf);
	break;
      case '_':
	put_altar(x1,y1,outf);
	break;
      case '}':
	put_fountain(x1,y1,outf);
	break;
      case '{':
	put_water(x1,y1,outf);
	break;
      case '^':
	put_trap(x1,y1,outf);
	break;
      default:
	break;
      }
    }
  }
  //
  // objects: the easiest
  //
  for ( int y1 = 1 ; ( y1 < 22 ) ; ++y1 ) {
    for ( int x1 = 0 ; x1 < map->ncols ; ++x1 ) {
      glyph_t* g = map_get_object(map,x1,y1);
      fprintf ( outf, "object { Tiles[%d][%d] ", g->ascii, g->color);	  
      fprintf( outf, " translate < %2d, %2d,  0>\n}\n", x1, y1);
    }
  }
  //
  // monsters: these move
  //
  double hx = map->hero_x;
  double hy = map->hero_y;
  for ( int y1 = 1 ; ( y1 < 22 ) ; ++y1 ) {
    for ( int x1 = 0 ; x1 < map->ncols ; ++x1 ) {
      int is_hero = (x1 == hx) && (y1 == hy);
      glyph_t* g1 = map_get_monster(map,x1,y1);
      const double dx1 = g1->dx;
      const double dy1 = g1->dy;
      
      // previous info on this glyph
      // is located at its previous position (given by displacement)
      const int x0 = x1 - g1->dx;
      const int y0 = y1 - g1->dy;
      const glyph_t* g0 = map_get_monster(map,x0,y0);
      double dx0, dy0;
      double dx, dy;
      double angle;
      double x, y;
      const double T = time;
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
      fprintf ( outf, "object { Tiles[%d][%d] ", g1->ascii, g1->color);	  
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
    } // for x
  } // for y
  //
  // we cannot identify effects  for now
  //

}
