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
#include "povray.h"


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
	snprintf(ofname,127,"%s_map_%08d.txt",cfg.output_prefix,frame_number);
	fout = fopen(ofname,"w");
	window_save(term->windows[WIN_MAP],  fout);
	fclose(fout);

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


