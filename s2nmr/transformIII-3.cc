///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.3  2001/04/13 20:52:29  jurgen
error messages corrected

Revision 1.2  2000/11/18 03:02:40  madings
*** empty log message ***

// Revision 1.1  1999/07/30  23:41:18  june
// This is Steve Mading maquerading as 'june' to check in this one
// file she missed when she left BMRB.
//
Revision 1.6  1999/04/09 20:15:55  madings
added many, many transforms.  transform 17 still does not work yet, though.

// Revision 1.5  1999/01/28  06:15:09  madings
// Some bugfixes from Eldon's attempts to use this, as well as a major
// design change:
//
//    Bug fix 1: save_loops was re-creating tags that had been removed
//       earlier in the run.  This has been fixed.
//
//    Bug fix x: after bug fix 1 above, save_loops was leaving behind
//       loops that had no contents.  This has been fixed.
//
//    Massive design change:
//       There is no longer any default order of operations at all.
//       Instead, it uses the new 'save_order_of_operations' saveframe
//       in the mapping dictionary file to decide what rules to run
//       when.  Also, to allow it to have the same rule run more than
//       once at different times from different mapping dictionary
//       saveframes, the name of the saveframe is no longer the rule
//       to run.  Instead the name of the saveframe is irrelevant, and
//       it's the new _Saveframe_category tag that tells the program
//       what rule to run for that saveframe.  This required a small,
//       but repetative change in all the transform rules so that the
//       main.cc program can tell the transforms which saveframe in the
//       mapping file is the one they are supposed to use instead of the
//       transforms trying to look for it themselves.
//
// Revision 1.4  1999/01/27  00:26:51  madings
// Fixed a bug in save_loops that was causing deleted tags to be recreated
// when making a loop.  Also re-arranged the order of operations and adjusted
// the documentation to match this.
//
// Revision 1.3  1998/12/15  04:33:10  madings
// Addded transforms 9 though 14.  (Lots of stuff)
//
// Revision 1.2  1998/11/11  04:08:26  madings
// Added transform6, but did so in such a way that CVS is falsely
// marking every single file as being changed, so this comment will
// probably appear in many files where it does not apply.
//
// Revision 1.4  1997/12/09  17:28:40  bsri
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
// Revision 1.2  1997/10/01  22:39:00  bsri
// Replaced transform2.cc with new file, and added RCS Log comment
// to all the transform*.cc files.
//
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

  ////////////////////////////////////
 //        save_loops_test()            //
