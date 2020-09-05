///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.1  2005/11/07 20:21:20  madings
*** empty log message ***

Revision 1.1  2005/03/18 21:07:20  madings
Massive update after too long without a checkin

*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

void save_split_ambiguity_codes(  StarFileNode *inStar, 
                                StarFileNode *outStar, 
                                BlockNode    *,
			        SaveFrameNode *currDicRuleSF)
{
    int matches_i;
    List<ASTnode*> *matches = outStar->searchForTypeByTag( ASTnode::DATALOOPNODE, "_Ambiguous_atom_chem_shift.Atom_chem_shift_ID" );
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
	{   dln->getNames()[0]->insert( dln->getNames()[0]->end(), new DataNameNode( setIdTagName ) );
	    setIdCol = dln->getNames()[0]->size() -1;
	}
	if( shiftIdCol < 0 )
	{   (*errStream)<<"#transform72# Error: No column called "<<shiftIdTagName<<" exists in the loop.\n"<<endl;
	    break;
	}

	LoopTableNode *tbl = dln->getValsPtr();
	int rowNum;
	int largestSetIdSoFar = 0 ;
	for( rowNum = 0 ; rowNum < tbl->size() ; ++rowNum )
	{
	    DataValueNode *setIdNode = (*( (*tbl)[rowNum] ))[setIdCol];
	    int thisID =  atoi( setIdNode->myValue().c_str() );
	    if( thisID > largestSetIdSoFar )
	    {
	        largestSetIdSoFar = thisID;
	    }
	}
	for( rowNum = 0 ; rowNum < tbl->size() ; ++rowNum )
	{
	    LoopRowNode   *thisRow = (*tbl)[rowNum];
	    DataValueNode *setIdNode = (*thisRow)[setIdCol];
	    DataValueNode *shiftIdNode = (*thisRow)[shiftIdCol];

	    // only edit the row if it has commas in it, else leave it alone:
	    if( strchr( shiftIdNode->myValue().c_str(), ',' ) != NULL )
	    {
	        ++largestSetIdSoFar;
		char setIdStr[20];
		sprintf(setIdStr, "%d", largestSetIdSoFar );
		string oldVal = shiftIdNode->myValue();
		string newVal = string("");
		string tmpVal = string("");
		while( oldVal.length() > 0 )
		{
		   string::size_type commaPos = oldVal.find( ',' );
		   if( commaPos == string::npos )
		   {
		       commaPos = oldVal.length();
		   }
		   tmpVal = oldVal.substr( 0, commaPos );
		   oldVal.erase( 0, commaPos+1 );

		   LoopRowNode *newRow = new LoopRowNode( *thisRow );
		   (*newRow)[setIdCol]->setValue( string( setIdStr ) );
		   (*newRow)[shiftIdCol]->setValue( tmpVal );
		   tbl->insert( tbl->begin() + rowNum + 1, newRow );
		}
	        delete thisRow; // destructor should erase it from the table list.
	    }
	    else
	    {
	        // leave it alone as-is.
	    }
	}
    }
    delete matches;
}
