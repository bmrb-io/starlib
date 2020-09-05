///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
// $Log: not supported by cvs2svn $
// Revision 1.12  2003/07/30 05:14:50  madings
// debugged a problem that made it necessary to add better output
// messages to transform43.cc, also updated the specs.html
// documentation to add rules 42 through 45.
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

static void do_one_iteration(
	    string    input_key_tag,
	    string    input_sf_name,
	    string    input_sf_key_tag,
	    string    input_sf_key_value,
	    string    output_sf_category,
	    DataValueNode::ValType    output_sf_category_type,
	    string    output_sf_base_name,
	    string    output_sf_name_tag,
	    string    copy_or_move_flag,
	    LoopTableNode   *mapInneLoop,
	    StarFileNode  *inStar,
	    StarFileNode  *outStar );

static void do_all_iterations(
	    int    input_key_tag_pos,
	    int    input_sf_name_pos,
	    int    input_sf_key_tag_pos,
	    int    input_sf_key_value_pos,
	    int    output_sf_category_pos,
	    int    output_sf_base_name_pos,
	    int    output_sf_name_tag_pos,
	    int    copy_or_move_flag_pos,
	    LoopTableNode  *mapLoopTbl,
	    StarFileNode   *inStar,
	    StarFileNode   *outStar );

void save_values_to_saveframes_many_to_many(
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
    int                   output_sf_category_pos = -1;
    int                   output_sf_base_name_pos = -1;
    int                   output_sf_name_tag_pos = -1;
    int                   copy_or_move_flag_pos = -1;


    // Find loop from the mapping file:
    // --------------------------------
    mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, string( "_input_key_tag" ) );
    if( mapLoopMatch->size() <= 0 )
    {
	(*errStream) << "#transform-17# ERROR: no loop tag called '_input_key_tag' in mapping file." << endl;
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
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_output_sf_category" ) )
	    output_sf_category_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_output_sf_base_name" ) )
	    output_sf_base_name_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_output_sf_name_tag" ) )
	    output_sf_name_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_copy_or_move_flag" ) )
	    copy_or_move_flag_pos = i;
    }
    if( input_key_tag_pos < 0  )
    {   (*errStream) << "#transform-17# ERROR: input mapping file is missing tag '_input_key_tag'." << endl;
	return; /* do nothing */
    }
    if( input_sf_name_pos < 0  )
    {   (*errStream) << "#transform-17# ERROR: input mapping file is missing tag '_input_sf_name'." << endl;
	return; /* do nothing */
    }
    if( input_sf_key_tag_pos < 0  )
    {   (*errStream) << "#transform-17# ERROR: input mapping file is missing tag 'input_sf_key_tag'." << endl;
	return; /* do nothing */
    }
    if( input_sf_key_value_pos < 0  )
    {   (*errStream) << "#transform-17# ERROR: input mapping file is missing tag '_input_sf_key_value'." << endl;
	return; /* do nothing */
    }
    if( output_sf_category_pos < 0  )
    {   (*errStream) << "#transform-17# ERROR: input mapping file is missing tag '_output_sf_catehgory'." << endl;
	return; /* do nothing */
    }
    if( output_sf_base_name_pos < 0  )
    {   (*errStream) << "#transform-17# ERROR: input mapping file is missing tag '_output_sf_base_name'." << endl;
	return; /* do nothing */
    }
    if( output_sf_name_tag_pos < 0  )
    {   (*errStream) << "#transform-17# ERROR: input mapping file is missing tag '_output_sf_name_tag'." << endl;
	return; /* do nothing */
    }
    if( copy_or_move_flag_pos < 0  )
    {   (*errStream) << "#transform-17# ERROR: input mapping file is missing tag 'copy_or_move_flag'." << endl;
	return; /* do nothing */
    }

    do_all_iterations(
	    input_key_tag_pos,
	    input_sf_name_pos,
	    input_sf_key_tag_pos,
	    input_sf_key_value_pos,
	    output_sf_category_pos,
	    output_sf_base_name_pos,
	    output_sf_name_tag_pos,
	    copy_or_move_flag_pos,
	    mapLoopTbl, inStar, outStar );

    return;
}

