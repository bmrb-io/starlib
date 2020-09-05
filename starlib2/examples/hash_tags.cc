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
#include <cstdio>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <errno.h>
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

struct tagCompStruct
{
    bool operator() (string s1, string s2 ) const
    {
        return s1 < s2;
    }
};

  ///////////////////
 //    main()     //
///////////////////
int main( int argc, char **argv )
{


    // make the star input/output be stdin and stdout:
    // -----------------------------------------------
    os = &cout;  // output


    map<string, int, tagCompStruct>  tagHash;
    map<string, int, tagCompStruct>::iterator  hashIter;


    for( int arg_i = 1 ; arg_i < argc ; arg_i++ )
    {
        yyin = fopen( argv[arg_i], "r" );
	if( yyin == NULL )
	{   cerr<< "# Error, failure opening "<<argv[arg_i]<<" for input: "<<strerror(errno)<<endl;
	    continue;
	}
	if (yyparse() == 0)   // returns nonzero on error
	{
	    List<ASTnode*> *allSaveFrames = AST->searchForType( ASTnode::SAVEFRAMENODE );
	    for( int sf_i = 0 ; sf_i < allSaveFrames->size() ; sf_i++ )
	    {
	        SaveFrameNode *sfn =  (SaveFrameNode*)( (*allSaveFrames)[sf_i] );

	        List<ASTnode*> *sfCatTagList = sfn->searchForTypeByTag( ASTnode::DATAITEMNODE, "_Saveframe_category" );
		if( sfCatTagList->size() != 1 )
		{   cerr << "# Error: In saveframe " << sfn->myName() <<
		            ", there are " << sfCatTagList->size() <<
			    " instances of _Saveframe_cateogry, but there should be exactly 1."<<endl;
		    continue;
		}
		string sfCat = ( (DataItemNode*)( (*sfCatTagList)[0] ) )->myValue();

		DataItemNode *sfCatItem = (DataItemNode*) ( (*sfCatTagList)[0] );
	        
	        List<ASTnode*> *allTagNames = sfn->searchForType( ASTnode::DATANAMENODE );
	        for( int tag_i = 0 ; tag_i < allTagNames->size() ; tag_i++ )
		{
		    DataNameNode *dnn = (DataNameNode*)( (*allTagNames)[tag_i] );
		    // We found the item, add it to the hash or increment it:
		    string hashKey = sfCat + "," + dnn->myName();
		    hashIter = tagHash.find( hashKey );
		    if( hashIter == tagHash.end() )
		    {   tagHash[ hashKey ] = 1;
		    }
		    else 
		    {   tagHash[ hashKey ]++;
		    }
		}
	        delete allTagNames;
	    }
	    delete allSaveFrames;
	}
	else
	    cerr << "# Error: Parsing problems in source file"<<endl;


	delete AST;  // yyparse() allocates a tree of data that must be
		     // destructed when finished.  The destructor for
		     // StarFileNode does a complete deep-destruction.

        fclose( yyin ); // will it allow this?
    }

    cout << "SF_CATEGORY,TAG_NAME,INSTANCES"<<endl;
    // Now print out the hash:
    for( hashIter = tagHash.begin() ; hashIter != tagHash.end() ; hashIter++ )
    {
        cout << (*hashIter).first << "," << (*hashIter).second << endl;
    }

    return 0;
}
