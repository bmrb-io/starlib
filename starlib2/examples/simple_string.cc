/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */

//////////////////////////// RCS COMMENTS: /////////////////////////////
//
//  This is a stub program for the sake of
//  testing the star library.  It performs
//  some get/set operations on a loop in the library:
//  Use this with input redirected from array_usage.str,
//  like so:
//      simple_string  < array_usage.str
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

//    PROTOTYPES FOR THE EXAMPLE FUNCTIONS:
//    -------------------------------------

// GLOBAL variable: the root of the AST built by a (successful) parse)
StarFileNode *AST;

// output stream:
ostream * os;

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
	List<ASTnode*>   *matches;
	List<DataNode*>  *global_list;
	DataLoopNode     *original_loop_ptr;
	GlobalBlockNode  *global_block_ptr;
	SaveFrameNode    *new_frame;

	// Find the loop that contains the tag "_outer_looptag_0":
	matches = AST->searchForTypeByTag(
		ASTnode::DATALOOPNODE, string("_outer_looptag_0") );
	
	// IF THIS WERE A REAL LIFE PROGRAM HERE YOU WOULD:
	//     error check if ( matches->size() == 0 )

	// Being sloppy here: just assuming there will only be one match:
	original_loop_ptr = (DataLoopNode*)( (*matches)[0] );
	delete matches;

	// Okay, now testing the new get/set methods on the loop:
	string val;

	// example 1: Get the 0,0 value and print it:
	val = original_loop_ptr->getVals().getString(0,0);
	cout << "In the outer loop, value 0,0 is: " << val <<endl;

	// example 2: Get the 2,3 value and print it:
	// (NOTE THAT there is no 2,3 value.  This will show what
	// happens when an index out of range is chosen.  A
	// null length string is returned.)
	val = original_loop_ptr->getVals().getString(2,3);
	cout << "In the outer loop, value 2,3 is: " << val <<endl;

	// example 3: Set the string at 4,2 to a new value and then
	// retrieve it to verify that it changed:
	original_loop_ptr->getVals().setString(4,2, string("99.99") );
	cout << "In the outer loop, value 4,0 has been changed to: "<<
	    original_loop_ptr->getVals().getString(4,2) << endl;

	// example 4: Print all the values for a named column, without
	// knowing ahead of time which index that column is in:
	int nest, col, idx, numRows;
	original_loop_ptr->tagPositionDeep( "_outer_looptag_2", &nest, &col );
	// (I'm ignoring the value for 'nest' and assuming the match is
	// in the outermost nesting level.)
	numRows = original_loop_ptr->getVals().size();
	cout << "Column values for"<<endl<<"_outer_looptag_2"<<endl;
	for( idx = 0 ; idx < numRows ; idx ++ )
	{   cout << original_loop_ptr->getVals().getString(idx,col) << endl;
	}

	// Now write the results out to stdout:
	// (This should be the same as the input, but with the one
	// value changed (4,2) into 99.99)
	cout << "---------------- WRITING THE FILE TO STDOUT: ---------" <<endl;
	os =  &cout;
	AST->Unparse(0);
    }
}
