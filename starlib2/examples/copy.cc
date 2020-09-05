
/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
//////////////////////////// RCS COMMENTS: /////////////////////////////
//
//  This is a stub program for the sake of
//  testing the star library.
//
//  It simply creates a copy of the data and prints it.
//
// $Log: copy.cc,v $
// Revision 1.3  2001/12/04 20:59:09  madings
// Major changes  - see doc/changes.html for details:
//    - setstring / getstring interface at the  loop level
//    - implement #<START-SKIP> ... #<END-SKIP> directives
//    - fix compiler errors related to use of iterators and const
//         on newer versions of gcc.
//    - ValCache usage, with LoopRows containing one string for the
//         whole row instead of seperate data value nodes.
//    - change the type of the *os file stream because the ability to
//         construct from a unix fileno() handle has been depreciated in
// 	newer C++ stream libraries.
//
// Revision 1.2  2001/11/13 20:30:43  madings
// This update includes several changes:
// (See doc/changes.html for the full explanation)
//    1- The previous compilers we had been using had allowed us to get
//        away with some mis-use of the const specifier without flagging
//        it as a warning or error.  That is no longer the case, so we had
//        to alter some of the code's use of "const".
//    2- The STL no longer allows vector iterators to be interchangable
//        with pointers, so we lost the ability to use that feature for
//        users of the library.
//    3- The streams library no longer allows you to make an ofstream out of
//        a pipe or standard output.  Callers of this method must now make
//        'os' be a pointer to an ostream instead of ofstream.
//    4- The newer compilers no longer allow a reference to a temporary store
//        that was created just for the duration of the function call.
//
// Revision 1.1.1.1  2001/04/17  10:01:07  madings
//
// Revision 2.0  1999/01/23  03:35:34  madings
// Since this new version compiles on
// multiple platforms(well, okay, 2 of them), I
// thought it warranted to call it version 2.0
//
// Revision 1.2  1999/01/22  23:58:42  madings
// Golly, where should I begin?  Okay, There was a port to linux being
// done in this version, which in turn meant a port to egcs instead of
// making code that can only compile on a CFront style compiler like SGI
// uses.  This port meant a lot of tiny changes all over the place to
// accomodate the differences in the way templates get linked.  Also,
// a bug was discovered in the way rows were deleted in the LoopTableNode's
// erase() operator.  The bug was there all along and it surfaced as part
// of the portation (what was just bad data in SGI was causing coredumps
// in Linux.)  Hopefully now that it compiles under egcs, it will be much
// more portable and I can begin getting it out to the public.
//
// Revision 1.1  1998/09/04  20:57:27  madings
// Added the examples.
//
//
////////////////////////////////////////////////////////////////////////

#include "portability.h"
#include <unistd.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
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

    StarFileNode   *newCopy;
    // make the star input/output be stdin and stdout:
    // -----------------------------------------------
    os = &cout;  // output
    yyin = stdin; // input

    if (yyparse() == 0)   // returns nonzero on error
    {
	// Make a copy of it:
	newCopy = new StarFileNode( true, *AST );

	(*os) << "============================" << endl 
	      << "============================" << endl
	      << "===  Original Version ======" << endl
	      << "============================" << endl
	      << "============================" << endl;

	AST->Unparse(0);

	(*os) << "============================" << endl 
	      << "============================" << endl
	      << "===  Copy Of Original ======" << endl
	      << "== (should be identical) ===" << endl
	      << "============================" << endl;
	newCopy->Unparse(0);

	delete AST;
	delete newCopy;
    }
    else
        cerr << "# Error: Parsing problems in source file"<<endl;


    os->flush(); // Must do this to close the output, else it never flushes
               // the last line of output.

    return 0;
}
