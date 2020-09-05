//////////////////////////// RCS COMMENTS: /////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
//  HISTORY LOG:
//  ------------
//  $Log: not supported by cvs2svn $
//  Revision 1.30  2008/08/14 18:44:27  madings
//  *** empty log message ***
//
//  Revision 1.29  2008/04/16 21:29:11  madings
//  Haven't committed in a while and this is a working version so I should.
//
//  Revision 1.28  2006/10/27 23:22:05  madings
//  Changes to allow for assigned chemical shift splitting.
//
//  Revision 1.27  2006/07/14 21:00:31  madings
//  *** empty log message ***
//
//  Revision 1.26  2006/05/02 21:20:37  madings
//  *** empty log message ***
//
//  Revision 1.25  2005/11/07 20:21:18  madings
//  *** empty log message ***
//
//  Revision 1.24  2005/03/18 22:00:30  madings
//  Added flat file creating rule
//
//  Revision 1.23  2005/03/18 21:07:20  madings
//  Massive update after too long without a checkin
//
//  Revision 1.22  2005/03/04 23:32:04  madings
//  checking in doc changes and a lot of incremental bugfixes
//  too many to mention (cvs repository was broken and confused
//  for a while so this represents about 5 months of stuff)
//
//  Revision 1.21  2003/07/24 18:49:13  madings
//  Changes to handle 2.1-3.0 mappings.
//
//  Revision 1.20  2001/06/18 16:57:08  madings
//  Several fixes for s2nmr segfault bugs.
//
//  Revision 1.19  2001/04/13 21:17:07  madings
//  Changes to use the new starlib2.
//  Also discovered an error in transform10 as well.
//
//  Revision 1.18  2000/11/18 03:02:36  madings
//  *** empty log message ***
//
// Revision 1.17  2000/03/20  09:45:48  madings
// Weekend fixes to s2nmr, to respond to some of Jurgen's comments:
// Note, the old s2nmr program is under the name "/bmrb/bin/s2nmr-old", just
// in case you discover a major bug and want to get the old version back.
//
// 1 - The max number allowed for a saveframe's order sequence used to be
// 999.  This was too small.  It has been increased to 9999999, and documented
// acccordingly in specs.html
//
// 2 - Documented that only one middle initial is recognized when
// parsing names from strings.
//
// 3 - Numerous places in the code that created new values did so without
// the proper delimiter type  (For example, it might put a multi-line
// value into a quoted string ("...") instead of a semicolon string.)
// Jurgen noticed this on the citation title, but when I looked for it
// in the code, I found that particular mistake was common throughout the
// newer rules that came after transform # 17.  This has been fixed.
//
// 4 - rule 19, change data_tag_name, did not work as expected:
// I looked at the code and this was simply a case of bad documentation.
// What this rule is meant to do is look for the _BMRB_accession_number
// tag somewhere in the file and use its value as the name of the data_
// tag.  That's all it does.  If a _BMRB_accession_number tag is not there,
// it fails to do anything.  The documentation has been changed to properly
// describe this.
//
// 5 - "rule 22 is unclear to me" - I have re-written explanation of this
// rule, and fixed a bug in the code that refused to operate on multiple
// target saveframes.  (If the key/value pair designated more than one
// output saveframe, it would fail.)
//
// 6 - "rule 25 change_tag_value has backward documenation".  Looked at
// the code to see what it does, and the code is really buggy.  A redo
// from scratch would be quicker than trying to fix it.  The documentation
// isn't backward - the code is.  It uses the wrong variables for things,
// but not in a consistant manner, so I can't just quickly swap things
// around.  Also, we need to define what should be the correct behaviour
// when multiple saveframes are found by either of the two key_tag/value
// searches.  If two input saveframes are found, and two output saveframes
// are found, then is the correct behaviour to copy input 1 to output 1 and
// input 2 to output 2?  Or is the correct behaviour to copy in a 'cross
// product' fashion and copy input 1 to both outputs 1 and 2, and copy
// input 2 to both outputs 1 and 2?  Issues of possible name clashes need
// to be considered too with this.  In light of this, I'm going to put off
// working on this rule untill I am sure it is worth the time, and this
// ambigious behaviour is defined better.  I have to be careful not to spend
// too much time on this program.
//
// 7 - Fixed the broken condition where output_sf_name_tag is null (.) in
// values_to_saveframes_many_to_many.
//
// 8 - Repaired a few typos here and there.
//
// CVS----------------------------------------------------------------------
//
// Revision 1.16  2000/03/17  01:54:01  madings
// Added transforms: rename_saveframes, and remove_saveframes.
//
// Revision 1.15  2000/03/16  05:09:42  madings
// Jurgen discovered some minor bugs in s2nmr and the fixes are installed:
//
//    1 - remove_null_tags was previously unable to handle asterisks
//       as wildcards when filling out the key_tag and key_value field.
//       This is fixed.
//
//    2 - The same goes for remove_tag, which will now take an asterisk
//       as the saveframe name, meaning all safeframes.
//
//    3 - The documentation on the internal website had some typos in
//       tag names.
//
// Revision 1.14  1999/07/30  18:22:06  june
// june, 7-30-99
// add transform1d.cc
//
// Revision 1.13  1999/06/29  19:04:10  zixia
// *** empty log message ***
//
// Revision 1.12  1999/06/18  21:14:34  zixia
// *** empty log message ***
//
// Revision 1.11  1999/06/10  17:38:54  zixia
// Adding a couple of new functions to transform files in NMR-STAR 1.0 to NMR-STAR 2.0.
//
// Revision 1.10  1999/04/20  21:41:42  madings
// Added transforms 17 and 18 to main.cc's case statement, and
// added transform17.cc and altered transform 2a and 1c so they
// can handle the case where the input is not coming from a saveframe,
// but is coming from outside a saveframe instead.
//
// Revision 1.9  1999/04/20  21:15:06  zixia
// *** empty log message ***
//
// Revision 1.8  1999/04/09  20:15:55  madings
// added many, many transforms.  transform 17 still does not work yet, though.
//
// Revision 1.7  1999/02/05  01:34:43  madings
// Many changes from debug sessions with Eldon:
// 1 - added new transforms:  transform15.cc and transform16.cc
// 2 - added 'don't-care' functionality to transform14.cc when
//     tag_value is a dot.
// 3 - Fixed some garbled errStream messages that printed the wrong
//     transform # to the log (transform11 printed error messages tagged
//     as transform6 for example.)
//
// Revision 1.6  1999/01/28  06:15:04  madings
// Some bugfixes from Eldon's attempts to use this, as well as a major
// design change:
//
//    Bug fix 1: save_loops was re-creating tags that had been removed
//       earlier in the run.  This has been fixed.
//
//    Bug fix x: after bug fix 1 above, save_loops was leaving behind
//       loops that had no contents.  This has been fixed.
//
//    Massive design change:
//       There is no longer any default order of operations at all.
//       Instead, it uses the new 'save_order_of_operations' saveframe
//       in the mapping dictionary file to decide what rules to run
//       when.  Also, to allow it to have the same rule run more than
//       once at different times from different mapping dictionary
//       saveframes, the name of the saveframe is no longer the rule
//       to run.  Instead the name of the saveframe is irrelevant, and
//       it's the new _Saveframe_category tag that tells the program
//       what rule to run for that saveframe.  This required a small,
//       but repetative change in all the transform rules so that the
//       main.cc program can tell the transforms which saveframe in the
//       mapping file is the one they are supposed to use instead of the
//       transforms trying to look for it themselves.
//
// Revision 1.5  1999/01/27  00:26:50  madings
// Fixed a bug in save_loops that was causing deleted tags to be recreated
// when making a loop.  Also re-arranged the order of operations and adjusted
// the documentation to match this.
//
// Revision 1.4  1998/12/15  04:33:04  madings
// Addded transforms 9 though 14.  (Lots of stuff)
//
// Revision 1.3  1998/11/20  06:33:23  madings
// Added some new transforms on the way to fixing the s2nmr converter.
//
// Revision 1.2  1998/11/11  04:08:15  madings
// Added transform6, but did so in such a way that CVS is falsely
// marking every single file as being changed, so this comment will
// probably appear in many files where it does not apply.
//
// Revision 1.6  1997/11/04  20:42:20  madings
// Changed so that the AST ---> NMR copy is a parallel copy.
//
// Revision 1.5  1997/10/25  02:59:29  madings
//  2 Things in this check-in:
//    -1- "Parallel copy".  Addded a new way to do a copy constructor
//        such that it will copy with a parallel link back to the
//        original tree it was copied from.  Any node in either tree
//        can refer to the corresponding node in the new tree.
//        See "parallel.html" for more detail.
//
//    -2- Bug fixes to copy constructors that did not properly deep-copy
//        all the way down into the LoopIter values.
//
// Revision 1.4  1997/10/01  22:35:05  bsri
// This version of starlib split main.cc into separate files (which contain
// the transformation functions - one in each file).
//
// Revision 1.2  1997/09/15  22:28:28  madings
// Split list.h into list.h and astlist.h.  DId the same with list.cc
//
// Revision 1.1.1.1  1997/09/12  22:54:40  madings
// Restarting CVS tree because I lost the CVS dir.
//
//Revision 1.6  1997/07/30  21:34:26  madings
//check-in before switching to cvs
//
//Revision 1.5  1997/07/11  22:40:05  madings
//Finally seems to work - just waiting for
//new dictionaries.
//
//Revision 1.4  1997/06/13  22:52:44  madings
//Checking in before bringing in sridevi code.
//
//Revision 1.3  1997/06/04  18:57:58  madings
//The RCS ID variables were missing the trailing $, so I
//changed them across all these files.
//
//Revision 1.2  1997/06/04  18:52:45  madings
//I Added the RCS log and ID variables.
//
//
#endif
////////////////////////////////////////////////////////////////////////

