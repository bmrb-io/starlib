/* This software is copyright (c) 2001 Board of Regents, University of
 * Wisconsin. All Rights Reserved.
 */
//////////////////////////// RCS COMMENTS: /////////////////////////////
//
//  This is a stub program for the sake of
//  testing the star library.  It takes
//  standard input as a star file, and prints to
//  standard output the very same star file unparsed.
//  This program uses the NMRSTAR formatter to add proper comments
//  to the file.
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
#include "ast.h"
#include "parser.h"

#define NO_ERR 0
#define SYN_ERR 1
#define COM_ERR 2
#define FILE_OPEN_ERR 3
#define DIF_ERR 4

// GLOBAL variable: the root of the AST built by a (successful) parse)
StarFileNode *AST;

char g_usage[] = 
   "USAGE:  %s [-blocks] [-ignore-padding] [-null nullVal] Left_FileName  Right_FileName\n"
   "   -blocks = Just compare the first block (data_ or global_) in file1 to\n"
   "     the first block in file2, even though their names might not match.\n"
   "     This is useful when comparing CIF and CIF-like files.\n"
   "   -ignore-padding = ignore differences in values that differ only by trailing\n"
   "     or leading whitespace (including blank lines).\n"
   "   -null ARG_HERE\n"
   "      gives a values to treat as null.\n"
   "      the default is \".\".\n"
   "      As soon as you define a -null, the default of '.' is\n"
   "        unset, so if you still want it you must explicitly\n"
   "        name it also with an additional -null '.' option.\n"
   "      You may give more than one -null arg to have more than one\n"
   "      value treated as null, for example:\n"
   "         stardiff -null '.' -null '?' file1 file2\n"
   "   -equal 'VAL1=VAL2'\n"
   "       Tells the program that when the value to a tag is VAL1 in one \n"
   "       file and VAL2 in the other file, that they should be processed\n"
   "       as if they were equivilent and not a 'real' difference.\n"
   "   -boilerplate-tag ARG_HERE\n"
   "      Tells the program that this tag is a boilerplate insertion and\n"
   "      thus a saveframe consisting of nothing but this tag\n"
   "      should be considered a match with an empty saveframe or even\n"
   "      a missing saveframe.  This is typically used for the saveframe\n"
   "      category tag.  (An empty or nonexistant saveframe is the same\n"
   "      thing as a saveframe consisting of only the saveframe category tag.)\n"
   "      As with the -null option, this option can be specified more than\n"
   "      once to indicate multiple such tags.)\n"
   "   -ignore-tag ARG_HERE\n"
   "      Ignore this tag entirely.  Act as if it's not even there.\n"
   "      You may give more than one such arge to ignore multiple tags.\n"
   "   -debug\n"
   "      Turn on some diagnosit output.\n"
   "\n"
   "   Both LeftFileName and RightFileName are star files.\n"
   "   This program only knows how to compare two star files that\n"
   "      have the following characteristics:\n"
   "      - They must put all their tags inside saveframes.  Tags that\n"
   "         are outside saveframes are not part of the comparasin.\n"
   "      - They must not have more than one level of loop nesting.\n"
   "   Either leftFileName or RightFileName can be a single dash '-', which\n"
   "      means use the standard input stream from a pipe or the keyboard.\n"
   "\n"
   "   The comparasin has the following features:\n"
   "      - Comments (#) and all whitespace separators are ignored.\n"
   "      - Order of tags or saveframes is ignored.\n"
   "      - Quote style is ignored: 'foo' == \"foo\"\n"
   "      - If one file has a tag that is a null value ('.'), and the other\n"
   "          file is missing that tag entirely, that is considered a match.\n"
   "          No diff report line is generated for this case.\n"
   "      - The case where a loop tag has only null values ('.'), is also\n"
   "          equivilent to a missing tag, just like the rule above.\n"
   "      - All that matters is the count of values for a tag, not whether that\n"
   "          tag is looped or free.  In other words, a loop with only one row\n"
   "          can end up being a match for a free tag if their values are the\n"
   "          same.\n"
   "   The output lines are formmatted as follows:\n"
   "      In the leftmost column:\n"
   "        \"--\" means \"There is a difference found.\"\n"
   "        \"**\" means \"Something invalid in one of the files here made it.\n"
   "                 impossible to perform the check correctly for this tag.\n"
   "      The next character determines the type of diff or error:\n"
   "        \"<\" means \"exists in the left file.\"\n"
   "        \">\" means \"exists in the right file.\"\n"
   "        \"~\" means \"both the files have this same tag, but the values differ.\"\n"
   "      The next characters often describe the line number at or near the tag\n"
   "        in the format: filename:linenumber\n"
   "      The remainder of the message is the text description of the error.\n"
   "\n"
   "   Errors about opening the files, or the syntax of the files appear on stderr.\n"
   "   But the difference report appears on stdout.\n"
   ;


