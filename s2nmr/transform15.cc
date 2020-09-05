///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.3  2000/11/18 03:02:37  madings
*** empty log message ***

// Revision 1.2  1999/07/30  18:22:06  june
// june, 7-30-99
// add transform1d.cc
//
Revision 1.1  1999/02/05 01:34:44  madings
Many changes from debug sessions with Eldon:
1 - added new transforms:  transform15.cc and transform16.cc
2 - added 'don't-care' functionality to transform14.cc when
    tag_value is a dot.
3 - Fixed some garbled errStream messages that printed the wrong
    transform # to the log (transform11 printed error messages tagged
    as transform6 for example.)

*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

void save_change_tag_value(  StarFileNode *inStar, 
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
    int                   tag_value_pos;
    int                   new_tag_value_pos;
    int                   second_tag_to_check_pos;
    int                   second_tag_value_pos;

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
    tag_value_pos = -1;
    new_tag_value_pos = -1;
    second_tag_to_check_pos = -1;
    second_tag_value_pos = -1;
    for( int i = 0 ; i < (*mapNames)[0]->size() ; i++ )
    {
	if(      (* ((*mapNames)[0]) )[i]->myName()==string( "_key_tag" ) )
	    key_tag_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_key_value" ) )
	    key_value_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_tag_with_a_value_to_change" ) )
	    tag_with_a_value_to_change_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_tag_value" ) )
	    tag_value_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_new_tag_value" ) )
	    new_tag_value_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_second_tag_to_check" ) )
	    second_tag_to_check_pos = i;
	else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_second_tag_value" ) )
	    second_tag_value_pos = i;
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
    if( tag_value_pos < 0  )
    {   (*errStream) << "#transform-15# ERROR: input mapping file is missing tag '_tag_value'." << endl;
	return; /* do nothing */
    }
    if( new_tag_value_pos < 0  )
    {   (*errStream) << "#transform-15# ERROR: input mapping file is missing tag '_new_tag_value'." << endl;
	return; /* do nothing */
    }

    // For each occurance of the tag/value, change it:
    // -----------------------------------------------
    for( mL_Idx = 0 ; mL_Idx < mapLoopTbl->size() ; mL_Idx++ )
    {
	string keyTagName = (*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue();
	string keyTagVal = (*(*mapLoopTbl)[mL_Idx])[key_value_pos]->myValue() ;
	List<ASTnode*> *matchSF;
	if( keyTagName == "*" ) {
  	    matchSF = AST->searchForType( ASTnode::SAVEFRAMENODE );
	} else if( keyTagVal == "*" ) {
  	    matchSF = AST->searchForTypeByTag( ASTnode::SAVEFRAMENODE, keyTagName );
	} else {
  	    matchSF = AST->searchForTypeByTagValue( ASTnode::SAVEFRAMENODE, keyTagName, keyTagVal );
	}

	// For each matched saveframe, get each matching value:
	for( int sf_i = 0 ; sf_i < matchSF->size() ; sf_i++ )
	{
	    List<ASTnode*> *matchVal = (*matchSF)[sf_i]->searchByTagValue(
			(*(*mapLoopTbl)[mL_Idx])[tag_with_a_value_to_change_pos]->myValue(),
			(*(*mapLoopTbl)[mL_Idx])[tag_value_pos]->myValue() );

	    for( int i = 0 ; i < matchVal->size() ; i++ )
	    {
	        // If this is in a loop row, and if the second_tag_ values are
		// active, then add the second tag check as well:
		// and only do the map if the second tag check also passes.
		if( second_tag_to_check_pos >= 0 && second_tag_value_pos >= 0 )
		{
		    LoopRowNode *lrn = NULL;
		    DataLoopNode *dln = NULL;
		    for( ASTnode *cur = (*matchVal)[i] ; cur != NULL ; cur = cur->myParent() )
		    {
		        if( cur->isOfType( ASTnode::LOOPROWNODE ) )
			{   lrn = (LoopRowNode*) cur;
			}
		        if( cur->isOfType( ASTnode::DATALOOPNODE ) )
			{   dln = (DataLoopNode*) cur;
			    break;
			}
		    }
		    bool okay_do_it = true;
		    if( lrn != NULL && dln != NULL )
		    {
		        okay_do_it = false;
			int nest = -1;
			int col = -1;
		        dln->tagPositionDeep( 
			    (*(*mapLoopTbl)[mL_Idx])[second_tag_to_check_pos]->myValue(),
			    &nest, &col );
		        if( col >= 0 ) 
			{  if( (*lrn)[col]->myValue() 
			              == 
			       (*(*mapLoopTbl)[mL_Idx])[second_tag_value_pos]->myValue() )
			   {
			       okay_do_it = true;
			   }
			}
		    }
		    if( ! okay_do_it )
		    {
		        continue;
		    }
		}

		// If it is a data item node, then change the parallel copy's value:
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
		    peerItem->setValue( (*(*mapLoopTbl)[mL_Idx])[new_tag_value_pos]->myValue() );
		    peerItem->setDelimType( (*(*mapLoopTbl)[mL_Idx])[new_tag_value_pos]->myDelimType() );
		}
		// Else it's a value node (in a loop), so change the value:
		else if((*matchVal)[i]->isOfType( ASTnode::DATAVALUENODE ) )
		{
		    DataValueNode *peerVal = (DataValueNode*) (*matchVal)[i]->myParallelCopy();
		    if( peerVal == NULL )
		    {
			(*errStream) << "#transform-15# Error: internal error, no peer at line "
			    << __LINE__ << ".  Fatal error, quitting rule." << endl;
			delete matchVal;
			delete matchSF;
			return;
		    }
		    peerVal->setValue( (*(*mapLoopTbl)[mL_Idx])[new_tag_value_pos]->myValue() );
		    peerVal->setDelimType( (*(*mapLoopTbl)[mL_Idx])[new_tag_value_pos]->myDelimType() );
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

