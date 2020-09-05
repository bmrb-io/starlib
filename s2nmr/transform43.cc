///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
// $Log: not supported by cvs2svn $
// Revision 1.6  2008/04/16 21:29:17  madings
// Haven't committed in a while and this is a working version so I should.
//
// Revision 1.5  2006/08/24 21:25:06  madings
// *** empty log message ***
//
// Revision 1.4  2005/11/07 20:21:19  madings
// *** empty log message ***
//
// Revision 1.3  2005/03/23 22:36:48  madings
// *** empty log message ***
//
// Revision 1.2  2003/07/30 05:14:50  madings
// debugged a problem that made it necessary to add better output
// messages to transform43.cc, also updated the specs.html
// documentation to add rules 42 through 45.
//
// Revision 1.1  2003/07/24 18:49:13  madings
// Changes to handle 2.1-3.0 mappings.
//
// Revision 1.11  2001/06/18 16:57:09  madings
// Several fixes for s2nmr segfault bugs.
//
// Revision 1.10  2001/04/13 20:52:29  jurgen
// error messages corrected
//
// Revision 1.9  2001/02/13 23:00:43  jurgen
// Elaborated and corrected error messages
//
// Revision 1.8  2000/11/18 03:11:27  madings
// *** empty log message ***
//
// Revision 1.7  2000/11/18 03:02:37  madings
// *** empty log message ***

// Revision 1.6  2000/11/18  02:26:28  madings
// Fixed a problem that was common to several of the files (apparently
// all the ones edited by 'june', although it's hard to tell if this is
// her doing or not.)  The problem is that there were several places in
// the code where something like this had been done:
//     char *strPtr1, *strPtr2;
//     ...
//     /* strPtr points at some string right now */.
//     strPtr2 = new char [ strlen(strPtr1) ];  /* error: needs a +1 */
//     strcpy( strPtr2, strPtr1 );
// The problem with this, of course. is that strlen() gives the length
// of the string NOT including the null terminator, so the above code
// doesn't allocate enough space for strPtr2 to store the terminator.
// The result is that when the strcpy() is called, a zero-byte (null
// terminator) overwrites whatever was in the next byte just after strPtr2.
// Depending on what that next byte actually is, this might corrupt the
// value, or might not if it was supposed to be a zero anyway.  I suspect
// that this is what was causing some erratic segfaults we were seeing
// in this program - where seemingly irrelevant changes to the input data
// would 'fix' crashes.  (Even though the data changed had nothing to do with
// the part of the code that crashed.)  I suspect that the overwritten byte
// was part of the data the C-library heap uses internally to keep track of
// how much memory a variable on the heap is taking up (so it knows how much
// to free later when free() is called).  When 'free()' was being called, the
// wrong amount of memory was being freed, and thus the heap was a jumbled
// mess after that.)
//
// Revision 1.5  2000/03/23  01:26:32  madings
// Added transform30's ability to map nomenclature down to one-letter codes
// via a mapping loop in the map file.  Also fixed a bug in transform17, when
// the tag to get the saveframe's name from is null it didn't work before.
//
// Revision 1.4  2000/03/20  09:45:49  madings
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
// Revision 1.3  1999/07/30  18:22:06  june
// june, 7-30-99
// add transform1d.cc
//
// Revision 1.2  1999/04/20 21:41:43  madings
// Added transforms 17 and 18 to main.cc's case statement, and
// added transform17.cc and altered transform 2a and 1c so they
// can handle the case where the input is not coming from a saveframe,
// but is coming from outside a saveframe instead.

// Revision 1.1  1999/04/09  20:59:46  madings
// I forgot to add transform17.cc to the last checkin.
//
// Revision 1.1  1999/02/05  01:34:44  madings
// Many changes from debug sessions with Eldon:
// 1 - added new transforms:  transform15.cc and transform16.cc
// 2 - added 'don't-care' functionality to transform14.cc when
//     tag_value is a dot.
// 3 - Fixed some garbled errStream messages that printed the wrong
//     transform # to the log (transform11 printed error messages tagged
//     as transform6 for example.)
//
*/
#endif
///////////////////////////////////////////////////////////////

//This is for values_to_saveframe_many_to_many which handles the free key tag 
//case based on transform17.cc
//created 7/15/99 by zhijun liu
//revision 7/16/99, test output_sf_name_tag, ignore "*", "." and "?"
//revision 7/19/99, fix the bug so that the multiple saveframes generated have their own NameListNode of the loop not share one. Now parse_split_name and insert_ordinal could work properly on the multiple saveframe generated by it.
//revision 7/20/99, add case data_block. But still parse_split_name can not  work on the free tag in their multiple saveframes (core dumped).
//revision 7/30/99, fixed bugs in parse_split_name, no more above problem
//revision 7/30/99, new version take free_or_loop_flag into account

#include "transforms.h"


/*Included for sbrk to calculate memory used by the program
  (I was trying to debug a memory leak.)*/
#include <unistd.h>
char                      *debug_start_sbrk;
char                      *debug_tmpstart_sbrk;
char                      *debug_cur_sbrk;


#define GARBAGE_COLLECT_EVERY_N_ROWS   5000

static void do_one_iteration(
	    string    input_key_tag,
	    bool      null_input_key_ok,
	    string    input_sf_name,
	    string    input_sf_key_tag,
	    string    input_sf_key_value,
	    string    output_sf_key_tag_pos,
	    DataValueNode::ValType    output_sf_key_tag_type,
	    string    output_sf_key_value_pos,
	    string    output_sf_base_name,
	    string    output_sf_name_tag,
	    string    copy_or_move_flag,
	    string    output_sf_label_back_tag,
	    string    output_sf_label_fore_tag,
	    LoopTableNode   *mapInneLoop,
	    StarFileNode  *inStar,
	    StarFileNode  *outStar );

static void do_all_iterations(
	    int    input_key_tag_pos,
	    int    input_sf_name_pos,
	    int    input_sf_key_tag_pos,
	    int    input_sf_key_value_pos,
	    int    output_sf_key_tag_pos,
	    int    output_sf_key_value_pos,
	    int    output_sf_base_name_pos,
	    int    output_sf_name_tag_pos,
	    int    copy_or_move_flag_pos,
	    int    output_sf_label_back_tag,
	    int    output_sf_label_fore_tag,
	    LoopTableNode  *mapLoopTbl,
	    StarFileNode   *inStar,
	    StarFileNode   *outStar );

void save_generic_value_mapper_A(
	                 StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
			 SaveFrameNode *currDicRuleSF  )
{
    List<ASTnode*>        *mapLoopMatch;
    DataLoopNameListNode  *mapNames;
    LoopTableNode         *mapLoopTbl;
    int                   input_key_tag_pos = -1;
    int                   input_sf_name_pos = -1;
    int                   input_sf_key_tag_pos = -1;
    int                   input_sf_key_value_pos = -1;
    int                   output_sf_key_tag_pos = -1;
    int                   output_sf_key_value_pos = -1;
    int                   output_sf_base_name_pos = -1;
    int                   output_sf_name_tag_pos = -1;
    int                   copy_or_move_flag_pos = -1;
    int                   output_sf_label_back_tag = -1;
    int                   output_sf_label_fore_tag = -1;


    debug_start_sbrk = (char*)sbrk(0);

    // Find loop from the mapping file:
    // --------------------------------
    mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, string( "_input_key_tag" ) );
    if( mapLoopMatch->size() <= 0 )
    {
	(*errStream) << "#transform-43# ERROR: no loop tag called '_input_key_tag' in mapping file." << endl;
	delete mapLoopMatch;
	return; /* do nothing */
    }
    mapLoopTbl = ((DataLoopNode*)(*mapLoopMatch)[0])->getValsPtr();
    mapNames =  ((DataLoopNode*)(*mapLoopMatch)[0])->getNamesPtr();

    delete mapLoopMatch;
    for( int i = 0 ; i < (*mapNames)[0]->size() ; i++ )
    {
	if(      (* ((*mapNames)[0]) )[i]->myName()==string( "_input_key_tag" ) )
	    input_key_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_input_sf_name" ) )
	    input_sf_name_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_input_sf_key_tag" ) )
	    input_sf_key_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_input_sf_key_value" ) )
	    input_sf_key_value_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_output_sf_key_tag" ) )
	    output_sf_key_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_output_sf_key_value" ) )
	    output_sf_key_value_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_output_sf_base_name" ) )
	    output_sf_base_name_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_output_sf_name_tag" ) )
	    output_sf_name_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_copy_or_move_flag" ) )
	    copy_or_move_flag_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_output_sf_label_back_tag" ) )
	    output_sf_label_back_tag = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_output_sf_label_fore_tag" ) )
	    output_sf_label_fore_tag = i;
    }
    if( input_key_tag_pos < 0  )
    {   (*errStream) << "#transform-43# ERROR: input mapping file is missing tag '_input_key_tag'." << endl;
	return; /* do nothing */
    }
    if( input_sf_name_pos < 0  )
    {   (*errStream) << "#transform-43# ERROR: input mapping file is missing tag '_input_sf_name'." << endl;
	return; /* do nothing */
    }
    if( input_sf_key_tag_pos < 0  )
    {   (*errStream) << "#transform-43# ERROR: input mapping file is missing tag 'input_sf_key_tag'." << endl;
	return; /* do nothing */
    }
    if( input_sf_key_value_pos < 0  )
    {   (*errStream) << "#transform-43# ERROR: input mapping file is missing tag '_input_sf_key_value'." << endl;
	return; /* do nothing */
    }
    if( output_sf_key_tag_pos < 0  )
    {   (*errStream) << "#transform-43# ERROR: input mapping file is missing tag '_output_sf_category'." << endl;
	return; /* do nothing */
    }
    if( output_sf_base_name_pos < 0  )
    {   (*errStream) << "#transform-43# ERROR: input mapping file is missing tag '_output_sf_base_name'." << endl;
	return; /* do nothing */
    }
    if( output_sf_name_tag_pos < 0  )
    {   (*errStream) << "#transform-43# ERROR: input mapping file is missing tag '_output_sf_name_tag'." << endl;
	return; /* do nothing */
    }
    if( copy_or_move_flag_pos < 0  )
    {   (*errStream) << "#transform-43# ERROR: input mapping file is missing tag 'copy_or_move_flag'." << endl;
	return; /* do nothing */
    }

    do_all_iterations(
	    input_key_tag_pos,
	    input_sf_name_pos,
	    input_sf_key_tag_pos,
	    input_sf_key_value_pos,
	    output_sf_key_tag_pos,
	    output_sf_key_value_pos,
	    output_sf_base_name_pos,
	    output_sf_name_tag_pos,
	    copy_or_move_flag_pos,
	    output_sf_label_back_tag,
	    output_sf_label_fore_tag,
	    mapLoopTbl, inStar, outStar );

    //debug_cur_sbrk = (char*) sbrk(0);
    //cerr<<"AT END OF TRANSFORM 43 AFTER CLEANUP, BUT BEOORE RETURN:"<<endl;
    //cerr<<"data segment growth since start of transform43 = "<< (debug_cur_sbrk - debug_start_sbrk) <<endl;
    //cerr<<endl;

    return;
}