bool g_optCommentErr = false;
bool g_ignorePadding = false;
bool g_justCompareFirstBlocks = false;
bool g_debug = false;
string g_fileNameOne;
string g_fileNameTwo;

struct my_strcmp
{
  bool operator()(const string &s1, const string &s2) const
  {
    return (s1 < s2);
  }
};
vector<string> g_nulls;
vector<string> g_boilerplates;
vector<string> g_ignores;
map<string,string,  my_strcmp> g_equivs;

// output stream:
ostream * os;

int diff_one_sf_or_block( ASTnode *node_A, const char *fName_A, ASTnode *node_B, const char *fName_B, bool leftToRight );
int do_the_diff( StarFileNode *A, const char *A_name, StarFileNode *B, const char *B_name, bool leftToRight );
vector<string> *valuesForName( DataNameNode *nameNode );
SaveFrameNode *get_same_sf_cat_hit( SaveFrameNode *thisSf_A, StarFileNode *B );

// For printing a string when debugging.  If the C++ string lib
// was not compiled with debug on, then you must call this
// function from the debugger prompt to see the string's contents:
void gdb_str( string const &s )
{
    cout << "len: " << s.length() << "Contents: \"" << s << "\"" << endl;
}
string strip_eoln( string in );

SaveFrameNode *g_emptySF;
void setup_emptySF( void );

  ///////////////////
 //    main()     //
