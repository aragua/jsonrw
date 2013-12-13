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
static int set_field_value( const char * file, const char * field, const char * value );

static int verbose = 0;

int main ( int argc, char ** argv )
{
    int c;
    action_mode_e mode = AM_GET;
    char * input = NULL, * output = NULL, * field = NULL, * content = NULL;

    opterr = 0;

    while ((c = getopt (argc, argv, "vm:f:i:o:c:")) != -1)
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
        case 'c':
            content = optarg;
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
            fprintf( stderr, "Usage : jsonrw -i <file> [OPTIONS]\n");
            return EXIT_FAILURE;
        }
        if ( !field )
        {
            fprintf( stderr, "Option field (-f) is mandatory with -m get\nUsage : jsonrw -m get -i <file> -f <field_value>\n");
            return EXIT_FAILURE;
        }
        if ( get_field_value( input, field ) != EXIT_SUCCESS )
            return EXIT_FAILURE;
        break;

    case AM_SET:
        if ( !input )
        {
            fprintf( stderr, "Usage : jsonrw -m set -i <file> [OPTIONS]\n");
            return EXIT_FAILURE;
        }
        if ( !field )
        {
            fprintf( stderr, "Option field (-f) is mandatory with -m set\nUsage : jsonrw -m set -i <file> -f <field_value>\n");
            return EXIT_FAILURE;
        }
        if ( !content )
        {
            fprintf( stderr, "Option field (-c) is mandatory with -m set\nUsage : jsonrw -m set -i <file> -f <field_value> -c <content>\n");
            return EXIT_FAILURE;
        }
        if ( set_field_value( input, field, content ) != EXIT_SUCCESS )
            return EXIT_FAILURE;
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
    if (!obj)
    {
        fprintf( stderr, "Field %s not found\n", field );
        json_object_clear(file_contents);
        return EXIT_FAILURE;
    }

    if ( json_is_array(obj) )
        verbose == 1 ? fprintf( stdout, "%s is an array\n", field ) : fprintf( stdout, "array\n" );
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

static int set_field_value( const char * file, const char * field, const char * value )
{
    json_t * file_contents, * obj;
    json_error_t err;
    int ret;

    file_contents = json_load_file( file, 0, &err);
    if (!json_is_object(file_contents))
    {
        fprintf( stderr, "JSON decode of '%s' failed(%d): %s\n", file, err.line, err.text);
        return EXIT_FAILURE;
    }

    obj = json_object_get( file_contents, field );
 /*   if (obj)
        json_object_set( file_contents, field, json_string(value) ) < 0 ? fprintf( stderr, "json_object_set failed(%d): %s\n", err.line, err.text) : fprintf(stderr,"succeed\n") ;
    else*/
        json_object_set_new( file_contents, field, json_string(value) ); // maybe a memory leak wit json_string(value)???

    if ( json_dump_file( file_contents, file, JSON_INDENT(2)) < 0 )
        fprintf( stderr, "json_dump_file failto write %s.Maybe the directory does not exists or you don't have right access\n", file );

    json_object_clear(obj);
    json_object_clear(file_contents);

    return EXIT_SUCCESS;
}
