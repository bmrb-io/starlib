///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.4  2000/11/18 03:02:39  madings
*** empty log message ***

// Revision 1.3  1999/07/30  18:22:09  june
// june, 7-30-99
// add transform1d.cc
//
Revision 1.2  1999/01/28 06:15:08  madings
Some bugfixes from Eldon's attempts to use this, as well as a major
design change:

   Bug fix 1: save_loops was re-creating tags that had been removed
      earlier in the run.  This has been fixed.

   Bug fix x: after bug fix 1 above, save_loops was leaving behind
      loops that had no contents.  This has been fixed.

   Massive design change:
      There is no longer any default order of operations at all.
      Instead, it uses the new 'save_order_of_operations' saveframe
      in the mapping dictionary file to decide what rules to run
      when.  Also, to allow it to have the same rule run more than
      once at different times from different mapping dictionary
      saveframes, the name of the saveframe is no longer the rule
      to run.  Instead the name of the saveframe is irrelevant, and
      it's the new _Saveframe_category tag that tells the program
      what rule to run for that saveframe.  This required a small,
      but repetative change in all the transform rules so that the
      main.cc program can tell the transforms which saveframe in the
      mapping file is the one they are supposed to use instead of the
      transforms trying to look for it themselves.

// Revision 1.1  1998/11/20  06:33:26  madings
// Added some new transforms on the way to fixing the s2nmr converter.
//
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

void save_free_all_category_tags(  StarFileNode *inStar, 
                                   StarFileNode *outStar, 
                                   BlockNode    *,
				   SaveFrameNode * )
{
    string            sf_cat = string( "_Saveframe_category" );
    List<ASTnode*>    *loopMatch;
    List<ASTnode*>::iterator  lCur;
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
    ASTnode           *peerSF;
    

    // We're going to have to iterate over every loop in the
    // input star tree, so let's get a list of them:
    loopMatch = inStar->searchForType( ASTnode::DATALOOPNODE );

    for( lCur = loopMatch->begin() ; lCur != loopMatch->end(); lCur++ )
    {
	curLoop = (DataLoopNode*) *lCur;
	peerLoop = (DataLoopNode*) curLoop->myParallelCopy();
	curNames = curLoop->getNames() [0] ; // outermost nesting level.
	curValues = curLoop->getValsPtr();

	// Check if this loop has a _Saveframe_category name in it,
	// assuming that it can only be in the first nesting level:
	for( nameIdx = 0 ; nameIdx < curNames->size() ; nameIdx++ )
	{
	    if( ( (*curNames)[nameIdx] )->myName() == sf_cat )
	    {
		nameNode = (*curNames)[nameIdx] ;
		// Get the first value of the loop.  Assume they are all
		// the same value:
		if( curValues->size() > 0 )
		{
		    valNode = (*(*curValues)[0])[nameIdx];
		}
		else // loop is empty - error
		{
		    (*errStream) <<
			"#transform8# ERROR: _Saveframe_category has no value "
			"in saveframe " <<
			( (SaveFrameNode*)curLoop->myParent() )->myName() <<
			endl;
		    delete loopMatch;
		    return;
		}

		// Now, make a new dataitemnode with that value:
		newItem = new DataItemNode(
				nameNode->myName(),
				valNode->myValue(),
				valNode->myDelimType() );
		// Delete the column from the output tree's loop:
		// ----------------------------------------------
		peerNames = (LoopNameListNode*) curNames->myParallelCopy();
		if( peerNames == NULL )
		{   // (should never happen)
		    (*errStream) <<
			"#transform8# ERROR: Paralell Peer is empty. " <<
			"(code's fault, not mapping file." << endl;
		    delete loopMatch;
		    return;
		}
		peerNames->erase(
			peerNames->iteratorFor( (*peerNames)[nameIdx] ) );
		
		if( peerLoop == NULL )
		{   // (should never happen)
		    (*errStream) <<
			"#transform8# ERROR: Paralell Peer loop is empty. " <<
			"(code's fault, not mapping file." << endl;
		    delete loopMatch;
		    return;
		}
		for(    peerSF = peerLoop ; 
			peerSF != NULL &&
			    ! peerSF->isOfType(ASTnode::SAVEFRAMENODE) ; 
			peerSF = peerSF->myParent() )
		{ // void-body
		}
		// If there are no other names left in the loop, remove it:
		peerMatch = peerLoop->searchForType( ASTnode::DATANAMENODE );
		peerMatchCount = (peerMatch == NULL ) ? 0 : peerMatch->size();
		delete peerMatch;
		if( peerMatchCount == 0 )
		{
		    delete peerLoop;
		}

		// Add the new node to the output tree's saveframe the loop
		// was in:
		if( peerSF != NULL )
		{
		    ASTlist<DataNode*>   *innards;
		    innards = ((SaveFrameNode*)peerSF)->GiveMyDataList();
		    innards->insert( innards->begin(), newItem );
		}
	    }
	}
    }
    delete loopMatch;
}