///////////////////
int main( int argc, char **argv )
{

    FILE *fileOne = NULL;
    FILE *fileTwo = NULL;
    StarFileNode *starTreeOne;
    StarFileNode *starTreeTwo;
    char         *commentFName;

    g_emptySF = new SaveFrameNode( "save___empty_dummy____" );

    char dummy[11];

    // make the star input/output be stdin and stdout:
    // -----------------------------------------------
    os = &cout;  // output

    if( argc < 3 ) {
      fprintf(stderr,g_usage, argv[0] );
      return COM_ERR;
    }

    for( int arg_i = 1 ; arg_i < argc; arg_i++ ) {
      if( strcmp( argv[arg_i], "-" ) == 0 ) {
        if( fileOne == NULL ) {
	  fileOne = stdin;
	  g_fileNameOne = string("<standard input>");
	}
	else {
	  fileTwo = stdin;
	  g_fileNameTwo = string("<standard input>");
	}
      } else if( strcmp( argv[arg_i], "-blocks" ) == 0 ) {
        g_justCompareFirstBlocks = true;
      } else if( strcmp( argv[arg_i], "-debug" ) == 0 ) {
        g_debug = true;
      } else if( strcmp( argv[arg_i], "-ignore-padding" ) == 0 ) {
        g_ignorePadding = true;
      } else if( strcmp( argv[arg_i], "-c" ) == 0 ) {
        g_optCommentErr = true;
      } else if( strcmp( argv[arg_i], "-null" ) == 0 ) {
        arg_i++;
	if( arg_i < argc ) {
	  g_nulls.insert( g_nulls.end(), argv[arg_i] );
	}
      } else if( strcmp( argv[arg_i], "-equal" ) == 0 ) {
        arg_i++;
	if( arg_i < argc ) {
	  char *eqpos = strchr( argv[arg_i], '=' );
	  if( eqpos != NULL ) {
	    string s1 = string(argv[arg_i]);
	    string s2 = string(eqpos+1);
	    s1.erase( eqpos - argv[arg_i] );  // cut off at the '='
	    g_equivs[ s1 ] = s2;
	    g_equivs[ s2 ] = s1;
            map<string,string,  my_strcmp>::iterator iter;
	  } else {
	    fprintf( stderr, "Error in command-line.  The argument to '-equal' must contain an '=' sign, as in: 'val1=val2'.\n" );
	    return COM_ERR;
	  }
	} else {
	  fprintf( stderr, "Error in command-line.  There must be an argument to '-equal'.\n" );
	  return COM_ERR;
	}
      } else if( strcmp( argv[arg_i], "-boilerplate-tag" ) == 0 ) {
        arg_i++;
	if( arg_i < argc ) {
	  g_boilerplates.insert( g_boilerplates.end(), argv[arg_i] );
	}
      } else if( strcmp( argv[arg_i], "-ignore-tag" ) == 0 ) {
        arg_i++;
	if( arg_i < argc ) {
	  g_ignores.insert( g_ignores.end(), argv[arg_i] );
	}
      } else {
        if( fileOne == NULL ) {
	  fileOne = fopen( argv[arg_i], "r" );
	  g_fileNameOne = string(argv[arg_i]);
	  if( fileOne == NULL )
	  {  fprintf(stderr, "Cannot open %s for reading: %s\n", argv[arg_i], strerror(errno) );
	     exit(FILE_OPEN_ERR);
	  }
	} else {
	  fileTwo = fopen( argv[arg_i], "r" );
	  g_fileNameTwo = string(argv[arg_i]);
	  if( fileTwo == NULL )
	  {  fprintf(stderr, "Cannot open %s for reading: %s\n", argv[arg_i], strerror(errno) );
	     exit(FILE_OPEN_ERR);
	  }
	}
      }
    }
    if( g_nulls.size() == 0 ) { // it was never defined, so pick the default:
       g_nulls.insert( g_nulls.end(), string(".") );
    }


    if( g_debug ) {
        int i;
	for( i = 0 ; i < argc; ++i ) {
	    fprintf( stderr, "debug: command-line arg[%d] = {%s}\n",i,argv[i] );
	}
	fprintf( stderr, "debug: List of Ignored Values:\n");
	if( g_ignores.size() == 0 ) {
  	    fprintf( stderr, "debug:    (none in list)\n");
        }
	for( i=0 ; i < g_ignores.size() ; ++i ) {
	    fprintf( stderr, "debug:    item %d = {%s}\n", i+1, g_ignores[i].c_str() );
	}
	fprintf( stderr, "debug: List Of Null Values:\n");
	if( g_nulls.size() == 0 ) {
  	    fprintf( stderr, "debug:    (none in list)\n");
        }
	for( i=0 ; i < g_nulls.size() ; ++i ) {
	    fprintf( stderr, "debug:    item %d = {%s}\n", i+1, g_nulls[i].c_str() );
	}
	fprintf( stderr, "debug: List Of Equivalent Values:, g_equivs size = %d\n",g_equivs.size());
	if( g_equivs.size() == 0 ) {
  	    fprintf( stderr, "debug:    (none in list)\n");
        }
        map<string,string,  my_strcmp>::iterator iter;
	for( iter = g_equivs.begin(); iter != g_equivs.end() ; ++iter ) {
	    fprintf( stderr, "debug:    item {%s} = {%s}\n", (*iter).first.c_str(), (*iter).second.c_str() );
	}
    }

    yyin = fileOne; // input
    if (yyparse() == 0) {
	starTreeOne = AST; // AST = star file parsed.
	fclose(yyin);
    } else {
        cerr << "# Error: Parsing problems in file: "<<g_fileNameOne<<endl;
	return SYN_ERR;
    }

    yyin = fileTwo; // input
    if (yyparse() == 0) {
	starTreeTwo = AST; // AST = star file parsed.
	fclose(yyin);
    } else {
        cerr << "# Error: Parsing problems in file: "<<g_fileNameTwo<<endl;
	return SYN_ERR;
    }

    int retVal = NO_ERR;
    retVal = do_the_diff( starTreeOne, g_fileNameOne.c_str(), starTreeTwo, g_fileNameTwo.c_str(), true );

    // Also try the other way around:
    retVal = retVal + do_the_diff( starTreeTwo, g_fileNameTwo.c_str(), starTreeOne, g_fileNameOne.c_str(), false );


    if( retVal == NO_ERR ) {
        fprintf( stdout, "%s:%s: NO DIFFERENCES REPORTED\n", g_fileNameOne.c_str(), g_fileNameTwo.c_str()  );
    }

    os->flush(); // Must do this to close the output, else it never flushes
                 // the last line of output.

    return retVal;
}