////////////////////////////////////
void save_loops_test( StarFileNode * input,
	         StarFileNode *output,
		 BlockNode    *,
		 SaveFrameNode *currDicRuleSF)
{
  // Just like the other transforms, this transform looks up things in the 
  // input and modifies the output via the parallel copy mechanism.

  // First I have to read the dictionary to find the correct save frame which
  // contains information about this transformation.
  SaveFrameNode *transSaveFrame = currDicRuleSF;

  if (!transSaveFrame)
    return;

  // Then I have to find the loop

  DataLoopNode *transLoop = (DataLoopNode *) 
    transSaveFrame->ReturnSaveFrameDataNode(string("_saveframe_name"));

  // Then I have to read the loop one element at a time to find the desired
  // save frame and also the tag names

  transLoop->reset();

  DataValueNode *s;
  (*os)<<"# at line "<< __LINE__ << "#from map the saveframe "<<endl;
  transSaveFrame->Unparse(2);
  (*os)<<"# at line "<< __LINE__ << "the loop in sf of map: "<<endl;
  transLoop->Unparse(2);

  // Loop on each item in the data dictionary's save_loops loop:
  // -----------------------------------------------------------
  while (transLoop->returnNextLoopElement(s) != DataLoopValListNode::STOP) {

    DataValueNode *kt;
    if (transLoop->returnNextLoopElement(kt) != DataLoopValListNode::CONTINUE)
      {
	(*os)<<"no loop element at line "<< __LINE__ <<endl;
	return;
      }

    DataValueNode *kv;
    if (transLoop->returnNextLoopElement(kv) != DataLoopValListNode::CONTINUE)
      {
	(*os)<<"no loop element at line "<< __LINE__ <<endl;
	return;
      }

    DataValueNode *tf;
    if (transLoop->returnNextLoopElement(tf) != DataLoopValListNode::CONTINUE)
     {
	(*os)<<"no loop element at line "<< __LINE__ <<endl;
	return;
      }
    (*os)<<"# at line "<< __LINE__ << "#from the saveframe the input"<<endl;
    s->Unparse(2);
    kt->Unparse(2);
    kv->Unparse(2);
    tf->Unparse(2);

    // Let's create a 2-D list for storing the loop.
    // One dimension contains the tags
    // The other dimension contains the values for different iterations.
    List<LoopVals*> tagList;

    DataValueNode *tag;

    // Fetch the items from the dictionary.
    while (transLoop->returnNextLoopElement(tag) != DataLoopValListNode::STOP)
      tagList.AddToEnd(new LoopVals(tag));

    // First from the save frame name, strip out the quotes if any.
    string saveFrameName(s->myName());
    SearchAndReplace(saveFrameName, string("\""), string("") );

    string keyTagName(kt->myName());
    SearchAndReplace(keyTagName, string("\""), string( "") );

    string keyValueName(kv->myName());
    SearchAndReplace(keyValueName, string("\""), string( "") );

    // There are two possible cases here :
    // Case 1 :  Saveframename is *
    // Case 2 :  Key tag/value is *

    bool found = false;
    SaveFrameNode *inputSaveFrame;
    List<SaveFrameNode*> *inputSaveFrameList;
    string foundSaveFrameName;

    int i;
    // Case 1 : 
    if(keyTagName != "*" && keyTagName != "."
       && keyValueName != "*" && keyValueName != ".")  //7/23/99 by june
      {
	List <ASTnode *> *keyTagMatches = input->searchForTypeByTagValue(
		ASTnode::SAVEFRAMENODE, keyTagName, keyValueName );
	inputSaveFrameList = new List<SaveFrameNode*>;
	for( i = 0 ; i < keyTagMatches->size() ; i++ )
	    inputSaveFrameList->insert( inputSaveFrameList->end(),
		    (SaveFrameNode*) (*keyTagMatches)[i] );
	delete keyTagMatches;
      }
    else
      {
	// Case 2 :

	// Search for the old tag name in the input tree
	List <ASTnode *> *oldTagMatches = input->searchForTypeByTag(
		ASTnode::SAVEFRAMENODE, saveFrameName);
	inputSaveFrameList = new List<SaveFrameNode*>;
	for( i = 0 ; i < oldTagMatches->size() ; i++ )
	    inputSaveFrameList->insert( inputSaveFrameList->end(),
		    (SaveFrameNode*) (*oldTagMatches)[i] );
	delete oldTagMatches;
      }

      for( int sf_i = 0 ; sf_i < inputSaveFrameList->size() ; sf_i++ )
      {
	  inputSaveFrame = (*inputSaveFrameList)[sf_i];
	  (*os)<<"# at line "<< __LINE__ << "#the input saveframe"<<endl;
	  inputSaveFrame->Unparse(2);
	  SaveFrameNode *outputSaveFrame = (SaveFrameNode *)inputSaveFrame->myParallelCopy();
	  (*os)<<"# at line "<< __LINE__ << "#the output saveframe"<<endl;
	  outputSaveFrame->Unparse(2);

	  // For each data item mentioned in the dictionary, try to fetch it from
	  // the input save frame.

	  // Loop on each tag in this dictionary entry:
	  // ------------------------------------------
	  for (tagList.Reset(); !tagList.AtEnd(); tagList.Next()) {
	    string tagName(tagList.Current()->tag->myName());
	    SearchAndReplace(tagName, string("\""), string( "") );

	    // For each loop tag from the dictionary, pull out the loop data from 
	    // the input and insert in the newly created loop.
	    // Delete the current item from the output because it will be inserted
	    // later.

	    ASTnode* outCurrentItem;

	    List <ASTnode *> *tagMatch = inputSaveFrame->searchByTag(tagName);
	    found = false;
	    for (tagMatch->Reset(); !tagMatch->AtEnd(); tagMatch->Next())
	      {
		if (tagMatch->Current()->isOfType(DataNode::DATAITEMNODE))
		  {
		    outCurrentItem = tagMatch->Current()->myParallelCopy();
		    tagList.Current()->valList = ((DataNode *)tagMatch->Current())->returnLoopValues(tagName);
		    found = true;
		  }
		else if (tagMatch->Current()->isOfType(DataNode::DATANAMENODE))
		  {
		    ASTnode *tmp;

		    for (tmp = tagMatch->Current(); 
			 !tmp->isOfType(DataNode::DATALOOPNODE);
			 tmp = tmp->myParent());

		    outCurrentItem = tmp->myParallelCopy();
		    tagList.Current()->valList = ((DataNode *)tmp)->returnLoopValues(tagName);
		    found = true;
		  }
	      }
	    delete tagMatch;
	    if (!found)
	      {
		*(errStream) << "#transformIII-3# Tag " << tagName << " not found in input file" << endl;
		tagList.Current()->valList = NULL;  
		continue;
	      }

	    delete outCurrentItem;
	  }

	  // Before we start, let's massage the list by getting rid of all those
	  // entries where the tag name was nonexistant in the input StarFileNode, 
	  // so that we don't output them into the loop and thus create the tag
	  // from thin air: - madings
	  for( tagList.Reset(); !tagList.AtEnd(); /* no incriment - see body*/ ) 
	  {
	      if(  tagList.Current()->valList == NULL  )
	      {
		  delete tagList.Current();
		  tagList.RemoveCurrent();
	      }
	      else
		  tagList.Next();
	  }

	  // If there is no longer any data at all left to be
	  // put into the new loop, then let's just not bother making
	  // the empty loop.  Note, this occasion happens when the
	  // mapping file describes a loop to be created for which none
	  // of the tag names given are actually in the input file anywhere.
	  if( tagList.size() == 0 )
	  {   continue;
	  }

	  // At this point we have found all the tags that need to be regrouped.
	  // Now we need to regroup them !
	  outputSaveFrame->AddLoop(tf->myName());
	  (*os)<<"# at line "<< __LINE__ << "#the output saveframe"<<endl;
	  outputSaveFrame->Unparse(2);
	  // Create all the tags in the loop
	  for (tagList.Reset(); !tagList.AtEnd(); tagList.Next()) {
	    string tagName(tagList.Current()->tag->myName());
	    SearchAndReplace(tagName, string("\""), string( "") );

	    outputSaveFrame->AddLoopDataName(tagName);
	    // Also reset the data lists for the next step
	    tagList.Current()->valList->Reset();
	  }
	  (*os)<<"# after tagList added at line "<< __LINE__ << "#the output saveframe"<<endl;
	  outputSaveFrame->Unparse(2);
	  // Now create all the values.
	  // This is done in iteration order. The tricky thing here is that all
	  // tags may not have the same number of values. Then all except the 
	  // longest ones have to be padded with `.'

	  bool goOn = true;
	  while (goOn) {
	    goOn = false;

	    for (tagList.Reset(); !tagList.AtEnd(); tagList.Next()) {
	      ASTlist<DataValueNode *> *currentValList = tagList.Current()->valList;

	      if (!currentValList->AtEnd()) {
		outputSaveFrame->AddLoopDataValue( currentValList->Current()->myName(),
						   currentValList->Current()->myDelimType() );
		currentValList->Next();
		if (!currentValList->AtEnd())
		  goOn = true;
	      }
	      else
		// Pad with .s
		outputSaveFrame->AddLoopDataValue(string("."), DataValueNode::NON );
	    }
	  }
	  (*os)<<"# after add loop values at line "<< __LINE__ << "#the output saveframe"<<endl;
	  outputSaveFrame->Unparse(2);

	  // Now we have finished making the loop, but there's one problem:
	  // The new loop ended up getting put at the end of the saveframe.
	  // What we really wanted was to put it at the spot where the first
	  // tag was encountered.  This was added as an afterthought, which
	  // is why it looks different than the rest of the code.  I was
	  // reluctant to touch something that I already knew was working,
	  // so I didn't rewrite the old code, even though that is really the
	  // right answer to this problem.
	  for( int tlIdx = 0 ; tlIdx < tagList.size() ; tlIdx++ )
	  {
	      List<DataNode*> *inputInnards = inputSaveFrame->GiveMyDataList();
	      List<DataNode*> *outputInnards = outputSaveFrame->GiveMyDataList();
  
	      List<DataNode*>::iterator startIter;
  
	      // Try to find this tag in the original save frame:
	      for( startIter = inputInnards->begin() ; startIter != inputInnards->end() ; startIter++ )
	      {
		  if( (*startIter)->myName() == tagList[tlIdx]->tag->myName() )
		  {
		      break;
		  }
	      }
	      if( startIter == inputInnards->end() )
	      {
		  continue;  // Try the next tag in the loop's tag list.
	      }
	      else
	      {
		  // Now go find the parallel copy in the output tree that goes
		  // with this match.  Problem: This node might not exist in the
		  // output tree anymore since we've been manipulating things in
		  // it.  In fact it is fairly certain that any tag name in
		  // tagList will have been moved now and won't be in its
		  // original spot.  So what we have to do is scan forward until
		  // the first thing is found in the input saveframe that still has
		  // a match in the output saveframe.  Then *that* is the node
		  // that we will be inserting in front of.
		  // Example:
		  //    input saveframe              output saveframe
		  //    ---------------              ----------------
		  //      _tag1   <---parallel--->  _tag1
		  //      _tag2   <---parallel--->  _tag2
		  //      loop_   <---brokenlink-/
		  //         _looptag1  
		  //         ...
		  //      stop_
		  //      loop_   <---brokenlink-/
		  //         _looptag2
		  //         ...
		  //      stop_
		  //      _tag3   <---parallel--->  _tag3
		  //      _tag4   <---parallel--->  _tag4
		  //      loop_   <---brokenlink-/
		  //         _looptag3
		  //         ...
		  //      stop_
		  //      _tag5   <---parallel--->  _tag5
		  //                                loop_
		  //                                    _looptag1
		  //                                    _looptag2
		  //                                    _looptag3
		  //                                    ...
		  //                                stop_
		  // In the above example, we want to, starting with _looptag1,
		  // scan forward in the input saveframe until the first
		  // good parallel link is found (in this case "_tag3"), and
		  // then we know that the loop we made needs to be moved to
		  // just before that node ("_tag3").
		  int parallel_pos;
		  DataNode  *parallel = NULL;
		  List<DataNode*>::iterator parScanCur;
		  for( parScanCur = startIter ;
		       parScanCur != inputInnards->end() ;
		       parScanCur++ )
		  {
		      parallel = (DataNode*)( (*parScanCur)->myParallelCopy() );
		      if( parallel != NULL )
		      {
			  break;
		      }
		  }
		  for(    parallel_pos = 0 ; 
			  parallel_pos < outputInnards->size() ;
			  parallel_pos++   )
		  {
		      if( (*outputInnards)[parallel_pos] == parallel )
		      {
			  break;
		      }
		  }
		  // Copy the loop (which we know is the last thing in the output
		  // saveframe) to the new location, then remove it from the
		  // bottom of the loop:  (If parallel is NULL, then the loop
		  // was *supposed* to be at the bottom, so leave it alone).
		  if( parallel != NULL )
		  {
		      if( (*(outputInnards->end()-1))->isOfType( ASTnode::DATALOOPNODE ) )
		      {
			  outputInnards->insert( outputInnards->begin()+parallel_pos,
			     new DataLoopNode( *(DataLoopNode*)(* (outputInnards->end()-1) ) ) );
			  outputInnards->erase( outputInnards->end() - 1 );
		      }
		      // This case should theoretically be impossible - the last
		      // thing had better be a data loop or the rule is acting
		      // strange - this is here just to keep this programmer from
		      // letting termites destroy civilization, as the saying goes:
		      else if( (*(outputInnards->end()-1))->isOfType( ASTnode::DATAITEMNODE ) )
		      {
			  outputInnards->insert( outputInnards->begin()+parallel_pos,
			     new DataItemNode( *(DataItemNode*)(* (outputInnards->end()-1) ) ) );
			  outputInnards->erase( outputInnards->end() - 1 );
		      }
		      break;
		  }//end-if
	      }//end-else
	  }//end-for
	  (*os)<<"# after relocation at line "<< __LINE__ << "#the output saveframe"<<endl;
	  outputSaveFrame->Unparse(2);
      }
  
      // At this point, we have to delete all the elements of the tag list
      for (tagList.Reset(); !tagList.AtEnd(); tagList.Next())
      {
	  delete tagList.Current();
      }

  }//end-while


  return;
}
