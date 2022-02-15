#ifndef OPTIONS_H
#define OPTIONS_H
#include <argp.h>

/**
 * General description of what this program does; appears when calling with --help
 */
#define PROGRAM_DOC "\n*** convert nethack games into POVRAY animations ***\n"

#define ARGS_DOC "<INPUT_FILE>"

/**
 * Program options. These are filled in by the argument parser
 */
typedef struct config {
    const char * input_file;
    const char * tileset_file;
    const char * output_prefix;
  int submaps;
} config_t;

config_t parse_opt ( int argc, char* * argv );

#endif
