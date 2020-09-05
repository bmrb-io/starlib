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

void save_remove_empty_frames(  StarFileNode *inStar, 
                                StarFileNode *outStar, 
                                BlockNode    *,
				SaveFrameNode *currDicRuleSF)
{
    DataNode        *currDicSaveFrame;
    List<ASTnode*>  *listOfFrames;
    List<ASTnode*>  *listOfDataItems;
    int             frameIdx;
    int             dataIdx;
    int             loopTagIdx;
    bool            isEmpty;
    string          sfCatStr;
    DataLoopNode    *thisLoopPtr;
    SaveFrameNode   *thisFramePtr;

    sfCatStr = string( "_Saveframe_category" );  // used in comparasin below

    currDicSaveFrame = currDicRuleSF;

    if(!currDicSaveFrame)//Return if the target saveframe is not found
	return; 

    // First, get a list of all the saveframes that exist in the
    // input STAR tree structure:
    listOfFrames = inStar->searchForType(ASTnode::SAVEFRAMENODE);

    // For each such saveframe, see if it is empty in the input Star Tree.
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    for( frameIdx = 0 ; frameIdx < listOfFrames->size() ; frameIdx++ )
    {
	thisFramePtr = (SaveFrameNode*)( (*listOfFrames)[frameIdx] );

	isEmpty = true; // Assume saveframe is empty until proven otherwise.

	// Get the innards of the saveframe (tags and loops):
	listOfDataItems = thisFramePtr->searchForType( ASTnode::DATANODE );

	// Note - the way I did this, I end up getting the saveframe itself
	// also as a hit (since a saveframe is of type DataNode).  I will
	// filter out this false hit down below.

	// If any matches are found that were not _Saveframe_category tags,
	// or loops of no rows, then the frame is not empty:
	for( dataIdx = 0 ; dataIdx < listOfDataItems->size() ; dataIdx++ )
	{
	    if( (*listOfDataItems)[dataIdx]->myType() ==
		            ASTnode::SAVEFRAMENODE )
	    {   // Do nothing - this is a false hit (the saveframe itself
		// is matched by the "searchForType()" call above.)
	    }
	    else if( (*listOfDataItems)[dataIdx]->myType() ==
		            ASTnode::DATAITEMNODE )
	    {
		if( ( (DataItemNode*)((*listOfDataItems)[dataIdx]) )->myName()
				!= sfCatStr   )
		{   isEmpty = false;
		}
	    }
	    else // a dataloop node - see if a tag exists inside:
	    {
		thisLoopPtr = (DataLoopNode*)( (*listOfDataItems)[dataIdx] );
		if( thisLoopPtr->getValsPtr() &&
		    thisLoopPtr->getValsPtr()->size() > 0 )
			isEmpty = false;
	    }
	}
	delete listOfDataItems;

	// We now know if the saveframe was empty or not.  If so,
	// remove it from the parallel output star tree:
	if( isEmpty )
	{
	    // Because some rules ruin the integerety of the
	    // tree, this routine can't safely delete - it has
	    // to just unlink (sloppy, I know):  Calling
	    // the destructor ends up core dumping on a lot
	    // of the files:
	    thisFramePtr->myParallelCopy()->unlinkMe();
	}
    }
    delete listOfFrames;
}
