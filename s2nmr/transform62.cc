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

struct stringcmp
{
    bool   operator()( string s1, string s2) const
    {
        return s1 < s2;
    }
};

SaveFrameNode *spawn_new_saveframe( 
                       SaveFrameNode *sourceSF,
		       string keyTag,
		       string keyVal);


void save_decouple_saveframes_3_to_2(
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
    int                   key_splitting_tag_pos;
    int                   sf_category_tag_pos;
    int                   numSaveFramesNow;

    // Find loop from the mapping file:
    // --------------------------------
    mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, string( "_key_splitting_tag" ) );
    if( mapLoopMatch->size() < 0 )
    {
	(*errStream) << "#transform-62# ERROR: no loop tag called '_key_spliiting_tag' in mapping file." << endl;
	delete mapLoopMatch;
	return; /* do nothing */
    }
    mapLoopTbl = ((DataLoopNode*)(*mapLoopMatch)[0])->getValsPtr();
    mapNames =  ((DataLoopNode*)(*mapLoopMatch)[0])->getNamesPtr();

    delete mapLoopMatch;

    key_splitting_tag_pos = -1;
    sf_category_tag_pos = -1;
    for( int i = 0 ; i < (*mapNames)[0]->size() ; i++ )
    {
	if(      (* ((*mapNames)[0]) )[i]->myName()==string( "_key_splitting_tag" ) )
	    key_splitting_tag_pos = i;
	if(      (* ((*mapNames)[0]) )[i]->myName()==string( "_sf_category_tag" ) )
	    sf_category_tag_pos = i;
    }
    if( key_splitting_tag_pos < 0  )
    {   (*errStream) << "#transform-62# ERROR: input mapping file is missing tag '_key_splitting_tag'." << endl;
	return; /* do nothing */
    }
    if( sf_category_tag_pos < 0  )
    {   (*errStream) << "#transform-62# ERROR: input mapping file is missing tag '_sf_category_tag'." << endl;
	return; /* do nothing */
    }

    bool didThisSaveframeSpawn = false;
    for( mL_Idx = 0 ; mL_Idx < mapLoopTbl->size() ; mL_Idx++ )
    {
	List<ASTnode*> *matchSF;
        string key_splitting_tag = (*(*mapLoopTbl)[mL_Idx])[key_splitting_tag_pos]->myValue();
        string sf_category_tag = (*(*mapLoopTbl)[mL_Idx])[sf_category_tag_pos]->myValue();
	matchSF = inStar->searchForTypeByTag( ASTnode::SAVEFRAMENODE,  key_splitting_tag );


	// For each matched saveframe, get each matching value:
	for( int sf_i = 0 ; sf_i < matchSF->size() ; sf_i++ )
	{
	    int  data_sf_new_count = 0;
	    didThisSaveframeSpawn = false;
	    SaveFrameNode *curSF = (SaveFrameNode*) ((*matchSF)[sf_i]);
	    SaveFrameNode *firstSpawnedSf = NULL;
	    bool moreThanOneSfSpawned = false;
	    ASTnode *tmp;
	    for( tmp = curSF ; tmp != NULL && !tmp->isOfType(ASTnode::BLOCKNODE ) ; tmp = tmp->myParent() )
	    {  /*-do-nothing-*/
	    }
	    BlockNode *curDB = (BlockNode*) tmp;

	    List<ASTnode*> *matchVal = curSF->searchByTag( key_splitting_tag );

	    for( int i = 0 ; i < matchVal->size() ; i++ )
	    {
		// If it is a data item node, then the rule makes no sense:
		if( (*matchVal)[i]->isOfType( ASTnode::DATAITEMNODE ) )
		{
		    (*errStream)<<"#transform-62# ERROR: The matching tag for decouple_savefreames_3_to_2 "<<
			"is a free unlooped tag, and thus the rule makes no sense.  (Tag = "<<
			key_splitting_tag<<")"<<endl;
		}
		// Else it's a name node (in a loop):
		else if((*matchVal)[i]->isOfType( ASTnode::DATANAMENODE ) )
		{
		    int rowNum;
		    int col;
		    int nest;

		    map<string,SaveFrameNode*,stringcmp> prevKeyVals;
		    map<string,SaveFrameNode*,stringcmp>::iterator  keySfLookup;
		    SaveFrameNode *keySaveframe;
		    prevKeyVals.clear();

		    ASTnode *cur = ((*matchVal)[i]);
		    while( cur != NULL && ! cur->isOfType(ASTnode::DATALOOPNODE) )
		    {
		        cur = cur->myParent();
		    }
		    if( cur != NULL )
		    {
		        DataLoopNode *dln = (DataLoopNode*)cur;
			dln->tagPositionDeep(  key_splitting_tag, &nest, &col );
			if( col >= 0 )
			{
			   for( rowNum = 0 ; rowNum < dln->getValsPtr()->size() ; rowNum++ )
			   {
			       DataValueNode *thisKeyVal = (*( dln->getVals()[rowNum] ) )[col];
			       keySfLookup = prevKeyVals.find( thisKeyVal->myValue() );
			       if( keySfLookup == prevKeyVals.end() )
			       {
			           keySaveframe = spawn_new_saveframe( curSF,
				                                       key_splitting_tag,
								       thisKeyVal->myValue() );
				   if( firstSpawnedSf == NULL ) {
				     firstSpawnedSf = keySaveframe;
				   } else {
				     moreThanOneSfSpawned = true;
				   }
				   didThisSaveframeSpawn = true;
				   prevKeyVals[thisKeyVal->myValue()] = keySaveframe;
				   BlockNode * parDB = (BlockNode*) (curDB->myParallelCopy());
				   parDB->GiveMyDataList()->AddToEnd( keySaveframe );

				   ++data_sf_new_count;

			       }
			       // Else we already made the SF, so skip this row.
			   }
			}
		    }


		    prevKeyVals.clear();
		}
		// Else it's unexpected and I have no clue what to do:
		else
		{
		    (*errStream) << "#transform-62# Error: internal error, found a type of " <<
			"match I don't understand at line " << __LINE__ <<
			".  Fatal error, quitting rule." << endl;
		    delete matchVal;
		    delete matchSF;
		    return;
		}
	    }
	    if( firstSpawnedSf != NULL && ! moreThanOneSfSpawned ) {
	      // Rename the one saveframe back to what it should be:
	      // (take off the _1 at the end):
	      string sfName = firstSpawnedSf->myName();
	      int lastUnderscorePos = sfName.rfind('_');
	      if( lastUnderscorePos >= 0 ) {
	        firstSpawnedSf->changeName( sfName.substr( 0, lastUnderscorePos ) );
	      }
	    }
	    delete matchVal;
	    // If we made a new one, then get rid of the old one:
	    if( didThisSaveframeSpawn )
	    {
	        if( curSF->myParallelCopy() )
		{
		    SaveFrameNode *outputSF = (SaveFrameNode*) curSF->myParallelCopy();

		    // recalculate the saveframe counts for this type of saveframe:
		    // ------------------------------------------------------------
		    List<ASTnode*>  *sfcatMatches = outputSF->searchByTag( sf_category_tag );
		    if( sfcatMatches->size() > 0 )
		    {
		        if( (*sfcatMatches)[0]->isOfType(ASTnode::DATAITEMNODE) )
			{
			    string thisCat = (  (DataItemNode*)( (*sfcatMatches)[0] )  )->myValue();

			    List<ASTnode*> *countMatches = outStar->searchForTypeByTag( ASTnode::DATALOOPNODE, "_Data_set.Type" );
			    if( countMatches->size() > 0 )
			    {
			        int typeCol = -1 , countCol = -1 , dummyNest = -1 ;
				int count;

				DataLoopNode *countLoop = (DataLoopNode*)( (*countMatches)[0] );
				countLoop->tagPositionDeep( "_Data_set.Type", &dummyNest, &typeCol );
				countLoop->tagPositionDeep( "_Data_set.Count", &dummyNest, &countCol );
				int r;
				for( r = 0 ; r < countLoop->getValsPtr()->size() ; r++ )
				{
				    if( (*(countLoop->getVals()[r]))[typeCol]->myValue() == thisCat )
				    {
				        count = atoi( (*(countLoop->getVals()[r]))[countCol]->myValue().c_str() );
					count += (data_sf_new_count - 1);
					char s[30];
					sprintf( s, "%d", count );
					(*(countLoop->getVals()[r]))[countCol]->setValue( string(s) );
				    }
				}
			    }
			    delete countMatches;
			}
		    }

		    // Now get rid of the old one:
		    outputSF->GiveMyDataList()->erase(
		            outputSF->GiveMyDataList()->begin(),
		            outputSF->GiveMyDataList()->end() );

		    delete sfcatMatches;
		}
	    }
	}
	delete matchSF;
    } //end-for
}

