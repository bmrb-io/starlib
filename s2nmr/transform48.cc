
///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

void save_remove_sf_if_missing_tag(
                         StarFileNode *inStar, 
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
    int                   needed_tag_pos;

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
    needed_tag_pos = -1;
    for( int i = 0 ; i < (*mapNames)[0]->size() ; i++ )
    {
	if(      (* ((*mapNames)[0]) )[i]->myName()==string( "_key_tag" ) )
	    key_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_key_value" ) )
	    key_value_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_needed_tag" ) )
	    needed_tag_pos = i;
    }
    if( key_tag_pos < 0  )
    {   (*errStream) << "#transform-16# ERROR: input mapping file is missing tag '_key_tag'." << endl;
	return; /* do nothing */
    }
    if( key_value_pos < 0  )
    {   (*errStream) << "#transform-16# ERROR: input mapping file is missing tag '_key_value'." << endl;
	return; /* do nothing */
    }
    if( needed_tag_pos < 0  )
    {   (*errStream) << "#transform-16# ERROR: input mapping file is missing tag '_needed_tag'." << endl;
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
	    List<ASTnode*> *matchVal = (*matchSF)[sf_i]->searchByTag(
			(*(*mapLoopTbl)[mL_Idx])[needed_tag_pos]->myValue() );

	    // Should be a match.  If there is not, delete the matching saveframe
	    // we are in:
	    if( matchVal->size() == 0 )
	    {
	        delete ( (*matchSF)[sf_i]->myParallelCopy() );
	    }
	    delete matchVal;
	}
	delete matchSF;
    } //end-for
}