#include "ast.h"
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "astlist.h"
#include "loopvals.h"
#include "transforms.h"
#include "parser.h"
#include <algorithm>

// GLOBAL variable: the root of the AST built by a (successful) parse)

StarFileNode *AST;
StarFileNode *NMR;
StarFileNode *DIC;

ostream * os;
ofstream * errStream;

string g_outFname;




//  Pointer to a transform function.
//  --------------------------------
typedef void    (*transformFuncPtr_t)(  StarFileNode* AST,
				        StarFileNode* NMR,
					BlockNode*    currDicDataBlock,
					SaveFrameNode *currDicRuleSF );

typedef struct
{
    int                 order;
    transformFuncPtr_t  fptr;
    char                fname[255];
} transformRuleEntry_t;

// Prototypes:
// -----------
void   getOrder( vector<transformRuleEntry_t> &rules, StarFileNode *mapStar );
bool   ruleSeqComp( transformRuleEntry_t entryA, transformRuleEntry_t entryB );
void   string_print( const string &printThis );
string getCategory( ASTnode *searchThrough, string sfName );
transformFuncPtr_t  getFuncFromName( string funcName );
void   bogusRule( StarFileNode *, StarFileNode *, BlockNode *, SaveFrameNode *);
int    yyparse_with_my_errs( char *fname, int warn_flg );

  ///////////////////
 //    main()     //
