//////////////////////////// RCS COMMENTS: /////////////////////////////
/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
//
//  This is a stub program for the sake of
//  testing the star library.  It takes
//  standard input as a star file, and prints to
//  standard output the very same star file unparsed.
//  It can be used as a pretty-print formatter
//  filter (but it loses comments in the translation).
//
////////////////////////////////////////////////////////////////////////

#include "portability.h"
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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


    cerr << endl << "======= Cache before Parsing: ========" << endl;
    ValCache::debugDump();

    // make the star input/output be stdin and stdout:
    // -----------------------------------------------
    os = &cout;  // output

    yyin = stdin; // input


    if (yyparse() == 0)   // returns nonzero on error
    {
	cerr << endl << "\n======= Cache after Parsing: ========" << endl;
	ValCache::debugDump();

	// Just print it back out without doing anything else to it:
        if( argc > 1 && strcmp( argv[1], "--nooutput") == 0 )
	    cerr << "Done. - output turned off at user request"<< endl;
	else
	    AST->Unparse(0);
    }
    else
        cerr << "# Error: Parsing problems in source file"<<endl;


    cerr << endl << "\n======= Cache after Un-Parsing: ========" << endl;
    ValCache::debugDump();

    delete AST;  // yyparse() allocates a tree of data that must be
                 // destructed when finished.  The destructor for
		 // StarFileNode does a complete deep-destruction.

    os->flush(); // Must do this to close the output, else it never flushes
               // the last line of output.

    cerr << endl << "\n======= Cache after deleting: ========" << endl;
    ValCache::debugDump();

    return 0;
}
