#include "povray.h"
#include <math.h>

//------------------------------------------------------------
static void put_void(int x, int y, FILE*outf) {
  //fprintf( outf, "PutVoid(%2d, %2d)\n ", x, y);
}

static void put_floor(int x, int y, FILE*outf) {
  fprintf( outf, "PutFloor(%2d, %2d)\n ", x, y);
}

static void put_ground(int x, int y, FILE*outf) {
  fprintf( outf, "PutGround(%2d, %2d)\n ", x, y);
}

static void put_corner(int x, int y, int angle, FILE* outf) {
  put_floor(x,y,outf);
  fprintf( outf, "PutCorner(%2d, %2d, %2d)\n ", x, y, angle);
}

static void put_corner2(int x, int y, int angle, FILE* outf) {
  put_floor(x,y,outf);
  fprintf( outf, "PutCorner2(%2d, %2d, %d)\n ", x, y, angle);
}

static void put_corner4(int x, int y, FILE* outf) {
  put_floor(x,y,outf);
  fprintf( outf, "PutCorner4(%2d, %2d)\n ", x, y);
}


static void put_wall(int x, int y, int angle, FILE* outf) {
  put_floor(x,y,outf);
  fprintf( outf, "PutWall(%2d, %2d, %2d)\n", x,y,angle); 
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
	put_void(x1,y1,outf);
	continue;
      }
      switch(g->ascii) {
      case ' ':
	put_void(x1,y1,outf);
	break;
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
	  case NES_CORNER:
	    put_corner2(x1, y1,-90, outf);
	    break;
	  case NWS_CORNER:
	    put_corner2(x1, y1, 90, outf);
	    break;
	  case ALL_CORNER:
	    put_corner4(x1, y1, outf);
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
	if ((g->color == 0) || (g->color == 15)) 
	  put_ground(x1,y1,outf);
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
      //put_floor(x1,y1,outf);
      fprintf ( outf, "object { Tiles[%d][%d] scale 0.8 ", g->ascii, g->color);	  
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
      
      // previous info on this glyph
      // is located at its previous position (given by displacement)
      const int x0 = x1 - g1->dx;
      const int y0 = y1 - g1->dy;
      const glyph_t* g0 = map_get_monster(prev_map,x0,y0);
      const double T = time;
      //put_floor(x1,y1,outf);
      //
      // heading
      //
      double x,y,dx0,dy0,dx1,dy1;
      // the character is indeed in the position
      // pointed at by the direction vectors
      if (g1->code == g0->code) {
	// the chararcter has a definite past direction
	dx0 = g0->code == g1->code ? g0->dx : dx1;
	dy0 = g0->code == g1->code ? g0->dy : dy1;
	if ((g1->dx != 0) || (g1->dy != 0)) {
	  // the character has a definite current direction
	  dx1 = g1->dx;
	  dy1 = g1->dy;
	} else {
	  // the character is still: maintain previous heading
	  dx1 = dx0;
	  dy1 = dy0;
	}
	x = (1.0-T)*x0 + T*x1;
	y = (1.0-T)*y0 + T*y1;	    
      } else {
	// we lost the reference on the previous position
	// and heading
	dx0 = dx1 = g1->dx;
	dy0 = dy1 = g1->dy;
	x = x1;
	y = y1;
      }
      double angle0 = (180.0/M_PI)*atan2f(dy0, dx0) - 90;
      double angle1 = (180.0/M_PI)*atan2f(dy1, dx1) - 90;
      if ((angle1 - angle0) > 180) {
	angle0 += 360;
      } else if ((angle1 - angle0) < -180) {
	angle0 -= 360;
      }
      const double angle = (1.0-T)*angle0 + T*angle1;	    
      
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
	fprintf ( outf, "  right AspectRatio*x\n" );
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

