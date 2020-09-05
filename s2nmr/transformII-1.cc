///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.5  2000/11/18 03:02:40  madings
*** empty log message ***

// Revision 1.4  1999/07/30  18:22:09  june
// june, 7-30-99
// add transform1d.cc
//
Revision 1.3  1999/01/28 06:15:09  madings
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
// Revision 1.4  1997/12/09  17:28:29  bsri
// Modified all the transformations so that the AST tree is searched for
// tags and save frames, and appropriate changes are made to the
// corresponding save frames in the NMR tree (by following pointers from
// the AST tree to the NMR tree: this is done by using myParallelCopy()).
//
// Revision 1.3  1997/10/10  18:34:31  madings
// Changed DataValueNode so that it now no longer has subclasses for
// single-quote-delimited strings, double-quote delimited strings,
// semicolon-delimited strings, and so on.  Now there is only one kind
// of DataValueNode, and it uses a flag to decide on the delimiter type.
// This also allows for the creation of methods to change a DataValueNode
// in place, which was not previously possible when different kinds of
// values had to be stored in different kinds of classes.
//
// Revision 1.2  1997/10/01  22:38:59  bsri
// Replaced transform2.cc with new file, and added RCS Log comment
// to all the transform*.cc files.
//
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

void save_order( StarFileNode *input, 
		 StarFileNode *output,
		 BlockNode    *,
		 SaveFrameNode *currDicRuleSF)
{
  // This function was taken from transform1() from the file transform2.cc
  // I have left transform2.cc in RCS for reference if someone needs to
  // see what this function originally looked like.  - Steve Mading

  /*
  // Copy the star file tree into a temp tree.  This will be used
  // later for keeping track of which tags were missing in the
  // trnaslation:
  // We will make use of the myParallelCopy function to locate corresponding
  // nodes.
  StarFileNode  *MissingTagFinder = new StarFileNode(true,  *input );
  */


  string  foundSaveFrameName;
  // First I have to read the dictionary to find the correct save frame which
  // contains information about this transformation.

  SaveFrameNode* transSaveFrame = currDicRuleSF;

  if (!transSaveFrame)
    return;

  // Then I have to find the loop

  DataLoopNode *transLoop = (DataLoopNode *) 
    transSaveFrame->ReturnSaveFrameDataNode(string("_saveframe_name"));

  if( transLoop == (DataLoopNode*)NULL )
  {
      (*errStream)<<
            "#transformII-1# Error in dictionary: in save_order, first tag is not _saveframe_name."<<endl;
      return;
  }
  
  // The saveframe that will mark the end of the list of saveframes.
  string sentinelName( "save_SeNtInEl_SaVe_FrAmE" );
  output->AddSaveFrame( sentinelName );

  // Then I have to read the loop one element at a time to find the desired
  // save frame and also the tag names

  transLoop->reset();

  DataValueNode *s;
  while( transLoop->returnNextLoopElement(s) != DataLoopValListNode::STOP )
  {
    DataValueNode *kt;
    if (transLoop->returnNextLoopElement(kt) != DataLoopValListNode::CONTINUE)
    {   (*errStream)<<"#transformII-1# Error in dictionary: _key_tag missing in save_order."<<endl;
        return;
    }

    DataValueNode *kv;
    if (transLoop->returnNextLoopElement(kv) != DataLoopValListNode::CONTINUE)
    {   (*errStream)<<"#transformII-1# Error in dictionary: _key_value missing in save_order."<<endl;
	return;
    }

    // Fetch the items from the dictionary.
    ASTlist <DataValueNode *> tagList;
    DataValueNode *tag;

    while (transLoop->returnNextLoopElement(tag) == DataLoopValListNode::CONTINUE)
      tagList.AddToEnd(tag);

    // First from the save frame name, strip out the quotes if any.
    string saveFrameName(s->myValue());
    SearchAndReplace( saveFrameName, string("\""), string("") );

    string keyTagName(kt->myName());
    SearchAndReplace( keyTagName, string("\""), string("") );

    string keyValueName(kv->myName());
    SearchAndReplace( keyValueName, string("\"") , string("") );


    // There are two possible cases here :
    // Case 1 :  Saveframename is *
    // Case 2 :  Key tag/value is *

    bool found = false;
    SaveFrameNode *inputSaveFrame;

    // List of all matched saveframes - algorithm works on all frames.
    List<SaveFrameNode*> *allMatchedFrames = new List<SaveFrameNode*>;

    // Case 1 : 
    if( saveFrameName == "*" || saveFrameName == "." )
      {
	List <ASTnode *> *keyTagMatches = input->searchByTagValue(keyTagName, keyValueName);

	for (keyTagMatches->Reset(); !keyTagMatches->AtEnd(); keyTagMatches->Next())
	  {
	    ASTnode *foundOldTag;

	    // Get the parent save frame
	    for ( foundOldTag = keyTagMatches->Current() ;
	       	   ((foundOldTag != NULL) && (foundOldTag->myType() != ASTnode::SAVEFRAMENODE)) ;
 	           foundOldTag = foundOldTag->myParent() )
	      ;

	    if (saveFrameName == "*" ||
		saveFrameName == "." ||
		saveFrameName == ((SaveFrameNode *)foundOldTag)->myName())
	      {
		found = true;
		allMatchedFrames->AddToEnd( (SaveFrameNode *)foundOldTag );
	      }
	  }
	delete keyTagMatches;
      }
    else
      {
	// Case 2 :

	// Search for the old tag name in the input tree
	List <ASTnode *> *oldTagMatches = input->searchByTag(saveFrameName);

	for (oldTagMatches->Reset(); !oldTagMatches->AtEnd(); oldTagMatches->Next())
	  {
	    inputSaveFrame = (SaveFrameNode *) oldTagMatches->Current();
	    if (inputSaveFrame->isOfType(DataNode::SAVEFRAMENODE))
	      {
		found = true;
		foundSaveFrameName = inputSaveFrame->myName();
		allMatchedFrames->AddToEnd( inputSaveFrame );
	      }
	  }
	delete oldTagMatches;
      }
      if (!found)
	continue;

      ASTnode *prevSaveFrame = NULL;
      // For each saveframe that matched the pattern, do the algorithm:
      // --------------------------------------------------------------
      for(    allMatchedFrames->Reset() ;
	      !allMatchedFrames->AtEnd() ;
	      allMatchedFrames->Next()   )
      {
	  // Check if this is the same saveframe as the last time.
	  // If so, then skip it.  (only do a saveframe once).
	  if( prevSaveFrame == allMatchedFrames->Current() )
              continue;
	  else
	      prevSaveFrame = allMatchedFrames->Current();


	  inputSaveFrame = allMatchedFrames->Current();
	  foundSaveFrameName = inputSaveFrame->myName();

	  // Remove the old SaveFrame from the output, and write a stub for it at the end.
	  // We will build off of this stub a bit further down to re-build the save frame
	  // in the new order.

	  // Used to say "global_" :
	  /*
	  output->RemoveSaveFrame( missingblockList->Current()->myName(), foundSaveFrameName );
	  */
	  SaveFrameNode* outputSaveFrame = (SaveFrameNode *)inputSaveFrame->myParallelCopy();
	  delete outputSaveFrame;

	  output->AddSaveFrame(foundSaveFrameName);

	  // For each data item mentioned in the dictionary, try to fetch it from
	  // the input save frame.

	  for (tagList.Reset(); !tagList.AtEnd(); tagList.Next()) {
	    string tagName(tagList.Current()->myName());
	    SearchAndReplace( tagName, string("\""), string("") );


	    List <ASTnode *> *inputDataList = inputSaveFrame->searchByTag(tagName);

	    for (inputDataList->Reset(); !inputDataList->AtEnd(); inputDataList->Next())
	      {
		if (inputDataList->Current()->isOfType(DataNode::DATAITEMNODE))
		  {
		    DataItemNode *currentItem = (DataItemNode *)inputDataList->Current();
		    // Copy the items and values from input to output
		    output->AddSaveFrameDataItem( currentItem->myName(),
						  currentItem->myValue(),
						  currentItem->myDelimType() );
		    /*
		  // Remove this item from the missing-list, because it is not missing:

		    DataItemNode *missingItem = (DataItemNode *)currentItem->myParallelCopy();
		    delete missingItem;
		    */
		  }
		else if (inputDataList->Current()->isOfType(DataNode::DATANAMENODE))
		  {
		    DataLoopNode *loopNode;
		    ASTnode *tmp;

		    for (tmp = inputDataList->Current(); 
			 !tmp->isOfType(DataNode::DATALOOPNODE);
			 tmp = tmp->myParent());

		    loopNode = (DataLoopNode *)tmp;

		// Now we must search the loop for the tag name and if found, it
		// must supply a list of values for it.
		ASTlist<DataValueNode *> *loopVals = loopNode->returnLoopValues(tagName);

		// In the output we must create a loop with a single tag and all
		// these values
		if (loopVals) 
		  {
		  output->AddSaveFrameLoop();
		  output->AddSaveFrameLoopDataName(tagName);

		  for (loopVals->Reset(); !loopVals->AtEnd(); loopVals->Next())
		    output->AddSaveFrameLoopDataValue( loopVals->Current()->myName(),
						       loopVals->Current()->myDelimType() );
		  delete loopVals;
		  /*
		  // Now we must remove the column we just copied from the loop from
		  // the missing-tracker tree.

		  DataLoopNode *missingLoop = (DataLoopNode *)currentLoop->myParallelCopy();
		  missingLoop->RemoveColumn( tagName );
		  */
		  }
		  }
	      }
	    delete inputDataList;
	  }
      }
      delete allMatchedFrames;
      ValCache::flushValCache();
  }

  // Now check to see if anything was left unsorted.  Do this by
  // looking to see if anything exists before the sentinel saveframe.
  // If the sentinel saveframe is the first thing, then it's all
  // been re-ordered, and the sentinel saveframe can be removed.
  // -----------------------------------------------------------------
  ASTlist <BlockNode*> *allBlocks = output->GiveMyDataBlockList();
  bool done = false;
  for( allBlocks->Reset() ; !allBlocks->AtEnd() && !done ; allBlocks->Next() )
  {
      ASTlist<DataNode*> *allNodesInBlock =
		  allBlocks->Current()->GiveMyDataList();

      for(    allNodesInBlock->Reset() ;
	      !allNodesInBlock->AtEnd() && !done ;
	      allNodesInBlock->Next()   )
      {
	  if( allNodesInBlock->Current()->myName() == sentinelName )
	      done = true;
	  else
	      (*errStream) << "#transformII-1# Error: One of the tags in " <<
		      allNodesInBlock->Current()->myName() <<
		      " was not mapped." << endl;
      }
  }
  // Delete the sentinel saveframe now: assume there is exactly one:
  //
  List <ASTnode*> *sentinels = output->searchByTag( sentinelName );
  sentinels->Reset();
  delete sentinels->Current();
  delete sentinels;


  /*
  // Okay - now it did all the transforms, so now we can check
  // to see if any tags were left behind (not mapped across).
  // If any tags were left behind, then that's an error.
  // ----------------------------------------------------------
  ASTlist <BlockNode *>* missingblockList = MissingTagFinder->GiveMyDataBlockList();
  ASTlist <DataNode *> *missingdataList;
  for(    missingblockList->Reset() ;
	  ! missingblockList->AtEnd() ;
	  missingblockList->Next()   )
  {
      missingdataList = missingblockList->Current()->GiveMyDataList();
      missingdataList->Reset();
      for(  missingdataList->Reset() ;
	    !missingdataList->AtEnd() ;
	    missingdataList->Next()  )
      {
	  ListAllTagsInDataNode(
	      *missingdataList->Current(),
	      (*errStream),
	      string("#transformII-1# Error in save_order: missing mapping for tag: ") );
      }
  }

  delete MissingTagFinder;
  return;
  */
}