// Do a destructive diff - destroy tags in B that exist in A,
// then we can diff in the opposite direction and look at what
// remains in B that is not in A.

int do_the_diff( StarFileNode *A, const char *A_name, StarFileNode *B, const char *B_name, bool leftToRight )
{
  int retVal = NO_ERR;
  int result = 0;

  // For each data/global block in file A:
  // (or for just the first block if that flag is set):
  List<ASTnode*> *blocks_A = A->searchForType( ASTnode::BLOCKNODE );
  int i_blocks_A;
  for(   i_blocks_A = 0 ;
	 // Stop when reaching the end of the list of blocks or after just the first
	 // block if the compare-first-blocks flag is on:
         i_blocks_A < blocks_A->size() && !(g_justCompareFirstBlocks && i_blocks_A > 0)  ;

	 i_blocks_A++ ) {

    BlockNode *thisBlock_A = (BlockNode*) ( (*blocks_A)[i_blocks_A] );


    // Check for a matching block in file B, or just take the first block if the compare-first-blocks flag is on:
    List<ASTnode*> *blocks_B = NULL;
    if( g_justCompareFirstBlocks ) {
      // list of all blocks:
      blocks_B = B->searchForType( ASTnode::BLOCKNODE );
      // truncate to just take the first block:
      if( blocks_B->size() > 1 ) {
        blocks_B->erase( blocks_B->begin()+1, blocks_B->end() );
      }
    } else {
      blocks_B = B->searchForTypeByTag( ASTnode::BLOCKNODE, thisBlock_A->myName().c_str() );
    }
    BlockNode *thisBlock_B;
    if( blocks_B->size() == 0 ) {
      fprintf( stdout, "--%c %s:%d: %s\n", (leftToRight?'<':'>'), A_name, thisBlock_A->getLineNum(), thisBlock_A->myName().c_str() );
      retVal = DIF_ERR;
    } else if( blocks_B->size() > 1 ) {
      fprintf( stdout, "**%c %s:%d A block called %s exists more than once.\n", (leftToRight?'>':'<'), B_name, (*blocks_B)[1]->getLineNum(), thisBlock_A->myName().c_str() );
      retVal = SYN_ERR;
    } else {
      thisBlock_B = (BlockNode*) ( (*blocks_B)[0] );

      // For each saveframe in the datablock in file A:
      List<ASTnode*> *saveFrames_A = thisBlock_A->searchForType( ASTnode::SAVEFRAMENODE );
      int i_saveFrames_A;
      // fprintf(stderr,"eraseme: Got here: %d\n",__LINE__ );
      for(   i_saveFrames_A = 0 ;
	     i_saveFrames_A < saveFrames_A->size() ;
	     i_saveFrames_A++ ) {

      // fprintf(stderr,"eraseme: Got here: %d\n",__LINE__ );
	SaveFrameNode *thisSf_A = (SaveFrameNode*) ( (*saveFrames_A)[i_saveFrames_A] );

	// Check for a matching SF in file B:
      // fprintf(stderr,"eraseme: Got here: %d\n",__LINE__ );
	List<ASTnode*> *saveFrames_B = B->searchForTypeByTag( ASTnode::SAVEFRAMENODE, thisSf_A->myName().c_str() );
      // fprintf(stderr,"eraseme: Got here: %d\n",__LINE__ );
	SaveFrameNode *thisSf_B;
	if( saveFrames_B->size() == 0 ) {
	  // This is an ad-hoc hack - check to see if there exists a
	  // saveframe with the same saveframe category as this saveframe
	  // if there is, and there is only ONE such hit, then assume this
	  // saveframe matches up with that one and diff them.
	  thisSf_B = get_same_sf_cat_hit( thisSf_A, B );
	  if( thisSf_B == NULL ) {
	      result = diff_one_sf_or_block( thisSf_A, A_name, g_emptySF, B_name, leftToRight );
	  } else {
	      result = diff_one_sf_or_block( thisSf_A, A_name, thisSf_B, B_name, leftToRight );
	  }

	  if( result != NO_ERR ) {
	      retVal = result;
	  }
	} else if( saveFrames_B->size() > 1 ) {
	  SaveFrameNode *thisSf_B_1 = (SaveFrameNode*) ( (*saveFrames_B)[0] );
	  SaveFrameNode *thisSf_B_2 = (SaveFrameNode*) ( (*saveFrames_B)[1] );
	  fprintf( stdout, "**%c %s:%d and %s:%d: Saveframe %s exists more than once.\n",
	           (leftToRight?'>':'<'),
		   B_name,
		   thisSf_B_1->getLineNum(),
		   B_name,
		   thisSf_B_2->getLineNum(),
		   thisSf_B_2->myName().c_str()
		   );


	  retVal = SYN_ERR;
	} else {
	  thisSf_B = (SaveFrameNode*) ( (*saveFrames_B)[0] );


	  // The code gets rather ugly from here on out, so I'm making it a seperate routine:
	  result =  diff_one_sf_or_block( thisSf_A, A_name, thisSf_B, B_name, leftToRight );
	  if( result != NO_ERR ) {
	      retVal = result;
	  }
	  
	}

      // fprintf(stderr,"eraseme: Got here: %d\n",__LINE__ );
	delete saveFrames_B;
      // fprintf(stderr,"eraseme: Got here: %d\n",__LINE__ );
      }
      // fprintf(stderr,"eraseme: Got here: %d\n",__LINE__ );


      // For each name that is outside of the saveframes (for cif files, for example)
      // check them too at the block level:
      result = diff_one_sf_or_block( thisBlock_A, A_name, thisBlock_B, B_name, leftToRight );
      if( result != NO_ERR ) {
	  retVal = result;
      }

    }
    delete blocks_B; // clean up after searchForTypeByTag
  }

  delete blocks_A; // clean up after searchForType
  return retVal;
}