///////////////////
int main( int argc, char **argv )
{

  FILE     *inFile;         // The file lex/yacc will use to read in the star file.
  FILE     *dictFile;       // The file lex/yacc will use to read in the dictionary file.
  FILE     *readerrFP;      // The file to send errors to.
  char     errFName[512];   // The filename to send errors to.
  int      choice;          // Which thing was picked from the main menu?
  bool     tryAgain;        // true = must re-run the loop, false = done with loop.
  int      retVal;          // temp storage of functions' return values.
  bool     automode;        // True if the program should run all through
                            //     without using the menu.  (Just run all
		            //     transforms in the order of operations.)
  			    //     (Actually this is always true now.  The
  			    //     command-line option has been disabled.)
  int      yy_retval;
  int      idx;
  int      warn_on;

  // func ptr to one of the rule functions:
  transformFuncPtr_t  ruleFunc;
  string              ruleName;

  // This is an array of function pointers to all the transform functions.
  // the ordering of the elements in this list determines the order of
  // opearations for running these functions in automatic mode:
  //
  vector<transformRuleEntry_t> automatic_order;

  inFile = dictFile = (FILE*)NULL;
  errStream = (ofstream*)NULL;
  os = (ofstream*)NULL;
  automode = true;   // (The default is always true now.)
  retVal = 0;
  warn_on = true; // Show warnings by default.

  // Iterate over the command-line arguments:
  // -----------------------------------------
  for( int argNum = 1 ; argNum < argc ; argNum++ )
  {
      // "-a" means "automatic mode".  Not really necessary anymore:
      // -----------------------------------------------------------
      if( strcmp( argv[argNum], "-a" ) == 0 )
      {
	  automode = true;
      }
      
      // "-w" means "warnings off".
      // -----------------------------------------------------------
      else if( strcmp( argv[argNum], "-w" ) == 0 )
      {
	  warn_on = false;
      }
      
      // "-o" means "output filename":
      // -----------------------------
      else if( strcmp( argv[argNum], "-o" ) == 0 )
      {   if( ++argNum >= argc )
	  {   cerr<<"-o requires a parameter."<<endl;
	      exit(1);
	  }
	  else
	  {   if( (os = new ofstream( argv[argNum] ) ) == NULL)
	      {   cerr << "Unable to open " << argv[argNum] << endl;
		  exit(1);
	      }
	      g_outFname = string( argv[argNum] );
	  }
      }

      // "-e" means "error filename":
      // ----------------------------
      else if( strcmp( argv[argNum], "-e" ) == 0 )
      {   if( ++argNum >= argc )
	  {   cerr<<"-e requires a parameter."<<endl;
	      exit(1);
	  }
	  else
	  {   if( (errStream = new ofstream( argv[argNum] ) ) == NULL)
	      {   cerr << "Unable to open " << argv[argNum] << endl;
		  exit(1);
	      }
	  }
      }
      // Default - not a dashed option. dictFile comes first,
      // then if it has already been set, go on to inFile:
      // ---------------------------------------------------
      else if( dictFile == NULL )
      {   if( (dictFile = fopen(argv[argNum], "r")) == NULL ) 
	  {   cerr << "Unable to open " << argv[argNum] << endl;
	      exit(1);
	  }
      }
      else
      {   if( strcmp( argv[argNum], "-" ) == 0 )  // If it's a dash - use stdin.
	      inFile = stdin;
	  else
	  {   if( (inFile = fopen(argv[argNum], "r")) == NULL )
	      {   cerr << "Unable to open " << argv[argNum] << endl;
		  exit(1);
	      }
	  }
	  // Now, if they have not already been set, open
	  // the error output and the regular output.
	  if( errStream == (ofstream*)NULL )
	  {
	      strcpy( errFName, argv[argNum] );
	      strcat( errFName, ".err" );
	      if( ( errStream = new ofstream( errFName ) ) == NULL )
	      {   cerr << "Unable to open " << errFName << endl;
		  exit(1);
	      }
	  }
	  if( os == (ofstream*)NULL )
	  {
	      char fName[ 512 ];
	      strcpy( fName, argv[argNum] );
	      strcat( fName, ".nmrstr" );
	      if( ( os = new ofstream( fName ) ) == NULL )
	      {   cerr << "Unable to open " << fName << endl;
		  exit(1);
	      }
	  }
      }
  }

  // Print usage if any parameters were forgotten:
  if(  errStream == (ofstream*)NULL      ||
       os == (ofstream*)NULL             ||
       dictFile == (FILE*)NULL           ||
       inFile == (FILE*)NULL             ||
       strcmp( argv[1], "--help" ) == 0  || 
       strcmp( argv[1], "?" ) == 0      )
  {    cerr <<
        "usage: "<<argv[0]<<" [-w] [-o filename] [-e filename] <diction> <input>" << endl <<
	"\t-w\tTurn off parse warnings.  Only show syntax errors that are fatal. " << endl <<
	"\t-o\tThe output filename.  Default is *.nmrstr, " << endl <<
	"\t\t     with the name taken from the input filename." << endl <<
	"\t-e\tThe error filename.  Default is *.err, " << endl <<
	"\t\t     with the name taken from the input filename." << endl <<
	"\tdiction\tThe dictionary file to drive the transform." << endl <<
	"\tinput\tThe input file in generic-star format. "<< endl <<
	"\t\tNote that the -a flag has been removed.  Automatic mode is now" << endl <<
	"\t\tthe only mode supported, so the flag is meaningless and ignored." << endl <<
	"\t\tAlso note that the ability to use a filename of '-' to indicate " << endl <<
	"\t\tstandard input or standard output has also been removed, due " << endl <<
	"\t\tto the fact that the C++ iostream library has stopped supporting " <<endl <<
	"\t\tthe ability to construct file streams from open file handles. "<< endl;
	 
      exit(1);
  }

    if( !automode )
        readerrFP = fopen( errFName, "r" );

    yyin = dictFile;  // Now read the dictionary file into AST:
    star_syntax_print = false;
    yy_retval = yyparse_with_my_errs("MAPPING FILE", warn_on);
    if(yy_retval != 1)
      {
	DIC = AST;

	yyin = inFile;
	yy_retval = yyparse_with_my_errs("INPUT FILE", warn_on);
	if(yy_retval !=1)  // Now read the input file again ( because the above
			   // step step clobbered the input star file in AST.)
	  {
	    NMR = new StarFileNode(true, *AST);  // copy the original AST structure into the NMR structure.
			// (AST will be the "pre-transform" tree, and NMR will
			// be the "post-transform" tree.


	    ASTlist<BlockNode*>* listOfDicDataBlocks = DIC->GiveMyDataBlockList();

	    // Now read the order of operations from the mapping dictionary:
	    getOrder( automatic_order, DIC );

	    int funcNum = 0;

	    while( 1 )  // loop forever until explicit break.
	      {
		tryAgain = false;
		if( !automode )
		{
		    // do nothing.
		}
		else
		{   // Automatic - so pull the next function to run from the array.
		    ruleFunc = automatic_order[ funcNum ].fptr;
		    ruleName = string( automatic_order[ funcNum ].fname );

		    // Progress bar thingy:
		    // ---------------------
		    printf( "\r[" );
		    for( int r = 0 ; r < automatic_order.size(); r++ )
			if( r <= funcNum )
			    putchar( '|' );
			else
			    putchar( '.' );

		    // If the null was pulled from the array, then we are done:
		    if( ruleFunc == (transformFuncPtr_t)NULL )
		    {   NMR->Unparse(0);
			printf( "] \n %s\n", "Done" );

			FILE *fp;
			int ch;
			char tempfname[256];
                        sprintf( tempfname, "/tmp/s2nmr_%d.str", getpid() );
			if( (fp = fopen( tempfname, "r" )) != NULL )
			{
			   while( ! feof(fp) )
			   {
				ch = fgetc( fp );
				if( ch != EOF )
				{   (*os) << ((char)ch);
				    if( ch == '\r' || ch == '\n' )
				    {  (*os) << ((char)'#');
				    }
				}
			   }
			   fclose(fp);
			}
			break;
		    }
		    else
		    {
		        printf( "] %s", automatic_order[ funcNum ].fname );
			if( funcNum > 0 )
			{
			    int t = strlen( automatic_order[ funcNum ].fname );
			    int charNum;
			    for( charNum = 60 ; charNum > t ; charNum-- )
			       putchar(' ');
			}

		        fflush( stdout );
		    }
		    funcNum++;
		}


		if( ! automode ) 
		    printf( "Applying rule to data..." );

		listOfDicDataBlocks->Reset();
		while(!listOfDicDataBlocks->AtEnd())
		  {
		    BlockNode* currDicDataBlock = listOfDicDataBlocks->Current();

		    // Find the saveframe that this rule refers to:
		    List<ASTnode*>  *matchesDicSF = currDicDataBlock->searchForTypeByTag(
			    ASTnode::SAVEFRAMENODE, ruleName );
		    if( matchesDicSF->size() != 1 )
		    {
			(*errStream) <<
			    "#main# Error: Found " << matchesDicSF->size() <<
			    "saveframes in map file with name: " <<
			    ruleName << "." << endl;
		    }
		    else
		    {
 
			// Call whichever one was selected from the menu:
			(*ruleFunc)(AST,NMR,currDicDataBlock, (SaveFrameNode*)(*matchesDicSF)[0] );
 
		    }
		    delete matchesDicSF;
		    listOfDicDataBlocks->Next();
		  } 


                  ValCache::flushValCache();

		  delete AST;                     // Copy the transformed tree back into the

		  AST = new StarFileNode(true, *NMR ); // original tree for the next pass.

                  ValCache::flushValCache();


		if( ! automode ) 
		    printf( "done.\n" );

		// If in interactive mode, then read the next bunch of stuff
		// from the error file and dump it to the screen.  This will
		// be the lines from the error file that this transformation
		// just produced.
		// ---------------------------------------------------------
		if( !automode )
		{
		    int  ch;

		    while(  ( ch = fgetc( readerrFP ) ) != EOF )
		    {
			fputc( ch, stdout );
			retVal = 1;
		    }
		}
		       
		
	      }//end-while
	  }
	else
	  (*errStream) << "#main# Error: Parsing problems in source file"<<endl;

	if( ! automode )
	    fclose( readerrFP );
      }
    else
    {
      (*errStream) << "#main# Error: Parsing problems in dictionary"<<endl;
      return retVal;
    }

  delete errStream;  // closes file.
  delete os; // closes file.
  fclose( dictFile );

  return retVal;
}	//END OF MAIN


