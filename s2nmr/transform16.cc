///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.6  2003/07/24 18:49:13  madings
Changes to handle 2.1-3.0 mappings.

Revision 1.5  2000/11/18 03:02:37  madings
*** empty log message ***

// Revision 1.4  2000/11/18  02:26:28  madings
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
// Revision 1.3  1999/07/30  18:22:06  june
// june, 7-30-99
// add transform1d.cc
//
Revision 1.2  1999/04/09 20:15:55  madings
added many, many transforms.  transform 17 still does not work yet, though.

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
#include "transforms.h"

void save_tag_to_loop(  StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
			 SaveFrameNode *currDicRuleSF)
{
    List<ASTnode*>        *mapLoopMatch;
    DataLoopNameListNode  *mapNames;
    LoopTableNode         *mapLoopTbl;
    int                   mL_Idx; /* map loop index */
    string                tagname;
    int                   key_tag_pos;
    int                   key_value_pos;
    int                   tag_to_put_in_loop_pos;

    // Find loop from the mapping file:
    // --------------------------------
    mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, string( "_key_tag" ) );
    if( mapLoopMatch->size() < 0 )
    {
	(*errStream) << "#transform-16# ERROR: no loop tag called '_key_tag' in mapping file." << endl;
	delete mapLoopMatch;
	return; /* do nothing */
    }
    mapLoopTbl = ((DataLoopNode*)(*mapLoopMatch)[0])->getValsPtr();
    mapNames =  ((DataLoopNode*)(*mapLoopMatch)[0])->getNamesPtr();

    delete mapLoopMatch;

    key_tag_pos = -1;
    key_value_pos = -1;
    tag_to_put_in_loop_pos = -1;
    for( int i = 0 ; i < (*mapNames)[0]->size() ; i++ )
    {
	if(      (* ((*mapNames)[0]) )[i]->myName()==string( "_key_tag" ) )
	    key_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_key_value" ) )
	    key_value_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_tag_to_put_in_loop" ) )
	    tag_to_put_in_loop_pos = i;
    }
    if( key_tag_pos < 0  )
    {   (*errStream) << "#transform-16# ERROR: input mapping file is missing tag '_key_tag'." << endl;
	return; /* do nothing */
    }
    if( key_value_pos < 0  )
    {   (*errStream) << "#transform-16# ERROR: input mapping file is missing tag '_key_value'." << endl;
	return; /* do nothing */
    }
    if( tag_to_put_in_loop_pos < 0  )
    {   (*errStream) << "#transform-16# ERROR: input mapping file is missing tag '_tag_to_put_in_loop'." << endl;
	return; /* do nothing */
    }

    // For each occurance of the tag/value, change it:
    // -----------------------------------------------
    for( mL_Idx = 0 ; mL_Idx < mapLoopTbl->size() ; mL_Idx++ )
    {
	List<ASTnode*> *matchSF = AST->searchForTypeByTagValue(
			ASTnode::SAVEFRAMENODE, 
			(*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue(),
			(*(*mapLoopTbl)[mL_Idx])[key_value_pos]->myValue() );


	// For each matched saveframe, get each matching value:
	for( int sf_i = 0 ; sf_i < matchSF->size() ; sf_i++ )
	{
	    List<ASTnode*> *matchVal = (*matchSF)[sf_i]->searchForTypeByTag(
		        ASTnode::DATAITEMNODE,
			(*(*mapLoopTbl)[mL_Idx])[tag_to_put_in_loop_pos]->myValue() );

	    // Should be only one match - safe to assume that, but for
	    // completeness, it doesn't hurt to do it in a loop like this:
	    for( int i = 0 ; i < matchVal->size() ; i++ )
	    {
		DataItemNode *peerItem = (DataItemNode*) (*matchVal)[i]->myParallelCopy();
		if( peerItem == NULL )
		{
		    (*errStream) << "#transform-16# Error: internal error, no peer at line "
			<< __LINE__ << " for tag: " <<
			(*(*mapLoopTbl)[mL_Idx])[tag_to_put_in_loop_pos]->myValue() 
			<<" - Fatal error, quitting rule." << endl;
		    delete matchVal;
		    delete matchSF;
		    return;
		}
		// Now make the new empty data loop:
		DataLoopNode *newLoop = new DataLoopNode( string("tabulate") );
		DataLoopNameListNode *nameListList = newLoop->getNamesPtr();
		LoopTableNode *valTable = newLoop->getValsPtr();

		// Populate the list of tag names for the loop with one tag name:
		nameListList->insert( nameListList->end(), new LoopNameListNode() );
		LoopNameListNode *nameList = (*nameListList)[0];
		DataNameNode *newName = new DataNameNode( peerItem->myName() );
		nameList->insert( nameList->end(), newName );

		// populate the values for the loop with the one value:
                LoopRowNode *valRow = new LoopRowNode(true);
		valRow->insert( valRow->end(), new DataValueNode( peerItem->myValue(), peerItem->myDelimType()  ) );

		valTable->insert( valTable->end(), valRow );

		// Now delete the old tag, and insert the new loop to the saveframe.
		SaveFrameNode *peerSF = (SaveFrameNode*) (*matchSF)[sf_i]->myParallelCopy();
		if( peerSF == NULL )
		{
		    (*errStream) << "#transform-16# Error: internal error, no peer at line "
			<< __LINE__ << ".  Fatal error, quitting rule." << endl;
		    delete matchVal;
		    delete matchSF;
		    return;
		}

		delete peerItem;
		peerSF->GiveMyDataList()->insert( peerSF->GiveMyDataList()->end(), newLoop );
	    }
	    delete matchVal;
	}
	delete matchSF;
    } //end-for
}

