///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

void save_free_and_rename_looped_tags(
                                   StarFileNode *inStar, 
                                   StarFileNode *outStar, 
                                   BlockNode    *,
				   SaveFrameNode *dictEntry )
{
    string            tag_to_change = string( "" );
    string            new_tag_name = string( "" );
    List<ASTnode*>    *loopMatch;
    int               lCur;
    DataLoopNode      *curLoop;
    LoopNameListNode  *curNames;
    int               nameIdx;
    LoopTableNode     *curValues;
    DataNameNode      *nameNode;
    DataValueNode     *valNode;
    DataItemNode      *newItem;
    DataLoopNode      *peerLoop;
    LoopNameListNode  *peerNames;
    List<ASTnode*>    *peerMatch;
    int               peerMatchCount;
    ASTnode           *curSF;
    ASTnode           *peerSF;
    
    int               dictIdx;
    List<ASTnode*>    *dictLoops;
    LoopTableNode     *dictTbl;


    // We're going to have to iterate over every loop in the
    // input star tree, so let's get a list of them:
    loopMatch = outStar->searchForType( ASTnode::DATALOOPNODE );

    dictLoops = dictEntry->searchForType( ASTnode::DATALOOPNODE );

    if( dictLoops->size() >= 1 )
       dictTbl = ( (DataLoopNode*)  ( (*dictLoops)[0]) )->getValsPtr();
    else
       return;

    
    for( dictIdx = 0 ; dictIdx < dictTbl->size() ; dictIdx++ )
    {
        tag_to_change = (*((*dictTbl)[dictIdx]))[0]->myValue();
        new_tag_name = (*((*dictTbl)[dictIdx]))[1]->myValue();

	for( lCur = loopMatch->size() - 1 ; lCur >=0 ; lCur-- )
	{
	    curLoop =  (DataLoopNode*) *(loopMatch->begin() + lCur);
	    curNames = curLoop->getNames() [0] ; // outermost nesting level.
	    curValues = curLoop->getValsPtr();


	    // Check if this loop has the tag name in it,
	    // assuming that it can only be in the first nesting level:
	    for( nameIdx = curNames->size() - 1 ; nameIdx >= 0 ; nameIdx-- )
	    {
		if( ( (*curNames)[nameIdx] )->myName() == tag_to_change )
		{
		    nameNode = (*curNames)[nameIdx] ;
		    // Get the first value of the loop which is not null.  only take
		    // a null value if all values are null:
		    // 
		    if( curValues->size() > 0 )
		    {
			int i;
			for( i = 0 ; i < curValues->size() ; i++ )
			{
			    valNode = (*(*curValues)[i])[nameIdx];
			    if( valNode->myValue() != string("?") && valNode->myValue() != string(".") )
			    {
			        break;
			    }
			}
		    }
		    else // loop is empty - error
		    {
			(*errStream) <<
			    "#transform58# ERROR: "<<tag_to_change<<" has no value "<<
			    "in saveframe " <<
			    ( (SaveFrameNode*)curLoop->myParent() )->myName() <<
			    endl;
			continue;
		    }

		    // Now, make a new dataitemnode with that value:
		    newItem = new DataItemNode(
				    new_tag_name,
				    valNode->myValue(),
				    valNode->myDelimType() );
		    
		    if( curLoop == NULL )
		    {   // (should never happen)
			(*errStream) <<
			    "#transform58# ERROR: Paralell Peer loop is empty. " <<
			    "(code's fault, not mapping file." << endl;
			delete loopMatch;
			return;
		    }
		    for(    curSF = curLoop ; 
			    curSF != NULL &&
				! curSF->isOfType(ASTnode::SAVEFRAMENODE) ; 
			    curSF = curSF->myParent()  )
		    { // void-body
		    }

		    // Delete the column from the output tree's loop:
		    // If it's the last column, the library should
		    // autodelete the loop it's inside too.
		    // ----------------------------------------------
		    if( curNames == NULL )
		    {   // (should never happen)
			(*errStream) <<
			    "#transform58# ERROR: Paralell Peer is empty. " <<
			    "(code's fault, not mapping file.)" << endl;
			continue;
		    }

		    // Add the new node to the output tree's saveframe the loop
		    // was in:
		    if( curSF != NULL )
		    {
			ASTlist<DataNode*>   *innards;
			innards = ((SaveFrameNode*)curSF)->GiveMyDataList();
			innards->insert( innards->end(), newItem );
		    }

		    // commented out: Leave the old value in place:
		    // 
		    // if( curNames->size() == 1 )
		    // {
		    //	
		    //	// Take this loop out of the list we are iterating over as well as deleting
		    //	// it from the Star tree:
		    // 	  loopMatch->erase(loopMatch->begin()+lCur);
		    //    delete curLoop;
		    // }
		    // else
		    // {
		    //   curNames->erase( curNames->begin() + nameIdx );
		    // }

		}
	    }
	}
    }
    if( loopMatch != NULL )
        delete loopMatch;
    if( dictLoops != NULL )
        delete dictLoops;
}