int diff_one_sf_or_block( ASTnode *node_A, const char *fName_A, ASTnode *node_B, const char *fName_B, bool leftToRight )
{


  // Make a list of all name hits:
  // These are the looped hits:
  List<ASTnode*> *allNames_A = NULL;
  allNames_A = node_A->searchForType( ASTnode::DATANAMENODE );


#if 0
/**/  // These are the free hits: - This has been removed because now the datanamenode list catches everything including dataitemnodes:
/**/  List<ASTnode*> *appendMe_A =  node_A->searchForType( ASTnode::DATAITEMNODE );
/**/  fprintf(stderr,"eraseme: there are %d free tags in node_A\n",appendMe_A->size() );
/**/  // Concatenate the free hits to the end of the looped hits so it's all one list of data name nodes:
/**/  for( int i = 0 ; i < appendMe_A->size() ; i++ ) {
/**/    allNames_A->insert( allNames_A->end(), ((DataItemNode*)(*appendMe_A)[i])->myNameNodePtr() );
/**/  }
/**/  delete appendMe_A;
#endif


  bool A_isSF = false;
  if( node_A->isOfType( ASTnode::SAVEFRAMENODE ) ) {
    A_isSF = true;
  } else {
    A_isSF = false;
    // If this is not a saveframe, then remove all the hits that are inside of saveframes, as
    // they are handled on other passes - for blocknodes we only want to cover the
    // stuff outside of the saveframes:
    for( int idx = allNames_A->size() - 1 ; idx >= 0 ; --idx ) {
      // for-loop counts backward so deletions won't result in skipping items.
      ASTnode *parent = (*allNames_A)[idx];
      while( parent != NULL && ! parent->isOfType(ASTnode::SAVEFRAMENODE) ) {
        parent = parent->myParent(); // grandparent, then great-grandparent, and so on.
      }
      if( parent != NULL ) {
        // It found that it was inside a saveframe node, so remove it:
	allNames_A->erase( allNames_A->begin() + idx );
      } else {
        // do nothing - I had a debug print statement here once.
      }
    }
  }

  int allNamesLen_A = allNames_A->size();
  int retVal = 0;
  vector<string> *values_A;
  vector<string> *values_B;

  for( int iName_A = 0 ; iName_A < allNamesLen_A ; iName_A++ ) {
    DataNameNode *thisName_A;
    thisName_A = (DataNameNode*) ( (*allNames_A)[iName_A] );
    values_A = valuesForName( thisName_A );

    bool isIgnored = false;
    for( int j = 0 ; j < g_ignores.size() ; j++ ) {
      if( thisName_A->myName() == g_ignores[j] ) {
        isIgnored = true;
	break;
      }
    }
    if( isIgnored ) {
      continue;
    }

    bool isBoilerplate = false;
    for( int j = 0 ; j < g_boilerplates.size() ; j++ ) {
      if( thisName_A->myName() == g_boilerplates[j] ) {
	isBoilerplate = true;
	break;
      }
    }

    bool allNulls = true;
    string dot = string(".");
    for( int i = 0 ; i < values_A->size() ; i++ ) {
      string val = (*values_A)[i];
      bool isNull = false;
      for( int j = 0 ; j < g_nulls.size() ; j++ ) {
        if( val == g_nulls[j] ) {
	  isNull = true;
	  break;
	}
      }
      if( ! isNull ) {
        allNulls = false;
	break;
      }
    }
    if( allNulls ) {
      continue; // Skip the check for this direction, but check it in the other
                // direction when A becomes B.
    }

    DataNameNode *thisName_B;
    // Try to find the matching name in B:
    // These are the looped hits:
    List<ASTnode*> *allNames_B = node_B->searchForTypeByTag( ASTnode::DATANAMENODE, thisName_A->myName().c_str() );
    // These are the free hits, which have to be seperate because of how searchForTypeByTag works:
    List<ASTnode*> *appendMe_B = node_B->searchForTypeByTag( ASTnode::DATAITEMNODE, thisName_A->myName().c_str() );
    // Concatenate the free hits to the end of the looped hits so it's all one list of data name nodes:
    string name_B = "((unspecified name))";
    for( int i = 0 ; i < appendMe_B->size() ; i++ ) {
      allNames_B->insert( allNames_B->end(), ((DataItemNode*)(*appendMe_B)[i])->myNameNodePtr() );
    }
    delete appendMe_B;


    if( node_B->isOfType(ASTnode::SAVEFRAMENODE) ) {
      name_B = ( (SaveFrameNode*) node_B )->myName();
    } else if( node_B->isOfType(ASTnode::BLOCKNODE) ) {
      name_B = ( (BlockNode*) node_B )->myName();
      // If this is not a saveframe, then remove all the hits that are inside of saveframes, as
      // they are handled on other passes - for blocknodes we only want to cover the
      // stuff outside of the saveframes:
      for( int idx = allNames_B->size() - 1 ; idx >= 0 ; --idx ) {
	// for-loop counts backward so deletions won't result in skipping items.
	ASTnode *parent = (*allNames_B)[idx];
	while( parent != NULL && ! parent->isOfType(ASTnode::SAVEFRAMENODE) ) {
	  parent = parent->myParent(); // grandparent, then great-grandparent, and so on.
	}
	if( parent != NULL ) {
	  // It found that it was inside a saveframe node, so remove it:
	  allNames_B->erase( allNames_B->begin() + idx );
	} else {
        }
      }
    }


    if( allNames_B->size() == 0 ) {
      if( isBoilerplate && node_B == g_emptySF ) {
	// Say nothing: not an error to be reported.
      } else {
	fprintf( stdout, "--%s %s:%d: %s [%svalue=%s]\n",
	         (leftToRight?"<":">"),
	         fName_A,
		 thisName_A->getLineNum(),
		 thisName_A->myName().c_str(),
		 (values_A->size() > 1 ? "first ":""),
		 (values_A->size() > 0 ? strip_eoln((*values_A)[0]).c_str() : "<<non existant>>" )
		 );
	retVal = DIF_ERR;
      }
    } else if( allNames_B->size() > 1 ) {
      thisName_B = (DataNameNode*) ( (*allNames_B)[1] );
      fprintf( stdout, "**%s %s:%d: Tag %s exists more than once in '%s' in %s.\n",
               (leftToRight?"<":">"),
	       fName_B,
	       thisName_B->getLineNum(),
	       thisName_B->myName().c_str(),
	       name_B.c_str(),
	       fName_B );
      retVal = SYN_ERR;
    } else {
      thisName_B = (DataNameNode*) ( (*allNames_B)[0] );

      values_B = valuesForName( thisName_B );

      if( values_A->size() > values_B->size() ) {
        fprintf(stdout, "--%s %s:%d vs %s:%d : %s : more values in %s than in %s\n", 
	        (leftToRight?"<":">"),
		(leftToRight?fName_A:fName_B),
		(leftToRight?thisName_A->getLineNum():thisName_B->getLineNum()),
		(leftToRight?fName_B:fName_A),
		(leftToRight?thisName_B->getLineNum():thisName_A->getLineNum()),
		thisName_A->myName().c_str(),
		fName_A,
		fName_B);
	retVal = DIF_ERR;
      } else if(  values_A->size() < values_B->size() ) {
        fprintf(stdout, "--%s %s:%d vs %s:%d : %s : more values in %s than in %s\n", 
	        (leftToRight?"<":">"),
		(leftToRight?fName_A:fName_B),
		(leftToRight?thisName_A->getLineNum():thisName_B->getLineNum()),
		(leftToRight?fName_B:fName_A),
		(leftToRight?thisName_B->getLineNum():thisName_A->getLineNum()),
		thisName_A->myName().c_str(),
		fName_B,
		fName_A );
	retVal = DIF_ERR;
      }
      for( int val_i = 0 ; val_i < values_A->size() || val_i < values_B->size() ; val_i++ ) {
	string aVal;
	if( val_i < values_A->size() ) {
	  aVal = (*values_A)[val_i];
	} else {
	  aVal = string("(((not-present)))");
	}
	string bVal;
	if( val_i < values_B->size() ) {
	  bVal = (*values_B)[val_i];
	} else if( isBoilerplate && node_B == g_emptySF ) {
	  bVal = aVal; // force them to act like they match.
	} else {
	  bVal = string("(((not-present)))");
	}
	if( g_ignorePadding ) {
	  int aSubStrStart = aVal.find_first_not_of( " \t\n\r" );
	  int bSubStrStart = bVal.find_first_not_of( " \t\n\r" );
	  int aSubStrLast = aVal.find_last_not_of( " \t\n\r" );
	  int bSubStrLast = bVal.find_last_not_of( " \t\n\r" );
	  if( aSubStrStart >= 0 && aSubStrLast >= 0  ) {
	     aVal = aVal.substr( aSubStrStart, (aSubStrLast - aSubStrStart)+1 );
	  }
	  if( bSubStrStart >= 0 && bSubStrLast >= 0  ) {
	     bVal = bVal.substr( bSubStrStart, (bSubStrLast - bSubStrStart)+1 );
	  }
	}


	bool isMatch;
	isMatch = ( aVal == bVal );
	if( ! isMatch ) {
	  // see if the hashmap has an equivilency map for this:
	  if( g_equivs.find(aVal) != g_equivs.end() ) {
	    if( g_equivs[aVal] == bVal ) {
	      isMatch = true;
	    }
	  }
	  // Have to check bi-directionally, so here's the lookup the other way around:
	  if( g_equivs.find(bVal) != g_equivs.end() ) {
	    if( g_equivs[bVal] == aVal ) {
	      isMatch = true;
	    }
	  }
	}
	if( ! isMatch ) {
	  string printAVal = strip_eoln( aVal.substr( 0, 2048 ) );
	  if( aVal.length() > 2048 ) {
	    printAVal += "<--STRING TOO LONG - TRUNCATING DISPLAY-->";
	  }
	  string printBVal = strip_eoln( bVal.substr( 0, 2048 ) );
	  if( bVal.length() > 2048 ) {
	    printBVal += "<--STRING TOO LONG - TRUNCATING DISPLAY-->";
	  }
          fprintf(stdout, "--~ %s:%d vs %s:%d : %s : '%s' versus '%s'\n",
		(leftToRight?fName_A:fName_B),
		(leftToRight?thisName_A->getLineNum():thisName_B->getLineNum()),
		(leftToRight?fName_B:fName_A),
		(leftToRight?thisName_B->getLineNum():thisName_A->getLineNum()),
		thisName_A->myName().c_str(),
		printAVal.c_str(),
		printBVal.c_str() );
	  retVal = DIF_ERR;
	}
      }


      delete values_B;
    }
    
    for( int iName_B = 0 ; iName_B < allNames_B->size() ; iName_B++ ) {
      ASTnode *nameB = ( (*allNames_B)[iName_B] );
      if( nameB->myParent()->isOfType( ASTnode::DATAITEMNODE ) ) {
        delete nameB->myParent(); // delete the free tag node
      } else {
        delete nameB; // delete the column of the loop.
      }
    }
    delete values_A;
    delete allNames_B; // clean up after searchForTypeByTag

  }
  delete allNames_A; // clean up after searchForType

  return retVal;
}

