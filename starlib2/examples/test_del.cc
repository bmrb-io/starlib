/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
//////////////////////////// RCS COMMENTS: /////////////////////////////
//
//  This is a stub program for the sake of
//  testing the star library.  It takes
//  standard input as a star file, and tests the deletion
//  of a loop.
//
////////////////////////////////////////////////////////////////////////

#include "portability.h"
#include <unistd.h>
#include <stdio.h>
#include <fstream.h>
#include <iostream.h>
#include <stdlib.h>
#include <string.h>
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

    // make the star input/output be stdin and stdout:
    // -----------------------------------------------
    os = &cout;  // output
    yyin = stdin; // input

    if (yyparse() == 0)   // returns nonzero on error
    {
	// Remove any loops that have a tag called '_eraseme'
	List<ASTnode*> *matches = AST->searchForTypeByTag(
		ASTnode::DATALOOPNODE, string("_eraseme") );
	if( matches->size() == 0 )
	{
	    cerr << "# No loops containing '_eraseme'.  do nothing." << endl;
	}
	else
	{
	    for( int i = 0 ; i < matches->size() ; i++ )
		delete (*matches)[i];
	    delete matches;
	}
	AST->Unparse(0);
    }
    else
        cerr << "# Error: Parsing problems in source file"<<endl;


    delete AST;  // yyparse() allocates a tree of data that must be
                 // destructed when finished.  The destructor for
		 // StarFileNode does a complete deep-destruction.

    os->flush(); // Must do this to close the output, else it never flushes
               // the last line of output.

    return 0;
}
