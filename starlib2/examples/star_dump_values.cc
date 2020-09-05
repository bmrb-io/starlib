//////////////////////////// RCS COMMENTS: /////////////////////////////
/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
//
//  This is a stub program for the sake of
//  testing the star library.  It takes
//  standard input as a star file, and prints to
//  standard output just the DataValueNodes from it.
//  One value per line.
//  In order that the one-value-per-line rule can be adhered to,
//  any values with \n or \r in them will be printed with the \n
//  or \r rendered into backslashed form, i.e. this value:
//       ;This value
//       spans three
//       lines
//       ;
// gets printed as:
//       This value\nspans three\nlines\n
////////////////////////////////////////////////////////////////////////

#include "portability.h"
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <errno.h>
#include <map>
#include "ast.h"
#include "parser.h"

char nl = '\n';
char cr = '\r';
  
struct map_compare
{
  bool operator() (string s1, string s2) const
  {
    return s1 < s2;
  }
};
map<string,int,map_compare> ignore_tags;

struct map_compare_int
{
  bool operator() (int arg1, int arg2) const
  {
    return arg1 < arg2;
  }
};
map<int, int, map_compare_int> ignore_cols;

// GLOBAL variable: the root of the AST built by a (successful) parse)
StarFileNode *AST;

// output stream:
ostream * os;

void dumpValues( ASTnode *curNode, LoopNameListNode *names = NULL, int colNum = -1);
void populate_ignoreList( const char *fName);
string maybeTrimWhitespace( string inString );

