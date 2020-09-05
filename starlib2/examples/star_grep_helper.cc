//////////////////////////// RCS COMMENTS: /////////////////////////////
/* This software is copyright (c) 2005 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
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

// output stream:
ostream * os;

// print line numbers or not:
bool showLineNum = false;

// For printing a string when debugging.  If the C++ string lib
// was not compiled with debug on, then you must call this
// function from the debugger prompt to see the string's contents:
void gdb_str( string const &s )
{
    cout << "len: " << s.length() << "Contents: \"" << s << "\"" << endl;
}

List<string>  *getValuesFor( ASTnode *searchRoot, const char *tagname, bool withNums );
void printWithEscapes( const char *val );

int mainErr = 0;
#define ERR_NOTFOUND 1
#define ERR_SYNTAX 2

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
        int argnum;
	// Just print it back out without doing anything else to it:
	for( argnum = 1 ; argnum < argc ; argnum++ )
	{
	    if( strcmp( argv[argnum], "--line" ) == 0  )
	    {
	       showLineNum = true;
	    }
	    else
	    {
		List<string> *vals = getValuesFor( AST, argv[argnum], showLineNum );
		if( vals->size() == 0 )
		{
		    mainErr = ERR_NOTFOUND;
		}
		else
		{
		    int i;
		    for( i = 0 ; i < vals->size() ; i++ )
		    {
			printWithEscapes( argv[argnum] );
			printf( ": " );
			printWithEscapes( (*vals)[i].c_str() );
			printf( "\n");
		    }
		    delete vals;
		}
	    }
	}
    } else 
    {
       mainErr = ERR_SYNTAX;
    }

    delete AST;  // yyparse() allocates a tree of data that must be
                 // destructed when finished.  The destructor for
		 // StarFileNode does a complete deep-destruction.

    os->flush(); 
    return mainErr;
}

// for each char in the string, print it as-is if it is
// not a control-char, or escape it with backslash if it is:
void printWithEscapes( const char *val )
{
    int i;
    char c;

    for( i = 0 ; val[i] != '\0'; i++ )
    {
        c = val[i];
        if( c == '\n' )
            printf("\\n");
        else if( c == '\r' )
            printf("\\r");
	else if( c == '\b' )
            printf("\\b");
	else if( c == '\t' )
            printf("\\t");
	else if( c == '\f' )
            printf("\\f");
	else if( c  <= 30 ) // for other control chars, do octal escape
            printf("\\%o", c);
	else  // default - just print it as is.
            printf("%c", c);
    }
}

// Return the values for a tag name, as a List of DataValueNodes.
// If the tag is free, the list will only have one thing in it.)
List<string>  *getValuesFor( ASTnode *searchRoot, const char *tagname, bool withNum )
{
    List<string> *retVal = new List<string>;
    int srch_i;
    List<ASTnode*>  *searchResults;
    string      hitVal;

    const char *actualTagName = tagname;
    // If the context is limited with a semicolon part then limit it to that first.
    char *buf = strdup( tagname );
    char *semiPosition = strchr(buf, ';');
    List<ASTnode*> *searchContext;
    if( semiPosition != NULL ) {
        actualTagName = semiPosition + 1;
        char *equalPos = strchr( buf, '=' );
	if( equalPos != NULL ) {
	    *equalPos = '\0';
	    *semiPosition = '\0';
	    searchContext = searchRoot->searchForTypeByTagValue( ASTnode::SAVEFRAMENODE, buf, equalPos+1 );
	} else {
	    searchContext = new List<ASTnode*>;
  	    searchContext->insert( searchContext->end(), searchRoot );
	}
    } else {
        searchContext = new List<ASTnode*>;
        searchContext->insert( searchContext->end(), searchRoot );
    }


    for( int context_i = 0 ; context_i < searchContext->size(); ++context_i ) {
	searchResults = (*searchContext)[context_i]->searchByTag( actualTagName );

	for( srch_i = 0 ; srch_i < searchResults->size() ; srch_i++ )
	{
	    if( (*searchResults)[srch_i]->isOfType( ASTnode::DATAITEMNODE ) )
	    {
		DataItemNode *hitItem = (DataItemNode*) (*searchResults)[srch_i];
		if( withNum )
		{   char num[12];
		    sprintf( num, "%d", hitItem->getLineNum() );
		    hitVal = string(" line ") + string(num) + ": ";
		} else
		{   hitVal = "";
		}
		hitVal += hitItem->myValue();
		retVal->insert( retVal->end(), hitVal );
	    }
	    else 
	    {
		DataNameNode *hitName = (DataNameNode*) (*searchResults)[srch_i];
		ASTnode *par;
		for( par = hitName ; par != NULL && !  par->isOfType(ASTnode::DATALOOPNODE) ; par = par->myParent() )
		{  }
		if( par == NULL )
		   return retVal;
		DataLoopNode *dln = (DataLoopNode*)par;
		int i;
		int col, nest;
		dln->tagPositionDeep( actualTagName, &nest, &col );
		LoopTableNode *ltn = dln->getValsPtr();
		for( i = 0 ; i < ltn->size() ; i++ ) 
		{
		    if( withNum )
		    {   char num[12];
			sprintf( num, "%d", (*((*ltn)[i]))[col]->getLineNum() );
			hitVal = string(" line ") + string(num) + ": ";
		    } else
		    {   hitVal = "";
		    }
		    hitVal += (*((*ltn)[i]))[col]->myValue();
		    retVal->insert( retVal->end(), hitVal );
		}
	    }
	}
    }
    delete searchResults;
    delete searchContext;
    return retVal;
}
