///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
//  $Log: not supported by cvs2svn $
// Revision 1.4  1999/07/30  18:22:08  june
// june, 7-30-99
// add transform1d.cc
//
//  Revision 1.3  1999/01/28 06:15:07  madings
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
// Revision 1.2  1998/11/11  04:08:23  madings
// Added transform6, but did so in such a way that CVS is falsely
// marking every single file as being changed, so this comment will
// probably appear in many files where it does not apply.
//
// Revision 1.6  1997/12/09  17:27:21  bsri
// Modified all the transformations so that the AST tree is searched for
// tags and save frames, and appropriate changes are made to the
// corresponding save frames in the NMR tree (by following pointers from
// the AST tree to the NMR tree: this is done by using myParallelCopy()).
//
// Revision 1.4  1997/10/21  21:27:15  bsri
//  This transformation code has been changed so that it no longer iterates
//  through lists of AST nodes, instead, use the starlib API.
//
// Revision 1.2  1997/10/01  22:38:58  bsri
// Replaced transform2.cc with new file, and added RCS Log comment
// to all the transform*.cc files.
//
*/
#endif
///////////////////////////////////////////////////////////////

#include "transforms.h"

// ##########################################################################
// ##########################################################################

void save_value_to_loop( StarFileNode* AST,
		         StarFileNode* NMR,
		         BlockNode*,
			 SaveFrameNode *currDicRuleSF)

{

//The function applies transformation 3b  for a specified dictionary datablock
//First get the "save_value_to_loop" saveframe in the current dictionary datablock
//
  
  int i;
  DataNode* currDicSaveFrame = currDicRuleSF;

  if(!currDicSaveFrame)
    return;     //Simply return if the desire saveframe is not found


//Get the loop in the data dictionary
  DataNode* currDicLoop = 
    ((SaveFrameNode*)currDicSaveFrame)->ReturnSaveFrameDataNode(string("_old_tag_name"));

  //Flatten the nested loop
  List<DataNameNode*>* currDicNameList;
  List<DataValueNode*>* currDicValList;

  ((DataLoopNode*) currDicLoop)->FlattenNestedLoop(currDicNameList,currDicValList);
  int length = currDicNameList->Length();

  //Apply this transformation to each save frame

  string *iterVal = new string[length];
    
  //Move through the dictionary until this transformation is done
  currDicValList->Reset();
  while(!currDicValList->AtEnd())
  {
        //Get the values for one loop iteration
        for(i =0; i<length && !currDicValList->AtEnd(); i++)
      	{
	   iterVal[i] = currDicValList->Current()->myName();
	   currDicValList->Next();
        }

        // Check if we got all 4 elements
	if (i <length) 
	    return;


	// Search for this saveframe in the AST tree
	List <ASTnode *> *hits = AST->searchByTag(iterVal[1]);

	for (hits->Reset(); !hits->AtEnd(); hits->Next())
	{
	     SaveFrameNode * astSaveFrame = (SaveFrameNode *) hits->Current();

	     if (!astSaveFrame->isOfType(DataNode::SAVEFRAMENODE))
		continue;	//since we are looking for a saveframe

	     List<ASTnode *> *itemList = astSaveFrame->searchByTag(iterVal[0]);
	     for (itemList->Reset(); !itemList->AtEnd() 
			&& (!itemList->Current()->isOfType(DataNode::DATAITEMNODE))
				; itemList->Next())
		    ;

	     if (itemList->AtEnd())
		  {
		     (*errStream) << "#transform-3b# Item " << iterVal[0] << "not found!"<< endl;
		     return;
	          }

	      SaveFrameNode *nmrSaveFrame = (SaveFrameNode *)(astSaveFrame->myParallelCopy());
              if (nmrSaveFrame == NULL)
		continue;

	     //Apply dictionary transformation to this saveframe
	
	      DataItemNode *nmrTagPeer = (DataItemNode *)(itemList->Current()->myParallelCopy());

	      // When item is found "construct" the new loop item with new tagname

	    // This is the list of tag names that make up the 'heading' of the loop:
	    List<string>* NMRtagList = new List<string>;
	    NMRtagList->AddToEnd( tagToVal(iterVal[2]) );

	    // This is the list of values for the data in the loop:
	    List<string>* NMRvalList = new List<string>;

	    // Look at the _delimitter and _width fields to see
	    // which kind of split of the data value should be done:
	    if( tagToVal(iterVal[3]) != "." )
	    {
	        if( tagToVal(iterVal[4]) != "." )
	        {
	    	  (*errStream)<< "#transform-3b# Error: Illegal dictionary file: \n"
		         "#            in save_value_to_loop:\n"
			 "#                Both _delimitter and _width are non-null.\n";
		  return;
	        }
	        else // good - _delimitter is non-null, but _width is null.
	        {
			  
	  	  SplitStringByDelimiter( tagToVal(((DataItemNode*)nmrTagPeer)
				->myValue()), tagToVal( iterVal[3] ), NMRvalList );
	        }
	    }
	    else
	    {
	        if( tagToVal(iterVal[4]) == "." )
	        {
	  	  (*errStream)<< "#transform-3b# Error: Illegal dictionary file: \n"
	  	         "    in save_value_to_loop:\n"
	  		 "        Both _delimitter and _width are null.\n";
	  	  return;
	        }
	        else // good - _delimitter is null, but _width is non-null.
	        {
	  	  SplitStringByWidth( tagToVal(((DataItemNode*)nmrTagPeer)
			->myValue()), atoi( (tagToVal( iterVal[4] )).c_str() ), NMRvalList );
	        }
  
    	    }

	    string tagValue = ((DataItemNode*)nmrTagPeer)->myValue();
	    DataLoopNode *newItem = new DataLoopNode( string("") );

	    //Now actually construct the loop from the NMRtagList and NMRvalList
  	    NMRtagList->Reset();
	    while(!NMRtagList->AtEnd())
	    {
	        newItem->AddDataName(NMRtagList->Current());
	        NMRtagList->Next();
	    }
		  
	    NMRvalList->Reset();
	    while(!NMRvalList->AtEnd())
	    {
	        string tmpString = NMRvalList->Current();
	        InsertNeededQuoteStyle( tmpString );
	        NMRvalList->AlterCurrent( tmpString );

	        newItem->AddDataValue(NMRvalList->Current(), DataValueNode::NON);
	        NMRvalList->Next(); 
	    }

	    delete NMRtagList;
	    delete NMRvalList;

	    //Insert this item into the NMR save frame
	    nmrSaveFrame->AddItemToSaveFrame(newItem);

	    // Finally delete the old item from the NMR tree
	    delete (DataItemNode *)nmrTagPeer;
       }
       delete hits;
    }
    delete []iterVal;
}

