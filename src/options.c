#include <stdlib.h>
#include "options.h"
#include "logging.h"

static struct argp_option options[] = {
    {"verbose",        'v', 0, OPTION_ARG_OPTIONAL, "Produce verbose output", 0 },
    {"quiet",          'q', 0, OPTION_ARG_OPTIONAL, "Don't produce any output", 0 },
    {"input",          'i', "path",    0, "input file", 0 },
    {"prefix",         'o', "path",    0, "output file prefix", 0 },
    {"dump",           'd', 0, OPTION_ARG_OPTIONAL, "Save dump of frames to readable files.", 0 },
    {"tileset",        't', "path",    0, "tileset file", 0 },
    {"oversample",     'r', "int",     0, "number of video maps between game maps", 0 },
    { 0 } // terminator
};

/*
 * argp callback for parsing a single option.
 */
static int _parse_opt ( int key, char * arg, struct argp_state * state ) {
    /* Get the input argument from argp_parse,
     * which we know is a pointer to our arguments structure.
     */
    config_t * cfg = ( config_t* ) state->input;
    switch ( key ) {
    case 'q':
        set_log_level ( LOG_ERROR );
        break;
    case 'v':
        set_log_level ( LOG_DEBUG );
        break;
    case 'd':
      cfg->dump = 1;
        break;
    case 'i':
        cfg->input_file = arg;
        break;
    case 'o':
        cfg->output_prefix = arg;
        break;
    case 't':
        cfg->tileset_file = arg;
        break;
    case 'r':
      cfg->submaps = atoi(arg);
        break;

    case ARGP_KEY_ARG:
        switch ( state->arg_num ) {
        default:
            /** too many arguments! */
            error ( "There are no free arguments in this program.\n" );
            argp_usage ( state );
            break;
        }
        break;
    case ARGP_KEY_END:
        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/**
 * argp configuration structure
 */
static const char args_doc[]    = ARGS_DOC;
static const char program_doc[] = PROGRAM_DOC;

static struct argp argp = { options, _parse_opt, args_doc, program_doc, 0, 0, 0 };

config_t parse_opt ( int argc, char* * argv ) {

    config_t cfg;
    cfg.dump = 0;
    cfg.submaps = 1;
    cfg.input_file    = NULL;
    cfg.output_prefix = NULL;
    cfg.tileset_file  = "tileset.pov";
    argp_parse ( &argp, argc, argv, 0, 0, &cfg );

    return cfg;
}
