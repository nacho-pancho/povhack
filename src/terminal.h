/**
 * our own special-purpose in-memory terminal handler
 * operating directly on Nethack frames
 */
#ifndef TERMINAL_H
#define TERMINAL_H
#include "frame.h"

void frame_write_char(frame_t* f, char c);

#endif
