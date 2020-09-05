///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.7  2001/06/18 16:57:09  madings
Several fixes for s2nmr segfault bugs.

Revision 1.6  2000/11/18 03:02:39  madings
*** empty log message ***

// Revision 1.5  2000/03/16  05:09:42  madings
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
// Revision 1.4  1999/07/30  18:22:09  june
// june, 7-30-99
// add transform1d.cc
//
Revision 1.3  1999/06/10 17:38:54  zixia
Adding a couple of new functions to transform files in NMR-STAR 1.0 to NMR-STAR 2.0.

// Revision 1.2  1999/01/28  06:15:08  madings
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
// Revision 1.1  1998/12/15  04:33:10  madings
// Addded transforms 9 though 14.  (Lots of stuff)
//
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

void save_make_new_saveframe(  StarFileNode *inStar, 
                               StarFileNode *outStar, 
                               BlockNode    *,
			       SaveFrameNode *currDicRuleSF)
{
    string                map_sf_name = string("save_make_new_saveframe" );
    string                sf_cat = string( "_Saveframe_category" );
    List<ASTnode*>        *mapLoopMatch;
    DataLoopNameListNode  *outerMapNames;
    LoopTableNode         *outerMapLoopTbl;
    LoopTableNode         *innerMapLoopTbl;
    int                   mL_Idx; /* map loop index */
    SaveFrameNode         *newSaveFrame;
    string                tagname;
    int                   i;
    int                   from_key_tag_pos;
    int                   from_key_value_pos;
    int                   from_tagname_pos;
    int                   copy_or_move_flag_pos;

    mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, string( "_new_frame_name" ) );
    if( mapLoopMatch->size() < 0 )
    {
	(*errStream) << "#transform-9# ERROR: no loop tag called '_new_frame_name' in mapping file." << endl;
	delete mapLoopMatch;
	return; /* do nothing */
    }
    outerMapLoopTbl = ((DataLoopNode*)(*mapLoopMatch)[0])->getValsPtr();
    outerMapNames =  ((DataLoopNode*)(*mapLoopMatch)[0])->getNamesPtr();
    if( outerMapNames->size() != 2 )
    {
	(*errStream) << "#transform-9# ERROR: input mapping file has " <<
	    "wrong nesting level (should be one inner loop)." << endl ;
	delete mapLoopMatch;
	return; /* do nothing */
    }

    delete mapLoopMatch;

    from_key_tag_pos = -1;
    from_key_value_pos = -1;
    from_tagname_pos = -1;
    copy_or_move_flag_pos = -1;
    for( i = 0 ; i < (*outerMapNames)[1]->size() ; i++ )
    {
	if( (* ((*outerMapNames)[1]) )[i]->myName() == string( "_from_key_tag" ) )
	    from_key_tag_pos = i;
	else if( (* ((*outerMapNames)[1]) )[i]->myName()==string( "_from_key_value" ) )
	    from_key_value_pos = i;
	else if( (* ((*outerMapNames)[1]) )[i]->myName()==string( "_from_tagname" ) )
	    from_tagname_pos = i;
	else if( (* ((*outerMapNames)[1]) )[i]->myName()==string( "_copy_or_move_flag" ) )
	    copy_or_move_flag_pos = i;
    }
    if( from_key_tag_pos < 0  )
    {   (*errStream) << "#transform-9# ERROR: input mapping file is missing tag '_from_key_tag'." << endl;
	return; /* do nothing */
    }
    if( from_key_value_pos < 0  )
    {   (*errStream) << "#transform-9# ERROR: input mapping file is missing tag '_from_key_value'." << endl;
	return; /* do nothing */
    }
    if( from_tagname_pos < 0  )
    {   (*errStream) << "#transform-9# ERROR: input mapping file is missing tag '_from_tagname'." << endl;
	return; /* do nothing */
    }
    if( copy_or_move_flag_pos < 0  )
    {   (*errStream) << "#transform-9# ERROR: input mapping file is missing tag '_copy_or_move_flag'." << endl;
	return; /* do nothing */
    }


    // For each row of the map loop, make a new saveframe in outStar:
    // --------------------------------------------------------------
    for( mL_Idx = 0 ; mL_Idx < outerMapLoopTbl->size() ; mL_Idx++ )
    {
	ValCache::flushValCache();
	newSaveFrame = new SaveFrameNode( (*(*outerMapLoopTbl)[mL_Idx])[0]->myValue() );

	if( (*outerMapLoopTbl)[mL_Idx]->myLoop() != NULL )
	{
	    innerMapLoopTbl   = (*outerMapLoopTbl)[mL_Idx]->myLoop();

	    // For each row of the inner loop, make a new data item in the
	    // saveframe:
	    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	    for( int idx = 0 ; idx < innerMapLoopTbl->size() ; idx++ )
	    {
		string searchTag = (*(*innerMapLoopTbl)[idx])[from_key_tag_pos]->myValue();
		string searchVal = (*(*innerMapLoopTbl)[idx])[from_key_value_pos]->myValue();

		// Get the saveframe in the input star that matches:
		//
		List<ASTnode*>  *matchesSF = inStar->searchForTypeByTagValue(
			                            ASTnode::SAVEFRAMENODE,
						    searchTag,
						    searchVal );
		if( matchesSF->size() < 1 )
		{   (*errStream) << "#transform-9# WARNING: Cannot find a "<<
			"saveframe where tag '" << searchTag << "' = " <<
			searchVal << endl;
		    delete matchesSF;
		    return;
		}
		else if( matchesSF->size() > 1 )
		{   (*errStream) << "#transform-9# ERROR: Found  "<<
			"more than one saveframe where tag '" <<
			searchTag << "' = " << searchVal << endl;
	            for( int i = 0 ; i < matchesSF->size() ; i++ )
		    {
			(*errStream) << "#transform-9# (continued)   " <<
			    "matched saveframe number "<< i+1 <<" is " <<
			    ( (SaveFrameNode*)(*matchesSF)[i])->myName() << endl;
		    }
		    delete matchesSF;
		    return;
		}
		List<ASTnode *> *matches = (*matchesSF)[0]->searchForTypeByTag(
			ASTnode::DATANODE,
			(*(*innerMapLoopTbl)[idx])[from_tagname_pos]->myValue() );
		delete matchesSF;
		if( matches->size() < 1 )
		{ 
		    delete matches;  /* no matches found, ignore */
		    continue;
		}
		else if( matches->size() > 1 )
		{   (*errStream) << "#transform-9# ERROR: Found  "<<
			"more than one tag in saveframe with name ='" <<
			(*(*innerMapLoopTbl)[idx])[from_tagname_pos]->myValue()<<
			"'" << endl;
		    delete matches;
		    return;
		}
		if( (*matches)[0]->isOfType( ASTnode::DATAITEMNODE ) )
		{
		    // THIS IS A SINGLE DATA ITEM.
		    //
		    // Make a new data Item and append it to the new
		    // saveframe:
		    DataItemNode *newItem = new DataItemNode( *(DataItemNode*)((*matches)[0]) );
		    newSaveFrame->GiveMyDataList()->insert( newSaveFrame->GiveMyDataList()->end(), newItem );

		    // If we were supposed to delete the old version,
		    // do so now:
		    string copy_or_move_flag = (*(*innerMapLoopTbl)[idx])[copy_or_move_flag_pos]-> myValue();
		    if( copy_or_move_flag == string( "M" ) )
			if( (*matches)[0]->myParallelCopy() != NULL )
			{
			    delete (*matches)[0]->myParallelCopy();
			}
		}
		else
		{
		    // THIS IS A DATA LOOP.
		    //
		    // Make a new loop and add this column to the
		    // new loop:
		    DataLoopNode *newLoop = new DataLoopNode( string("tabulate") );
		    if( newLoop->getNames().size() < 1 )
			newLoop->getNames().insert( newLoop->getNames().end(), new LoopNameListNode() );
		    newLoop->getNames()[0]->insert(
			    newLoop->getNames()[0]->end(),
			    new DataNameNode( (*(*innerMapLoopTbl)[idx])[from_tagname_pos]->myValue() ) 
			    );

		    /* ---comment out begin ----- old code did this:
			List< DataValueNode *> *oldVals = 
			    ((DataLoopNode*)((*matches)[0]))->returnLoopValues(
				(*(*innerMapLoopTbl)[idx])[from_tagname_pos]->myValue() 
				);
		    ---- comment out end ----- */

		    LoopTableNode *fromLoopTbl = ((DataLoopNode*)((*matches)[0]))->getValsPtr();
		    int fromTagFromIdx = ((DataLoopNode*)((*matches)[0]))->
			    getNames()[0]->tagPosition(
				    (*(*innerMapLoopTbl)[idx])[from_tagname_pos]->myValue() );
		    for( i = 0 ; i < fromLoopTbl->size() ; i++ )
		    {
			LoopRowNode *newRow = new LoopRowNode( true );

			/*---- comment-out: old code did this:
			     newRow->insert( newRow->end(), new DataValueNode( *( (*oldVals)[i] ) ) );
			-----comment-out end */

			newRow->insert( newRow->end(), *( (*(*fromLoopTbl)[i])[fromTagFromIdx] ) );
			newLoop->getValsPtr()->insert( newLoop->getValsPtr()->end(), newRow );
		        ValCache::flushValCache();
		    }
		    newSaveFrame->GiveMyDataList()->insert( newSaveFrame->GiveMyDataList()->end(), newLoop );

		    // If we were supposed to delete the old version,
		    // do so now:
		    string copy_or_move_flag = (*(*innerMapLoopTbl)[idx])[copy_or_move_flag_pos]-> myValue();
		    if( copy_or_move_flag == string( "M" ) )
		    {
			DataLoopNode *parCopy = (DataLoopNode*) ( (*matches)[0]->myParallelCopy() );
			if( parCopy != NULL )
			{
			    if( parCopy->getNames().size() == 0 )
			    {   (*errStream) << "#transform-9# ERROR: Cannot "<<
				    "delete old name from DataLoopNode (SHOULD NOT HAPPEN)" <<
				    endl;
				return;
			    }

			    // Delete the tag from the parallel loop.  Note
			    // that if there is only one tag left in the
			    // loop (the tag we are about to delete), then we
			    // will just delete the whole loop altogether
			    // instead of leaving an empty loop behind.
			    if( parCopy->getNames()[0]->size() == 1 )
			    {
				delete parCopy;
			    }
			    else
			    {   for( i = 0 ; i < parCopy->getNames()[0]->size() ; i++ )
				{
				    if( (*(parCopy->getNames()[0]))[i]->myName() == 
					  (*(*innerMapLoopTbl)[idx])[from_tagname_pos]->myValue() )
				    {
					LoopNameListNode *eraseFrom = parCopy->getNames()[0];
					eraseFrom->erase( eraseFrom->iteratorFor( (*(eraseFrom))[i] ) );
					break;
				    }
				}
			    }
			}
		    }
		}
		delete matches;
	    } //end-for
	    //
	    // Insert the newly made saveframe at the end of the
	    // output star file:
	    //
	    List<ASTnode*> *outputBlocks = outStar->searchForType( ASTnode::BLOCKNODE );
	    BlockNode *lastOutputBlock = (BlockNode*)( (*outputBlocks)[outputBlocks->size() - 1] );
	    delete outputBlocks;
	    lastOutputBlock->GiveMyDataList()->insert( lastOutputBlock->GiveMyDataList()->end(), newSaveFrame );
	}
	else
	{
	    delete newSaveFrame;
	}


    } //end-for
}

