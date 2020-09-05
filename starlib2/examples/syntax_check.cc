/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
//////////////////////////// RCS COMMENTS: /////////////////////////////
//
//  This is a very simple system to demonstrate the new
//  syntax handling in starlib.
//
////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "ast.h"
#include "portability.h"
#include "parser.h"

// GLOBAL variable: the root of the AST built by a (successful) parse)
StarFileNode *AST;

// output stream, unused:
ostream * os = NULL;

  ///////////////////
 //    main()     //
///////////////////
int main( int argc, char **argv )
{
    char fname[1024];
    char syntax_choice[3];

    // Get a filename from the user, and open that file
    // as the input for yyparse():
    // ------------------------------------------------
    printf( "Name a file you want me to work with: " );
    fgets( fname, 1024, stdin );
    if( fname[strlen(fname)-1] == '\n' )
	fname[strlen(fname)-1] = '\0';  /* truncate the newline */
    yyin = fopen( fname, "r" );
    if( yyin == NULL )
    {
	fprintf( stderr, "Error opening %s: %s\n", fname,
		 strerror(errno) );
	exit(1);
    }

    // Find out what kind of error check the user wants
    // to have:
    // ------------------------------------------------
    printf( "Menu\n" );
    printf( "\t d = default syntax handler\n" );
    printf( "\t c = custom syntax handler\n" );
    printf( "choose d or c: " );
    fgets( syntax_choice, 3, stdin );

    if( tolower(syntax_choice[0]) == 'c' )
    {
	// Turn off the syntax printing in the starlib.
	// (star_syntax_print is a global in the starlib.)
	star_syntax_print = false;
    }

    // Call yyparse() to read the star file:
    // -------------------------------------
    if (yyparse() == 0)   // returns nonzero on error
    {
	cout << "No syntax errors detected in " << fname
	     << "." << endl;
    }
    else
    {
	// If the error printing was done by starlib, then
	// there's no more work to do.  If we are doing custom
	// syntax error handling, then handle it here:
	// ---------------------------------------------------
	if( tolower(syntax_choice[0]) == 'c' )
	{
	    // Note, starErrReport is a global STL vector of error
	    // structs that was populated by yyparse().  The
	    // declaration is:
	    //    vector<StarErrEntry>  starErrReport;
	    // The type "StarErrEntry" is a struct.  It is
	    // described in "scanner.h" in the starlib.
	    // - - - - - - - - - - - - - - - - - - - - - - -
	    printf( "A total of %d errors were found.\n",
		    starErrReport.size() );
	    printf( "num    line col token         \n"
		    "--- ------- --- --------------\n" );
	    for( int idx = 0 ; idx < starErrReport.size() ; idx++ )
	    {
		
		printf( "%3d %6d %3d %s\n\t\t(%d)%s\n",
			idx+1,
			starErrReport[idx].lineNum,
			starErrReport[idx].colNum,
			starErrReport[idx].tokenStr,
		        starErrReport[idx].errID,
			starErrReport[idx].errStr );
	    }
	}
    }


    delete AST;  // yyparse() allocates a tree of data that must be
                 // destructed when finished.  The destructor for
		 // StarFileNode does a complete deep-destruction.

    return 0;
}
