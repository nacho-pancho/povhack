#include <stdio.h>
#include <tty.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "logging.h"
#include "options.h"
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
		terminal_t* prev_term,
		int frame,
		double time,
		const config_t* cfg,
		FILE* outf );

/**
 * generates the text overlay as a POV scene
 */
void txt_to_pov(terminal_t* term,
		terminal_t* prev_term,
		int frame,
		double time,
		const config_t* cfg,
		FILE* outf );

//------------------------------------------------------------

int main ( int argc, char * argv[] ) {
    config_t cfg = parse_opt ( argc, argv );
    
    char ofname[128];
    FILE * fin = NULL;
    FILE* fout = NULL;
    printf ( "POVHACK\n" );
    //
    //
    if (!cfg.input_file) {
      error("Must specify input file with -i option\n");
      exit(1);
    } else if (!cfg.output_prefix) {
      error("Must specify output file prefix with -o option\n");
      exit(1);
    }
    fin = fopen ( cfg.input_file, "r" );
    if (!fin) {
      fprintf(stderr,"could not open file %s for reading.\n",cfg.input_file);
      exit(1);
    }

    terminal_t* term = terminal_init();
    terminal_t* prev_term = terminal_init();
    int frame_number = 0;
    int video_number = 0;
    while (!feof(fin)) {
      int c = fgetc(fin);
      terminal_put(term,c);
      if (term->data_has_ended) {
	debug("END OF DATA. frame %d active window %d\n",
	      frame_number,term->current_window);
	//
	// dump current windows
	//
	status_t s = get_status(term);
	detect_motion(term->map,prev_term->map);
	if (cfg.dump) {
	  snprintf(ofname,127,"%s_%06d.dump",cfg.output_prefix,frame_number);
	  FILE* fdump = fopen(ofname,"w");
	  if (!fdump) {
	    error("cannot open %s for writing.\n",ofname);
	  }
	  fprintf(fdump,"\n====================\n");
	  fprintf(fdump,"END OF DATA: FRAME %4d TIME %4d\n",frame_number,s.moves);
	  fprintf(fdump,"====================\n");
	  print_status(&s,fdump);
	  for (int w = 1; w <= WIN_INVENTORY; ++w) {
	    fprintf(fdump,"\nWINDOW NUMBER %4d\n",w);
	    window_dump(term->windows[w],fdump);
	  }
	  fprintf(fdump,"\nMAP\n");
	  map_dump(term->map,fdump);
	  fprintf(fdump,"\nMOTION\n");
	  map_dump_motion(term->map, fdump);
	  fclose(fdump);
	}
	//
	// generate POV frame from window map
	//
	double step = 1.0/(double)cfg.submaps;
	for (double T = step; T <= 1.0; T += step) {
	  snprintf(ofname,127,"%s_map_%08d.pov",cfg.output_prefix,video_number);
	  fout = fopen(ofname,"w");
	  if (!fout) exit(1);
	  map_to_pov(term,prev_term,frame_number,T,&cfg,fout);
	  fclose(fout);

	  snprintf(ofname,127,"%s_txt_%08d.pov",cfg.output_prefix,video_number);
	  fout = fopen(ofname,"w");
	  if (!fout) exit(1);
	  txt_to_pov(term,prev_term,frame_number,T,&cfg,fout);
	  fclose(fout);
	  video_number ++;
	}
	//
	// save other windows
	//
	snprintf(ofname,127,"%s_msg_%08d.txt",cfg.output_prefix,frame_number);
	fout = fopen(ofname,"w");
	window_save(term->windows[WIN_MESSAGE],  fout);
	fclose(fout);

	snprintf(ofname,127,"%s_sta_%08d.txt",cfg.output_prefix,frame_number);
	fout = fopen(ofname,"w");
	window_save(term->windows[WIN_STATUS],   fout);
	fclose(fout);
	
	snprintf(ofname,127,"%s_inv_%08d.txt",cfg.output_prefix,frame_number);
	fout = fopen(ofname,"w");
	window_save(term->windows[WIN_INVENTORY],fout);
	fclose(fout);
	//
	// move on
	//
	// update state
	//
	term->data_has_ended = 0;
	//
	// only messages and status are persistent
	// and need to be preserved
	//
	window_copy(prev_term->windows[WIN_BASE],term->windows[WIN_BASE ]);
	window_copy(prev_term->windows[WIN_MESSAGE],term->windows[WIN_MESSAGE]);
	window_copy(prev_term->windows[WIN_STATUS],term->windows[WIN_STATUS]);
	// previous map is only updated if the current window was the map
	if (term->current_window == WIN_MAP) {
	  window_copy(prev_term->windows[WIN_MAP],term->windows[WIN_MAP]);
	  map_copy(prev_term->map,term->map);
	  //window_reset(term->windows[WIN_MESSAGE]);
	  //window_reset(term->windows[WIN_STATUS]);
	  //window_reset(term->windows[WIN_INVENTORY]);
	}
	frame_number++;
      }
    }

    fclose ( fin );
    terminal_free(term);
    terminal_free(prev_term);
    exit ( 0 );
}


