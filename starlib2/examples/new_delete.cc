/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
//////////////////////////// RCS COMMENTS: /////////////////////////////
//
//  This is a stub program for the sake of
//  testing the star library.  It takes
//  standard input as a star file, and repeatedly parses and deletes it.
//
//  Then it makes transative copies of it like so, deleting as it goes:
//       parse in copy A
//       B = copy of A
//       Delete A
//       C = copy of B
//       Delete B
//       D = copy of C
//       Delete C
//       ...etc...
//
//  After each step it pauses and waits for the user to hit return.
//
//  (The idea is that you can watch the memory size with 'ps' and
//  ensure that everything is being properly deleted without orphaning.
//
////////////////////////////////////////////////////////////////////////

#include "portability.h"
#include <unistd.h>
#include <stdio.h>
#include <fstream.h>
#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ast.h"
#include "parser.h"

// GLOBAL variable: the root of the AST built by a (successful) parse)
StarFileNode *AST;

// output stream:
ostream * os;

// For printing a string when debugging.  If the C++ string lib
// was not compiled with debug on, then you must call this
// function from the debugger prompt to see the string's contents:
void gdb_str( string const &s )
{
    cout << "len: " << s.length() << "Contents: \"" << s << "\"" << endl;
}

  ///////////////////
 //    main()     //
///////////////////
int main( int argc, char **argv )
{

    size_t numbytes;

    // First arg is the input filename:
    yyin = fopen( argv[1], "r" );
    if( yyin == NULL )
    {
	fprintf( stderr, "Error opening %s: %s\n", argv[1], strerror(errno) );
	exit(1);
    }
    // Make the second argument be the filename
    // -----------------------------------------------
    os = new ostream( argv[2] );  // output filename
    if( os == NULL )
    {
	fprintf( stderr, "Error opening %s: %s\n", argv[2], strerror(errno) );
	exit(1);
    }

    // star_warning_check = false;  // turn off warnings: eraseme
    // star_syntax_print = false;  // turn off warnings: eraseme

    int i;
    int N;
    char duh[10];

    N = 3;
    for( i = 1 ; i <= N ; i++ )
    {
	printf( "Parse test.  Trial #%d...\n", i );
	rewind( yyin );

	if (yyparse() == 0)   // returns nonzero on error
	{
	}
	else
	{
	    cerr << "# Error: Parsing problems in source file"<<endl;
	}
	numbytes = AST->sizeOfMe();
	printf("The size of the AST is %d bytes (~%d Kb, or ~%d Mb)\n",
		numbytes,  // bytes
		(numbytes+512)/1024,  // Kbytes
		( ( (numbytes+512)/1024)+512) / 1024  // Mbytes
		);
	printf("done.\n\tPress return to continue. ");
	fgets( duh, 10, stdin );
	printf( "deleting..." );
	delete AST;
	printf("done.\n\tPress return to continue. ");
	fgets( duh, 10, stdin );
    }
    printf( "Re-parsing file to prepare for subsequent tests.\n" );
    rewind( yyin );
    if (yyparse() == 0)   // returns nonzero on error
    {
	printf( "outputting to filename: %s...", argv[2] );
	printf( "\n\tHit return to start outputting: " );
	fgets( duh, 10, stdin );
	AST->Unparse(0);
	printf( "\tdone.  Hit return to continue..." );
	fgets( duh, 10, stdin );
    }
    else
    {
	cerr << "# Error: Parsing problems in source file"<<endl;
    }

    os->flush();
    printf( "Beginning the copy test.\n" );

    StarFileNode  *AST_arr[4];  // N+1 == 4, can't use variables in this.
    AST_arr[0] = AST;  // pointer copy, this is not a deep copy.

    for( i = 1 ; i <= N ; i++ )
    {
	printf("Copying AST_arr[%d] into AST_arr[%d]...", i-1, i );
	printf("Press Return to begin copying." );
	fgets( duh, 10, stdin );
	AST_arr[i] = new StarFileNode( *(AST_arr[i-1]) );
	numbytes = AST_arr[i]->sizeOfMe();
	printf("The size of the AST is %d bytes (~%d Kb, or ~%d Mb)\n",
		numbytes,  // bytes
		(numbytes+512)/1024,  // Kbytes
		( ( (numbytes+512)/1024)+512) / 1024  // Mbytes
		);
	printf("\tdone.\n" );
	printf("Deleting AST_arr[%d]...", i-1 );
	printf("Press Return to begin deletion." );
	fgets( duh, 10, stdin );
	delete AST_arr[i-1];
	printf("\tdone.\n" );
    }

    printf("Outputting last copy into %s...", argv[3] );
    printf("Press Return to begin." );
    fgets( duh, 10, stdin );
    os = new ostream( argv[3] );  // output filename 2
    AST_arr[N]->Unparse(0);
    os->flush(); // closes the file and flushes.
    printf("\tdone.\n");

    printf("Deleting last copy...Press Return to begin." );
    fgets( duh, 10, stdin );
    delete AST_arr[i-1]; 
    printf("\tdone.\n");
    printf("Press return to end the program." );
    fgets( duh, 10, stdin );
    fclose(yyin);
    return 0;
}