bool g_with_tags = false;

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

    for( int arg_i = 1; arg_i < argc; ++arg_i ) {
      if( strcmp( argv[arg_i] , "--ignorelist") == 0 ) {
        populate_ignoreList( argv[arg_i+1] );
	++arg_i;
      }
      else if( strcmp( argv[arg_i] , "--withtags") == 0 ) {
        g_with_tags = true;
      }
    }

    // make the star input/output be stdin and stdout:
    // -----------------------------------------------
    os = &cout;  // output

    yyin = stdin; // input


    if (yyparse() == 0)   // returns nonzero on error
    {
	dumpValues( AST );
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


  //////////////////
 //  dumpValues  //
//////////////////
// This function is recursive.
// Given any ASTnode in the tree, it dumps all the values that are under
// it.  That might just mean recursively calling itself again on the next
// node down the tree if it's not a DataItemNode or DataValueNode.
void dumpValues( ASTnode *curNode, LoopNameListNode *names, int colNum)
{
  int idx;
  if( curNode == NULL ) {
    return;
  }

  int whichType = 0;
  if( (curNode->isOfType( ASTnode::DATAVALUENODE ) && (whichType = 1) /*SINGLE-EQUAL IS NOT A TYPO! I *MEANT* TO DO ASSIGNMENT*/  ) ||
      (curNode->isOfType( ASTnode::DATAITEMNODE ) && (whichType = 2) /*SINGLE-EQUAL IS NOT A TYPO! I *MEANT* TO DO ASSIGNMENT*/ )  ) {
    string val;
    string tag = "dummy";
    switch( whichType ) {
        case 1: val = maybeTrimWhitespace( ((DataValueNode*)curNode)->myValue() );
	        tag = (*names)[colNum]->myName();
	        break;
	case 2: val = maybeTrimWhitespace( ((DataItemNode*)curNode)->myValue() );
	        tag = ((DataItemNode*)curNode)->myName();
	        break;
    }

    // Ignore free tags on the ignore list:
    if( ignore_tags.find( tag.c_str() ) != ignore_tags.end() ) {
      return;
    }
    
    if( g_with_tags ) {
	cout << tag << ": ";
    }

    for( idx = 0 ; idx < val.size() ; ++idx ) {
      char ch = val[idx];
      if( ch == nl ) {
        cout<<"\\n";
      } else if( ch == cr ) {
        cout<<"\\r";
      } else 
        cout<<ch;
    }
    cout<<endl;
  }
  else if( curNode->isOfType( ASTnode::DATALOOPNODE ) ) {

    DataLoopNode *dln = (DataLoopNode*)curNode;

    // Remove the columns from memory that we want to skip:
    //
    // Iterating backward over the columns from right to left
    // because I'll be deleting from the list as I go, and
    // iterating backward is a simple way to avoid the 
    // every-other-column bug that often happens when
    // deleting from the thing you're iterating over as
    // you go:
    for( int i =  dln->getNames()[0]->size() -1 ; i >= 0  ; --i ) {
      string tag = (*(dln->getNames()[0]) )[i]->myName();
      if( ignore_tags.find( tag.c_str() ) != ignore_tags.end() ) {
        (*( dln->getNames()[0] )).erase( (*( dln->getNames()[0] ) ).begin() + i );
      }
    }

    dumpValues( ((DataLoopNode*)curNode)->getValsPtr(), 
                (*( ((DataLoopNode*)curNode)->getNamesPtr() ) ) [0] , -1  );
  }
  else if( curNode->isOfType( ASTnode::LOOPROWNODE ) ) {
      for( idx = 0 ; idx < ((LoopRowNode*)curNode)->size() ; ++idx ) {
          dumpValues( (*( ((LoopRowNode*)curNode) ))[idx], names, idx );
      }
      ValCache::flushValCache();
  }
  else if( curNode->isOfType( ASTnode::LOOPTABLENODE ) ) {
      for( idx = 0 ; idx < ((LoopTableNode*)curNode)->size() ; ++idx ) {
          dumpValues( (*( ((LoopTableNode*)curNode) ))[idx], names, -1 );
      }
  }
  else if( curNode->isOfType( ASTnode::SAVEFRAMENODE ) ) {
    ASTlist<DataNode*> *contents = ((SaveFrameNode*)curNode)->GiveMyDataList();
    if( contents == NULL ) {
      return;
    }
    for( idx = 0 ; idx < contents->size() ; idx++ ) {
       dumpValues( (*contents)[idx] );
    }
  }

  else if( curNode->isOfType( ASTnode::BLOCKNODE ) ) {
    ASTlist<DataNode*> *contents = ((BlockNode*)curNode)->GiveMyDataList();
    if( contents == NULL ) {
      return;
    }
    for( idx = 0 ; idx < contents->size() ; idx++ ) {
       dumpValues( (*contents)[idx] );
    }
  }

  else if( curNode->isOfType( ASTnode::STARFILENODE ) ) {
    ASTlist<BlockNode*> *contents = ((StarFileNode*)curNode)->GiveMyDataBlockList();
    if( contents == NULL ) {
      return;
    }
    for( idx = 0 ; idx < contents->size() ; idx++ ) {
       dumpValues( (*contents)[idx] );
    }
  }
}
void populate_ignoreList( const char *fName )
{
  char str[128];
  FILE *fp = fopen( fName, "r" );
  if( fp == NULL ) {
    fprintf( stderr, "Can't open %s for reading : %s\n", fName, strerror(errno) );
  } else {
    while (! feof( fp ) ) {
      fgets( str, 128, fp );
      // strip trailing whitespace and eoln's
      while( strlen(str) > 0 &&
              (
		 str[strlen(str)-1] == '\n' ||
		 str[strlen(str)-1] == '\r' ||
		 str[strlen(str)-1] == '\t' ||
		 str[strlen(str)-1] == ' '  
              )
	   ) {
        str[strlen(str)-1] = '\0';
        ignore_tags[str] = 1;
      }
    }
  }
  fclose(fp);
}

string maybeTrimWhitespace( string inString )
{
    // To implement - take all the whitespace runs and squash them
    // down to one space char first:
    string s = inString;;
    
    // Replace all types of whitespace with space:
    int i = 0;
    while( i != string::npos && i < s.size() ) {
	i = s.find_first_of( " \t\n\r", i );
	if( i == string::npos ) {
	   continue;
	}
	s[i] = ' ';
	++i;
	while( i < s.size() &&
	       (  s[i] == ' ' ||
		  s[i] == '\t' ||
		  s[i] == '\n' ||
		  s[i] == '\r'   )
	       )   {
	   s.erase( i, 1 );
       }
    }

    // Remove leading or trailing whitespace.
    // (because of the above work, we only have to
    // check one char - the leading or trailing white
    // space will have been collapsed to one char.)
    if ( s.size() > 0 && s[0] == ' ' ) {
        s.erase(0,1);
    }
    if ( s.size() > 0 && s[s.size()-1] == ' ' ) {
        s.erase(s.size()-1,1);
    }

    return s;
}