//------------------------------------------------------------

static void put_floor(int x, int y, FILE*outf) {
  fprintf( outf, "object { Floor  ");
  fprintf( outf, "translate < %2d, %2d,  0 > ", x, y);
  fprintf( outf, "texture {FloorTexture} } \n");
}


static void put_corner(int x, int y, int angle, FILE* outf) {
  put_floor(x,y,outf);
  fprintf( outf, "object { Corner rotate %d*z ", angle); 
  fprintf( outf, "translate < %2d, %2d,  0 > ", x, y);
  fprintf( outf, "texture {WallTexture} } \n");
}


static void put_wall(int x, int y, int angle, FILE* outf) {
  put_floor(x,y,outf);
  fprintf( outf, "object { Wall   rotate %d*z ", angle); 
  fprintf( outf, "translate < %2d, %2d,  0 > ", x, y);
  fprintf( outf, "texture {WallTexture} } \n");
}

static void put_open_door(int x, int y, int angle, FILE* outf) {
  put_floor(x,y,outf);
  fprintf( outf, "object { OpenDoor   rotate %d*z ", angle); 
  fprintf( outf, "translate < %2d, %2d,  0 > }\n", x, y);
}

static void put_closed_door(int x, int y, int angle, FILE* outf) {
  put_floor(x,y,outf);
  fprintf( outf, "object { ClosedDoor   rotate %d*z ", angle); 
  fprintf( outf, "translate < %2d, %2d,  0 > }\n", x, y);
}

static void put_broken_door(int x, int y, int angle, FILE* outf) {
  put_floor(x,y,outf);
  fprintf( outf, "object { BrokenDoor   rotate %d*z ", angle); 
  fprintf( outf, "translate < %2d, %2d,  0 > }\n", x, y);
}

static void put_ladder_up(int x, int y, FILE* outf) {
  put_floor(x,y,outf);
  fprintf( outf, "object { Ladder ");
  fprintf( outf, "translate < %2d, %2d,  0 > }\n", x, y);
}

static void put_ladder_down(int x, int y, FILE* outf) {
  // no floor
  fprintf( outf, "object { Ladder ");
  fprintf( outf, "translate < %2d, %2d,  0 > }\n", x, y);
}

static void put_altar(int x, int y, FILE* outf) {
  put_floor(x,y,outf);
  fprintf( outf, "object { Trap   ");
  fprintf( outf, "translate < %2d, %2d,  0 > }\n", x, y);
}

static void put_fountain(int x, int y, FILE* outf) {
  put_floor(x,y,outf);
  fprintf( outf, "object { Fountain ");
  fprintf( outf, "translate < %2d, %2d,  0 > }\n", x, y);
}

static void put_water(int x, int y, FILE* outf) {
  put_floor(x,y,outf);
  fprintf( outf, "object { Water  ");
  fprintf( outf, "translate < %2d, %2d,  0 > }\n", x, y);
}

