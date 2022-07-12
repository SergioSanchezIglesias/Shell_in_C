/*
 * This code has been made to understand how to make a basic shell.
 * To see the tutorial, you can visit the following website: 
 * https://brennan.io/2015/01/16/write-a-shell-in-c/
*/

/* Imports from different libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Declaration of constants for the programme */
#define SHELL_LINE_BUFFSIZE 1024
#define SHELL_TOK_BUFFSIZE 64
#define SHELL_TOK_CHAR " \t\r\n\a"


int shell_launch( char **args )
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if( pid == 0 )
    {
        /* Child Process */
        if( execvp( args[0], args ) == -1 )
        {
            perror( "shell" );
        }
        exit( EXIT_FAILURE );
    } else if( pid < 0 ){
        /* Error Forking */
        perror( "shell" );
    } else{
        /* Parent Process */
        do
        {
            wpid = waitpid( pid, &status, WUNTRACED );
        } while( !WIFEXITED( status ) && !WIFSIGNALED( status ) );
    }

    return 1;
}


char **shell_split_line( char *line )
{
    int buffsize = SHELL_TOK_BUFFSIZE;
    int position = 0;
    char **tokens = malloc( buffsize * sizeof( char* ) );
    char *token;


    if( !tokens )
    {
        fprintf( stderr, "shell: allocation error\n" );
        exit( EXIT_FAILURE );
    }

    token = strtok( line, SHELL_TOK_CHAR );
    while( token != NULL )
    {
        tokens[position] = token;
        position++;
    

        if( position >= buffsize )
        {
            buffsize += SHELL_TOK_BUFFSIZE;
            tokens = realloc( tokens, buffsize * sizeof( char* ) );
        
            if( !tokens )
            {
                fprintf( stderr, "shell, allocation error\n" );
                exit(EXIT_FAILURE);
            }
        }
    token = strtok( NULL, SHELL_TOK_CHAR );
    }
    tokens[position] = NULL;
    return tokens;
}

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


