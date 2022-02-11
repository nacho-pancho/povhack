#include <stdlib.h>
#include "options.h"
#include "logging.h"

static struct argp_option options[] = {
    {"verbose",        'v', 0, OPTION_ARG_OPTIONAL, "Produce verbose output", 0 },
    {"quiet",          'q', 0, OPTION_ARG_OPTIONAL, "Don't produce any output", 0 },
    {"prefix",         'o', "path",    0, "output file prefix", 0 },
    {"tileset",        't', "path",    0, "input file", 0 },
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
    case 'o':
        cfg->output_prefix = arg;
        break;
    case 't':
        cfg->tileset_file = arg;
        break;

    case ARGP_KEY_ARG:
        switch ( state->arg_num ) {
        case 0:
            cfg->input_file = arg;
            break;
        default:
            /** too many arguments! */
            error ( "Too many arguments!.\n" );
            argp_usage ( state );
            break;
        }
        break;
    case ARGP_KEY_END:
        if ( state->arg_num < 1 ) {
            /* Not enough mandatory arguments! */
            error ( "Too FEW arguments!\n" );
            argp_usage ( state );
        }
        break;

    default:
      return 1;
    }
    return 0;
}

/**
 * argp configuration structure
 */

config_t parse_opt ( int argc, char* * argv ) {

    config_t cfg;
    const char args_doc[]    = ARGS_DOC;
    const char program_doc[] = PROGRAM_DOC;
    
    struct argp argp = { options, _parse_opt, args_doc, program_doc, 0, 0, 0 };
    cfg.input_file  = NULL;
    cfg.output_prefix = "out_";
    cfg.tileset_file = "tileset.pov";
    argp_parse ( &argp, argc, argv, 0, 0, &cfg );

    return cfg;
}
