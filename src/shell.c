/*
 * This code has been made to understand how to make a basic shell.
 * To see the tutorial, you can visit the following website: 
 * https://brennan.io/2015/01/16/write-a-shell-in-c/
*/

/* Imports from different libraries */
#include <stdio.h>
#include <stdlib.h>

/* Declaration of constants for the programme */
#define SHELL_LINE_BUFFSIZE 1024

char *shell_read_line()
{
    int buffsize = SHELL_LINE_BUFFSIZE;
    int position = 0;
    int c;
    char *buffer = malloc( sizeof( char ) * buffsize );

    if( !buffer )
    {
        fprintf( stderr, "shell: allocation error\n" );
        exit( EXIT_FAILURE );
    }

    while( 1 )
    {
        /* Read a character */
        c = getchar();
        
        /* If we hit EOF, replace it with a null character and return. */
        if( c == EOF || c == '\n' )
        {
            buffer[position] = '\0';
            return buffer;
        }else{
            buffer[position] = c;
        }

        position++;

        /* If we have exceeded the buffer, reallocate. */
        if( position >= buffsize )
        {
            buffsize += SHELL_LINE_BUFFSIZE;
            buffer = realloc( buffer, buffsize );

            if( !buffer )
            {
                fprintf( stderr, "shell: allocation error\n" );
                exit( EXIT_FAILURE );
            }
        }
    }
}


void main_loop()
{
    char *line;
    char **args;
    int status;

    do
    {
        printf( "> " );
        line = shell_read_line();
        args = shell_split_line( line );
        status = shell_execute( args );

        free( line );
        free( args );
    } while( status );
}


int main( int argc, char **argv )
{

    main_loop();

    return EXIT_SUCCESS;
}


