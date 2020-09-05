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

void save_join_ambiguity_codes(  StarFileNode *inStar, 
                                StarFileNode *outStar, 
                                BlockNode    *,
			        SaveFrameNode *currDicRuleSF)
{
    int matches_i;
    List<ASTnode*> *matches = outStar->searchForTypeByTag( ASTnode::DATALOOPNODE, "_Ambiguous_atom_chem_shift.Ambiguous_shift_set_ID" );
    string setIdTagName = string("_Ambiguous_atom_chem_shift.Ambiguous_shift_set_ID");
    string shiftIdTagName = string("_Ambiguous_atom_chem_shift.Atom_chem_shift_ID");

    // For each such loop, do the joining:
    // This algorithm assumes the groups are sorted contiguously in the loop.
    for( matches_i = 0 ; matches_i < matches->size() ; matches_i++ )
    {
	DataLoopNode *dln = (DataLoopNode*) ( (*matches)[matches_i] );
	int setIdCol, shiftIdCol;
	int dummyNest;
	dln->tagPositionDeep( setIdTagName, &dummyNest, &setIdCol );
	dln->tagPositionDeep( shiftIdTagName, &dummyNest, &shiftIdCol );

	if( setIdCol < 0 )
	{   (*errStream)<<"#transform67# Error: No column called "<<setIdTagName<<" exists in the loop.\n"<<endl;
	    break;
	}
	if( shiftIdCol < 0 )
	{   (*errStream)<<"#transform67# Error: No column called "<<shiftIdTagName<<" exists in the loop.\n"<<endl;
	    break;
	}

	LoopTableNode *tbl = dln->getValsPtr();
	int rowNum;
	int prevRow = -1;
	for( rowNum = 0 ; rowNum < tbl->size() ; ++rowNum )
	{
	    DataValueNode *setIdNode = (*( (*tbl)[rowNum] ))[setIdCol];
	    DataValueNode *shiftIdNode = (*( (*tbl)[rowNum] ))[shiftIdCol];

	    if( prevRow > 0 && setIdNode->myValue() == (*( (*tbl)[prevRow] ))[setIdCol]->myValue() )
	    {
	        (*( (*tbl)[prevRow] ))[shiftIdCol]->setValue(
		    (*( (*tbl)[prevRow] ))[shiftIdCol]->myValue() + "," + shiftIdNode->myValue() );
		// since the value changed, see if the delimiter needs to change:
		char qStyle = FindNeededQuoteStyle( (*( (*tbl)[prevRow] ))[shiftIdCol]->myValue() );
		if( qStyle = '\'' )
	            (*( (*tbl)[prevRow] ))[shiftIdCol]->setDelimType( DataValueNode::SINGLE );
		else if( qStyle = '\"' )
	            (*( (*tbl)[prevRow] ))[shiftIdCol]->setDelimType( DataValueNode::DOUBLE );
		else if( qStyle = ';' )
	            (*( (*tbl)[prevRow] ))[shiftIdCol]->setDelimType( DataValueNode::SEMICOLON );
		else
	            (*( (*tbl)[prevRow] ))[shiftIdCol]->setDelimType( DataValueNode::DOUBLE);

	        // Remove the current row, decrement the row number so the for loop stays here.
		// that way it skips the current value that we just tacked onto the end of the
		// previous row's value.
		tbl->erase( tbl->begin() + rowNum );
		--rowNum;
	    }
	    else
	    {   // Do nothing, skip over the nice new group ID.
		prevRow = rowNum;
	    }

	}
    }
    delete matches;
}
