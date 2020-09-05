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
//
// output stream:
ostream * os;


void replace_all_entryIDs(StarFileNode *root, string id );

int main( int argc, char **argv )
{
    // make the star input/output be stdin and stdout:
    // -----------------------------------------------
    os = &cout;  // output

    yyin = stdin; // input

    string entryIDToUse = "";

    if( argc > 1 )
    {   entryIDToUse = string(argv[1]);
    } else 
    {
        fprintf(stderr, "No entry ID given on command line, so using 'NEED_ACC_NUM'.\n" );
        entryIDToUse = string("NEED_ACC_NUM");
    }

    if (yyparse() == 0)   // returns nonzero on error
    {
        replace_all_entryIDs(AST, entryIDToUse );
	AST->Unparse(0);
    }
    else
        cerr << "# Error: Parsing problems in source file"<<endl;


    os->flush();

    delete AST;

    return 0;
}

void replace_all_entryIDs(StarFileNode *root, string id )
{
    fprintf( stderr, "file,line,tag,oldval,newval\n" );
    string name = "";
    size_t findPos = string::npos;
    List<ASTnode*> *allFreeItems  = root->searchForType( ASTnode::DATAITEMNODE );
    for( int i = 0 ; i < allFreeItems->size() ; ++i )
    {
        DataItemNode *din = (DataItemNode*) ( (*allFreeItems)[i] );
	name = din->myName();
	findPos = name.rfind( ".Entry_ID" ) ;
	if( (  findPos != string::npos &&
	       findPos == ( name.length() - 9 )   )  ||
	    name == string("_Entry.ID")   )
	{
	    if( din->myValue() != id ) {
	        fprintf( stderr, ",%d,%s,%s,%s\n",
		                 din->getLineNum(),
				 name.c_str(),
				 din->myValue().c_str(),
				 id.c_str()
				 );
		din->setValue( id );
		din->setDelimType( DataValueNode::NON );
	    }
	}
    }
    delete allFreeItems;

    ValCache::flushValCache();

    List<ASTnode*> *allLoops      = root->searchForType( ASTnode::DATALOOPNODE );
    for( int i = 0 ; i < allLoops->size() ; ++i )
    {
        DataLoopNode *dln = (DataLoopNode*) ( (*allLoops)[i] );
	LoopNameListNode *names = (*( dln->getNamesPtr() ) )[0];
	for( int j = 0 ; j < names->size() ; ++j )
	{
	    // If the string ends with ".Entry_ID" 
	    name = (*names)[j]->myName();
	    findPos = name.rfind( ".Entry_ID" ) ;
	    if( (  findPos != string::npos &&
	           findPos == ( name.length() - 9 )   )  ||
		name == string("_Entry.ID")   )
	    {
	        // It's the right kind of tag, so do the remap:
	        // remember that the current 'j' is the column
		// of the loop where the match was found.
		LoopTableNode *table = dln->getValsPtr();
		for( int row = 0 ; row < table->size() ; ++row )
		{
		    if( (*( (*table)[row] ))[j]->myValue() != id ) 
		    {
			fprintf( stderr, ",%d,%s,%s,%s\n",
			                 (*( (*table)[row] ))[j]->getLineNum(),
					 name.c_str(),
			                 (*( (*table)[row] ))[j]->myValue().c_str(),
					 id.c_str()
					 );
			(*( (*table)[row] ))[j]->setValue( id );
			(*( (*table)[row] ))[j]->setDelimType( DataValueNode::NON );
		    }

	            if( row % 50 == 0 )
		    {
		        ValCache::flushValCache();
		    }	
		}
	    }
	}
	ValCache::flushValCache();
    }

    delete allLoops;
}