static void put_trap(int x, int y, FILE* outf) {
  fprintf( outf, "object { Trap ");
  fprintf( outf, "translate < %2d, %2d,  0 > }\n", x, y);
}

void map_to_pov(terminal_t* term,
		terminal_t* prev_term,
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
  fprintf(outf,"//\n// STRUCTURE \n//\n");
  for ( int y1 = 1 ; ( y1 < 22 ) ; ++y1 ) {
    for ( int x1 = 0 ; x1 < map->ncols ; ++x1 ) {
      glyph_t* g = map_get_dungeon(map,x1,y1);
      if (g->code == 0) {
	continue;
      }
      switch(g->ascii) {
      case '.':
	if (is_door(map,x1,y1)) {
	  switch (is_door(map,x1,y1)) { 
	  case H_DOOR_BROKEN:
	    put_broken_door(x1,y1, 0,outf);
	    break;
	  case V_DOOR_BROKEN:
	    put_broken_door(x1,y1,90,outf);
	    break;
	  }
	} else { // is floor
	  put_floor(x1,y1,outf);
	}
	break;
	
      case '-':
	if (is_door(map,x1,y1)) {
	  put_open_door(x1,y1,90, outf);
	} else if (is_corner(map,x1,y1)) {
	  switch (is_corner(map,x1,y1)) {
	  case NW_CORNER:
	    put_corner(x1, y1,  0, outf);
	    break;
	  case NE_CORNER:
	    put_corner(x1, y1, 90, outf);
	    break;
	  case SW_CORNER:
	    put_corner(x1, y1,-90, outf);
	    break;
	  case SE_CORNER:
	    put_corner(x1, y1,180, outf);
	    break;
	  default:
	    put_floor(x1,y1,outf);
	    break;
	  }
	} else { // is a horizontal wall
	  put_wall(x1,y1,0,outf);
	}
	break;
	
      case '|':
	if (is_door(map,x1,y1)) {
	  put_open_door(x1,y1,0,outf);
	} else if (is_wall(map,x1,y1)) {
	  put_wall(x1,y1,90,outf);
	} else {
	  // may be something else, object or moster
	}
	break;
	
      case '+':	
	switch (is_door(map,x1,y1)) {
	case NO_DOOR:
	  // could be something else
	  break;
	case H_DOOR_CLOSED:
	  put_closed_door(x1,y1, 0,outf);
	  break;
	case V_DOOR_CLOSED:
	  put_closed_door(x1,y1,90,outf);
	  break;
	default:
	  put_floor(x1,y1,outf);
	  break;
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
	put_water(x1,y1,outf);
	break;
      case '{':
	put_fountain(x1,y1,outf);
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
  fprintf(outf,"//\n// OBJECTS \n//\n");
  for ( int y1 = 1 ; ( y1 < 22 ) ; ++y1 ) {
    for ( int x1 = 0 ; x1 < map->ncols ; ++x1 ) {
      glyph_t* g = map_get_object(map,x1,y1);
      if (g->code == 0) {
	continue;
      }
      put_floor(x1,y1,outf);
      fprintf ( outf, "object { Tiles[%d][%d] ", g->ascii, g->color);	  
      fprintf( outf, " translate < %2d, %2d,  0> }\n", x1, y1);
    }
  }
  //
  // monsters: these move
  //
  fprintf(outf,"//\n// MONSTERS \n//\n");
  double hx = map->hero_x;
  double hy = map->hero_y;
  map_t* prev_map = prev_term->map;
  for ( int y1 = 1 ; ( y1 < 22 ) ; ++y1 ) {
    for ( int x1 = 0 ; x1 < map->ncols ; ++x1 ) {
      int is_hero = (x1 == hx) && (y1 == hy);
      glyph_t* g1 = map_get_monster(map,x1,y1);
      if (g1->code == 0) {
	continue;
      }
      const double dx1 = g1->dx;
      const double dy1 = g1->dy;
      
      // previous info on this glyph
      // is located at its previous position (given by displacement)
      const int x0 = x1 - g1->dx;
      const int y0 = y1 - g1->dy;
      const glyph_t* g0 = map_get_monster(prev_map,x0,y0);
      double dx0, dy0;
      double dx, dy;
      double angle;
      double x, y;
      const double T = time;
      put_floor(x1,y1,outf);
      if (g0->code != g1->code) {
	// not same glyph!
	// surely we are not in the right window
	// warn("frame %06d: not same glyph g1:%4d %c g0:%4d %c !\n",frame,g1->code,g1->ascii,g0->code,g0->ascii);
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
      fprintf( outf, " rotate %5f*z ", angle );
      // when T < 1, we are showing the maps between the previous and
      // the current. Thus we need to displace the glyph *backwards* 
      // according to current speed and position
      if (!is_hero) {
	fprintf( outf, " translate < %7.3f, %7.3f,  0> }\n", x, y);
      } else { // it's our hero
	fprintf(outf,"//\n// HERO \n//\n");
	fprintf( outf, " translate < %7.3f, %7.3f,  0> no_shadow }\n", x, y);
	fprintf ( outf, "light_source { GlobalLight translate < %7.3f, %7.3f,  0 > }\n", x, y );
	fprintf ( outf, "light_source { LocalLight  translate < %7.3f, %7.3f,  0 > }\n", x, y);
	fprintf ( outf, "camera {\n" );
	fprintf ( outf, "  perspective\n" );
	fprintf ( outf, "  right (1920/1080)*x\n" );
	fprintf ( outf, "  sky z\n" );
	fprintf ( outf, "  location < %7.3f+CamDistX, %7.3f+CamDistY, CameraHeight> \n", x, y);
	fprintf ( outf, "  look_at  < %7.3f, %7.3f, LookAtHeight>\n", x, y );
	fprintf ( outf, "}\n" );
      }      
    } // for x
  } // for y
  fprintf(outf,"//\n// EFFECTS \n//\n");
  //
  // we cannot identify effects  for now
  //

}
  
void txt_to_pov(terminal_t* term1,
		terminal_t* term0,
		int frame,
		double T, // 0 < T <= 1
		const config_t* cfg,
		FILE* outf ) {
  window_t* w;
  fputs("#version 3.7;\n",outf);
  fputs("#include \"terminal.inc\"\n",outf);
  // message
  fputs("\n\n//\n// MESSAGE\n//\n\n",outf);
  w = term1->windows[WIN_MESSAGE];
  for (int y = 0; y < 2; ++y) {
    for (int x = 0; x < w->ncols; ++x) {
      const int k = y*w->ncols+x;
      const char c1 = w->ascii[k];      
      if (c1 <= ' ') continue;
      const int  s1 = w->color[k];
      fprintf(outf,"PutString(\"%c\",%d,%f,%d,%d)\n",c1,s1,0.0,x,y);
    }
  }
  fputs("\n\n//\n// STATUS\n//\n\n",outf);
  w = term1->windows[WIN_STATUS];
  for (int y = w->nrows-2; y < w->nrows; ++y) {
    for (int x = 0; x < w->ncols; ++x) {
      const int k = y*w->ncols+x;
      const char c1 = w->ascii[k];      
      if (c1 <= ' ') continue;
      const int  s1 = w->color[k];
      fprintf(outf,"PutString(\"%c\",%d,%f,%d,%d)\n",c1,s1,0.0,x,y+15);
    }
  }
  //
  // windows above STATUS only appear when active
  //
  for (int r = WIN_INVENTORY; r < NWINDOWS; ++r) {
    if (term1->current_window == r) {
      fprintf(outf,"\n\n//\n// WINDOW %d\n//\n\n",r);
      w = term1->windows[r];
      for (int y = 0; y < w->nrows; ++y) {
	for (int x = 0; x < w->ncols; ++x) {
	  const int k = y*w->ncols+x;
	  const char c1 = w->ascii[k];      
	  if (c1 <= ' ') continue;
	  const int  s1 = w->color[k];
	  fprintf(outf,"PutString(\"%c\",%d,%f,%d,%d)\n",c1,s1,0.0,x,y);      
	}
      }
    }
  }
}

