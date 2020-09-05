///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.1  2005/03/18 21:07:20  madings
Massive update after too long without a checkin

*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"


void save_merge_loop_rows_to_text(  StarFileNode *inStar, 
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
    int                   looped_tag_pos;
    int                   new_text_tag_pos;

    // Find loop from the mapping file:
    // --------------------------------
    mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, string( "_key_tag" ) );
    if( mapLoopMatch->size() < 0 )
    {
	(*errStream) << "#transform-15# ERROR: no loop tag called '_key_tag' in mapping file." << endl;
	delete mapLoopMatch;
	return; /* do nothing */
    }
    mapLoopTbl = ((DataLoopNode*)(*mapLoopMatch)[0])->getValsPtr();
    mapNames =  ((DataLoopNode*)(*mapLoopMatch)[0])->getNamesPtr();

    delete mapLoopMatch;

    key_tag_pos = -1;
    key_value_pos = -1;
    looped_tag_pos = -1;
    new_text_tag_pos = -1;
    for( int i = 0 ; i < (*mapNames)[0]->size() ; i++ )
    {
	if(      (* ((*mapNames)[0]) )[i]->myName()==string( "_key_tag" ) )
	    key_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_key_value" ) )
	    key_value_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_looped_tag" ) )
	    looped_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_new_text_tag" ) )
	    new_text_tag_pos = i;
    }
    if( key_tag_pos < 0  )
    {   (*errStream) << "#transform-68# ERROR: input mapping file is missing tag '_key_tag'." << endl;
	return; /* do nothing */
    }
    if( key_value_pos < 0  )
    {   (*errStream) << "#transform-68# ERROR: input mapping file is missing tag '_key_value'." << endl;
	return; /* do nothing */
    }
    if( looped_tag_pos < 0  )
    {   (*errStream) << "#transform-68# ERROR: input mapping file is missing tag '_looped_tag'." << endl;
	return; /* do nothing */
    }
    if( new_text_tag_pos < 0  )
    {   (*errStream) << "#transform-68# ERROR: input mapping file is missing tag '_new_text_tag'." << endl;
	return; /* do nothing */
    }


    // For each occurance of the tag/value, change it:
    // -----------------------------------------------
    for( mL_Idx = 0 ; mL_Idx < mapLoopTbl->size() ; mL_Idx++ )
    {
	List<ASTnode*> *matchSF;
	if( (*(*mapLoopTbl)[mL_Idx])[key_value_pos]->myValue() == string("*") )
	{
	    matchSF = AST->searchForType( ASTnode::SAVEFRAMENODE );
	}
	else
	{
	    matchSF = AST->searchForTypeByTagValue(
			    ASTnode::SAVEFRAMENODE, 
			    (*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue(),
			    (*(*mapLoopTbl)[mL_Idx])[key_value_pos]->myValue() );
        }

	// For each matched saveframe, get each matching value:
	for( int sf_i = 0 ; sf_i < matchSF->size() ; sf_i++ )
	{
	    List<ASTnode*> *matchVal = (*matchSF)[sf_i]->searchByTag(
			(*(*mapLoopTbl)[mL_Idx])[looped_tag_pos]->myValue() );

	    for( int i = 0 ; i < matchVal->size() ; i++ )
	    {
		// If it is a data item node, then do nothing
		if( (*matchVal)[i]->isOfType( ASTnode::DATAITEMNODE ) )
		{
		}
		// Else it's a loop node, so do the change
		else if((*matchVal)[i]->isOfType( ASTnode::DATANAMENODE ) )
		{
		    string newValue = string("");
		    ASTnode *cur = (*matchVal)[i];
		    while( cur != NULL && !(  cur->isOfType(ASTnode::DATALOOPNODE) ) ) 
		    {
		      cur = cur->myParent();
		    }

		    if( cur == NULL )
		    {
                        (*errStream) << "#transform-68# ERROR: No DataLoopNode parent of match." << endl;
		        continue;
		    }

		    DataLoopNode *dln = (DataLoopNode*)cur;
		    LoopTableNode *tbl = dln->getValsPtr();
		    int rowNum, colNum;
		    for( rowNum = 0 ; rowNum < tbl->size(); rowNum++ )
		    {
		        LoopRowNode *lrn = (*tbl)[rowNum];
		        for( colNum = 0 ; colNum < lrn->size(); colNum++ )
		        {
		            newValue += (*lrn)[colNum]->myValue();
			    newValue += " ";
		        }
			newValue += "\n";
		    }

		    // Now find the appropriate place to put the new tag:
		    while( cur != NULL && !( cur->isOfType(ASTnode::SAVEFRAMENODE) ) ) 
		    {
		      cur = cur->myParent();
		    }
		    if( cur == NULL )
		    {
                        (*errStream) << "#transform-68# ERROR: No SaveFrameNode parent of match." << endl;
		        continue;
		    }
		    ASTnode *parCopy = cur->myParallelCopy();
		    if( parCopy == NULL )
		    {
                        (*errStream) << "#transform-68# ERROR: No parellel copy of saveframe parent of match." << endl;
		        continue;
		    }
		    SaveFrameNode *sfn = (SaveFrameNode *)parCopy;
		    sfn->GiveMyDataList()->insert(
		        sfn->GiveMyDataList()->end(),
		        new DataItemNode( 
				(*(*mapLoopTbl)[mL_Idx])[new_text_tag_pos]->myValue(),
				newValue,
				DataValueNode::SEMICOLON )
			);
	        }
	    }
	    delete matchVal;
	}
	delete matchSF;
    } //end-for
}
