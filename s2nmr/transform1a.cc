#ifdef BOGUS_DEF_COMMENT_OUT
//  $Log: not supported by cvs2svn $
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
// Revision 1.4  1999/07/30  18:22:07  june
// june, 7-30-99
// add transform1d.cc
//
//  Revision 1.3  1999/01/28 06:15:05  madings
//  Some bugfixes from Eldon's attempts to use this, as well as a major
//  design change:
//
//     Bug fix 1: save_loops was re-creating tags that had been removed
//        earlier in the run.  This has been fixed.
//
//     Bug fix x: after bug fix 1 above, save_loops was leaving behind
//        loops that had no contents.  This has been fixed.
//
//     Massive design change:
//        There is no longer any default order of operations at all.
//        Instead, it uses the new 'save_order_of_operations' saveframe
//        in the mapping dictionary file to decide what rules to run
//        when.  Also, to allow it to have the same rule run more than
//        once at different times from different mapping dictionary
//        saveframes, the name of the saveframe is no longer the rule
//        to run.  Instead the name of the saveframe is irrelevant, and
//        it's the new _Saveframe_category tag that tells the program
//        what rule to run for that saveframe.  This required a small,
//        but repetative change in all the transform rules so that the
//        main.cc program can tell the transforms which saveframe in the
//        mapping file is the one they are supposed to use instead of the
//        transforms trying to look for it themselves.
//
// Revision 1.2  1998/11/11  04:08:17  madings
// Added transform6, but did so in such a way that CVS is falsely
// marking every single file as being changed, so this comment will
// probably appear in many files where it does not apply.
//
// Revision 1.7  1997/12/09  17:25:27  bsri
// Modified all the transformations so that the AST tree is searched for
// tags and save frames, and appropriate changes are made to the
// corresponding save frames in the NMR tree (by following pointers from
// the AST tree to the NMR tree: this is done by using myParallelCopy()).
//
// Revision 1.6  1997/10/21  21:07:25  bsri
// This transformation code has been changed so that it no longer iterates
// through lists of AST nodes, instead, use the starlib API.
//
#endif
#include "transforms.h"
//########################################################################################
//#######################################################################################

void save_renamed_tags(StarFileNode* AST,
		       StarFileNode* NMR,
		       BlockNode*    ,
		       SaveFrameNode *currDicRuleSF)
{

//The function applies transformation 1a  for a specified dictionary datablock
//First get the "save_renamed_tags" saveframe in the current dictionary datablock
  
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

  ASTlist<BlockNode*>* listOfDataBlocks =  AST->GiveMyDataBlockList();
    
  listOfDataBlocks->Reset();
  while(!listOfDataBlocks->AtEnd())
  {
      BlockNode* currDatablock    = listOfDataBlocks->Current();
      string currDatablockName    = currDatablock->myName();

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

	  // Special case - skip if this is an identity transform:
	  // -----------------------------------------------------
	  if( tagToVal(iterVal[2]) == tagToVal(iterVal[0])  
		&&  tagToVal(iterVal[1]) == tagToVal(iterVal[3])  )
	      continue;

	  //Retrieve the appropriate saveframe containing tag to be changed
	  DataNode* astSaveFrame = AST->ReturnDataBlockDataNode(currDatablockName,iterVal[1]); 
	  
	  if(astSaveFrame !=  NULL) //Which means such a saveframe does exist
	    {
	      //Get the new saveframe into which the tag must go
	      DataNode* nmrSaveFrame = 
		NMR->ReturnDataBlockDataNode(currDatablockName,iterVal[3]);
	      
	      if((nmrSaveFrame == NULL)||(nmrSaveFrame->myType() != DataNode::SAVEFRAMENODE))
		{
		  (*errStream)<<"#transform-1a# Error....destination saveframe does not exist!!!"<<endl;
		  return;
		}
	      else
		{

		  // -------------- begin comment-out -------------------------------
		  // This has been commented out because it is hard to get it to work
		  // with the change to case-insensitive searches.  (It was preventing
		  // renames of the same tag where all you are changing is case, because
		  // it was detecting the old name already there.)
		  //
		  // //First check if the new_tag_name already exists in the new_saveframe
                  // //if it does, flag an error
		  // List <ASTnode *> *matches;
		  // matches = ((SaveFrameNode*)nmrSaveFrame)->searchByTag(iterVal[2]);
		  //     if (matches->Length())
		  // 	 {
		  // 	   (*errStream) <<"#transform-1a# Error:  The new tag already exists in saveframe"<<endl;
		  // 	   return;
		  // 	 }
		  // delete matches;
		  // //At this point we know that the new tag does not exist in new_saveframe
		  // -------------- end comment-out -------------------------------
		  
		  //Get the item with _old_tag_name from the astSaveFrame
		  List<ASTnode*>* hits =
			((SaveFrameNode*) astSaveFrame)->searchByTag(iterVal[0]);

		  hits -> Reset();
		  while (!hits->AtEnd())
		  {
			ASTnode *astTagCur = hits->Current();
			if (astTagCur->isOfType (ASTnode::DATAITEMNODE))
			{
				//Construct new data item
				string tagValue = ((DataItemNode *)astTagCur)->myValue();
			  	DataValueNode::ValType tagDelim = 
					((DataItemNode*)astTagCur)->myDelimType();
			  	DataItemNode* newItem = 
					new DataItemNode(iterVal[2],tagValue, tagDelim);
				
				//delete data item from old saveframe from NMR tree
				ASTnode *nmrTagPeer = astTagCur->myParallelCopy();
				if (nmrTagPeer != NULL)
					delete (DataItemNode *)nmrTagPeer;
				
		  		//Insert this item into nmrSaveFrame
			        ((SaveFrameNode*)nmrSaveFrame)->AddItemToSaveFrame(newItem);
				break;
			}
			hits->Next();
		 }
  	         delete hits;
		}
           }
	
	} //End while loop -- for all dictionary entries for this transformation
     
      
      listOfDataBlocks->Next();
    }
}
