///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
// Revision 1.1  1999/07/30  18:28:57  june
// 7-30-99, June
// add transform1d.cc, transform26a.cc, transform38.cc, transform39.cc,
//  updated transform17.cc, doc/specs.html
//
created 7/28/99, by june
This function will delete all the tags in datablock which are 
not in saveframe. It's used for conversion from mmCIF file 
to nmrSTAR file as last step after all useful information 
extracted.
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

void save_remove_tags_not_in_saveframe(  StarFileNode *inStar, 
                                StarFileNode *outStar, 
                                BlockNode    *,
				SaveFrameNode *currDicRuleSF)
{
    DataNode        *currDicSaveFrame;
    List<ASTnode*>  *listOfDataNodes;
    int             dataIdx;
    DataNode   *thisDataPtr;

    currDicSaveFrame = currDicRuleSF;

    if(!currDicSaveFrame)//Return if the target saveframe is not found
	return; 

    // First, get a list of all the dataNodes that exist in the
    // input STAR tree structure:
    listOfDataNodes = inStar->searchForType(ASTnode::DATANODE);

    // For each dataNode.
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    for( dataIdx = 0 ; dataIdx < listOfDataNodes->size() ; dataIdx++ )
    {
	    if( (*listOfDataNodes)[dataIdx]->myType() ==
		            ASTnode::SAVEFRAMENODE )
	    {   // Do nothing 
	    }
	    else if( (*listOfDataNodes)[dataIdx]->myType() ==
		            ASTnode::DATAITEMNODE )
	    {
		thisDataPtr = (DataNode*)((*listOfDataNodes)[dataIdx]);
		//check whether data in saveframe
		ASTnode *par = thisDataPtr->myParent();
		while(par!=NULL && ! par->isOfType(ASTnode::SAVEFRAMENODE))
		  par = par->myParent();
		if( !par)
		  delete thisDataPtr->myParallelCopy();
	    }
	    else if( (*listOfDataNodes)[dataIdx]->myType() ==
		            ASTnode::DATALOOPNODE )
	    {
		thisDataPtr = (DataNode*)((*listOfDataNodes)[dataIdx]);
		//check whether data in saveframe
		ASTnode *par = thisDataPtr->myParent();
		while(par!=NULL && ! par->isOfType(ASTnode::SAVEFRAMENODE))
		  par = par->myParent();
		if( !par)
		  delete thisDataPtr->myParallelCopy();
	    }
    }
	delete listOfDataNodes;
}
