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
#include <fstream.h>
#include <iostream.h>
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


    // make the star input/output be stdin and stdout:
    // -----------------------------------------------
    os = &cout;  // output

    yyin = stdin; // input

    vector<string> cases;
    vector<string> names;


    if (yyparse() != 0)   // returns nonzero on error
    {
        cerr << "# Error: Parsing problems in source file"<<endl;
	exit(1);
    }


    List<ASTnode*> *sfList = AST->searchForTypeByTagValue( ASTnode::SAVEFRAMENODE, "_Saveframe_category", "NMR_applied_experiment");
    for( int sfNum = 0 ; sfNum < sfList->size() ; sfNum++ ) {
        SaveFrameNode *thisSf  = (SaveFrameNode*) ((*sfList)[sfNum]);

	List<ASTnode*> *match;
	match = thisSf->searchForTypeByTag( ASTnode::DATAITEMNODE, "_Details" );
	DataItemNode *detTag = NULL;
	if( match->size() > 0 ) {
	    detTag = (DataItemNode*) ((*match)[0]);
        }
	match = thisSf->searchForTypeByTag( ASTnode::DATAITEMNODE, "_Experiment_name" );
	DataItemNode *nameTag = NULL;
	if( match->size() > 0 ) {
	    nameTag = (DataItemNode*) ((*match)[0]);
	}

	bool nameExist = false;
        bool detExist = false;

	if( nameTag != NULL && 
	       ( nameTag->myValue() != string("") &&
	         nameTag->myValue() != string(".") &&
	         nameTag->myValue() != string("?") ) ) {
	    nameExist = true;
	   // debug  nameTag->Unparse(0);
	}
	if( detTag != NULL && 
	       ( detTag->myValue() != string("") &&
	         detTag->myValue() != string(".") &&
	         detTag->myValue() != string("?") ) ) {
	    detExist = true;
	   // debug  detTag->Unparse(0);
	}

	string caseStr = "";
	if( nameExist ) {
	  caseStr += "N";
	} else {
	  caseStr += "";
	}
	if( detExist ) {
	  caseStr += "D";
	} else {
	  caseStr += "";
	}
        cases.insert( cases.end(), caseStr );
	names.insert( names.end(), thisSf->myName() );
    }

    bool homo_cases = true;
    for( int i = 1 ; i < cases.size() ; i++ ) {
        if( cases[i] != cases[i-1] ) {
	    homo_cases = false;
	}
    }

    fprintf( stdout, "%s: ", (homo_cases ? "homo" : "hetero" ) );
    for( int i = 0 ; i < cases.size() ; i++ ) {
      fprintf( stdout, "(%s:%s) ", names[i].c_str(), cases[i].c_str() );
    }
    fprintf( stdout, "\n" );

    delete sfList;


    delete AST;  // yyparse() allocates a tree of data that must be
                 // destructed when finished.  The destructor for
		 // StarFileNode does a complete deep-destruction.

    os->flush(); // Must do this to close the output, else it never flushes
               // the last line of output.

    return 0;
}