// Loop over all the iterations of the mapping file, doing all
// the rules found there:
static void do_all_iterations(
	    int    input_key_tag_pos,
	    int    input_sf_name_pos,
	    int    input_sf_key_tag_pos,
	    int    input_sf_key_value_pos,
	    int    output_sf_key_tag_pos,
	    int    output_sf_key_value_pos,
	    int    output_sf_base_name_pos,
	    int    output_sf_name_tag_pos,
	    int    copy_or_move_flag_pos,
	    int    output_sf_label_back_tag,
	    int    output_sf_label_fore_tag,
	    LoopTableNode *mapLoopTbl,
	    StarFileNode  *inStar,
	    StarFileNode  *outStar )
{
    int i;


    for( i = 0 ; i < mapLoopTbl->size() ; i++ )
    {
	bool null_input_key_ok = false;
        string input_key_tag = (*(*mapLoopTbl)[i])[input_key_tag_pos]->myValue();
	int spacePos =  input_key_tag.find( ' ' );
	if( spacePos != string::npos ) {
	    if( input_key_tag.substr(spacePos+1) == "null-ok" ) {
	       null_input_key_ok = true;
	    }
	    input_key_tag.erase( spacePos );
	}
	do_one_iteration( 
	    input_key_tag,
	    null_input_key_ok,
	    (*(*mapLoopTbl)[i])[input_sf_name_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[input_sf_key_tag_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[input_sf_key_value_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[output_sf_key_tag_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[output_sf_key_value_pos]->myDelimType(),
	    (*(*mapLoopTbl)[i])[output_sf_key_value_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[output_sf_base_name_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[output_sf_name_tag_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[copy_or_move_flag_pos]->myValue(),
	    ( output_sf_label_back_tag >= 0 ) ?
		(*(*mapLoopTbl)[i])[output_sf_label_back_tag]->myValue() :
		string(""),
	    ( output_sf_label_fore_tag >= 0 ) ?
		(*(*mapLoopTbl)[i])[output_sf_label_fore_tag]->myValue() :
		string(""),
	    (*mapLoopTbl)[i]->myLoop(), inStar, outStar );
    }
}

// Do one iteration of the mapping file
static void do_one_iteration(
	    string    input_key_tag,
	    bool      null_input_key_ok,
	    string    input_sf_name,
	    string    input_sf_key_tag,
	    string    input_sf_key_value,
	    string    output_sf_key_tag,
	    DataValueNode::ValType output_sf_key_tag_type,
	    string    output_sf_key_value,
	    string    output_sf_base_name,
	    string    output_sf_name_tag,
	    string    copy_or_move_flag,
	    string    output_sf_label_back_tag,
	    string    output_sf_label_fore_tag,
	    LoopTableNode   *mapInnerLoop,
	    StarFileNode  *inStar,
	    StarFileNode  *outStar )
{
    int                       i;
    int                       j;
    int                       k;
    int                       m;
    int                       n;
    int                       sf_name_tag_pos;
    int                       sf_label_fore_pos;
    bool                      freeFlag; //to tell if input_key_tag not in loop
    int                       input_key_tag_col = -1;
    bool                      skipInsert;
    List<ASTnode*>            *checkMatches;
    List<ASTnode*>            *checkMatchesFull;
    ASTnode                   *matching;
    string                    tmpVal;
    DataValueNode             *tmpDVN;



    // Build the list of all the places that match the input criteria:
    // ---------------------------------------------------------------
    List<ASTnode*> *input_matches = inStar->searchForTypeByTag(
	    ASTnode::DATALOOPNODE, input_key_tag );
    if(input_matches->size()>0)
      freeFlag = false;
    else
    {
	freeFlag = true;
	input_matches = inStar->searchForTypeByTag(
				        ASTnode::DATAITEMNODE, input_key_tag );
    }
    
    //check input_matches
    if( (input_matches->size())<=0)
    { /* JFD elaboration of error message */
	(*errStream)<< "#transform-43# ERROR: can't find input_key_tag item ("
                    <<input_key_tag
                    <<" in input file."
                    <<endl;
	return;
    }


    SaveFrameNode  *sf;
    DataBlockNode* db;
    bool isInSF = false;
    // Second, eliminate those matches that don't fit the criteria:
    for( i = 0 ; i < input_matches->size() ; i++ )
      {
	// Find the saveframe or datablock this is inside:
	ASTnode *par = (*input_matches)[i];
	while( par != NULL && ! par->isOfType( ASTnode::SAVEFRAMENODE )
	       && ! par->isOfType( ASTnode::DATABLOCKNODE ) )
        {
	  par = par->myParent();
        }
	if(par->isOfType( ASTnode::SAVEFRAMENODE ) )
	  {
	    isInSF = true;
	    sf = (SaveFrameNode*) par;
	  }
	else if(par->isOfType( ASTnode::DATABLOCKNODE ) )
	  {
	    db = (DataBlockNode*) par;
            (cerr)<<"transform43:    "<<input_key_tag<<"  was found in a datablock instead of a saveframe.  That confuses this software."<<endl;
	    cerr.flush();
	  }
	else
	  {
	    (*errStream) << "#transform-43# ERROR: can't find the input_key_tag item in input file"<<endl;
	    return;
	  }

	// Check: Name match or don't care:
	if(     input_sf_name == string("*") ||
		input_sf_name == string(".") ||
		sf->myName() == input_sf_name  )
	  {
	    // Good, keep it.
	  }
	// Check: tag/value pair:
	if( input_sf_key_tag == string ("*") ||
	    input_sf_key_tag == string (".")  )
	  {
	    // Good, keep it.
	  }
	else
	  {
	    List<ASTnode*> *tagval_look = sf->searchByTagValue(
							       input_sf_key_tag, input_sf_key_value );
	    if( tagval_look->size() > 0 )
	      {
		// Good, keep it.
	      }
	    else
	      {
		// It's not a good enough match, remove it:
		input_matches->erase( input_matches->begin()+i );
		i-- ; // compensate for the auto-increment the forloop
		// will do.
	      }
	    delete tagval_look;
	  }
      }
    //check whether inner loop empty
    if( mapInnerLoop == NULL || mapInnerLoop->size() == 0 )
      {    (*errStream) << "#transform-43# ERROR: input mapping file is missing the inner loop for _input_tag." << endl;
      return;
      }

    if(!freeFlag)    //input_key_tag in loop
      {
	// For each input loop (ignores nesting)
	for( i = 0 ; i < input_matches->size() ; i++ )
	  {
	    DataLoopNode  *inputLoop = (DataLoopNode*) ( (*input_matches)[i] );
	    ASTnode *par;
	    for( par = inputLoop ; par != NULL && ! par->isOfType(ASTnode::SAVEFRAMENODE) ; par = par->myParent() )
	    {   /*do nothing*/  }
	    SaveFrameNode *thisInputSf = (SaveFrameNode*)par;

	    
	    // For each name mentioned in the inner loop of the map file,
	    // add it into a name list for a new loop:
	    LoopNameListNode  *newNames_loop = new LoopNameListNode();
	    LoopNameListNode  *newNames_free = new LoopNameListNode();
	    vector<short>  *delimFilters =
	         new vector<short>;
	    vector<string>  *checkTags = new vector<string>;
	    vector<string>  *checkVals = new vector<string>;
	    LoopNameListNode  *reNames_loop = new LoopNameListNode();
	    LoopNameListNode  *reNames_free = new LoopNameListNode();
	    sf_name_tag_pos = -1;
	    sf_label_fore_pos = -1;
	    string oldName;
	    string newName;
	    string delimiter;
	    short delimFilter = 0;
	    char  *charPos1, *charPos2;
	    char delimField[2048];
	    string checkTag, checkVal;
	    for( j = 0 ; j < mapInnerLoop->size() ; j++ )
	      {
		int newNameLine = 0;
		oldName = (*(*mapInnerLoop)[j])[0]->myValue();
		if( (*mapInnerLoop)[j]->size() >= 4 ) {
		    newName = (*(*mapInnerLoop)[j])[3]->myValue();
		    newNameLine = (*(*mapInnerLoop)[j])[3]->getLineNum();
		}
		else {
		    newName = (*(*mapInnerLoop)[j])[0]->myValue();
		    newNameLine = (*(*mapInnerLoop)[j])[0]->getLineNum();
		}


		delimiter = (*(*mapInnerLoop)[j])[1]->myValue();
		if( strncmp( delimiter.c_str(), "FRAMECODE",9 ) == 0 )
		    delimFilter = (short) DataValueNode::FRAMECODE ;
		else if( delimiter == string("SINGLE") )
		    delimFilter = (short) DataValueNode::SINGLE ;
		else if( delimiter == string("DOUBLE") )
		    delimFilter = (short) DataValueNode::DOUBLE ;
		else if( delimiter == string("SEMICOLON") )
		    delimFilter = (short) DataValueNode::SEMICOLON ;
		else if( delimiter == string("NON") )
		    delimFilter = (short) DataValueNode::NON ;
		else if( strncmp( delimiter.c_str(), "!FRAMECODE", 10 ) == 0 )
		    delimFilter = - (short) DataValueNode::FRAMECODE ;
		else if( delimiter == string("!SINGLE") )
		    delimFilter = - (short) DataValueNode::SINGLE ;
		else if( delimiter == string("!DOUBLE") )
		    delimFilter = - (short) DataValueNode::DOUBLE ;
		else if( delimiter == string("!SEMICOLON") )
		    delimFilter = - (short) DataValueNode::SEMICOLON ;
		else if( delimiter == string("!NON") )
		    delimFilter = - (short) DataValueNode::NON ;
		else
		    delimFilter = (short) 0;

		if(   ( delimFilter == (short) DataValueNode::NON )
		         && 
		      strncmp( newName.c_str(), "_", 1 ) != 0   ) {
		   (*errStream) << "transform43.c# Warning: the output tag name "<<
		       newName << " on line " << newNameLine << " of the mapper file " <<
		       "is not a valid tag name, and therefore this mapping will be commented" <<
		       "out."<<endl;
		   newName = string( "## Invalid tag name in map file: " ) + newName;
		}

		
		if((*(*mapInnerLoop)[j])[2]->myValue()==string("L") )  
		{
		     newNames_loop->insert(
				      newNames_loop->end(),
				      new DataNameNode( oldName )
				      );
		     reNames_loop->insert( reNames_loop->end(), newName );
		     delimFilters->insert( delimFilters->end(), delimFilter );
		     checkTag = string("");
		     checkVal = string("");
		     strcpy(delimField, delimiter.c_str() );
		     charPos1 = strchr( delimField, ':' );
		     if( charPos1 != NULL )
		     {  charPos2 = strchr( charPos1+1, ':' );
		        if( charPos2 != NULL )
			{
			   *charPos2 = '\0';
			   checkTag = string( charPos1+1 );
			   checkVal = string( charPos2+1 );
			}
		     }
		     checkTags->insert( checkTags->end(), checkTag );
		     checkVals->insert( checkVals->end(), checkVal );
		}
		if((*(*mapInnerLoop)[j])[2]->myValue()==string("F") )
		{
		     newNames_free->insert(
				      newNames_free->end(),
				      new DataNameNode( oldName )
				      ); 
		     reNames_free->insert( reNames_free->end(), newName );
		     delimFilters->insert( delimFilters->end(), delimFilter );
		     checkTag = string("");
		     checkVal = string("");
		     strcpy(delimField, delimiter.c_str() );
		     charPos1 = strchr( delimField, ':' );
		     if( charPos1 != NULL )
		     {  charPos2 = strchr( charPos1+1, ':' );
		        if( charPos2 != NULL )
			{
			   *charPos2 = '\0';
			   checkTag = string( charPos1+1 );
			   checkVal = string( charPos2+1 );
			}
		     }
		     checkTags->insert( checkTags->end(), checkTag );
		     checkVals->insert( checkVals->end(), checkVal );
		}

	      } 
	
	    // Find out where those names are in the original loop:
	    List<int>         *namePositions_loop = new List<int>();
	    List<int>         *namePositions_free = new List<int>();
	    bool  gotInserted;
	    for( j = 0 ; j < newNames_loop->size() ; j++ )
	      {
		gotInserted = false;
		if( strncmp( (*(*mapInnerLoop)[j])[0]->myValue().c_str(), "FRAMECODE", 9) == 0 )
		{   
		    namePositions_loop->insert( namePositions_loop->end(), -999 );
		    gotInserted = true;
		}
		else if( (*(*mapInnerLoop)[j])[0]->myValue().c_str()[0] != '_' )
		{   
		    namePositions_loop->insert( namePositions_loop->end(), -1000 -j );
		    gotInserted = true;
		}
		else
		{
		    for( k = 0 ; k < inputLoop->getNames()[0]->size() ; k++ )
		      {

			if(     (* (inputLoop->getNames()[0]) )[k]->myName() ==
				input_key_tag  )
			  {
			    input_key_tag_col = k;
			  }
			if( (*newNames_loop)[j]->myName() ==
			       (* (inputLoop->getNames()[0]) )[k]->myName() )
			{
			    namePositions_loop->insert( namePositions_loop->end(), k );
			    gotInserted = true;
			    break;
			}
			if(     (* (inputLoop->getNames()[0]) )[k]->myName() ==
				output_sf_name_tag  )
			  {
			    sf_name_tag_pos = k;
			  }
			if(     (* (inputLoop->getNames()[0]) )[k]->myName() ==
				output_sf_label_fore_tag  )
			  {
			    sf_label_fore_pos = k;
			  }
		      }
		}
		if( ! gotInserted ) 
		{
		    namePositions_loop->insert( namePositions_loop->end(), -998 );

		    (*errStream) << "#transform43# warning: (A) tag name " << 
			(*newNames_loop)[j]->myName() << " not found in the " <<
			"same loop as input_key_tag: " << input_key_tag << endl;
		}
	      }

			
	    for( j = 0 ; j < newNames_free->size() ; j++ )
	      {
		gotInserted = false;
		if( strncmp( (*(*mapInnerLoop)[j])[0]->myValue().c_str(), "FRAMECODE", 9 ) == 0 )
		{
		    namePositions_free->insert( namePositions_free->end(), -999 );
		    gotInserted = true;
		}
		else if( (*(*mapInnerLoop)[j])[0]->myValue().c_str()[0] != '_' )
		{   
		    namePositions_loop->insert( namePositions_loop->end(), -1000 -j );
		    gotInserted = true;
		}
		else
		{
		    for( k = 0 ; k < inputLoop->getNames()[0]->size() ; k++ )
		      {
			if(     (* (inputLoop->getNames()[0]) )[k]->myName() ==
				input_key_tag  )
			{
			    input_key_tag_col = k;
			}
			if( (*newNames_free)[j]->myName() ==
			       (* (inputLoop->getNames()[0]) )[k]->myName() )
			{
			    namePositions_free->insert( namePositions_free->end(), k );
			    gotInserted = true; 
			}
			else
			{
			    namePositions_free->insert( namePositions_free->end(), -998 );
			}
			
			if(     (* (inputLoop->getNames()[0]) )[k]->myName() ==
				output_sf_name_tag  )
			  {
			    sf_name_tag_pos = k;
			  }
			if(     (* (inputLoop->getNames()[0]) )[k]->myName() ==
				output_sf_label_fore_tag  )
			  {
			    sf_label_fore_pos = k;
			  }
		      }
		    if( ! gotInserted )
		    {
			(*errStream) << "#transform43# warning: (B) tag name " << 
			    (*newNames_loop)[j]->myName() << " not found in the " <<
			    "same loop as input_key_tag: " << input_key_tag << endl;
		    }
		}
	      }

 
	    // For each row of the input loop:
	    LoopTableNode *inputVals = inputLoop->getValsPtr();
	    for( j = 0 ; j < inputVals->size() ; j++ )
	      {
		string out_sf_name_tag;
		//check output_sf_name_tag value
		if(output_sf_name_tag == string("FRAMECODE") )
	        {
		    out_sf_name_tag = thisInputSf->myName();
		    // skip the beginning "save_" part:
		    if( strncmp( out_sf_name_tag.c_str(), "save_", 5 ) == 0 )
		    {   out_sf_name_tag = string(out_sf_name_tag.c_str() + 5 );
		    }
		}
		else 
		{
		    if( sf_name_tag_pos == -1 )
			out_sf_name_tag = "";
		    else
			out_sf_name_tag = (*(*inputVals)[j])[sf_name_tag_pos]->myValue();
		    if(out_sf_name_tag == string("*") ||
		       out_sf_name_tag == string(".") ||
		       out_sf_name_tag == string("?"))
		      {
			(*errStream) << "#transform-43# WARNING:invalid value '"<<out_sf_name_tag<<
				"' for naming a new saveframe.  Taken from a value to tag "<<
				output_sf_name_tag<< "." << endl;
			break;
		      }
		  }

		// Check if the input key is null, and if so, skip this row:
		// Except, if the "null ok" field is turned on, then allow it even if it's null.
		if(  input_key_tag_col >= 0  &&
		     (!null_input_key_ok)  &&
		     (  (*(*inputVals)[j])[input_key_tag_col]->myValue() == string("?") ||
		        (*(*inputVals)[j])[input_key_tag_col]->myValue() == string(".") )  )
		{
		   continue;
		}

		// Build a new row consisting of the values
		// we need copied out of the input loop's row:
		LoopRowNode  *newRow = new LoopRowNode(
			 inputLoop->getTabFlag() ? "tabulate" : "linear" );
		for( k = 0 ; k < namePositions_loop->size() ; k++ )
		  {
		    if( (*namePositions_loop)[k] >= 0 ) 
		    {
			short filterDelim = 
			   (short) ( (*delimFilters)[ k ] );
			DataValueNode  *insertVal = 
			   (  (*(*inputVals)[j])[ (*namePositions_loop)[k] ] );
			short valueDelim = 
			   (short) ( insertVal->myDelimType() );

			DataValueNode *tmpInsertVal;
			if(   ( filterDelim  > 0 && valueDelim != filterDelim  ) ||
			      ( filterDelim  < 0 && valueDelim == -(filterDelim)  )  )
			{  tmpInsertVal = new DataValueNode( ".", DataValueNode::NON );
			   insertVal = tmpInsertVal;
			}
			else
			   tmpInsertVal = NULL;

			skipInsert = false;
			if( filterDelim == DataValueNode::FRAMECODE && insertVal != NULL )
			{
			   skipInsert = true;
			   string saveName = string( "save_") + insertVal->myValue();
			   checkMatches = 
			      inStar->searchForTypeByTag( 
			                 ASTnode::SAVEFRAMENODE,
					 saveName );
			   if( checkMatches->size() > 0 )
			   {
			       matching = (*checkMatches)[0];
			       if( (*checkTags)[k].length() > 0  )
			       {
				   if( checkMatches != NULL )
				   {  delete checkMatches;
				      checkMatches = NULL;
				   }
				   
				   m = 0;
				   while( m != string::npos && m < (*checkVals)[k].length() )
				   {
				       n = (*checkVals)[k].find( ':', m );
				       if( n == string::npos )
				           n = (*checkVals)[k].length();
				       tmpVal = string( (*checkVals)[k], m, (n-m) );
				       checkMatches = matching->searchByTagValue( (*checkTags)[k], tmpVal );
				       if( checkMatches->size() > 0 )
				       {
				           break;
				       }
				       else
				       {
				           delete checkMatches;
					   checkMatches = NULL;
				       }
				       m = n+1;
				   }
				   if( checkMatches != NULL && checkMatches->size() > 0 )
				   {
				      skipInsert = false;
				   }
				   if( checkMatches != NULL)
				   { delete checkMatches;
				     checkMatches = NULL;
				   }
			       }
			       else // no colons in the delim string, no special checks to worry about:
			            // just unconditionally work as long as there's a saveframe there.
			       {
			           skipInsert = false;
			       }
			   }
			   if( checkMatches != NULL)
			   { delete checkMatches;
			     checkMatches = NULL;
			   }
			}

			if( skipInsert )
			{  tmpInsertVal = new DataValueNode( ".", DataValueNode::NON );
			   insertVal = tmpInsertVal;
			}
			newRow->insert(
			  newRow->end(),  *insertVal  );

			if(  tmpInsertVal != NULL )
			{   delete tmpInsertVal;
			}
		    }
		    else if( (*namePositions_loop)[k] == -998 ) 
		    {
			tmpDVN = new DataValueNode( string("?"), DataValueNode::NON );
			newRow->insert( newRow->end(), *tmpDVN);
			delete tmpDVN;
		    }
		    else if ( (*namePositions_loop)[k] == -999 ) 
		    {
			ASTnode *parent;
			for( parent = inputVals ; 
			     parent != NULL && ! parent->isOfType(ASTnode::SAVEFRAMENODE) ;
			     parent = parent->myParent() )
			 { /*null body*/ }
			string inputFrameCode = ((SaveFrameNode*)parent)->myName();
			if( strncmp( inputFrameCode.c_str(), "save_", 5 ) == 0 )
			    inputFrameCode = string( inputFrameCode.c_str() + 5 );
			char neededQ = FindNeededQuoteStyle( inputFrameCode );
			tmpDVN = new DataValueNode( inputFrameCode,
				     neededQ == ';' ? DataValueNode::SEMICOLON :
				       neededQ == '\'' ? DataValueNode::SINGLE :
					 neededQ == '\"' ? DataValueNode::DOUBLE  :
					   DataValueNode::NON  );
			newRow->insert( newRow->end(), *tmpDVN );
			delete tmpDVN;
		    }
		    else if( (*namePositions_loop)[k] < 0 ) 
		    {
			tmpDVN = 
			  new DataValueNode( (*(*mapInnerLoop)[ -1000 - (*namePositions_loop)[k] ])[0]->myValue(),
			                     (*(*mapInnerLoop)[ -1000 - (*namePositions_loop)[k] ])[0]->myDelimType() );
			newRow->insert( newRow->end(), *tmpDVN );
			delete tmpDVN;
		    }
		  }
		 

		// Figure out which saveframe it should go into:
		string sf_name;
		List<ASTnode*> *sfSearch = NULL;
		if( output_sf_base_name == string(".") )
		{
		    sfSearch = outStar->searchForTypeByTagValue(
		               ASTnode::SAVEFRAMENODE, output_sf_key_tag,
			       output_sf_key_value   );
		}
		else
		{
		    sf_name = output_sf_base_name ;
		    if( out_sf_name_tag.length() > 0 )
			sf_name += string("_");
		    sf_name += out_sf_name_tag;
		    if( strncmp( sf_name.c_str(), "save_", 5 ) != 0 )
			sf_name = string("save") + 
			          (sf_name[0] == '_' ? "" : "_" ) + sf_name ;
		    SearchAndReplace( sf_name, string(" "), string("_" ) );
		    SearchAndReplace( sf_name, string("\t"), string("_" ) );
		    sfSearch = 
			outStar->searchForTypeByTag( ASTnode::SAVEFRAMENODE, sf_name );
		}


	        // If the output_sf_label_back_tag is set, check that it matches
		// the framecode of the input saveframe, or it's not the
		// right match.  If none match, the source file is invalid.
		if( output_sf_label_back_tag != string(".") )
		{
		    int     matchIdx;
		    string  inputFrameCode = thisInputSf->myName();
		    if( strncmp( inputFrameCode.c_str(), "save_", 5 ) == 0 )
			inputFrameCode = string( inputFrameCode.c_str() + 5 );
		    for( matchIdx = 0 ; matchIdx < sfSearch->size(); matchIdx++ )
		    {   SaveFrameNode *sfn = (SaveFrameNode*) ( (*sfSearch)[matchIdx] );
		        List<ASTnode*> *labelSearch = sfn->searchByTagValue(
			    output_sf_label_back_tag, inputFrameCode );
			if( labelSearch->size() == 0 )
			{   sfSearch->erase( sfSearch->begin() + matchIdx );
			    matchIdx--; /* to compensate for the deletion */
			}
			delete labelSearch;
		    }
		}
	        // If the output_sf_label_fore_tag is set, check that it matches
		// the framecode of the output saveframe, or it's not the
		// right match.  If none match, the source file is invalid.
		if( output_sf_label_back_tag != string(".") )
		{
		    int     matchIdx;
		    string  inputLabelVal = (*(*inputVals)[j])[sf_label_fore_pos]->myValue();
		    if( inputLabelVal.c_str()[0] == '$' )
		        inputLabelVal = string(inputLabelVal.c_str()+1);
                    if( strncmp( inputLabelVal.c_str(), "save_", 5) == 0 )
		        inputLabelVal = string(inputLabelVal.c_str()+5);
		    for( matchIdx = 0 ; matchIdx < sfSearch->size(); matchIdx++ )
		    {   SaveFrameNode *sfn = (SaveFrameNode*) ( (*sfSearch)[matchIdx] );
			if( sfn->myName() != inputLabelVal )
			{   sfSearch->erase( sfSearch->begin() + matchIdx );
			    matchIdx--; /* to compensate for the deletion */
			}
		    }
		}
	        // If the output_sf_key_tag is set, check that the tag/val
		// pair matches the found saveframe - if not then delete
		// it from consideration:
		if( output_sf_key_tag != string(".") )
		{
		    int     matchIdx;
		    for( matchIdx = 0 ; matchIdx < sfSearch->size(); matchIdx++ )
		    {   SaveFrameNode *sfn = (SaveFrameNode*) ( (*sfSearch)[matchIdx] );
		        List<ASTnode*> *labelSearch = sfn->searchByTagValue(
			    output_sf_key_tag, output_sf_key_value );
			if( labelSearch->size() == 0 )
			{   sfSearch->erase( sfSearch->begin() + matchIdx );
			    matchIdx--; /* to compensate for the deletion */
			}
			delete labelSearch;
		    }
		}


		// Create a new saveframe if need be, or use the one that
		// was found:
		SaveFrameNode *outputSF;
		if( sfSearch->size() > 0 )
		{
		  // If there's more than one choice, prefer the one that
		  // is identically named as the input loop.  otherwise use
		  // the first one:
		  int matchNumToUse;
		  for(    matchNumToUse = 0 ;
		          matchNumToUse < sfSearch->size() ;
		          matchNumToUse++ )
		  {   string outputCandidateName = 
		          ((SaveFrameNode*)(*sfSearch)[matchNumToUse])->myName();
		      string myOwnName = thisInputSf->myName();
		      if( outputCandidateName == myOwnName )
		          break;
		  }
		  if( matchNumToUse >= sfSearch->size() )
		      matchNumToUse = 0;
		  outputSF = (SaveFrameNode*) ( (*sfSearch)[matchNumToUse] );
		  // Check to see that it has the output key tag, if not
		  // create it:
		  List<ASTnode*> *catMatches = outputSF->searchByTag(
		                                output_sf_key_tag );
		  if( catMatches->size() <= 0 )
		  {   outputSF->GiveMyDataList()->insert(
				     outputSF->GiveMyDataList()->end(),
			new DataItemNode( output_sf_key_tag,
			      output_sf_key_value, output_sf_key_tag_type )
				     );
		  }
		  delete catMatches;
		}
		else
		  {
		    if( sf_name == string("save_NEW_SAME") )
		    {   sf_name = thisInputSf->myName();
		    }
		    outputSF = new SaveFrameNode( sf_name );
		    if( output_sf_key_tag != string("*") &&
		        output_sf_key_tag != string(".") )
		    {
			outputSF->GiveMyDataList()->insert(
				     outputSF->GiveMyDataList()->end(),
			new DataItemNode( output_sf_key_tag,
			      output_sf_key_value, output_sf_key_tag_type )
				     );
		    }
		    BlockNode *lastBlock;
		    if( outStar->GiveMyDataBlockList()->size() == 0 )
		      {
			(*errStream) <<
			  "#transform-43# ERROR: File has no data_ or global_ sections." << endl ;
			break;
		      }
		    else
		      {
			lastBlock =
		   (* (outStar->GiveMyDataBlockList() ) )
			  [outStar->GiveMyDataBlockList()->size() - 1 ];
		      }
		    lastBlock->GiveMyDataList()->insert(
				   lastBlock->GiveMyDataList()->end(),
				    outputSF
				   );
		  }

		// [ no leak

		// Get the loop in the saveframe where the new row will be
		// inserted.  If no such loop exists yet, start a new one:
		if(reNames_loop != NULL && reNames_loop->size()>0)
		  {
		    DataLoopNode *outputLoop;
		    List<ASTnode*> *tagSearch = outputSF
		      ->searchForTypeByTag(
		    ASTnode::DATALOOPNODE, (*reNames_loop)[0]->myName() );
		    
		    if( tagSearch->size() > 0 )
		      {
		        outputLoop = (DataLoopNode*) ( (*tagSearch)[0] );
		      }
		    else
		      {
			outputLoop = new DataLoopNode(
			inputLoop->getTabFlag() ? "tabulate" : "linear" );
			outputLoop->getNamesPtr()->insert(
			    outputLoop->getNamesPtr()->end(),
			    new LoopNameListNode( *reNames_loop ) );
			outputSF->GiveMyDataList()->insert(
					 outputSF->GiveMyDataList()->end(),
					 outputLoop );
		      }
		    // Copy the new row into the loop:
		    outputLoop->getValsPtr()->insert(
					   outputLoop->getValsPtr()->end(),
					   newRow );

		    delete tagSearch;
		  }

		// ] no leak

		delete sfSearch;

		//copy free tags into the output saveframe
		 
		for( k = 0 ; k < namePositions_free->size() ; k++ )
		  { 
		    // erase the below stuff: testing to see what is setting us up the bomb:
		    /*    new DataItemNode(  
			     (*reNames_free)[k]->myName(),
			     (*(*inputVals)[j])[ (*namePositions_free)[k] ]->myValue(),
			     (*(*inputVals)[j])[ (*namePositions_free)[k] ]->myDelimType() );  */
		    if( (*namePositions_free)[k] >= 0 ) 
		    {
			short filterDelim = 
			   (short) ( (*delimFilters)[ k ] );
			DataValueNode  *insertVal = 
			   (*(*inputVals)[j])[ (*namePositions_free)[k] ];
			short valueDelim = 
			   (short) ( insertVal->myDelimType() );

		        skipInsert = false;

			DataValueNode *tmpInsertVal;
			if(   ( filterDelim  > 0 && valueDelim != filterDelim  ) ||
			      ( filterDelim  < 0 && valueDelim == -(filterDelim)  )  )
			{  tmpInsertVal = new DataValueNode( ".", DataValueNode::NON );
			   insertVal = tmpInsertVal;
			}
			else
			   tmpInsertVal = NULL;
			if( filterDelim == DataValueNode::FRAMECODE && insertVal != NULL )
			{
			   skipInsert = true;
			   string saveName = string( "save_") + insertVal->myValue();
			   checkMatches = 
			      inStar->searchForTypeByTag( 
			                 ASTnode::SAVEFRAMENODE,
					 saveName );
			   if( checkMatches->size() > 0 )
			   {
			       matching = (*checkMatches)[0];
			       if( (*checkTags)[k].length() > 0  )
			       {
				   if( checkMatches != NULL )
				   {  delete checkMatches;
				      checkMatches = NULL;
				   }
				   m = 0;
				   while( m != string::npos && m < (*checkVals)[k].length() )
				   {
				       n = (*checkVals)[k].find( ':', m );
				       if( n == string::npos )
				           n = (*checkVals)[k].length();
				       tmpVal = string( (*checkVals)[k], m, (n-m) );
				       checkMatches = matching->searchByTagValue( (*checkTags)[k], tmpVal );
				       if( checkMatches->size() > 0 )
				       {
				           break;
				       }
				       else
				       {
				           delete checkMatches;
					   checkMatches = NULL;
				       }
				       m = n+1;
				   }
				   if( checkMatches && checkMatches->size() > 0 )
				   {
				      skipInsert = false;
				   }
				   if( checkMatches != NULL)
				   { delete checkMatches;
				     checkMatches = NULL;
				   }
			       }
			       else // no colons in the delim string, no special checks to worry about:
			            // just unconditionally work as long as there's a saveframe there.
			       {
			           skipInsert = false;
			       }
			   }
			   if( checkMatches != NULL)
			   { delete checkMatches;
			     checkMatches = NULL;
			   }
			}

			if( skipInsert )
			{  tmpInsertVal = new DataValueNode( ".", DataValueNode::NON );
			   insertVal = tmpInsertVal;
			}
			outputSF->GiveMyDataList()->insert(
			    outputSF->GiveMyDataList()->end(),
			    new DataItemNode(  
				 (*reNames_free)[k]->myName(),
				 insertVal->myValue(),
				 insertVal->myDelimType() ) );

			if( tmpInsertVal != NULL )
			   delete tmpInsertVal;
		    }
		    else if( (*namePositions_free)[k] == -998 ) 
		    {
			outputSF->GiveMyDataList()->insert(
			    outputSF->GiveMyDataList()->end(),
			    new DataItemNode(  (*reNames_free)[k]->myName(),
			                       string("?"),
					       DataValueNode::NON ) );
		    }
		    else if ( (*namePositions_free)[k] == -999 ) 
		    {
			ASTnode *parent;
			for( parent = inputVals ; 
			     parent != NULL && ! parent->isOfType(ASTnode::SAVEFRAMENODE) ;
			     parent = parent->myParent() )
			 { /*null body*/ }
			string inputFrameCode = ((SaveFrameNode*)parent)->myName();
			if( strncmp( inputFrameCode.c_str(), "save_", 5 ) == 0 )
			    inputFrameCode = string( inputFrameCode.c_str() + 5 );
			char  neededQ = FindNeededQuoteStyle(inputFrameCode);
			outputSF->GiveMyDataList()->insert(
			    outputSF->GiveMyDataList()->end(),
			    new DataItemNode(  
				 (*reNames_free)[k]->myName(),
				 inputFrameCode,
				 neededQ == ';' ? DataValueNode::SEMICOLON :
				   neededQ == '\'' ? DataValueNode::SINGLE :
				     neededQ == '\"' ? DataValueNode::DOUBLE  :
				       DataValueNode::NON  ) );
		    }
		    else if( (*namePositions_loop)[k] < 0 ) 
			outputSF->GiveMyDataList()->insert(
			    outputSF->GiveMyDataList()->end(),
			    new DataItemNode(  (*reNames_free)[k]->myName(),
			                     (*(*mapInnerLoop)[ -1000 - (*namePositions_loop)[k] ])[0]->myValue(),
			                     (*(*mapInnerLoop)[ -1000 - (*namePositions_loop)[k] ])[0]->myDelimType() ) );
		  }
		
		
	        if( j % GARBAGE_COLLECT_EVERY_N_ROWS == 0 )
		{

		    ValCache::flushValCache();

		}
	      }
	    
	    
	    
	    // If the copy flag says to, remove the columns from the
	    // peer of the input loop:
	    if(     copy_or_move_flag == string("M") ||
		    copy_or_move_flag == string("m")   )
	      {
	        List<ASTnode*> *matchDel;
		for( j = 0 ; j < newNames_loop->size() ; j++ )
		  {
		    matchDel = ((DataLoopNode*)(inputLoop))
		      ->searchByTag( (*newNames_loop)[j]->myName() );
		    if( matchDel != NULL )
		    {
		      if( matchDel->size() > 0 )
		        if( (*matchDel)[0]->myParallelCopy() != NULL )
			    delete (*matchDel)[0]->myParallelCopy();
		      delete matchDel;
		    }
		  }
		for( j = 0 ; j < newNames_free->size() ; j++ )
		  {
		    matchDel = ((DataLoopNode*)(inputLoop))
		      ->searchByTag( (*newNames_free)[j]->myName() );
		    if( matchDel != NULL )
		    {
		      if( matchDel->size() > 0 )
		        if( (*matchDel)[0]->myParallelCopy() != NULL )
			    delete (*matchDel)[0]->myParallelCopy();
		      delete matchDel;
		    }
		  }
	      }
	    delete namePositions_loop;
	    delete namePositions_free;
	    delete newNames_loop;
	    delete newNames_free;
	    delete delimFilters;
	    delete checkTags;
	    delete checkVals;
	    delete reNames_loop;
	    delete reNames_free;
	  }
      }

      else   //input_key_tag is a freetag
      {
	for( i = 0 ; i < input_matches->size() ; i++ )
	{
	    ASTnode *par = (*input_matches)[i];
	    while( true )
	    {   par = par->myParent();
		if( par->isOfType( ASTnode::SAVEFRAMENODE ) )
		{   isInSF = true;
		    sf = (SaveFrameNode*)par;
		    break;
		}
		if( par->isOfType( ASTnode::DATABLOCKNODE ) )
		{   isInSF = false;
		    db = (DataBlockNode*)par;
		    break;
		}
	    }
	    SaveFrameNode *thisInputSf = sf; // an alternate name for the same thing
	                                     // for backward compatability with some
					     // older code pasted in here.
	    //check output_sf_name_tag value
	    List<ASTnode*>* out_sf_tag;
	    string out_sf_name_tag;
	    if( strcmp(output_sf_name_tag.c_str(), "." ) != 0 )
	    {
		if( isInSF )
		{
		    if( output_sf_name_tag == string("FRAMECODE") )
		    {   out_sf_name_tag = thisInputSf->myName();
		        if( strncmp( out_sf_name_tag.c_str(), "save_", 5 ) == 0 )
			{   out_sf_name_tag = string(out_sf_name_tag.c_str() + 5 ) ;
			}
		    }
		    else
		    {   out_sf_tag = thisInputSf->searchByTag(output_sf_name_tag);
		    }
	        }
		else 
		{
		    out_sf_tag =db->searchByTag(output_sf_name_tag);
		}
		if( output_sf_name_tag != string("FRAMECODE") )
		{
		    //should always have one matching item
		    if( out_sf_tag->size() > 0 ) 
		    {
			if( (*out_sf_tag)[0]->isOfType( ASTnode::DATAITEMNODE ) )
			{
			    // The value is the free tag value
			    out_sf_name_tag = ((DataItemNode*)((*out_sf_tag)[0]))->myValue();
			}
			else if( (*out_sf_tag)[0]->isOfType( ASTnode::DATANAMENODE ) )
			{ // The value is the first row of the loop's value:
			    ASTnode *par;
			    for( par = (*out_sf_tag)[0] ; par != NULL && ! par->isOfType( ASTnode::DATALOOPNODE) ; par = par->myParent() )
			    { // null loop body
			    }
			    if( par != NULL )
			    { int col, nest;
			      ((DataLoopNode*)par)->tagPositionDeep( output_sf_name_tag, &nest, &col );
			      if( col >= 0 )
			      { out_sf_name_tag = 
				      (*( ((DataLoopNode*)par)->getVals()[0] ))[col]->myValue();
			      }
			    }
			}
		    }
		    else
		    {
		      // nothing
		    }
		}
	    }
	    else
	    {
		out_sf_name_tag = string("");
	    }

	    if ( out_sf_name_tag == string ("*") ||out_sf_name_tag == 
		 string (".")||out_sf_name_tag == string ("?") )
	      {
		(*errStream) << "#transform-43# WARNING:invalid sf name tag value in input data."<<
			     "(Cannot have null values for naming saveframes with)." << endl;
		break;
	      }
	    
	    // Figure out which saveframe it should go into:
	    string sf_name;
	    List<ASTnode*> *sfSearch = NULL;
	    if( output_sf_base_name == string(".") )
	    {
		sfSearch = outStar->searchForTypeByTagValue(
			   ASTnode::SAVEFRAMENODE, output_sf_key_tag,
			   output_sf_key_value   );
	    }
	    else
	    {
		sf_name = output_sf_base_name ;
		if( out_sf_name_tag.length() > 0 )
		    sf_name += string("_");
		sf_name += out_sf_name_tag;
		if( strncmp( sf_name.c_str(), "save_", 5 ) != 0 )
		    sf_name = string("save") +
			  (sf_name[0] == '_' ? "" : "_" ) + sf_name ;
		SearchAndReplace( sf_name, string(" "), string("_" ) );
		SearchAndReplace( sf_name, string("\t"), string("_" ) );
		sfSearch = 
		    outStar->searchForTypeByTag( ASTnode::SAVEFRAMENODE, sf_name );
	    }
	    // If the output_sf_label_back_tag is set, check that it matches
	    // the framecode of the input saveframe, or it's not the
	    // right match.  If none match, the source file is invalid.
	    if( output_sf_label_back_tag != string(".") )
	    {
		int     matchIdx;
		string  inputFrameCode = thisInputSf->myName();
		if( strncmp( inputFrameCode.c_str(), "save_", 5 ) == 0 )
		    inputFrameCode = string( inputFrameCode.c_str() + 5 );
		for( matchIdx = 0 ; matchIdx < sfSearch->size(); matchIdx++ )
		{   SaveFrameNode *sfn = (SaveFrameNode*) ( (*sfSearch)[matchIdx] );
		    List<ASTnode*> *labelSearch = sfn->searchByTagValue(
			output_sf_label_back_tag, inputFrameCode );
		    if( labelSearch->size() == 0 )
		    {   sfSearch->erase( sfSearch->begin() + matchIdx );
			matchIdx--; /* to compensate for the deletion */
		    }
		    delete labelSearch;
		}
	    }
	    // If the output_sf_label_fore_tag is set, check that it matches
	    // the framecode of the output saveframe, or it's not the
	    // right match.  If none match, the source file is invalid.
	    if( output_sf_label_fore_tag != string(".") )
	    {
		int     matchIdx;
		List<ASTnode*> *inputLabelMatches = thisInputSf->searchForTypeByTag(
			ASTnode::DATAITEMNODE, output_sf_label_fore_tag );
		string  inputLabelVal = 
		        ((DataItemNode*)( (*inputLabelMatches)[0]) )->myValue();
		if( inputLabelVal.c_str()[0] == '$' )
		    inputLabelVal = string(inputLabelVal.c_str()+1);
		if( strncmp( inputLabelVal.c_str(), "save_", 5) == 0 )
		    inputLabelVal = string(inputLabelVal.c_str()+5);
		for( matchIdx = 0 ; matchIdx < sfSearch->size(); matchIdx++ )
		{   SaveFrameNode *sfn = (SaveFrameNode*) ( (*sfSearch)[matchIdx] );
		    if( sfn->myName() != inputLabelVal )
		    {   sfSearch->erase( sfSearch->begin() + matchIdx );
			matchIdx--; /* to compensate for the deletion */
		    }
		}
		delete inputLabelMatches;
	    }
	    // If the output_sf_key_tag is set, check that the tag/val
	    // pair matches the found saveframe - if not then delete
	    // it from consideration:
	    if( output_sf_key_tag != string(".") )
	    {
		int     matchIdx;
		for( matchIdx = 0 ; matchIdx < sfSearch->size(); matchIdx++ )
		{   SaveFrameNode *sfn = (SaveFrameNode*) ( (*sfSearch)[matchIdx] );
		    List<ASTnode*> *labelSearch = sfn->searchByTagValue(
			output_sf_key_tag, output_sf_key_value );
		    if( labelSearch->size() == 0 )
		    {   sfSearch->erase( sfSearch->begin() + matchIdx );
			matchIdx--; /* to compensate for the deletion */
		    }
		    delete labelSearch;
		}
	    }

	    // Create a new saveframe if need be, or use the one that
	    // was found:
	    SaveFrameNode *outputSF;
	    if( sfSearch->size() > 0 )
	    {
		  // If there's more than one choice, prefer the one that
		  // is identically named as the input loop.  otherwise use
		  // the first one:
		  int matchNumToUse;
		  for(    matchNumToUse = 0 ;
		          matchNumToUse < sfSearch->size() ;
		          matchNumToUse++ )
		  {   if( ((SaveFrameNode*)(*sfSearch)[matchNumToUse])->myName() ==
		               thisInputSf->myName() )
		          break;
		  }
		  if( matchNumToUse >= sfSearch->size() )
		      matchNumToUse = 0;
		  outputSF = (SaveFrameNode*) ( (*sfSearch)[matchNumToUse] );
		  List<ASTnode*> *catMatches = outputSF->searchByTag(
		                                output_sf_key_tag );
		  if( catMatches->size() <= 0 )
		  {   outputSF->GiveMyDataList()->insert(
				     outputSF->GiveMyDataList()->end(),
			new DataItemNode( output_sf_key_tag,
			      output_sf_key_value, output_sf_key_tag_type )
				     );
		  }
		  delete catMatches;
	    }
	    else
	      {
		if( sf_name == string("save_NEW_SAME") )
		{   sf_name = thisInputSf->myName();
		}
		outputSF = new SaveFrameNode( sf_name );
		if( output_sf_key_value != string("*") &&
		    output_sf_key_value != string(".") )
		{
		    outputSF->GiveMyDataList()->insert(
			    outputSF->GiveMyDataList()->end(),
			    new DataItemNode( output_sf_key_tag,
			    output_sf_key_value, output_sf_key_tag_type )
			    );
		}
		List<ASTnode*> *outputBlocks = outStar->searchForType( ASTnode::BLOCKNODE );
		BlockNode *lastOutputBlock = (BlockNode*)( (*outputBlocks)[outputBlocks->size() - 1] );
		delete outputBlocks;
		lastOutputBlock->GiveMyDataList()->insert( lastOutputBlock->GiveMyDataList()->end(), outputSF ); 
	      }
	    //find the mapInnerLoop's data item in input saveframe and copy into 
	    //output saveframe
	    for(j = 0; j < mapInnerLoop->size(); j++)
	      {	
		
		short delimFilter = 0;
		string renameName;
		string checkTag;
		string checkVal;
		char   *charPos1, *charPos2;
		char   delimField[2048];

		checkTag = string("");
		checkVal = string("");

		if( (*mapInnerLoop)[j]->size() >= 4 )
		    renameName = (*(*mapInnerLoop)[j])[3]->myValue();
		else
		    renameName = (*(*mapInnerLoop)[j])[0]->myValue();

		if(     copy_or_move_flag == string("M") ||
			copy_or_move_flag == string("m")   )
		  {
		    List<ASTnode*>* newDataItem ;
		    if(isInSF)
		    {
		      if( strncmp( (*(*mapInnerLoop)[j])[0]->myValue().c_str(), "FRAMECODE", 9 ) == 0 )
		      {
			  string inputFrameCode = thisInputSf->myName();
			  if( strncmp( inputFrameCode.c_str(), "save_", 5 ) == 0 )
			    inputFrameCode = string( inputFrameCode.c_str() + 5 );
		          newDataItem = new List<ASTnode*>;
			  char neededQ = FindNeededQuoteStyle( inputFrameCode );
			  newDataItem->insert( newDataItem->end(),
			                       new DataItemNode( "_FRAMECODE" /*name is a dummy*/,
			                             inputFrameCode,
						     neededQ == ';' ? DataValueNode::SEMICOLON :
						       neededQ == '\'' ? DataValueNode::SINGLE :
							 neededQ == '\"' ? DataValueNode::DOUBLE  :
							   DataValueNode::NON  ) );
		      }
		      else if( (*(*mapInnerLoop)[j])[0]->myValue().c_str()[0] != '_' )
		      {
		          newDataItem = new List<ASTnode*>;
			  char neededQ = FindNeededQuoteStyle( (*(*mapInnerLoop)[j])[0]->myValue() );
			  newDataItem->insert( newDataItem->end(),
			                       new DataItemNode( "_hardCODE" /*name is a dummy*/,
					                  (*(*mapInnerLoop)[j])[0]->myValue(),
							   neededQ == ';' ? DataValueNode::SEMICOLON :
							     neededQ == '\'' ? DataValueNode::SINGLE :
							       neededQ == '\"' ? DataValueNode::DOUBLE  :
								 DataValueNode::NON  ) );
		      }
		      else
		      {
		          string inputDelim = (*(*mapInnerLoop)[j])[1]->myValue();
			  if( strncmp( inputDelim.c_str(),"FRAMECODE", 9 ) == 0 )
			      delimFilter = (short) DataValueNode::FRAMECODE ;
			  else if( inputDelim == string("SINGLE") )
			      delimFilter = (short) DataValueNode::SINGLE ;
			  else if( inputDelim == string("DOUBLE") )
			      delimFilter = (short) DataValueNode::DOUBLE ;
			  else if( inputDelim == string("SEMICOLON") )
			      delimFilter = (short) DataValueNode::SEMICOLON ;
			  else if( inputDelim == string("NON") )
			      delimFilter = (short) DataValueNode::NON ;
			  else if( strncmp( inputDelim.c_str(),"!FRAMECODE", 10 ) == 0 )
			      delimFilter = - (short) DataValueNode::FRAMECODE ;
			  else if( inputDelim == string("!SINGLE") )
			      delimFilter = - (short) DataValueNode::SINGLE ;
			  else if( inputDelim == string("!DOUBLE") )
			      delimFilter = - (short) DataValueNode::DOUBLE ;
			  else if( inputDelim == string("!SEMICOLON") )
			      delimFilter = - (short) DataValueNode::SEMICOLON ;
			  else if( inputDelim == string("!NON") )
			      delimFilter = - (short) DataValueNode::NON ;
			  else 
			      delimFilter = (short) 0;

			  newDataItem = thisInputSf->searchByTag(
					   (*(*mapInnerLoop)[j])[0]->myValue()
					   );
			 strcpy(delimField, inputDelim.c_str() );
			 charPos1 = strchr( delimField, ':' );
			 if( charPos1 != NULL )
			 {  charPos2 = strchr( charPos1+1, ':' );
			    if( charPos2 != NULL )
			    {
			       *charPos2 = '\0';
			       checkTag = string( charPos1+1 );
			       checkVal = string( charPos2+1 );
			    }
			 }
		      }
		    }
		    else
		      newDataItem = db->searchByTag(
					   (*(*mapInnerLoop)[j])[0]->myValue()
					   );

		    if(newDataItem->size() != 1)
		      {
			(*errStream)<< "#transform43# warning: The input file has " <<
			      newDataItem->size() << " occurances of " <<
			      (*(*mapInnerLoop)[j])[0]->myValue() << endl;
			continue;
		      }
		    //should only find one in the saveframe

		    if( (*newDataItem)[0]->isOfType( ASTnode::DATANAMENODE ) )
		    {
			(*errStream)<<"#transform43# ERROR: The _input_tag ("<<
					 (*(*mapInnerLoop)[j])[0]->myValue() <<
					 ") is inside a loop, but the _input_key_tag ("<<
					 input_key_tag <<") is not in the loop.  This is "<<
					 "an error.  The _input_key_tag  is supposed to "<<
					 "be used to name which loop to copy values from, "<<
					 "(since loops don't have names)." << endl;
			continue;
		    }
		    else
		    {
			// Skip if the input key tag has a null value:
			if(  ( (DataItemNode*)(*newDataItem)[0])->myName() == input_key_tag  &&
			     ( ( (DataItemNode*)(*newDataItem)[0])->myValue() == string("?")  ||
			       ( (DataItemNode*)(*newDataItem)[0])->myValue() == string(".")  )  )
			{
			   continue;
			}

			
		        skipInsert = false;
			if(   delimFilter == 0 ||
			      ( delimFilter > 0 && 
			        delimFilter ==  
			         (short) ( (  (DataItemNode*)(*newDataItem)[0]
				           )->myDelimType() )  ) || 
			      ( delimFilter < 0 && 
			        -(delimFilter) !=  
			         (short) ( (  (DataItemNode*)(*newDataItem)[0]
				           )->myDelimType() )  )
			  )
			{
			    if( delimFilter == DataValueNode::FRAMECODE )
			    {
			       skipInsert = true;
			       checkMatches = 
				  inStar->searchForTypeByTag( 
					     ASTnode::SAVEFRAMENODE,
					     string( "save_") + ((DataItemNode*)(*newDataItem)[0])->myValue() );
			       if( checkMatches->size() > 0 )
			       {
				   matching = (*checkMatches)[0];
				   if( checkTag.length() > 0  )
				   {
				       if( checkMatches != NULL )
				       {  delete checkMatches;
				          checkMatches = NULL;
				       }
				       m = 0;
				       while( m != string::npos && m < checkVal.length() )
				       {
					   n = checkVal.find( ':', m );
					   if( n == string::npos )
					       n = checkVal.length();
					   tmpVal = string( checkVal, m, (n-m) );
					   checkMatches = matching->searchByTagValue( checkTag, tmpVal );
					   if( checkMatches->size() > 0 )
					   {
					       break;
					   }
					   else
					   {
					       delete checkMatches;
					       checkMatches = NULL;
					   }
					   m = n+1;
				       }
				       if( checkMatches && checkMatches->size() > 0 )
				       {
					  skipInsert = false;
				       }
				       if( checkMatches != NULL)
				       { delete checkMatches;
					 checkMatches = NULL;
				       }
				   }
			       }
			       if( checkMatches != NULL)
			       { delete checkMatches;
				 checkMatches = NULL;
			       }
			    }

			  outputSF->GiveMyDataList()->insert(
			      outputSF->GiveMyDataList()->end(),
			      new DataItemNode( renameName,
					    (skipInsert ? string(".") : ((DataItemNode*)(*newDataItem)[0])->myValue() ),
					    (skipInsert ? DataValueNode::NON : ((DataItemNode*)(*newDataItem)[0])->myDelimType()  ) ) );
			}

			delete (*newDataItem)[0]->myParallelCopy();
		    }
		  }
		else
		  {
		    List<ASTnode*>* newDataItem;
		    if (isInSF)
		    {
		      if( strncmp( (*(*mapInnerLoop)[j])[0]->myValue().c_str(), "FRAMECODE", 9 ) == 0 )
		      {
			  string inputFrameCode = thisInputSf->myName();
			  if( strncmp( inputFrameCode.c_str(), "save_", 5 ) == 0 )
			    inputFrameCode = string( inputFrameCode.c_str() + 5 );
		          newDataItem = new List<ASTnode*>;
			  char neededQ = FindNeededQuoteStyle( inputFrameCode );
		          newDataItem->insert( newDataItem->end(),
			                       new DataItemNode( "_FRAMECODE" /*name is a dummy*/,
			                                  inputFrameCode,
							   neededQ == ';' ? DataValueNode::SEMICOLON :
							     neededQ == '\'' ? DataValueNode::SINGLE :
							       neededQ == '\"' ? DataValueNode::DOUBLE  :
								 DataValueNode::NON  ) );
		      }
		      else if( (*(*mapInnerLoop)[j])[0]->myValue().c_str()[0] != '_' )
		      {
		          newDataItem = new List<ASTnode*>;
			  char neededQ = FindNeededQuoteStyle( (*(*mapInnerLoop)[j])[0]->myValue() );
			  newDataItem->insert( newDataItem->end(),
			                       new DataItemNode( "_hardCODE" /*name is a dummy*/,
					                  (*(*mapInnerLoop)[j])[0]->myValue(),
							   neededQ == ';' ? DataValueNode::SEMICOLON :
							     neededQ == '\'' ? DataValueNode::SINGLE :
							       neededQ == '\"' ? DataValueNode::DOUBLE  :
								 DataValueNode::NON  ) );
		      }
		      else
		      {
			  newDataItem = thisInputSf->searchByTag(
					     (*(*mapInnerLoop)[j])[0]->myValue() 
					     );
		          string inputDelim = (*(*mapInnerLoop)[j])[1]->myValue();
			  if( strncmp( inputDelim.c_str(), "FRAMECODE", 9 ) == 0 )
			      delimFilter = (short) DataValueNode::FRAMECODE ;
			  else if( inputDelim == string("SINGLE") )
			      delimFilter = (short) DataValueNode::SINGLE ;
			  else if( inputDelim == string("DOUBLE") )
			      delimFilter = (short) DataValueNode::DOUBLE ;
			  else if( inputDelim == string("SEMICOLON") )
			      delimFilter = (short) DataValueNode::SEMICOLON ;
			  else if( inputDelim == string("NON") )
			      delimFilter = (short) DataValueNode::NON ;
			  else if( strncmp( inputDelim.c_str(), "!FRAMECODE", 10 ) == 0 )
			      delimFilter = - (short) DataValueNode::FRAMECODE ;
			  else if( inputDelim == string("!SINGLE") )
			      delimFilter = - (short) DataValueNode::SINGLE ;
			  else if( inputDelim == string("!DOUBLE") )
			      delimFilter = - (short) DataValueNode::DOUBLE ;
			  else if( inputDelim == string("!SEMICOLON") )
			      delimFilter = - (short) DataValueNode::SEMICOLON ;
			  else if( inputDelim == string("!NON") )
			      delimFilter = - (short) DataValueNode::NON ;
			  else
			      delimFilter = (short) 0;
			       
		      }
		    }
		    else
		    {
		      newDataItem = db->searchByTag(
					 (*(*mapInnerLoop)[j])[0]->myValue() 
					 );
		    }

		    if(newDataItem->size() != 1)
		    {
			(*errStream)<< "#transform43# warning: The input file has " <<
			      newDataItem->size() << " occurances of " <<
			      (*(*mapInnerLoop)[j])[0]->myValue() << endl;
			continue;
		    }
		    //should only find one in the saveframe

		    // double check to ensure that the value we have is really
		    // a single data item:
		    if( (*newDataItem)[0]->isOfType( ASTnode::DATANAMENODE ) )
		    {
			(*errStream)<<"#transform43 error: The _input_tag ("<<
					 (*(*mapInnerLoop)[j])[0]->myValue() <<
					 ") is inside a loop, but the _input_key_tag ("<<
					 input_key_tag <<") is not in the loop.  This is "<<
					 "an error.  The _input_key_tag  is supposed to "<<
					 "be used to name which loop to copy values from, "<<
					 "(since loops don't have names)." << endl;
		    }
		    else
		    {
			// Skip if the input key tag has a null value:
			if(  ( (DataItemNode*)(*newDataItem)[0])->myName() == input_key_tag  &&
			     ( ( (DataItemNode*)(*newDataItem)[0])->myValue() == string("?")  ||
			       ( (DataItemNode*)(*newDataItem)[0])->myValue() == string(".")  )  )
			{
			   continue;
			}

			skipInsert = false;
			if(   delimFilter == 0 ||
			      ( delimFilter > 0 &&
			        ( delimFilter ==
			          (short) (  ( (DataItemNode*)(*newDataItem)[0]
				             )->myDelimType() )   ) ) ||
			      ( delimFilter < 0 &&
			        ( -(delimFilter) !=
			          (short) (  ( (DataItemNode*)(*newDataItem)[0]
				             )->myDelimType() )   ) ) 
			   )
			{
			    if( delimFilter == DataValueNode::FRAMECODE )
			    {
			       skipInsert = true;
			       checkMatches = 
				  inStar->searchForTypeByTag( 
					     ASTnode::SAVEFRAMENODE,
					     string( "save_" ) + ((DataItemNode*)(*newDataItem)[0])->myValue() );
			       if( checkMatches->size() > 0 )
			       {
				   matching = (*checkMatches)[0];
				   if( checkTag.length() > 0  )
				   {
				       if( checkMatches != NULL )
				       {  delete checkMatches;
				          checkMatches = NULL;
				       }
				       m = 0;
				       while( m != string::npos && m < checkVal.length() )
				       {
					   n = checkVal.find( ':', m );
					   if( n == string::npos )
					       n = checkVal.length();
					   tmpVal = string( checkVal, m, (n-m) );
					   checkMatches = matching->searchByTagValue( checkTag, tmpVal );
					   if( checkMatches->size() > 0 )
					   {
					       break;
					   }
					   else
					   {
					       delete checkMatches;
					       checkMatches = NULL;
					   }
					   m = n+1;
				       }
				       if( checkMatches->size() > 0 )
				       {
					  skipInsert = false;
				       }
				       if( checkMatches != NULL)
				       { delete checkMatches;
					 checkMatches = NULL;
				       }
				   }
			       }
			       if( checkMatches != NULL)
			       { delete checkMatches;
				 checkMatches = NULL;
			       }
			    }

			  DataItemNode* newItem = new DataItemNode(
				  renameName,
				  ( skipInsert ? string(".") : ((DataItemNode*)(*newDataItem)[0])->myValue() ),
				  ( skipInsert ? DataValueNode::NON : ((DataItemNode*)(*newDataItem)[0])->myDelimType() ) );
			  outputSF->GiveMyDataList()->insert(
			     outputSF->GiveMyDataList()->end(), newItem );
			}
			delete newDataItem;
		    }
		  } 
	      }
	
	    delete sfSearch;
	  }
      }
    
    delete input_matches;

    
}
