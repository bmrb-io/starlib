/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
//////////////////////////// RCS COMMENTS: /////////////////////////////
//
//  This is a stub program for the sake of
//  testing the star library.  It takes
//  standard input as a star file, and loads it.
//  Then it prompts the user to search for tags
//  in the file.
//
////////////////////////////////////////////////////////////////////////

#include "portability.h"
#include <unistd.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <errno.h>
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

    char fname[256];
    char searchForTag[256];
    char searchForValue[256];
    char tagOrTagValue[5];
    List<ASTnode*> *searchResults;
    int  i;

    // make the star input/output be stdin and stdout:
    // -----------------------------------------------
    os = &cout;

    // star_warning_check = false;  // turn off warnings: eraseme
    // star_syntax_print = false;  // turn off warnings: eraseme

    printf( "Please enter the filename for the STAR file: " );
    fgets( fname, 256, stdin );
    if( fname[strlen(fname)-1] == '\n' )
	fname[strlen(fname)-1] = '\0';
    yyin = fopen( fname, "r" );
    if( yyin == NULL )
    {
	fprintf( stderr, "filename %s: %s\n", fname, strerror(errno) );
	exit(1);
    }

    if (yyparse() == 0)   // returns nonzero on error
    {
	printf( "done parsing.\n" );
        strcpy( searchForTag, "zzzzzzz" );
	while( 1 )
	{
	    ValCache::debugDump();
	    printf( "choose: 't' = search by tag, 'tv' = by tag value, 'c' = cache clear : " );
	    fgets( tagOrTagValue, 5, stdin );
	    if( tagOrTagValue[strlen(tagOrTagValue)-1] == '\n' )
		tagOrTagValue[strlen(tagOrTagValue)-1] = '\0';
	    if( strcmp( tagOrTagValue, "tv" ) == 0 )
	    {
	        printf( "What tag or name to search for: " );
		fgets( searchForTag, 256, stdin );
		if( searchForTag[strlen(searchForTag)-1] == '\n' )
		    searchForTag[strlen(searchForTag)-1] = '\0';
		printf( "      What value to search for: " );
		fgets( searchForValue, 256, stdin );
		if( searchForValue[strlen(searchForValue)-1] == '\n' )
		    searchForValue[strlen(searchForValue)-1] = '\0';

		searchResults = AST->searchByTagValue(
			string(searchForTag), string(searchForValue) );
	    }
	    else if( strcmp( tagOrTagValue, "t" ) == 0 )
	    {
	        printf( "What tag or name to search for: " );
		fgets( searchForTag, 256, stdin );
		if( searchForTag[strlen(searchForTag)-1] == '\n' )
		    searchForTag[strlen(searchForTag)-1] = '\0';
		searchResults = AST->searchByTag( string(searchForTag) );
	    }
	    else if(  strcmp( tagOrTagValue, "c" ) == 0 )
	    {
		ValCache::flushValCache();
		continue;
	    }

	    if( searchResults->size() == 0 )
		printf( "No matches.\n" );

	    for( i = 0 ; i < searchResults->size() ; i++ )
	    {
		printf( "===Matched result %d:===\n", i+1 );
		(*searchResults)[i]->Unparse(2);
		os->flush();
	    }
	    printf( "\n" );

	    delete searchResults;
	}
    }
    else
        cerr << "# Error: Parsing problems in source file"<<endl;


    delete AST;  // yyparse() allocates a tree of data that must be
                 // destructed when finished.  The destructor for
		 // StarFileNode does a complete deep-destruction.

    os->flush(); // Must do this to close the output, else it never flushes
               // the last line of output.

    fclose( yyin );
    return 0;
}
