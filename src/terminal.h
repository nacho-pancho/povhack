/**
 * our own special-purpose in-memory terminal handler
 * operating directly on Nethack maps
 */
#ifndef TERMINAL_H
#define TERMINAL_H
#include "window.h"
#include "map.h"
/**
 * represents the terminal as an output device
 * with the additional abstractions defined by NetHack:
 * windows: 
 *   different screens; Nethack handles like 5 of them
 *   each with different roles
 * glyph vs normal text:
 *   everything that is written between a 'begin glyph' command
 *   and an 'end glyph' command has additional information besides
 *   style (bold, etc.) and character
 *
 * windows:
 * window ids are assigned dynamically and depend of compile time stuff.
 * for Nethack 3.6.6, these appear to be the assignments
 *
 *   0
 *   1: messages to the player such as 'The grid bug bites'
 *   2: status bar
 *   3: map
 *   4: inventory
 *   5: other text windows
 *   6: epitaph
 */  
typedef struct {
  window_t* windows[NWINDOWS];
  // state of the window
  int16_t current_window;
  int32_t current_glyph_code;
  int32_t current_glyph_flags;
  int     is_glyph;
  map_t*  map; // game map; a superset of window with additional game info
  int     data_has_ended;
} terminal_t;

terminal_t* terminal_init();
void terminal_free(terminal_t* f);
void terminal_reset(terminal_t* f);
void terminal_copy(terminal_t* dst, const terminal_t* src);
void terminal_put(terminal_t* f, int c);
#endif
