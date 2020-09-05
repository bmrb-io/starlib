///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"


void save_change_tag_delim(  StarFileNode *inStar, 
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
    int                   tag_with_a_value_to_change_pos;
    int                   new_tag_delim_pos;

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
    tag_with_a_value_to_change_pos = -1;
    new_tag_delim_pos = -1;
    for( int i = 0 ; i < (*mapNames)[0]->size() ; i++ )
    {
	if(      (* ((*mapNames)[0]) )[i]->myName()==string( "_key_tag" ) )
	    key_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_key_value" ) )
	    key_value_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_tag_with_a_value_to_change" ) )
	    tag_with_a_value_to_change_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_new_tag_delim" ) )
	    new_tag_delim_pos = i;
    }
    if( key_tag_pos < 0  )
    {   (*errStream) << "#transform-15# ERROR: input mapping file is missing tag '_key_tag'." << endl;
	return; /* do nothing */
    }
    if( key_value_pos < 0  )
    {   (*errStream) << "#transform-15# ERROR: input mapping file is missing tag '_key_value'." << endl;
	return; /* do nothing */
    }
    if( tag_with_a_value_to_change_pos < 0  )
    {   (*errStream) << "#transform-15# ERROR: input mapping file is missing tag 'tag_with_a_value_to_change'." << endl;
	return; /* do nothing */
    }
    if( new_tag_delim_pos < 0  )
    {   (*errStream) << "#transform-15# ERROR: input mapping file is missing tag '_new_tag_value'." << endl;
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
			(*(*mapLoopTbl)[mL_Idx])[tag_with_a_value_to_change_pos]->myValue() );

	    for( int i = 0 ; i < matchVal->size() ; i++ )
	    {
		// If it is a data item node, then change the parallel copy's delimiter type:
		if( (*matchVal)[i]->isOfType( ASTnode::DATAITEMNODE ) )
		{
		    DataItemNode *peerItem = (DataItemNode*) (*matchVal)[i]->myParallelCopy();
		    if( peerItem == NULL )
		    {
			(*errStream) << "#transform-15# Error: internal error, no peer at line "
			    << __LINE__ << ".  Fatal error, quitting rule." << endl;
			delete matchVal;
			delete matchSF;
			return;
		    }
		    peerItem->setDelimType( (*(*mapLoopTbl)[mL_Idx])[new_tag_delim_pos]->myDelimType() );
		}
		// Else it's a name node (in a loop), so change the values for the column:
		else if((*matchVal)[i]->isOfType( ASTnode::DATANAMENODE ) )
		{
		    int rowNum;
		    int col;
		    int nest;
		    ASTnode *cur = ((*matchVal)[i]);
		    while( cur != NULL && ! cur->isOfType(ASTnode::DATALOOPNODE) )
		    {
		        cur = cur->myParent();
		    }
		    if( cur != NULL )
		    {
		        DataLoopNode *dln = (DataLoopNode*)cur;
			dln->tagPositionDeep( 
			        (*(*mapLoopTbl)[mL_Idx])[tag_with_a_value_to_change_pos]->myValue(),
				&nest,
				&col );
			if( col >= 0 )
			{
			   for( rowNum = 0 ; rowNum < dln->getValsPtr()->size() ; rowNum++ )
			   {
			       DataValueNode *peerVal = (DataValueNode*)( ( *( dln->getVals()[rowNum] ) )[col]->myParallelCopy() );
			       peerVal->setDelimType( (*(*mapLoopTbl)[mL_Idx])[new_tag_delim_pos]->myDelimType() );
			   }
			}
		    }
		}
		// Else it's unexpected and I have no clue what to do:
		else
		{
		    (*errStream) << "#transform-15# Error: internal error, found a type of " <<
			"match I don't understand at line " << __LINE__ <<
			".  Fatal error, quitting rule." << endl;
		    delete matchVal;
		    delete matchSF;
		    return;
		}
	    }
	    delete matchVal;
	}
	delete matchSF;
    } //end-for
}