//--------------
//  getOrder
//--------------
void getOrder( vector<transformRuleEntry_t> &rules, StarFileNode *mapStar )
{
    List<ASTnode*>   *matches1, *matches2;
    DataLoopNode     *oopLoop;  // Order Of Operations loop
    LoopNameListNode *oopNames;
    LoopTableNode    *oopTable;
    int              seq_idx, rule_idx;
    transformRuleEntry_t thisRule;

    // start with the terminator rule, in case this errors out:
    thisRule.order = 9999999;
    thisRule.fname[0] = '\0';
    thisRule.fptr = NULL;
    rules.insert( rules.end(), thisRule );

    // Search for 1 saveframe with name: "save_order_of_operations"
    matches1 = mapStar->searchForTypeByTag(
	    ASTnode::SAVEFRAMENODE, string("save_order_of_operations") );
    if( matches1->size() != 1 )
    {   (*errStream)<<
	    "#main# Error: expected 1 save_order_of_operations.  Found "
	    << matches1->size() << " instead." << endl;
	delete matches1;
	return;
    }

    // Search within that saveframe for 1 loop with tag: "_rule_framecode":
    matches2 = (*matches1)[0]->searchForTypeByTag(
	    ASTnode::DATALOOPNODE, string("_rule_framecode") );
    if( matches2->size() != 1 )
    {   (*errStream)<<
	    "#main# Error: expected 1 loop containing _rule_framecode.  Found "
	    << matches2->size() << " instead." << endl;
	delete matches1;
	delete matches2;
	return;
    }

    // Get the loop we want to be dealing with:
    oopLoop = (DataLoopNode*)( (*matches2)[0] );

    delete matches1;
    delete matches2;

    // find out the index of the two tags we are interested in:
    oopNames = (oopLoop->getNames())[0]; // oopNames = outermost loop tagnames
    seq_idx = rule_idx = -1;
    for( int i = 0 ; i < oopNames->size() ; i++ )
    {
	if( (*oopNames)[i]->myName() == string("_order_sequence") )
	    seq_idx = i;
	else if( (*oopNames)[i]->myName() == string("_rule_framecode") )
	    rule_idx = i;
    }
    if( seq_idx == -1 )
    {   (*errStream) <<
	    "#main# Error: no _order_sequence tag in the loop in save_order_of_operations" <<
	    endl;
	return;
    }
    if( rule_idx == -1 )
    {   (*errStream) <<
	    "#main# Error: no _rule_framecode tag in the loop in save_order_of_operations" <<
	    endl;
	return;
    }

    // Fill the vector with the values of the loop:
    oopTable = oopLoop->getValsPtr();
    for( int j = 0 ; j <oopTable->size() ; j++ )
    {
	thisRule.order = atoi( (*(*oopTable)[j])[seq_idx]->myValue().c_str() );
	sprintf( thisRule.fname, "save_%s", (*(*oopTable)[j])[rule_idx]->myValue().c_str(), 80 );
	thisRule.fname[255] = '\0'; // truncate for safety in case value is really big.
	thisRule.fptr = getFuncFromName(
		            getCategory( DIC, string(thisRule.fname) ) );
	// insert just before the end:
	rules.insert( rules.end()-1, thisRule );
    }

    // Sort the vector according to the sequence order:
    stable_sort( rules.begin(), rules.end(), ruleSeqComp );

}

