#ifndef POVRAY_H
#define POVRAY_H

#include "options.h"
#include "terminal.h"
#include <stdio.h>

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

#endif