SaveFrameNode *spawn_new_saveframe( 
                       SaveFrameNode *sourceSF,
		       string keyTag,
		       string keyVal)
{
    // First - Use the copy constructor to make a new copy of the whole SF:
    // Second - delete all rows that don't match the given key tag/value
    // Third - rename the new saveframe, using the value as a suffix.
    //
    //  This algorithm is a bit slow, but simple to impliment because it uses
    // previously established code.
    //
    //  In the case of 3.0 tags this leaves behind some incorrect values in the
    //  ID tags that didn't change to reflect the new saveframe ID, but this
    //  rule is really only used when going back to 2.1, where those values
    //  are going to go away anyway so I don't care.

    // First - copy SF:
    SaveFrameNode *retVal = new SaveFrameNode( (*sourceSF) );

    // Second - remove all rows from the loop that don't match the key tag/value:
    int row;
    int col;
    int nest;
    ASTnode *cur = NULL;
    List<ASTnode*> *matches;
    DataLoopNode *curLoop;
    LoopTableNode *curTbl;

    matches = retVal->searchForTypeByTag(  ASTnode::DATALOOPNODE, keyTag );
    if( matches->size() > 0 )
    {   cur = (*matches)[0];
    }
    if( cur == NULL )
    {
        (*errStream)<<"#transform-62# Error: key tag '"<<keyTag<<"' was not inside a loop"<<endl;
	return retVal;
    }
    curLoop = (DataLoopNode*)cur;
    curTbl = curLoop->getValsPtr();
    curLoop->tagPositionDeep(  keyTag, &nest, &col );
    for( row = 0 ; row < curTbl->size() ; row++ )
    {
        if( (*(*curTbl)[row])[col]->myValue() != keyVal )
	{
	    delete (*curTbl)[row];
	    --row; // compensate for the fact that the loop will be shifting up one row.
	}
    }

    // Third - rename the saveframe:
    //   (and find the saveframe's framecode tag and rename it too)
    int i;
    for( i = 0 ; i < retVal->GiveMyDataList()->size() ; i++ )
    {
	if( (*( retVal->GiveMyDataList() ))[i]->isOfType(ASTnode::DATAITEMNODE) )
	{
	    DataItemNode  *thisItem =  (DataItemNode*)(  (*(retVal->GiveMyDataList() ))[i]  );
	    if( thisItem->myName().rfind("_framecode") != string::npos  && 
	        (string("save_") + thisItem->myValue() ) == retVal->myName()  )
	    {
                retVal->changeName( retVal->myName() + string("_") + keyVal );
                thisItem->setValue( thisItem->myValue() + string("_") + keyVal );
	    }
	}
    }
    return retVal;
}