// ------------
// ruleSeqComp
// ------------
bool ruleSeqComp( transformRuleEntry_t entryA, transformRuleEntry_t entryB )
{
    return (entryA.order < entryB.order);
}

// ------------
// getCategory
// ------------
//     Gets the saveframe category given a saveframe name and something
//     to search through.  stipulation: the something to search through
//     should have only one occurance of the saveframe name given, or
//     this function will error out.
string getCategory( ASTnode *searchThrough, string sfName )
{
    List<ASTnode*>  *matches, *catMatches;

    matches = searchThrough->searchForTypeByTag( ASTnode::SAVEFRAMENODE, sfName );

    if( matches->size() > 1 )
    {
	(*errStream) <<
	    "#main# Error: multiple saveframes found with name = "
	    << sfName << ".  Continuing anyway with the first one"
	    << " found." << endl;
    }
    if( matches->size() == 0 )
    {
	(*errStream) <<
	    "#main# Error: no saveframes were found with name = "
	    << sfName << "." << endl;
        delete matches;
	return string("zilch");
    }

    catMatches = (*matches)[0]->searchForTypeByTag(
	    ASTnode::DATAITEMNODE, "_Saveframe_category" );
    delete matches;

    if( catMatches->size() != 1 )
    {
	(*errStream) <<
	    "#main# Error: found " << catMatches->size() << " hits for " <<
	    "_Saveframe_category in " << sfName << " when expecting 1." << endl;
	delete catMatches;
	return string("zilch");
    }

    string retVal = (  (DataItemNode*)(  (*catMatches)[0] )  )->myValue();
    delete catMatches;
    return retVal;
}

