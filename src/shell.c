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

/*
  Function Declarations for builtin shell commands:
 */

 int shell_cd( char **args );
 int shell_help( char **args );
 int shell_exit( char **args );

/*
  List of builtin commands, followed by their corresponding functions.
 */

 char *builtin_str[] = {
    "cd",
    "help",
    "exit"
 };

 int ( *builtin_func[] ) ( char ** ) = {
    &shell_cd,
    &shell_help,
    &shell_exit
 };

 int shell_num_builtin()
 {
    return sizeof( builtin_str ) / sizeof( char * );
 }


 /*
  Builtin function implementations.
*/

int shell_cd( char **args )
{
    if( args[1] == NULL ){
        fprintf( stderr, "shell: expected argument to \"cd\"\n");
    }else {
        if( chdir( args[1] ) != 0 )
        {
            perror( "shell" );
        }
    }
    return 1;
}

int shell_help( char **args )
{
    int i;
    printf( "Sergio's Shell\n" );
    printf( "Type program names and arguments, and hit enter.\n" );
    printf("The following are built in:\n");

    for( i = 0; i < shell_num_builtin(); i++)
    {
        printf( " %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int shell_exit( char **args )
{
    return 0;
}

int shell_launch( char **args )
{
    pid_t pid;
    pid_t wpid;
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

int shell_execute( char **args )
{
    int i;

    if( args[0] == NULL )
    {
        /* An empty command was entered. */
        return 1;
    }

    for( i = 0; i < shell_num_builtin(); i++ )
    {
        if( strcmp( args[0], builtin_str[i] ) == 0 )
        {
            return ( *builtin_func[i] ) ( args );
        }
    }
    return shell_launch( args );
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