vector<string> *valuesForName( DataNameNode *nameNode )
{
  vector<string> *retVal;
  ASTnode      *parent;
  DataItemNode *itemNode = NULL;
  DataLoopNode *loopNode = NULL;

  retVal = new vector<string>;

  // walk up the parent tree until hitting a loop or freetag:
  for( parent = nameNode ; parent != NULL ; parent = parent->myParent() ) {
    if( parent->isOfType(ASTnode::DATAITEMNODE) ) {
      itemNode = (DataItemNode*)parent;
      break;
    } else if( parent->isOfType(ASTnode::DATALOOPNODE) ) {
      loopNode = (DataLoopNode*)parent;
      break;
    }
  }
  if( parent != NULL ) {
    if( itemNode != NULL ) {
      retVal->insert( retVal->end(), itemNode->myValue() );
    } else {
      int col, dummyNest;
      loopNode->tagPositionDeep( nameNode->myName(), &dummyNest, &col );
      LoopTableNode *tbl = loopNode->getValsPtr();
      for( int row=0 ; row < tbl->size() ; row++ ) {
	retVal->insert( retVal->end(), (*((*tbl)[row]))[col]->myValue()  );
      }
    }
  }
  return retVal;
}
string strip_eoln( string in )
{
  string retVal = string("");

  int i; int j;
  i = 0;
  for( j = 0 ; j < in.length() ; j++ ) {
    if( in[j] == '\n' ) {
      retVal += "\\n";
    } else if( in[j] == '\r' ) {
      retVal += "\\r";
    } else {
      retVal += in[j];
    }
  }
  return retVal;
}
SaveFrameNode *get_same_sf_cat_hit( SaveFrameNode *thisSf_A, StarFileNode *B ) 
{
   SaveFrameNode *retVal = NULL;
   // First, try to find the sf_category tag, in either 2.1 parlance or in 3.1 parlance:
   List<ASTnode*>* matchFreeTags = thisSf_A->searchForType( ASTnode::DATAITEMNODE );

   string sfCatName = string ("_Saveframe_category");
   string sfCatSuffix = string( ".Sf_category" );
   int suffixLen = sfCatSuffix.length();
   int i;
   DataItemNode *din;
   string name = string("");
   string val = string("");
   for( i = 0 ; i < matchFreeTags->size() ; ++i ) {
      din = (DataItemNode*) (  (*matchFreeTags)[i]  );
      name = din->myName();
      int nameLen = name.length();
      if( // 2.1 parlance:
          name == sfCatName ||
	  // 3.1 parlance:
          (  (nameLen >= suffixLen) &&   
	      name.substr( nameLen - suffixLen, suffixLen ) == sfCatSuffix )
        )  {
	val = din->myValue();
	break;
      }
   }
   if( val != "" ) {
      List<ASTnode*>*matchSfs = B->searchForTypeByTagValue( ASTnode::SAVEFRAMENODE, name, val );
      if( matchSfs->size() > 0 ) {
         retVal = (SaveFrameNode*)  (  (*matchSfs)[0]  );
      }
      delete matchSfs;
   }

   delete matchFreeTags;
   return retVal;
}
