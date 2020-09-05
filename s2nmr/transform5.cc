///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.6  2000/11/18 03:02:39  madings
*** empty log message ***

// Revision 1.5  2000/03/16  05:09:42  madings
// Jurgen discovered some minor bugs in s2nmr and the fixes are installed:
//
//    1 - remove_null_tags was previously unable to handle asterisks
//       as wildcards when filling out the key_tag and key_value field.
//       This is fixed.
//
//    2 - The same goes for remove_tag, which will now take an asterisk
//       as the saveframe name, meaning all safeframes.
//
//    3 - The documentation on the internal website had some typos in
//       tag names.
//
// Revision 1.4  1999/07/30  18:22:09  june
// june, 7-30-99
// add transform1d.cc
//
Revision 1.3  1999/01/28 06:15:07  madings
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

// Revision 1.2  1998/11/11  04:08:25  madings
// Added transform6, but did so in such a way that CVS is falsely
// marking every single file as being changed, so this comment will
// probably appear in many files where it does not apply.
//
// Revision 1.4  1997/12/09  17:28:00  bsri
// Modified all the transformations so that the AST tree is searched for
// tags and save frames, and appropriate changes are made to the
// corresponding save frames in the NMR tree (by following pointers from
// the AST tree to the NMR tree: this is done by using myParallelCopy()).
//
// Revision 1.3  1997/10/21  21:29:55  bsri
//    This transformation code has been changed so that it no longer iterates
//    through lists of AST nodes, instead, uses the starlib API.
//
// Revision 1.2  1997/10/01  22:38:59  bsri
// Replaced transform2.cc with new file, and added RCS Log comment
// to all the transform*.cc files.
//
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

  ////////////////////////////
 //    save_remove_tag     //
////////////////////////////
void save_remove_tag( StarFileNode *input,
	              StarFileNode *output,
		      BlockNode    *,
		      SaveFrameNode *currDicRuleSF)
{
  
    DataNode* currDicSaveFrame = currDicRuleSF;

    if(!currDicSaveFrame)
	return;     //Simply return if the desire saveframe is not found


    //Get the loop in the data dictionary
    string currDicLoopName = string("_old_tag_name");
    DataNode* currDicLoop = 
		((SaveFrameNode*)currDicSaveFrame)->ReturnSaveFrameDataNode(currDicLoopName);

    //Flatten the nested loop
    List<DataNameNode*>* currDicNameList;
    List<DataValueNode*>* currDicValList;
    ((DataLoopNode*) currDicLoop)->FlattenNestedLoop(currDicNameList,currDicValList);
    int length = currDicNameList->Length();


    //This transformation is applied on a datablock basis

	//For each datablock we will apply this transformation
	currDicValList->Reset();
	string*  iterVal = new string[length];
      
	//This while loop processes successive iterations in the dictionary loop
	while(!currDicValList->AtEnd())
	{
	    //Get the values for one loop iteration
	    for(int i =0; i<length ; i++)
	    {
	iterVal[i] = tagToVal(currDicValList->Current()->myName());
		currDicValList->Next();
	    }
	    //Apply this change to the relevant saveframes within this datablock

	    //Retrieve the appropriate saveframes containing tag to be changed
	    List<ASTnode *> *saveFrameHits = NULL;
	    if( strcmp( iterVal[1].c_str(), "*" ) == 0 )
		saveFrameHits = AST->searchForType(ASTnode::SAVEFRAMENODE);
	    else
		saveFrameHits = AST->searchByTag(iterVal[1]);

	    ASTnode *saveFrameCur;
 	
	    for ( saveFrameHits->Reset(); !saveFrameHits->AtEnd(); saveFrameHits->Next() )
	    {
		saveFrameCur = saveFrameHits->Current();
		if ( saveFrameCur->isOfType(ASTnode::SAVEFRAMENODE) )
		{
		    List<ASTnode *> *tagHits = 
			((SaveFrameNode *)saveFrameCur)->searchByTag(iterVal[0]);
	    	    for ( tagHits->Reset(); !tagHits->AtEnd(); tagHits->Next() )
	    	    {
		        ASTnode *astTagCur = tagHits->Current();
			ASTnode *nmrTagPeer;

		    	if ( astTagCur->isOfType(ASTnode::DATAITEMNODE) )
			{
			     nmrTagPeer = astTagCur->myParallelCopy();
			     if (nmrTagPeer != NULL )
			     {
				delete ((DataItemNode *)nmrTagPeer);
				nmrTagPeer = NULL;
			     }
			}
		    	else
		    	{
			     nmrTagPeer = NULL;
			     // If the delete-all-loop flag is set to "L",
			     // then walk up to the parent loop and just delete
			     // the whole loop.
			     if( length >= 3 &&
			         strcmp( iterVal[2].c_str(), "L" ) == 0 )
			     {
			         while( astTagCur != NULL &&
			                ! astTagCur->isOfType(ASTnode::DATALOOPNODE) )
				 {
				     astTagCur = astTagCur->myParent();
				 }
			     }
			     if( astTagCur != NULL )
			         nmrTagPeer = astTagCur->myParallelCopy();
			     if (nmrTagPeer != NULL )
			     {
				delete ((DataNameNode *)nmrTagPeer);
				nmrTagPeer = NULL;
			     }
			}
		    }
		    delete tagHits;
		}
	    }
	    delete saveFrameHits;
	}

}	
