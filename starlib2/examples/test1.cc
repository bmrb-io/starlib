/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */

//////////////////////////// RCS COMMENTS: /////////////////////////////
//
//  This is a stub program for the sake of
//  testing the star library.  It performs
//  some operations on the loop data in the
//  star file as if it was an array.  Use this
//  with input redirected from array_usage.str,
//  like so:
//       array_usage < array_usage.str
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

//    PROTOTYPES FOR THE EXAMPLE FUNCTIONS:
//    -------------------------------------
void make_inner_loop_gone( DataLoopNode *loop_ptr );

// GLOBAL variable: the root of the AST built by a (successful) parse)
StarFileNode *AST;

// output stream:
ostream * os;

// For printing a string when debugging.  If the C++ string lib
// was not compiled with debug on, then you must call this
// function from the debugger prompt to see the string's contents:
void gdb_str( string const &s )
{
    cout << "len: " << s.length() << " Contents: \"" << s << "\"" << endl;
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
	// Find the loop containing _outer_looptag_0:
	List<ASTnode*>   *matches;
	List<DataNode*>  *global_list;
	DataLoopNode     *original_loop_ptr;
	GlobalBlockNode  *global_block_ptr;
	SaveFrameNode    *new_frame;

	cerr << endl << "\n======= Cache after Parsing: ========" << endl;
	ValCache::debugDump();

	matches = AST->searchForTypeByTag(
		ASTnode::DATALOOPNODE, string("_outer_looptag_0") );
	
	cerr << endl << "\n======= Cache after searching for _outer_looptag_0: ========" << endl;
	ValCache::debugDump();
	// IF THIS WERE A REAL LIFE PROGRAM HERE YOU WOULD:
	//     error check if ( matches->size() == 0 )

	// Being sloppy here: just assuming there will only be one match:
	original_loop_ptr = (DataLoopNode*)( (*matches)[0] );
	delete matches;
	cerr << endl << "\n======= Cache after deleting matches 1 ========" << endl;
	ValCache::debugDump();
	
	// Now do several example operators on it that return new
	// saveframes:

	// Make a version that has the inner loop level removed entirely:
	make_inner_loop_gone( original_loop_ptr );
	cerr << endl << "\n======= Cache at " << __LINE__ <<" =======" << endl;
	ValCache::debugDump();
	
    }
    else
        cerr << "# Error: Parsing problems in source file"<<endl;


    delete AST;  // yyparse() allocates a tree of data that must be
                 // destructed when finished.  The destructor for
		 // StarFileNode does a complete deep-destruction.

    os->flush(); // Must do this to close the output, else it never flushes
               // the last line(s) of output.

    cerr << endl << "\n======= Cache at " << __LINE__ <<" =======" << endl;
    ValCache::debugDump();

    ValCache::flushValCache();

    cerr << endl << "\n======= Cache at " << __LINE__ <<" =======" << endl;
    ValCache::debugDump();
    return 0;
}

void make_inner_loop_gone( DataLoopNode *loop_ptr )
{
    
    // Then, we eliminate both tags from the inner loop.
    // (Notice that after eliminating the zeroth inner tag, that
    // what was once the one-th inner tag has become the zeroth tag.
    // That is why we delete the "zeroth tag" twice.)
    // -------------------------------------------------------------
    loop_ptr->getNames()[1]->erase(
	    &( ( *(loop_ptr->getNames()[1]) )[0] )  );
    loop_ptr->getNames()[1]->erase(
	    &( ( *(loop_ptr->getNames()[1]) )[0] )  );
    loop_ptr->getNames()[1]->erase(
	    &( ( *(loop_ptr->getNames()[1]) )[0] )  );

    // Now that both inner tags are gone, the library automatically
    // deletes all the inner loops from the loop node.  When we
    // print this out, all we get is the outermost loop:

}

