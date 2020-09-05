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
#include <errno.h>

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

    if( argc > 1 ) {
      yyin = fopen( argv[1], "r" );
      if( ! yyin ) { 
         fprintf( stderr, "error opening %s: %s\n", argv[1], strerror(errno) );
      }
    } else {
      yyin = stdin; // input
    }


    if (yyparse() == 0)   // returns nonzero on error
    {

	List<ASTnode*> *sfs = AST->searchForTypeByTagValue(ASTnode::SAVEFRAMENODE, "_Saveframe_category", "NMR_applied_experiment" );
	List<ASTnode*> *hits = NULL;
	for( int s = 0; s < sfs->size(); ++s ) {
	  ASTnode *thisHit = (*sfs)[s];
	  string nameVal;
	  string detailsVal;
	  hits = thisHit->searchByTag("_Experiment_name");
	  if( hits->size() > 0 ) {
	    if( (*hits)[0]->isOfType( ASTnode::DATAITEMNODE) ) {
	      nameVal = ((DataItemNode*) ((*hits)[0]))->myValue();
	    } else {
	      nameVal = "(IS LOOPED)";
	    }
	    for( int j = 0; j < nameVal.length() ; ++j ) {
	      if( nameVal[j] == '\n' ) {
		nameVal[j] = ',';
	      }
	    }
	  }
	  delete hits;
	  hits = thisHit->searchByTag("_Details");
	  if( hits->size() > 0 ) {
	    if( (*hits)[0]->isOfType( ASTnode::DATAITEMNODE) ) {
	      detailsVal = ((DataItemNode*) ((*hits)[0]))->myValue();
	    } else {
	      detailsVal = "(IS LOOPED)";
	    }
	    for( int j = 0; j < detailsVal.length() ; ++j ) {
	      if( detailsVal[j] == '\n' ) {
		detailsVal[j] = ',';
	      }
	    }
	  }
	  delete hits;

	  printf( "_Experiment_name=%s;_Details=%s\n", nameVal.c_str(), detailsVal.c_str() );
	}
	delete sfs;
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
