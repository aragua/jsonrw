#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <jansson.h>
#include <getopt.h>

typedef enum
{
    AM_CREATE,
    AM_GET,
    AM_SET,
    AM_UNKNOWN
} action_mode_e;

static action_mode_e get_action_mode_from_string( const char * string );
static int get_field_value( const char * file, const char * field );

static int verbose = 0;

int main ( int argc, char ** argv )
{
    int c;
    action_mode_e mode = AM_GET;
    char * input = NULL, * output = NULL, * field = NULL;

    opterr = 0;

    while ((c = getopt (argc, argv, "vm:f:i:o:")) != -1)
    {
        switch (c)
        {
        case 'm':
            mode = get_action_mode_from_string(optarg);
            break;
        case 'f':
            field = optarg;
            break;
        case 'i':
            input = optarg;
            break;
        case 'o':
            output = optarg;
            break;
        case 'v':
            verbose = 1;
            break;
        case '?':
            if ( optopt == 'f' || optopt == 'm' || optopt == 'i' || optopt == 'o' )
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf (stderr,
                         "Unknown option character `\\x%x'.\n",
                         optopt);
            return EXIT_FAILURE;
        default:
            abort ();
        }
    }

    switch (mode)
    {

    case AM_CREATE:
        fprintf( stderr, "Action mode not implemented\n");
        break;

    case AM_GET:
        if ( !input )
        {
            fprintf( stderr, "Usage : jsonrw -f <file> [OPTIONS]\n");
            return EXIT_FAILURE;
        }
        if ( !field )
        {
            fprintf( stderr, "Option field (-f) is mandatory with -m get\nUsage : jsonrw -m get -f <field_value>\n");
            return EXIT_FAILURE;
        }
        if ( get_field_value( input, field ) != EXIT_SUCCESS )
            return EXIT_FAILURE;
        break;

    case AM_SET:
        fprintf( stderr, "Action mode not implemented\n");
        break;

    default:
        fprintf( stderr, "Unknown action mode\n");
        break;
    }


    return EXIT_SUCCESS;
}


static action_mode_e get_action_mode_from_string( const char * string )
{
    if ( string )
    {
        if ( strncmp("create", string,strlen("create")+1) == 0 )
            return AM_CREATE;
        else if ( strncmp("get", string,strlen("get")+1) == 0 )
            return AM_GET;
        else if ( strncmp("set", string,strlen("set")+1) == 0 )
            return AM_SET;
    }
    return AM_UNKNOWN;
}


static int get_field_value( const char * file, const char * field )
{
    json_t * file_contents, * obj;
    json_error_t err;

    file_contents = json_load_file( file, 0, &err);
    if (!json_is_object(file_contents))
    {
        fprintf( stderr, "JSON decode of '%s' failed(%d): %s\n", file, err.line, err.text);
        return EXIT_FAILURE;
    }

    obj = json_object_get( file_contents, field );
    if (json_is_object(obj))
    {
        fprintf( stderr, "Error parsing json %s to find field %s\n", file, field );
        json_object_clear(file_contents);
        return EXIT_FAILURE;
    }

    if ( json_is_array(obj) )
        verbose == 1 ? fprintf( stdout, "%s is an array\n", field ) : fprintf( stdout, "%s\n", json_string_value(obj) ) ;
    else
    {
        if ( verbose == 1 )
            fprintf( stdout, "%s = %s \n", field, json_string_value(obj) );
        else
            fprintf( stdout, "%s\n", json_string_value(obj) );
    }

    json_object_clear(obj);
    json_object_clear(file_contents);

    return EXIT_SUCCESS;
}
