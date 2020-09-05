//////////////////////////// RCS COMMENTS: /////////////////////////////
/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
//
//  This is a stub program for the sake of
//  testing the star library.  It takes
//  standard input as a star file, and prints out
//  any syntax errors the parser finds.
//
////////////////////////////////////////////////////////////////////////

#include "portability.h"
#include <unistd.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <errno.h>
#include "ast.h"
#include "parser.h"

// GLOBAL variable: the root of the AST built by a (successful) parse)
StarFileNode *AST;

// output stream:
ostream * os;

// usage message:
static char usage[] =
    "Usage: syntax_check starfilename\n";

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

    // Check to print usage 'help' string:
    // -----------------------------------
    if(   argc < 2 ||
	  strcmp( argv[1], "?" ) == 0 ||
	  strcmp( argv[1], "-?" ) == 0 ||
	  strcmp( argv[1], "-help" ) == 0 ||
	  strcmp( argv[1], "-h" ) == 0 )
    {
	fprintf( stderr, "%s", usage );
	exit(2);
    }

    // make the star input/output be stdin and stdout:
    // -----------------------------------------------
    os = &cout;  // output

    // Take input from filenames:
    for( int arg = 1 ; arg < argc ; arg++ )
    {
	char *fname = argv[arg];
	yyin = fopen( fname, "r" );
	if( yyin == NULL )
	{
	    fprintf( stderr, "Error trying to open %s: %s\n",
		     fname, strerror( errno ) );
	    continue;
	}
	if (yyparse() == 0)   // returns nonzero on error
	{
	    if( starErrReport.size() == 0 )
	        fprintf( stderr, "=== %s has valid syntax.\n", fname );
	    else
	        fprintf( stderr,
			"=== %s has warnings, but is technically valid syntax.\n",
			fname );
	}
	else
	    fprintf( stderr, "=== %s has invalid syntax.\n", fname );


	delete AST;  // yyparse() allocates a tree of data that must be
		     // destructed when finished.  The destructor for
		     // StarFileNode does a complete deep-destruction.

    }
    os->flush(); // Must do this to close the output, else it never flushes
               // the last line of output.

    return 0;
}