// Loop over all the iterations of the mapping file, doing all
// the rules found there:
static void do_all_iterations(
	    int    input_key_tag_pos,
	    int    input_sf_name_pos,
	    int    input_sf_key_tag_pos,
	    int    input_sf_key_value_pos,
	    int    output_sf_category_pos,
	    int    output_sf_base_name_pos,
	    int    output_sf_name_tag_pos,
	    int    copy_or_move_flag_pos,
	    LoopTableNode *mapLoopTbl,
	    StarFileNode  *inStar,
	    StarFileNode  *outStar )
{
    int i;

    for( i = 0 ; i < mapLoopTbl->size() ; i++ )
    {
	do_one_iteration( 
	    (*(*mapLoopTbl)[i])[input_key_tag_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[input_sf_name_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[input_sf_key_tag_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[input_sf_key_value_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[output_sf_category_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[output_sf_category_pos]->myDelimType(),
	    (*(*mapLoopTbl)[i])[output_sf_base_name_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[output_sf_name_tag_pos]->myValue(),
	    (*(*mapLoopTbl)[i])[copy_or_move_flag_pos]->myValue(),
	    (*mapLoopTbl)[i]->myLoop(), inStar, outStar );
    }
}

// Do one iteration of the mapping file
static void do_one_iteration(
	    string    input_key_tag,
	    string    input_sf_name,
	    string    input_sf_key_tag,
	    string    input_sf_key_value,
	    string    output_sf_category,
	    DataValueNode::ValType output_sf_category_type,
	    string    output_sf_base_name,
	    string    output_sf_name_tag,
	    string    copy_or_move_flag,
	    LoopTableNode   *mapInnerLoop,
	    StarFileNode  *inStar,
	    StarFileNode  *outStar )
{
    int                       i;
    int                       j;
    int                       k;
    int                       m;
    int                       sf_name_tag_pos;
    bool                      freeFlag; //to tell if input_key_tag not in loop

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
	(*errStream)<< "#transform-17# ERROR: can't find the input_key_tag item ("
                    <<input_key_tag
                    <<") in input file."
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
	       && ! par->isOfType( ASTnode::DATABLOCKNODE ) ){
	  par = par->myParent();}
	if(par->isOfType( ASTnode::SAVEFRAMENODE ) )
	  {
	    isInSF = true;
	    sf = (SaveFrameNode*) par;
	  }
	else if(par->isOfType( ASTnode::DATABLOCKNODE ) )
	  {
	    db = (DataBlockNode*) par;
	  }
	else
	  {
	    (*errStream) << "#transform-17# ERROR: can't find the input_key_tag item in input file"<<endl;
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
	else 
	  {
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
		    input_matches->erase( input_matches->begin()+i  );
		    i-- ; // compensate for the auto-increment the forloop
		    // will do.
		  }
		delete tagval_look;
	      }
	  }
      }

    //check whether inner loop empty
    if( mapInnerLoop == NULL || mapInnerLoop->size() == 0 )
      {    (*errStream) << "#transform-17# ERROR: input mapping file is missing the inner loop for _input_tag." << endl;
      return;
      }

    for( i = 0 ; i < input_matches->size() ; i++ )
      {
	if(!freeFlag)    //input_key_tag in loop
	  {
	    // For each input loop (ignores nesting)
	    for( i = 0 ; i < input_matches->size() ; i++ )
	      {
		DataLoopNode  *inputLoop = (DataLoopNode*) ( (*input_matches)[i] );
		
		// For each name mentioned in the inner loop of the map file,
		// add it into a name list for a new loop:
		LoopNameListNode  *newNames_loop = new LoopNameListNode();
		LoopNameListNode  *newNames_free = new LoopNameListNode();
		sf_name_tag_pos = -1;
		for( j = 0 ; j < mapInnerLoop->size() ; j++ )
		  {
		    if((*(*mapInnerLoop)[j])[1]->myValue()==string("L") )  
		      newNames_loop->insert(
					  newNames_loop->end(),
					  new DataNameNode( 
					  (*(*mapInnerLoop)[j])[0]->myValue() )
					  );
		    if((*(*mapInnerLoop)[j])[1]->myValue()==string("F") )
			 newNames_free->insert(
					  newNames_free->end(),
					  new DataNameNode( 
					  (*(*mapInnerLoop)[j])[0]->myValue() )
					  ); 
		  } 
	    
		// Find out where those names are in the original loop:
		List<int>         *namePositions_loop = new List<int>();
		List<int>         *namePositions_free = new List<int>();
		bool foundIt = false;
		for( j = 0 ; j < newNames_loop->size() ; j++ )
		  {
		    foundIt = false;
		    // was (*namePositions_loop)[j] = -1;
		    for( k = 0 ; k < inputLoop->getNames()[0]->size() ; k++ )
		      {
			if( (*newNames_loop)[j]->myName() ==
			       (* (inputLoop->getNames()[0]) )[k]->myName() )
			  {
			    namePositions_loop->insert( namePositions_loop->end(), k );
			    foundIt = true;
			  }
			if(     (* (inputLoop->getNames()[0]) )[k]->myName() ==
				output_sf_name_tag  )
			  {
			    sf_name_tag_pos = k;
			  }
		      }
		    // was: if( (*namePositions_loop)[j] == -1 )
		    if( ! foundIt )
		    {
			(*errStream) << "#transform17# warning: tag name " << 
			    (*newNames_loop)[j]->myName() << " not found in the " <<
			    "same loop as input_key_tag: " << input_key_tag << endl;
		    }
		  }

			    
		for( j = 0 ; j < newNames_free->size() ; j++ )
		  {
		    foundIt = false;
		    // was: (*namePositions_free)[j] = -1;
		    for( k = 0 ; k < inputLoop->getNames()[0]->size() ; k++ )
		      {
			if( (*newNames_free)[j]->myName() ==
			       (* (inputLoop->getNames()[0]) )[k]->myName() )
			  {
			    namePositions_free->insert( namePositions_free->end(), k );
			    foundIt = true;
			  }
			
			if(     (* (inputLoop->getNames()[0]) )[k]->myName() ==
				output_sf_name_tag  )
			  {
			    sf_name_tag_pos = k;
			  }
		      }
		    // was: if( (*namePositions_free)[j] == -1 )
		    if( ! foundIt )
		    {
			(*errStream) << "#transform17# warning: tag name " << 
			    (*newNames_loop)[j]->myName() << " not found in the " <<
			    "same loop as input_key_tag: " << input_key_tag << endl;
		    }
		  }

		// For each row of the input loop:
		LoopTableNode *inputVals = inputLoop->getValsPtr();
		for( j = 0 ; j < inputVals->size() ; j++ )
		  {
		    //check output_sf_name_tag value
		    string out_sf_name_tag;
		    if( sf_name_tag_pos == -1 )
			out_sf_name_tag = "";
		    else
			out_sf_name_tag = (*(*inputVals)[j])[sf_name_tag_pos]->myValue();
		    if(out_sf_name_tag == string("*") ||
		       out_sf_name_tag == string(".") ||
		       out_sf_name_tag == string("?"))
		      {
			(*errStream) << "#transform-17# WARNING:invalid value '"<<out_sf_name_tag<<
			        "' for naming a new saveframe.  Taken from a value to tag "<<
				output_sf_name_tag<< "." << endl;
			break;
		      }
		    // Build a new row consisting of the values
		    // we need copied out of the input loop's row:
		    LoopRowNode  *newRow = new LoopRowNode(
			     inputLoop->getTabFlag() ? "tabulate" : "linear" );
		    for( k = 0 ; k < namePositions_loop->size() ; k++ )
		      {
			newRow->insert(
			      newRow->end(),
			      *(  (*(*inputVals)[j])[ (*namePositions_loop)[k] ]  )
			      );
		      }
		    // Figure out which saveframe it should go into:
		    string  sf_name;
		    sf_name = output_sf_base_name ;
		    if( out_sf_name_tag.length() > 0 )
			sf_name += string("_");
		    sf_name += out_sf_name_tag;
		    List<ASTnode*> *sfSearch = 
		outStar->searchForTypeByTag( ASTnode::SAVEFRAMENODE, sf_name );

		    // Create a new saveframe if need be, or use the one that
		    // was found:
		    SaveFrameNode *outputSF;
		    if( sfSearch->size() > 0 )
		      outputSF = (SaveFrameNode*) ( (*sfSearch)[0] );
		    else
		      {
			outputSF = new SaveFrameNode( sf_name );
			outputSF->GiveMyDataList()->insert(
					 outputSF->GiveMyDataList()->end(),
			    new DataItemNode( string( "_Saveframe_category"),
				  output_sf_category, output_sf_category_type )
					 );
			BlockNode *lastBlock;
			if( outStar->GiveMyDataBlockList()->size() == 0 )
			  {
			    (*errStream) <<
			      "#transform-17# ERROR: File has no data_ or global_ sections." << endl ;
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
		    // Get the loop in the saveframe where the new row will be
		    // inserted.  If no such loop exists yet, start a new one:
		    if(newNames_loop != NULL && newNames_loop->size()>0)
		      {
			DataLoopNode *outputLoop;
			List<ASTnode*> *tagSearch = outputSF
			  ->searchForTypeByTag(
			ASTnode::DATALOOPNODE, (*newNames_loop)[0]->myName() );
			
			if( tagSearch->size() > 0 )
			  outputLoop = (DataLoopNode*) ( (*tagSearch)[0] );
			else
			  {
			    outputLoop = new DataLoopNode(
	  		    inputLoop->getTabFlag() ? "tabulate" : "linear" );
			    outputLoop->getNamesPtr()->insert(
			      outputLoop->getNamesPtr()->end(),
			      new LoopNameListNode(*newNames_loop) );
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

		    //copy free tags into the output saveframe
		     
		    for( k = 0 ; k < namePositions_free->size() ; k++ )
		      { 
		        // erase the below stuff: testing to see what is causing the bomb:
		        /*    new DataItemNode(  
			         (*newNames_free)[k]->myName(),
			         (*(*inputVals)[j])[ (*namePositions_free)[k] ]->myValue(),
			         (*(*inputVals)[j])[ (*namePositions_free)[k] ]->myDelimType() );  */
			outputSF->GiveMyDataList()->insert(
		            outputSF->GiveMyDataList()->end(),
		            new DataItemNode(  
			         (*newNames_free)[k]->myName(),
			         (*(*inputVals)[j])[ (*namePositions_free)[k] ]->myValue(),
			         (*(*inputVals)[j])[ (*namePositions_free)[k] ]->myDelimType() ) );
		      }
		    
		    delete sfSearch;
		    
		  }
		
		
		
		// If the copy flag says to, remove the columns from the
		// peer of the input loop:
		if(     copy_or_move_flag == string("M") ||
			copy_or_move_flag == string("m")   )
		  {
		    for( j = 0 ; j < newNames_loop->size() ; j++ )
		      {
			((DataLoopNode*)(inputLoop->myParallelCopy()) )
			  ->RemoveColumn( (*newNames_loop)[j]->myName() );
		      }
		    for( j = 0 ; j < newNames_free->size() ; j++ )
		      {
			((DataLoopNode*)(inputLoop->myParallelCopy()) )
			  ->RemoveColumn( (*newNames_free)[j]->myName() );
		      }
		  }
		delete namePositions_loop;
		delete namePositions_free;
		delete newNames_loop;
		delete newNames_free;
	      }
	  }

	else   //input_key_tag is a freetag
	  {
	    //check output_sf_name_tag value
	    List<ASTnode*>* out_sf_tag;
	    string out_sf_name_tag;
	    if( strcmp(output_sf_name_tag.c_str(), "." ) != 0 )
	    {
		if( isInSF )
		  out_sf_tag = sf->searchByTag(output_sf_name_tag);
		else 
		  out_sf_tag =db->searchByTag(output_sf_name_tag);
	        //should always have one matching item
		if( (*out_sf_tag)[0]->isOfType( ASTnode::DATANAMENODE ) )
		{
		    (*errStream)<<"#transform17# ERROR: The _output_sf_name_tag ("<<
				     output_sf_name_tag <<
				     ") is inside a loop, but the _input_key_tag ("<<
				     input_key_tag <<") is not in the loop.  This is "<<
				     "an error.  The _input_key_tag  is supposed to "<<
				     "be used to name which loop to copy values from, "<<
				     "(since loops don't have names)." << endl;
		    continue;
		}
	        out_sf_name_tag = ((DataItemNode*)((*out_sf_tag)[0]))->myValue();
	    }
	    else
	    {
		out_sf_name_tag = string("");
	    }

	    if ( out_sf_name_tag == string ("*") ||out_sf_name_tag == 
		 string (".")||out_sf_name_tag == string ("?") )
	      {
		(*errStream) << "#transform-17# WARNING:invalid sf name tag value in input data."<<
		             "(Cannot have null values for naming saveframes with)." << endl;
		break;
	      }
	    
	    // Figure out which output saveframe it should go into:
	    string  sf_name;
	    sf_name = output_sf_base_name ;
	    if( out_sf_name_tag.length() > 0 )
		sf_name += string("_");
	    sf_name += out_sf_name_tag;
	    List<ASTnode*> *sfSearch = 
	      outStar->searchForTypeByTag( ASTnode::SAVEFRAMENODE, sf_name );
	    // Create a new saveframe if need be, or use the one that
	    // was found:
	    SaveFrameNode *outputSF;
	    if( sfSearch->size() > 0 )
	      outputSF = (SaveFrameNode*)((*sfSearch)[0] );
	    else
	      {
		outputSF = new SaveFrameNode( sf_name );
		outputSF->GiveMyDataList()->insert(
			    outputSF->GiveMyDataList()->end(),
			    new DataItemNode( string( "_Saveframe_category"),
		       	    output_sf_category, output_sf_category_type )
			    );
		List<ASTnode*> *outputBlocks = outStar->searchForType( ASTnode::BLOCKNODE );
		BlockNode *lastOutputBlock = (BlockNode*)( (*outputBlocks)[outputBlocks->size() - 1] );
		delete outputBlocks;
		lastOutputBlock->GiveMyDataList()->insert( lastOutputBlock->GiveMyDataList()->end(), outputSF ); 
	      }
	    //find the mapInnerLoop's data item in input saveframe and copy into 
	    //output saveframe
	    for(j = 0; j < mapInnerLoop->size(); j++)
	      {	
		if(     copy_or_move_flag == string("M") ||
			copy_or_move_flag == string("m")   )
		  {
		    List<ASTnode*>* newDataItem ;
		    if(isInSF)
		      newDataItem = sf->searchByTag(
					   (*(*mapInnerLoop)[j])[0]->myValue()
					   );
		    else
		      newDataItem = db->searchByTag(
					   (*(*mapInnerLoop)[j])[0]->myValue()
					   );
		    if(newDataItem->size() != 1)
		      {
			// was (*errStream)
			(cerr) << "#transform17# warning: The input file has " <<
			      newDataItem->size() << " occurances of " <<
			      (*(*mapInnerLoop)[j])[0]->myValue() <<
			      ( (isInSF) ?
			          ( string(" in saveframe ") + sf->myName() ) :
			          ( string(" in datablock ") + db->myName() )  )
			      << endl;
			continue;
		      }
		    //should only find one in the saveframe

		    if( (*newDataItem)[0]->isOfType( ASTnode::DATANAMENODE ) )
		    {
			(*errStream)<<"#transform17# ERROR: The _input_tag ("<<
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
			
			outputSF->GiveMyDataList()->insert(
			  outputSF->GiveMyDataList()->end(),
			  new DataItemNode( *( (DataItemNode*)(*newDataItem)[0] ) )
			  );
			
			delete (*newDataItem)[0]->myParallelCopy();
		    }
		  }
		else
		  {
		    List<ASTnode*>* newDataItem;
		    if (isInSF)
		    {
		      newDataItem = sf->searchByTag(
					 (*(*mapInnerLoop)[j])[0]->myValue() 
					 );
		    }
		    else
		    {
		      newDataItem = db->searchByTag(
					 (*(*mapInnerLoop)[j])[0]->myValue() 
					 );
		    }
		    if(newDataItem->size() != 1)
		    {
			(*errStream)<< "#transform17# warning: The input file has " <<
			      newDataItem->size() << " occurances of " <<
			      (*(*mapInnerLoop)[j])[0]->myValue() <<
			      ( (isInSF) ?
			          ( string(" in saveframe ") + sf->myName() ) :
			          ( string(" in datablock ") + db->myName() )  )
			      << " internal note: warn 2 "<< endl;
			continue;
		    }
		    //should only find one in the saveframe

		    // double check to ensure that the value we have is really
		    // a single data item:
		    if( (*newDataItem)[0]->isOfType( ASTnode::DATANAMENODE ) )
		    {
			(*errStream)<<"#transform17 error: The _input_tag ("<<
					 (*(*mapInnerLoop)[j])[0]->myValue() <<
					 ") is inside a loop, but the _input_key_tag ("<<
					 input_key_tag <<") is not in the loop.  This is "<<
					 "an error.  The _input_key_tag  is supposed to "<<
					 "be used to name which loop to copy values from, "<<
					 "(since loops don't have names)." << endl;
		    }
		    else
		    {
			DataItemNode* newItem = new DataItemNode(
				    ((DataItemNode*)(*newDataItem)[0])->myName(),
				    ((DataItemNode*)(*newDataItem)[0])->myValue(),
				    ((DataItemNode*)(*newDataItem)[0])->myDelimType() );
			outputSF->GiveMyDataList()->insert(
			   outputSF->GiveMyDataList()->end(), newItem 
			  );
			delete newDataItem;
		    }
		  } 
	      }
	
	    delete sfSearch;
	  }
      }
    
    delete input_matches;
    
}
