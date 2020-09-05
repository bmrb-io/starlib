/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
//////////////////////////// RCS COMMENTS: /////////////////////////////
//
//  This is a stub program for the sake of
//  testing the star library.  It takes
//  standard input as a star file, and prints to
//  standard output the very same star file unparsed.
//  This program uses the NMRSTAR formatter to add proper comments
//  to the file.
//
////////////////////////////////////////////////////////////////////////

#include "portability.h"
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ast.h"
#include "parser.h"

#define COMMENT_FILE_NAME "/bmrb/lib/comment.str"

#define NO_ERR 0
#define SYN_ERR 1
#define COM_ERR 2
#define FILE_OPEN_ERR 3

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

    StarFileNode *inputStarTree;
    StarFileNode *commentStarTree;
    char         *commentFName;

    char dummy[11];

    // make the star input/output be stdin and stdout:
    // -----------------------------------------------
    os = &cout;  // output

    // star_warning_check = false;  // turn off warnings: eraseme
    // star_syntax_print = false;  // turn off warnings: eraseme

    if( argc >= 2 )
	commentFName = argv[1];
    else
	commentFName = COMMENT_FILE_NAME;

    yyin = stdin; // input

    if (yyparse() == 0)   // returns nonzero on error
    {
	inputStarTree = AST; // AST = star file parsed.


	// NOTE: I would normally close yyin here, but in this example,
	// yyin happens to be standard input, so I don't want to close
	// it.

	// Now do the formatting, by first parsing in the comment file.
	// The comment file is a global file that contains the
	// descriptions of what comments are supposed to go where.
	yyin = fopen( commentFName, "r" ); 

	// Error check for opening comment file:
	if( yyin == NULL )
	{
	    cerr << "# Error: Can't open file: "
		 << commentFName << 
		 " : Error was: " << strerror(errno) << endl;
	    return FILE_OPEN_ERR;
	}

	// Parse the comment file into a new StarFileNode, pointed
	// to by AST, check for parse errors and quit if any:
	if( yyparse() != 0 )
	{
	    cerr << "# Error: comment file parse problems in "
		 << commentFName <<  endl;
	    return COM_ERR;
	}
	commentStarTree = AST;
	fclose( yyin );


	// Now at this point inputStarTree is the input file, and
	// commentStarTree is the comment file, now we run the
	// formatter:
	formatNMRSTAR( inputStarTree, commentStarTree );

	// setColumnPadSpaces( 10 );

	// Now print the formatted version out:
	inputStarTree->Unparse(0);

	delete inputStarTree;
        delete commentStarTree;
	// Do not delete AST, since it is currently pointing
	// at the same place as commentStarTree, which is already
	// deleted.
    }
    else
    {
        cerr << "# Error: Parsing problems in source file"<<endl;
	return SYN_ERR;
    }


    os->flush(); // Must do this to close the output, else it never flushes
                 // the last line of output.

    return NO_ERR;
}