// ----------------
// getFuncFromName
// ----------------
//    Given the name of a function, get the function pointer
//    and return it.
transformFuncPtr_t  getFuncFromName( string funcName )
{
    if( funcName == string("remove_tag") )
    {   return save_remove_tag;   }
    if( funcName == string("tag_to_framecode") )
    {   return save_tag_to_framecode;   }
    if( funcName == string("renamed_tags") )
    {   return save_renamed_tags;   }
    if( funcName == string("tag_to_value") )
    {   return save_tag_to_value;   }
    if( funcName == string("tag_to_frame_loop_uniform") )
    {   return save_tag_to_frame_loop_uniform;   }
    if( funcName == string("tag_to_frame_loop_unique") )
    {   return save_tag_to_frame_loop_unique;   }
    if( funcName == string("tag_to_frame_free") )
    {   return save_tag_to_frame_free;   }
    if( funcName == string("keyed_renamed_tags") )
    {   return save_keyed_renamed_tags;   }
    if( funcName == string("keyed_renamed_tags_no_block") )
    {   return save_keyed_renamed_tags_no_block;   }
    if( funcName == string("make_new_saveframe") )
    {   return save_make_new_saveframe;   }
    if( funcName == string("keyed_tag_to_value") )
    {   return save_keyed_tag_to_value;   }
    if( funcName == string("free_all_category_tags") )
    {   return save_free_all_category_tags;   }
    if( funcName == string("make_citation_full_tag") )
    {   return save_make_citation_full_tag;   }
    if( funcName == string("make_new_tag") )
    {   return save_make_new_tag;   }
    if( funcName == string("explode_Mol_residue_sequence") )
    {   return save_explode_Mol_residue_sequence;   }
    if( funcName == string("insert_loop_defaults") )
    {   return save_insert_loop_defaults;   }
    if( funcName == string("remove_null_loop_rows") )
    {   return save_remove_null_loop_rows;   }
    if( funcName == string("remove_null_tags") )
    {   return save_remove_null_tags;   }
    if( funcName == string("remove_null_and_question_tags") )
    {   return save_remove_null_and_question_tags;   }
    if( funcName == string("order") )
    {   return save_order;   }
    if( funcName == string("order_and_loops") )
    {   return save_order_and_loops;   }
    if( funcName == string("loops") )
    {   return save_loops;   }
    if( funcName == string("loops_test") )
    {   return save_loops_test;   }
    if( funcName == string("remove_empty_frames") )
    {   return save_remove_empty_frames;   }
    if( funcName == string("remove_tags_not_in_saveframe") ) //June 7-26-99
    {   return save_remove_tags_not_in_saveframe;   }
    if( funcName == string("change_tag_value") )
    {   return save_change_tag_value;   }
    if( funcName == string("tag_to_loop") )
    {   return save_tag_to_loop;   }
    if( funcName == string("values_to_saveframes_many_to_many") )
      {   return save_values_to_saveframes_many_to_many;   }
    //modified by june, 7-30-99
    if( funcName == string("parse_split_name") )
    {  return save_parse_split_name;   }
    if( funcName == string("change_data_tag_name") )//Add by Zixia 5-24-99
    {  return save_change_data_tag_name;    }
    if( funcName == string("remove_global_block") )//Add by Zixia 5-26-99
    {  return save_remove_global_block;     }
    if( funcName == string("create_new_saveframe") )//Add by Zixia 5-26-99
    {  return save_create_saveframe;     }
    if( funcName == string("copy_tag_into_saveframe") )//Add by Zixia 5-26-99
    {  return save_copy_tag_into_saveframe;     }
    if( funcName == string("copy_saveframe") )//Add by Zixia 5-26-99
    {  return save_copy_saveframe;     }
    if( funcName == string("framecode_to_tag") )//Add by Zixia 5-26-99
    {  return save_framecode_to_tag;     }
    if( funcName == string("change_tag_value_II") )//Add by Zixia 6-1-99
    {  return save_change_tag_value_II;     }
    if( funcName == string("insert_column") )//Add by Zixia 6-1-99
    {  return save_insert_column;     }
    if( funcName == string("insert_ordinal") )//Add by June 7-19-99
    {  return save_insert_ordinal;     }
    if( funcName == string("insert_loop") )//Add by Zixia 6-3-99
    {  return save_insert_loop;     }
    if( funcName == string("rename_saveframe") )//Add by Zixia 6-7-99
    {  return save_rename_saveframe;     }
    if( funcName == string("check_chemical_shift_nomenclature") )//Add by Zixia 6-7-99
    {  return save_check_chemical_shift_nomenclature;     }
    if( funcName == string("calculate_residue_count") )//Add by Zixia 6-7-99
    {  return save_calculate_residue_count;     }
    if( funcName == string("change_database_name") )//Add by Zixia 6-8-99
    {  return save_change_database_name;     }
    if( funcName == string("convert_C_to_K") )//Add by Zixia 6-14-99
    {  return save_convert_C_to_K;     }
    if( funcName == string("assign_citation_type") )//Add by Zixia 6-15-99
    {  return save_assign_citation_type;     }
    if( funcName == string("make_new_tag_II") )//Add by Zixia 6-15-99
    {  return save_make_new_tag_II;     }
    if( funcName == string("insert_loop_defaults_II") )//Add by Zixia 6-17-99
    {  return save_insert_loop_defaults_II;     }
    if( funcName == string("create_enzyme_commission_number") )//Add by Zixia 6-18-99
    {  return save_create_enzyme_commission_number;     }
    if( funcName == string("copy_saveframe_content") )//Add by Zixia 6-26-99
    {  return save_copy_saveframe_content;     }
    if( funcName == string("parse_tag_to_new_tag") )//Add by June 7-29-99
    {  return save_parse_tag_to_new_tag;     }
    if( funcName == string("remove_saveframes") )//Add by madings 3-18-2000
    {  return save_remove_saveframes;     }
    if( funcName == string("rename_saveframes") )//Add by madings 3-18-2000
    {  return save_rename_saveframes;     }
    if( funcName == string("remove_2.1_tags") )//Add by madings 10-13-2002
    {  return save_remove_version_2_1_tags;     }
    if( funcName == string("remove_3.0_tags") )//Add by madings 10-13-2002
    {  return save_remove_version_3_0_tags;     }
    if( funcName == string("generic_value_mapper_A") )//Add by madings 10-13-2002
    {  return save_generic_value_mapper_A;     }
    if( funcName == string("invert_generic_value_mapper_A") )//Add by madings 11-27-2002
    {  return save_invert_generic_value_mapper_A;     }
    if( funcName == string("insert_nmrstr_3_IDs") )//Add by madings 10-19-2002
    {  return save_insert_nmrstr_3_IDs;     }
    if( funcName == string("insert_value_from_lookup_A") )//Add by madings 10-31-2002
    {  return save_insert_value_from_lookup_A;     }
    if( funcName == string("insert_value_from_lookup_B") )//Add by madings 10-31-2002
    {  return save_insert_value_from_lookup_B;     }
    if( funcName == string("remove_sf_if_missing_tag") )//Add by madings 8-12-2002
    {  return save_remove_sf_if_missing_tag;     }
    if( funcName == string("split_lines_to_loop") )//Add by madings 11-11-2003
    {  return save_split_lines_to_loop;  }
    if( funcName == string("explode_Mol_residue_seq_dict3_0") )
    {   return save_explode_Mol_residue_seq_dict3_0;   }
    if( funcName == string("make_framecodes_agree") )
    {   return save_make_framecodes_agree; }
    if( funcName == string("free_looped_tags") )
    {   return save_free_looped_tags; }
    if( funcName == string("make_submission_date_3") ||
        funcName == string("make_submission_date_3.0") )
    {   return save_make_submission_date_3; }
    if( funcName == string("delete_invalid_tag_syntax") )
    {   return save_delete_invalid_tag_syntax; }
    if( funcName == string("remove_duplicate_rows") )
    {   return save_remove_duplicate_rows; }
    if( funcName == string("free_looped_tags_by_value") )
    {   return save_free_looped_tags_by_value; }
    if( funcName == string("free_and_rename_looped_tags") )
    {   return save_free_and_rename_looped_tags; }
    if( funcName == string("default_if_not_exist") )
    {   return save_default_if_not_exist; }
    if( funcName == string("change_tag_delim") )
    {   return save_change_tag_delim; }
    if( funcName == string("remove_header_comments") )
    {   return save_remove_header_comments; }
    if( funcName == string("decouple_saveframes_3_to_2") )
    {   return save_decouple_saveframes_3_to_2; }
    if( funcName == string("merge_saveframes_2_to_3" ))
    {   return save_merge_saveframes_2_to_3; }
    if( funcName == string("rows_concatenated_to_free") )
    {   return save_rows_concatenated_to_free; }
    if( funcName == string("flat_file_name_if_not_exist") )
    {   return save_flat_file_name_if_not_exist; }
    if( funcName == string("collapse_3_to_2") )
    {   return save_collapse_3_to_2; }
    if( funcName == string("move_PDBX_poly_seq_to_assembly" ))
    {   return save_move_PDBX_poly_seq_to_assembly; }
    if( funcName == string("join_ambiguity_codes" ))
    {   return save_join_ambiguity_codes; }
    if( funcName == string("merge_loop_rows_to_text" ))
    {   return save_merge_loop_rows_to_text; }
    if( funcName == string("join_via_sprintf" ))
    {   return save_join_via_sprintf; }
    if( funcName == string("expand_2_to_3") )
    {   return save_expand_2_to_3; }
    if( funcName == string("split_ambiguity_codes" ))
    {   return save_split_ambiguity_codes; }
    if( funcName == string( "strip_chem_comp_from_ligand_sf_names" ))
    {   return save_strip_chem_comp_from_ligand_sf_names; }
    if( funcName == string( "generate_residue_mol_labels" ))
    {   return save_generate_residue_mol_labels; }
    if( funcName == string( "calculate_data_row_counts" ))
    {   return save_calculate_data_row_counts; }
    if( funcName == string( "insert_default_isotope_numbers" ))
    {   return save_insert_default_isotope_numbers; }
    if( funcName == string( "unparse_to_tagvalue" ))
    {   return save_unparse_to_tagvalue; }
    if( funcName == string( "swap_ref_tags_if_needed" ))
    {   return save_swap_ref_tags_if_needed; }
    
    
    // default:
    if( funcName != "zilch" )  // if it is zilch, suppress the error message.
    {   (*errStream) << 
	    "#main# Error: Rule name '"<<funcName<<"' is unknown to this program."
	    << endl;
    }
    return bogusRule;

}

void bogusRule( StarFileNode *, StarFileNode *, BlockNode *, SaveFrameNode * )
{
    return;  // do nothing.
}

// For the purpose of debugging strings, which don't
// print in a debugger unless you use the degbuggable
// C++ libs:
void string_print( string *printThis )
{
    (cerr) << "val is : \033[7m"<< *printThis << "\033[0m" << endl;
}


// To parse and handle error messages myself:
int yyparse_with_my_errs( char *fname, int warn_flg )
{
    int retVal;

    star_syntax_print = false;
    retVal = yyparse();

    // Print all the error messages found:
    for( int idx = 0 ; idx < starErrReport.size() ; idx++ )
    {
	// Print if this is not a warning.
	// If the warning flag is on, print anyway, warning or not.
	if( starErrReport[idx].errID < WARN_BASE_ID ||
		warn_flg )
	{
	    printf( "%s in %s [line %d:%d] %s (on token: %d)\n",
		    starErrReport[idx].errID >= WARN_BASE_ID ? "Warning" : "Error",
		    fname,
		    starErrReport[idx].lineNum,
		    starErrReport[idx].colNum,
		    starErrReport[idx].errStr,
		    starErrReport[idx].tokenStr );
	}
    }

    return retVal;
}


