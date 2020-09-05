///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.6  2000/11/18 03:02:37  madings
*** empty log message ***

// Revision 1.5  1999/07/13  16:12:04  zixia
// *** empty log message ***
//
// Revision 1.4  1999/06/29  19:04:10  zixia
// *** empty log message ***
//
// Revision 1.3  1999/02/05  01:34:44  madings
// Many changes from debug sessions with Eldon:
// 1 - added new transforms:  transform15.cc and transform16.cc
// 2 - added 'don't-care' functionality to transform14.cc when
//     tag_value is a dot.
// 3 - Fixed some garbled errStream messages that printed the wrong
//     transform # to the log (transform11 printed error messages tagged
//     as transform6 for example.)
//
// Revision 1.2  1999/01/28  06:15:05  madings
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
// Revision 1.1  1998/12/15  04:33:07  madings
// Addded transforms 9 though 14.  (Lots of stuff)
//
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

void save_make_new_tag(  StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
			 SaveFrameNode *currDicRuleSF)
{
    string                sf_cat = string( "_Saveframe_category" );
    List<ASTnode*>        *mapLoopMatch;
    DataLoopNameListNode  *mapNames;
    LoopTableNode         *mapLoopTbl;
    int                   mL_Idx; /* map loop index */
    string                tagname;
    int                   key_tag_pos;
    int                   key_value_pos;
    int                   new_tag_pos;
    int                   new_value_pos;

    // Find loop from the mapping file:
    // --------------------------------
    mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, string( "_key_tag" ) );
    if( mapLoopMatch->size() < 0 )
    {
	(*errStream) << "#transform-14# ERROR: no loop tag called '_key_tag' in mapping file." << endl;
	delete mapLoopMatch;
	return; /* do nothing */
    }
    mapLoopTbl = ((DataLoopNode*)(*mapLoopMatch)[0])->getValsPtr();
    mapNames =  ((DataLoopNode*)(*mapLoopMatch)[0])->getNamesPtr();

    delete mapLoopMatch;

    key_tag_pos = -1;
    key_value_pos = -1;
    new_tag_pos = -1;
    new_value_pos = -1;
    for( int i = 0 ; i < (*mapNames)[0]->size() ; i++ )
    {
	if(      (* ((*mapNames)[0]) )[i]->myName()==string( "_key_tag" ) )
	    key_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_key_value" ) )
	    key_value_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_new_tag" ) )
	    new_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_new_value" ) )
	    new_value_pos = i;
    }
    if( key_tag_pos < 0  )
    {   (*errStream) << "#transform-14# ERROR: input mapping file is missing tag '_key_tag'." << endl;
	return; /* do nothing */
    }
    if( key_value_pos < 0  )
    {   (*errStream) << "#transform-14# ERROR: input mapping file is missing tag '_key_value'." << endl;
	return; /* do nothing */
    }
    if( new_tag_pos < 0  )
    {   (*errStream) << "#transform-14# ERROR: input mapping file is missing tag '_new_tag'." << endl;
	return; /* do nothing */
    }
    if( new_value_pos < 0  )
    {   (*errStream) << "#transform-14# ERROR: input mapping file is missing tag '_new_value'." << endl;
	return; /* do nothing */
    }

    // For each row of the map loop, make a new tag in outStar:
    // --------------------------------------------------------------
    for( mL_Idx = 0 ; mL_Idx < mapLoopTbl->size() ; mL_Idx++ )
    {
	List<ASTnode*> *matchSF;
	if( (*(*mapLoopTbl)[mL_Idx])[key_value_pos]->myValue() == "." )
	{
	    matchSF = AST->searchForTypeByTag( ASTnode::SAVEFRAMENODE, 
			(*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue() );
	}
	else
	{
	    matchSF = AST->searchForTypeByTagValue( ASTnode::SAVEFRAMENODE, 
			(*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue(),
			(*(*mapLoopTbl)[mL_Idx])[key_value_pos]->myValue() );
	}
	
	for( int i = 0 ; i < matchSF->size() ; i++ )
	{
	    SaveFrameNode *inputSaveFrame = (SaveFrameNode*)( (*matchSF)[i] );
	    SaveFrameNode *outputSaveFrame = (SaveFrameNode*) inputSaveFrame->myParallelCopy();
	    if( outputSaveFrame == NULL )
	    {
		(*errStream) << "#transform14# ERROR: Saveframe '" <<
		    inputSaveFrame->myName() <<
		    "' has no peer in the output star file." <<
		    endl;
		delete matchSF;
		return;
	    }
	    
	    List<ASTnode *> *matchTag3 = ((SaveFrameNode*)( (*matchSF)[i] ))->searchByTag( (*(*mapLoopTbl)[mL_Idx])[new_tag_pos]->myValue() );
	    if(matchTag3->size() != 0 ){
                    /* JFD elaboration of error message */
                (*errStream)<<"#transform14# ERROR: the tag with name: "
		    <<(*(*mapLoopTbl)[mL_Idx])[new_tag_pos]->myValue()
		    <<" already exists in saveframe: "
                    <<inputSaveFrame->myName()
                    <<"."<<endl;
		delete matchTag3;
		continue;
	    }
	    delete matchTag3;
	    
	    outputSaveFrame->GiveMyDataList()->insert(
		    outputSaveFrame->GiveMyDataList()->end(),
		    new DataItemNode( 
			(*(*mapLoopTbl)[mL_Idx])[new_tag_pos]->myValue(),
			(*(*mapLoopTbl)[mL_Idx])[new_value_pos]->myValue(),
			(*(*mapLoopTbl)[mL_Idx])[new_value_pos]->myDelimType() ) );
	}
	delete matchSF;
    } //end-for
}

