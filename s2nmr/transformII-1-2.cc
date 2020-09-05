///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT


    /* This is the file that combines save_order and save_loops
     * together into a single rule, under the name
     * save_order_and_loops
     */
/*
$Log: not supported by cvs2svn $
Revision 1.6  2008/10/15 22:42:18  madings
*** empty log message ***

Revision 1.5  2008/04/16 21:29:20  madings
Haven't committed in a while and this is a working version so I should.

Revision 1.4  2006/03/27 21:57:34  madings
*** empty log message ***

Revision 1.3  2006/03/27 21:44:35  madings
*** empty log message ***

Revision 1.2  2005/11/07 20:21:20  madings
*** empty log message ***

Revision 1.1  2005/03/23 22:41:42  madings
*** empty log message ***

Revision 1.6  2001/06/18 16:57:09  madings
Several fixes for s2nmr segfault bugs.

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
#include <errno.h>
#include <map>


bool g_generate_comments;

/* For use with STL map: */
struct lessThanStr
{
    bool operator()(string s1, string s2) const
    {
	return s1 < s2 ;
    }
};

struct ltPtr
{
    bool operator()(void *p1, void *p2) const
    {
        return p1 < p2;
    }
};

void save_order_and_loops(
	         StarFileNode *input, 
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

  DataValueNode *insertDummyVal;
  List<ASTnode*> *find_insdummyval = currDicRuleSF->searchForTypeByTag( ASTnode::DATAITEMNODE, "_insert_dummy_value" );
  if( find_insdummyval->size() > 0 ) {
    insertDummyVal = new DataValueNode( ( (DataItemNode*)((*find_insdummyval)[0]))->myValue() ,
                                        ( (DataItemNode*)((*find_insdummyval)[0]))->myDelimType() );
  } else {
    insertDummyVal = NULL;
  }
  delete find_insdummyval;

  bool insertEntireDummyLoop = false;
  List<ASTnode*> *find_insEntireDummyLoop = currDicRuleSF->searchForTypeByTag( ASTnode::DATAITEMNODE, "_insert_entire_dummy_loop" );
  if( find_insEntireDummyLoop->size() > 0 ) {
    string str = ( (DataItemNode*)((*find_insEntireDummyLoop)[0]))->myValue();
    if( str == "yes" || str == "true" || str == "1" ) {
      insertEntireDummyLoop = true;
    }
  } 
  delete find_insEntireDummyLoop;
 
  map<string, int, lessThanStr> mandatoryLoops;
  List<ASTnode*> *find_mandatory_loop_tags = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, "_mandatory_loop_tags" );
  if( find_mandatory_loop_tags->size() > 0 ) {
    LoopTableNode *tbl = ( (DataLoopNode*)((*find_mandatory_loop_tags)[0]))->getValsPtr();
    // Assume a loop of one column only:
    for( int tbl_i = 0 ; tbl_i < tbl->size() ; ++tbl_i ) {
      mandatoryLoops [ (* ((*tbl)[tbl_i]) )[0]->myValue() ] = 1;
    }
  }
  delete find_mandatory_loop_tags;

  DataValueNode *dummyOneVal = new DataValueNode( "1", DataValueNode::NON );

  DataValueNode *entryIDVal;
  List<ASTnode*> *find_entryIDVal = input->searchForTypeByTag( ASTnode::DATAITEMNODE, "_Entry.ID" );
  if( find_entryIDVal->size() > 0 ) {
    entryIDVal = new DataValueNode( ( (DataItemNode*)((*find_entryIDVal)[0]))->myValue() ,
                                    ( (DataItemNode*)((*find_entryIDVal)[0]))->myDelimType() );
  } else {
    entryIDVal = insertDummyVal;
  }
  delete find_entryIDVal;


  g_generate_comments = false;
  List<ASTnode*> *findCommentFlags =
      currDicRuleSF->searchForTypeByTag( ASTnode::DATAITEMNODE, "_Generate_diagnostic_comments" );
  if( findCommentFlags->size() > 0 )
  {   if( ( (DataItemNode*)( (*findCommentFlags)[0]) )->myValue() == string( "yes" ) )
      {   g_generate_comments = true;
      }
  }
  delete findCommentFlags;

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

  int numInnerColumns = ( transLoop->getNames() )[1]->size();

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
    vector <string> tagList;
    List <string> whichLoopList;
    List <int> loopRowPerLineList;
    string   lastWhichLoopVal;
    int  col;
    DataValueNode *tag;

    /* Make two parallel lists - one of tag names, one of which_loop flags */
    col = 0;
    while( transLoop->returnNextLoopElement(tag) == DataLoopValListNode::CONTINUE)
    {
	if( col == 0 )
	{
	    tagList.insert(tagList.end(), tag->myValue() );
	}
	else if( col == 1 )
	{
	    lastWhichLoopVal = tag->myValue();
	}
	else if( col == 2 )
	{
	    if( tag->myValue().compare( "tabulate" ) == 0 )
		lastWhichLoopVal = string("t:") + lastWhichLoopVal;
	    else
		lastWhichLoopVal = string("l:") + lastWhichLoopVal;
	    whichLoopList.AddToEnd(lastWhichLoopVal);
	}
	else if( col == 3 )
	{
	    loopRowPerLineList.AddToEnd( atoi(tag->myValue().c_str()) );
	}
		    
	col++;
	if( col >= numInnerColumns ) 
	    col = 0;
    }
    map<string, DataLoopNode*, lessThanStr> whichLoopMap;

    // First from the save frame name, strip out the quotes if any.
    string saveFrameName(s->myName());
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
	    if (inputSaveFrame->isOfType(ASTnode::SAVEFRAMENODE))
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

      map<DataLoopNode*, int, ltPtr > loopsToKeep;
      loopsToKeep.clear();

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

	  whichLoopMap.clear();

	  // Remove the old SaveFrame from the output, and write a stub for it at the end.
	  // We will build off of this stub a bit further down to re-build the save frame
	  // in the new order.

	  // Used to say "global_" :
	  /*
	  output->RemoveSaveFrame( missingblockList->Current()->myName(), foundSaveFrameName );
	  */
	  SaveFrameNode* outputSaveFrame = (SaveFrameNode *)inputSaveFrame->myParallelCopy();
	  delete outputSaveFrame;


	  List<ASTnode*> *inputLoops = new List<ASTnode*>;
	  List<string> *inputWhichLoopStrings = new List<string>;

	  output->AddSaveFrame(foundSaveFrameName);

	  int tagsProcessed = 0;
	  int tagsInMapper = tagList.size();

	  // For each data item mentioned in the dictionary, try to fetch it from
	  // the input save frame.

	  string freeTagBaseThisSF = "DUMMYSTARTVAL";  // the base name of the free tags in this saveframe.  I.e. "Citaion" for "_Citation.ID"
	  DataValueNode *freeTagLocalIDVal = NULL;

	  int tagListIdx;
	  for(  tagsProcessed = 0 ,
	        tagListIdx = 0,
		whichLoopList.Reset(),
		loopRowPerLineList.Reset()
		;
		( tagListIdx < tagList.size() ) && !whichLoopList.AtEnd()
		;
		tagsProcessed++   ,
		tagListIdx++  ,
		whichLoopList.Next() ,
		// If not at the end of the row-per-line list, increment the list:
	        ( loopRowPerLineList.AtEnd() ? (void)NULL : loopRowPerLineList.Next() )
	     )
	  { 
	    string tagName(tagList[tagListIdx]);
	    string whichLoopStr( whichLoopList.Current() );
	    int    rowsPerLine;
	    SearchAndReplace( tagName, string("\""), string("") );

	    if(   whichLoopStr == string("t:F") || 
		  whichLoopStr == string("l:F")    ) {
	      int dotPos = tagName.find( "." );
	      if( dotPos != string::npos ) {
		freeTagBaseThisSF = tagName.substr( 1, dotPos - 1 );
		string nameAfterDot = tagName.substr( dotPos + 1 );

		// Oh, and if this is the ID tag, remember it's value:
		if( nameAfterDot == string("ID" ) ) {
		  List<ASTnode*> *inputSFLocalIDSearch = ((SaveFrameNode*)inputSaveFrame)->searchForTypeByTag(ASTnode::DATAITEMNODE, tagName );
		  if( inputSFLocalIDSearch->size() > 0 ) {
		    freeTagLocalIDVal = new DataValueNode( 
		          ( (DataItemNode*)(*inputSFLocalIDSearch)[0])->myValue(),
		          ( (DataItemNode*)(*inputSFLocalIDSearch)[0])->myDelimType() );
		  }
		  delete inputSFLocalIDSearch;
		}
	      }
	    }

	    if( ! loopRowPerLineList.AtEnd() )
	    {   rowsPerLine = loopRowPerLineList.Current() ;
	    }
	    else
	    {   rowsPerLine = 1;
	    }

	    bool tagFound = true;
	    List <ASTnode *> *inputDataList = inputSaveFrame->searchByTag(tagName);
	    if( insertDummyVal && inputDataList->size() == 0 && tagName.find(".Sf_ID") == string::npos ) {
	      tagFound = false;

	      DataValueNode *valToUse = insertDummyVal;
	      if( tagName.find( ".Entry_ID" ) != string::npos ) {
	        valToUse = entryIDVal;
	      }
	      else if( tagName.find( string(".") + freeTagBaseThisSF + string("_ID") ) != string::npos ) {
		if( freeTagLocalIDVal != NULL ) {
	          valToUse = freeTagLocalIDVal;
		} else {
	          valToUse = dummyOneVal;
		}
	      }
	      else if( tagName.find( (".ID") ) != string::npos ) {
	        valToUse = dummyOneVal;
	      }

	      if( mandatoryLoops.find( tagName  ) != mandatoryLoops.end() ) {
		tagFound = true; // pretend this loop had a tag found even though it didn't - so it will stick around.
	      }

	      // Create a dummy hit in the input for it to copy from:
	      // If it's supposed to be a free tag:
	      if(   whichLoopStr == string("t:F") || 
	            whichLoopStr == string("l:F")    ) {
	        inputSaveFrame->GiveMyDataList()->insert(
		  inputSaveFrame->GiveMyDataList()->end(),
		  new DataItemNode( new DataNameNode(tagName) , new DataValueNode( *valToUse ) )
		  );
	      } else {
	        // If it's supposed to be a loop tag, then make it as a dummy loop of one value:
	        DataLoopNode *dummyLoop  = new DataLoopNode("tabulate");
		dummyLoop->getNames().insert( dummyLoop->getNames().end(), new LoopNameListNode() );
		dummyLoop->getNames()[0]->insert( dummyLoop->getNames()[0]->end(), new DataNameNode( tagName ) );
		LoopRowNode *dummyRow = new LoopRowNode(true);
		dummyRow->insert( dummyRow->end(), new DataValueNode( *valToUse ) );
		dummyLoop->getVals().insert( dummyLoop->getVals().end(), dummyRow );

	        inputSaveFrame->GiveMyDataList()->insert( inputSaveFrame->GiveMyDataList()->end(), dummyLoop);
	      }
	      // Then do the search again, this time finding the new dummy value we just made, so
	      // that the rest of the algorithm can work correctly.
	      delete inputDataList;
	      inputDataList = inputSaveFrame->searchByTag(tagName);

	    } 

	    int sameTagCount = 0;
            vector <DataItemNode *> prevValItems;
            vector <DataValueNode *> prevVals;
	    prevValItems.clear();
	    prevVals.clear();
	    int fromTagCol = -1;
	    for ( inputDataList->Reset();
		  !inputDataList->AtEnd();
		  inputDataList->Next() )
	      {
	        ++sameTagCount;  // How many times have we seen this same tag?  (if more than 1, then there's a problem).
		if (inputDataList->Current()->isOfType(ASTnode::DATAITEMNODE))
		  {
		    DataItemNode *currentItem = (DataItemNode *)inputDataList->Current();
		    string tagNameToUse = currentItem->myName();
		    // append the (2) for the second instance of the same tag (or (3), etc)
		    if( sameTagCount > 1 )
		    {
		      // But don't bother if it's an exact match, or if the new tag has a null value.
		      // If either of those is true, then just don't insert it:
		      if(  (prevValItems.size() > 0 && prevValItems[0]->myValue() == currentItem->myValue()) ||
		           (prevValItems.size() > 0 && (  currentItem->myValue() == string("") ||
			                                  currentItem->myValue() == string(".") ||
			                                  currentItem->myValue() == string("?") ) ) )
		      {
			  // Remove this item from the missing-list, because we are deliberately not mapping it:
			  delete currentItem;
			  continue; // skip this tag - it's an exact duplicate.
		      // If the prev value was a null value, then copy the new value to the prev value and
		      // skip the new value.
		      } else if( prevValItems.size() > 0 &&
		                     ( prevValItems[0]->myValue() == string("") ) ||
		                     ( prevValItems[0]->myValue() == string(".") ) ||
		                     ( prevValItems[0]->myValue() == string("?") ) )
		      {
			  
			  prevValItems[0]->setValue( currentItem->myValue() );
			  prevValItems[0]->setDelimType( currentItem->myDelimType() );
			  delete currentItem;
			  continue;
		      }
		      // If they do not match and are both non-null, then merge them by paranthesizing the second string
		      // appended to the first, like so:
		      //    "value1(value2)"
		      else if( prevValItems.size() > 0 && prevValItems[0]->myValue() != currentItem->myValue() )
		      {
		        prevValItems[0]->setValue( prevValItems[0]->myValue() + " (" + currentItem->myValue() + ")" );
			char qStyle = FindNeededQuoteStyle( prevValItems[0]->myValue() );
			switch( qStyle )
			{
			  case ' ':  prevValItems[0]->setDelimType( DataValueNode::NON );
			             break;
			  case '\"': prevValItems[0]->setDelimType( DataValueNode::DOUBLE );
			             break;
			  case '\'': prevValItems[0]->setDelimType( DataValueNode::SINGLE );
			             break;
			  case ';' : prevValItems[0]->setDelimType( DataValueNode::SEMICOLON );
			             break;
			}
		        delete currentItem;
		        continue;
		      }

		      char tmpStr[12];
		      sprintf(tmpStr, "%d", sameTagCount );
		      tagNameToUse += "(";
		      tagNameToUse += tmpStr;
		      tagNameToUse += ")";
		    }
		    // Copy the items and values from input to output
		    output->AddSaveFrameDataItem( tagNameToUse,
						  currentItem->myValue(),
						  currentItem->myDelimType() );

		    ASTlist<BlockNode*> *outputList = output->GiveMyDataBlockList();
		    BlockNode *lastBlock = (*outputList)[ outputList->size() - 1 ];
		    ASTlist<DataNode*> *lastBlockList = lastBlock->GiveMyDataList();

		    SaveFrameNode *sfJustMade = (SaveFrameNode*)
						    ( (*lastBlockList)[lastBlockList->size() - 1 ] );
		    ASTlist<DataNode*> *sfJustMadeList = sfJustMade->GiveMyDataList();
		    DataItemNode *itemJustMade = (DataItemNode*) 
						    ( (*sfJustMadeList)[sfJustMadeList->size() - 1 ] );
	            prevValItems.clear();
		    prevValItems.insert( prevValItems.end(), itemJustMade );
		    if( tagsProcessed >= tagsInMapper)
		    {
		        string comm = sfJustMade->getPreComment();
			if( comm == string("") )
			{  comm = string("#  In the following saveframe, these tags were\n"
			                 "#  missing from the order mapping rule in s2nmr,\n"
					 "#  and thus might not appear in order:" );
			}
			comm += string("\n#\t");
			comm += tagNameToUse;
			sfJustMade->setPreComment( comm );
		    }

		  // Remove this item from the missing-list, because it is not missing:
		    delete currentItem;
		  }
		else if (inputDataList->Current()->isOfType(ASTnode::DATANAMENODE))
		{
		    string thisName = (  (DataNameNode*)(inputDataList->Current())  )->myName();
		    DataLoopNode *loopNode;
		    ASTnode *tmp;

		    for (tmp = inputDataList->Current(); 
			 !tmp->isOfType(ASTnode::DATALOOPNODE);
			 tmp = tmp->myParent())
		    { /*void body */ }

		    string tagNameToUse = thisName;

		    loopNode = (DataLoopNode *)tmp;

		    // Now we must search the loop for the tag name and if found, it
		    // must supply a list of values for it.
		    //
		    //   Was this, but that does not work for multiple hits where we must iterate over the list,
		    //   instead of always hitting only the leftmost column with this name, which is what
		    //   this routine from the library does:
		    //     ASTlist<DataValueNode *> *loopVals = loopNode->returnLoopValues(tagNameToUse);
		    //
		    for( int thisTagCol = fromTagCol+1 ; thisTagCol < loopNode->getNames()[0]->size() ; thisTagCol++ )
		    {
			if( (*(loopNode->getNames()[0]))[thisTagCol]->myName() == tagNameToUse )
			{
			     fromTagCol = thisTagCol;
			     break;
			}
		    }
		    ASTlist<DataValueNode *> *loopVals = new ASTlist<DataValueNode*>;
		    for( int rNum = 0 ; rNum < loopNode->getValsPtr()->size() ; ++rNum )
		    {
		        loopVals->AddToEnd( (*(loopNode->getVals()[rNum]))[fromTagCol] );
		    }
		    // Now it's built.


		    ASTlist<BlockNode*> *outputList = output->GiveMyDataBlockList();
		    BlockNode *lastBlock = (*outputList)[ outputList->size() - 1 ];
		    ASTlist<DataNode*> *lastBlockList = lastBlock->GiveMyDataList();



		    // Get the appropriate output loop.  It will either be
		    // a new one column loop we have to create or an existing
		    // loop we have to add a new column to.
		    if (loopVals) 
		    {
			// append the (2) for the second instance of the same tag (or (3), etc)
			if( sameTagCount > 1 )
			{
			  // But don't bother if it's an exact match, or if the new value is a null.
			  // If either of those is the case, then just don't insert it:
			  if( prevVals.size() == loopVals->size() )
			  {
			    bool theyMatch = true;
			    bool leftValueNull = true;
			    bool rightValueNull = true;
			    bool terminateLoop = false;
			    bool shouldDeleteRightCol = true;
			    for( int i = 0 ; !(terminateLoop)  && i < prevVals.size() ; i++ )
			    {
			      if( (  prevVals[i]->myValue() != string( "" ) &&
			             prevVals[i]->myValue() != string( "." ) &&
			             prevVals[i]->myValue() != string( "?" ) )  )
			      {
			        leftValueNull = false;
			      }
			      if( (  (*loopVals)[i]->myValue() != string( "" ) &&
			             (*loopVals)[i]->myValue() != string( "." ) &&
			             (*loopVals)[i]->myValue() != string( "?" ) )  )
			      {
			        rightValueNull = false;
			      }
			      if( prevVals[i]->myValue() != (*loopVals)[i]->myValue() )
			      {
			        theyMatch = false;
			      }
			      // Copy right value into left value if left is null and they donn't match:
			      if( leftValueNull && !(theyMatch) ) {
				prevVals[i]->setValue( (*loopVals)[i]->myValue() );
				prevVals[i]->setDelimType( (*loopVals)[i]->myDelimType() );
			      }
			      if( (!theyMatch) && (!leftValueNull) && (!rightValueNull) ) {
				//
			        // Once upon a time,
				// this if-clause was just this:
				//    shouldDeleteRightCol = false;
				//
				prevVals[i]->setValue( prevVals[i]->myValue() + " (" + (*loopVals)[i]->myValue() + ")" );
				prevVals[i]->setDelimType( (*loopVals)[i]->myDelimType() );
				char qStyle = FindNeededQuoteStyle( prevVals[i]->myValue() );
				switch( qStyle )
				{
				  case ' ':  prevVals[i]->setDelimType( DataValueNode::NON );
					     break;
				  case '\"': prevVals[i]->setDelimType( DataValueNode::DOUBLE );
					     break;
				  case '\'': prevVals[i]->setDelimType( DataValueNode::SINGLE );
					     break;
				  case ';' : prevVals[i]->setDelimType( DataValueNode::SEMICOLON );
					     break;
				}
				shouldDeleteRightCol = true;
			      }
			    }
			    if( shouldDeleteRightCol )
			    { 
				loopNode->RemoveColumn( tagNameToUse ); // remove it so it doesn't try copying it again later
				                                    // as an unmapped tag.
				continue; // skip this tag - it's an exact duplicate.
			    }
			  }
			  char tmpStr[12];
			  sprintf(tmpStr, "%d", sameTagCount );
			  tagNameToUse += "(";
			  tagNameToUse += tmpStr;
			}
			// If this is the first time we've encountered this particular
			// whichLoop string, or if the number of values don't match up to
			// the old one we previously encountered, then we make a new loop
			// and add these values to it:

			bool newLoop = false;
			bool memorize = true;
			if(  whichLoopMap.count( whichLoopStr ) == 0  )
			{   newLoop = true;
			}
			else
			{   DataLoopNode *lp = whichLoopMap[ whichLoopStr ];
			    LoopTableNode *tbl  = lp->getValsPtr();

			    /* COMMENT_OUT BEGIN
			    if( loopVals->size() != tbl->size() )
			    {   newLoop = true;
				memorize = false;
			    }
			    COMMENT_OUT END */
			}


			if( newLoop )
			{

			    output->AddSaveFrameLoop();
			    output->AddSaveFrameLoopDataName(thisName);


			    prevVals.clear();
			    for( loopVals->Reset(); !loopVals->AtEnd(); loopVals->Next() ) {
				output->AddSaveFrameLoopDataValue( loopVals->Current()->myValue(),
								   loopVals->Current()->myDelimType() );
				prevVals.insert(prevVals.end(), loopVals->Current() );
			    }

			    SaveFrameNode *sfJustMade = (SaveFrameNode*)
				                            ( (*lastBlockList)[lastBlockList->size() - 1 ] );
		            ASTlist<DataNode*> *sfJustMadeList = sfJustMade->GiveMyDataList();
			    DataLoopNode *loopJustMade = (DataLoopNode*) 
				                            ( (*sfJustMadeList)[sfJustMadeList->size() - 1 ] );

			    DataNameNode *newNameNode = (*(loopJustMade->getNames()[0] ) ) [0];

			    if( tagsProcessed >= tagsInMapper)
			    {
				string comm = sfJustMade->getPreComment();
				if( comm == string("") )
				{  comm = string("#  In the following saveframe, these tags were\n"
						 "#  missing from the order mapping rule in s2nmr,\n"
						 "#  and thus might not appear in order:" );
				}
				comm += string("\n#\t");
				comm += tagNameToUse;
				sfJustMade->setPreComment( comm  );
			    }

			    if( whichLoopStr.substr( 0, 2 ).compare( "t:" ) == 0 )
			    {
				loopJustMade->setTabFlag( true );
			    }
			    else
			    {
				loopJustMade->setTabFlag( false );
			    }

			    loopJustMade->setRowsPerLine( rowsPerLine );

			    if( tagFound ) {
			      loopsToKeep [ loopJustMade ] = 1;
			    }

			    // Add the new loop to the map of remembered
			    // loops, unless _which_loop was a dot ".",
			    // in which case we will deliberately forget
			    // about it so that each instance of dot (.)
			    // looks like a fresh new loop was encountered.
			    if( memorize &&
				whichLoopStr.compare( "t:.") != 0 &&
				whichLoopStr.compare( "l:.") != 0  )
			    {
				whichLoopMap[whichLoopStr] = loopJustMade;
			    }

			    inputLoops->AddToEnd(loopNode);
			    inputWhichLoopStrings->AddToEnd(whichLoopStr);

			    // Now we must remove the column we just copied from the loop from
			    // the missing-tracker tree.
			    loopNode->RemoveColumn( tagNameToUse );
			    --fromTagCol;

			}
			// Else, we have encountered this particular
			// whichLoop string before and so we will tack on
			// another column to the already existing loop:
			else
			{
			    int  rowIdx;
			    DataLoopNode *whichLoop = whichLoopMap[ whichLoopStr ];
			    if( tagFound ) {
			      loopsToKeep [ whichLoop ] = 1;
			    }
			    SaveFrameNode *sfJustMade = (SaveFrameNode*)whichLoop;
			    ASTnode *par;
			    for( par = whichLoop ;
			         par != NULL && !(par->isOfType(ASTnode::SAVEFRAMENODE)) ;
				 par = par->myParent() )
			    {}
			    if( par != NULL )
			        sfJustMade = (SaveFrameNode*)par;

			    DataNameNode *newNameNode = new DataNameNode( tagNameToUse );

			    whichLoop->setRowsPerLine( rowsPerLine );


			    if( tagsProcessed >= tagsInMapper)
			    {
				string comm = sfJustMade->getPreComment();
				if( comm == string("") )
				{  comm = string("#  In the following saveframe, these tags were\n"
						 "#  missing from the order mapping rule in s2nmr,\n"
						 "#  and thus might not appear in order:" );
				}
				comm += string("\n#\t");
				comm += newNameNode->myName();
				sfJustMade->setPreComment( comm );
			    }


			    // Create the new tag name, which creates all the
			    // associated new values in the body of
			    // the loop too.  The new values are copied from
			    // the last row of the new column we are going to
			    // insert.  This way if the new column we are going
			    // to insert has less rows than the target loop, then the
			    // target loop just has the last value replicated to match
			    // the number of rows in the target loop.  For example,
			    // If we insert this:
			    //    _tag_a
			    //    val1
			    //    val2
			    //
			    // into this:
			    //   _tag_x  _tag_y
			    //   a       b
			    //   c       d
			    //   e       f
			    //   g       h
			    // We end up with this result:
			    //  _tag_x  _tag_y  _tag_a
			    //  a       b       val1
			    //  c       d       val2
			    //  e       f       val2
			    //  g       h       val2
			    //
			    DataNameNode *lastNameInLoop = *( ( (*( whichLoop->getNamesPtr() ))[0]->end() ) - 1 );

			    bool addColumn = true;
			    if( lastNameInLoop->myName() == newNameNode->myName() )
			    {
				bool lastColumnIsAllNull = true;
				int row_i;
				LoopTableNode *whichLoopTbl = whichLoop->getValsPtr();
			        for( row_i = 0 ; row_i < whichLoopTbl->size() ; row_i++ )
				{   DataValueNode *lastDVN= (*( (*whichLoopTbl)[row_i]))[(*whichLoopTbl)[row_i]->size()-1];
				    string lastVal = lastDVN->myValue();

				    // If this row of the column has a non-null value:
				    if(    strcmp( lastVal.c_str(), "?") != 0 &&
				           strcmp( lastVal.c_str(), ".") != 0  )
				    {   lastColumnIsAllNull = false;
				    }
				    // If this row of the column has a null value:
				    else
				    {   // Change the old value in-place to the new value if it was null for this row:
					lastDVN->setValue( (*loopVals)[row_i]->myValue() );
					lastDVN->setDelimType( (*loopVals)[row_i]->myDelimType() );
				    }
				}
				if( lastColumnIsAllNull )
				{
				    // Remember to get rid of the previous column to make way for the new
				    // This name used to be called "__eraseme__", but I changed that because of
				    // the danger of accidentally deleting this line (I use the string "eraseme"
				    // as a marker indicating a temp debug line of code that should be deleted.
				    // If the word "eraseme" appears on a line, that line should be deleted.)
				    lastNameInLoop->setName( string("__erase_this_tag__") +
				                             lastNameInLoop->myName() );
				    addColumn = true;
				}
				else
				{
				    string comm = sfJustMade->getPreComment();
				    if( comm == string("") )
				    {  comm = string("#  In the following saveframe, these tags were\n"
						     "#  missing from the order mapping rule in s2nmr,\n"
						     "#  and thus might not appear in order:" );
				    }
				    comm += string("\n#\t      (SPECIAL: This name was duplicated, and only one "
						   "\n#\t          instance was preserved: ");
				    comm += newNameNode->myName();
				    comm += string(" )");
				    sfJustMade->setPreComment( comm );

				    addColumn = false;
				}
			    }
			    if( addColumn && loopVals->size() > 0 )
			    {
				(*(whichLoop->getNamesPtr()))[0]->insert(
						 (*(whichLoop->getNamesPtr()))[0]->end(), newNameNode,
						 *( (*loopVals)[loopVals->size()-1] ) );
				


				// Column index of the last column in the loop body (which
				// should be the one we just created in the line above):
				int  lastCol = (*(whichLoop->getNamesPtr()))[0]->size() - 1;


				
				LoopTableNode *tblNode  = whichLoop->getValsPtr();
				
				// If LoopVals has more rows than this table, then expand this table with dummies
				// to match the new, larger size before continuing:
				for( rowIdx = tblNode->size() ; rowIdx < loopVals->size() ; ++rowIdx ) {
				   LoopRowNode *dummyRow = new LoopRowNode(true);
				   for( int colIdx = 0 ; colIdx < (whichLoop->getNames())[0]->size() ; ++colIdx ) {
				     dummyRow->insert( dummyRow->end(), new DataValueNode( *insertDummyVal ) );
				   }
				   tblNode->insert( tblNode->end(), dummyRow );
				}

	                        prevVals.clear();
				for( rowIdx = 0 ; rowIdx < loopVals->size() && rowIdx < tblNode->size(); rowIdx++ )
				{
				    // Alter the last value in this row (which
				    // right now is just a default dummy) to be a
				    // copy of the value from loopVals:
				    DataValueNode *thisVal = (*(*tblNode)[rowIdx])[ lastCol ];
				    thisVal->setValue( (*loopVals)[rowIdx]->myValue() );
				    thisVal->setDelimType( (*loopVals)[rowIdx]->myDelimType() );
				    prevVals.insert(prevVals.end(), thisVal );
				}


			    }
			    if( strncmp( lastNameInLoop->myName().c_str(), "__erase_this_tag__", 11 ) == 0 ) 
			    {
			       delete lastNameInLoop;
			       prevVals.clear();
			    }

			    // Now we must remove the column we just copied from the loop from
			    // the missing-tracker tree.
			    loopNode->RemoveColumn( tagNameToUse );
			    --fromTagCol;
			}
			delete loopVals;
		    }
		}
	      }
	    ValCache::flushValCache();

	    if( tagsProcessed == tagsInMapper - 1 )
	    {
	        // Add tags to the end of the mapper for the unmapped
		// tags so they get copied too:
	        int i, j, k;
		DataValueNode *dvn = NULL;
		for( i = 0 ; i < inputSaveFrame->GiveMyDataList()->size() ; i++ )
		{
		   DataNode *cur = (*(inputSaveFrame->GiveMyDataList()))[i];
		   if( cur->isOfType( ASTnode::DATAITEMNODE ) )
		   {
		     tagList.insert(tagList.end(), ((DataItemNode*)cur)->myName() );
		     whichLoopList.AddToEnd( string("t:don't_care") );
		   }
		   else if( cur->isOfType( ASTnode::DATALOOPNODE ) )
		   {
		     DataLoopNode *curL = (DataLoopNode*) cur;
		     for( j = 0 ; j < ( (curL->getNames())[0] )->size() ; j++ )
		     {
		       tagList.insert(tagList.end(), (*( (curL->getNames())[0] ) )[j]->myName() );
		       for( k = 0 ; k < inputLoops->size() ; k++ )
		       {
			 if( (*inputLoops)[k] == curL )
			 {
			   whichLoopList.AddToEnd( (*inputWhichLoopStrings)[k] );
			   break;
			 }
			 if( k >= inputLoops->size() )
			 {
			   cerr<<"internal error: file: "<<__FILE__<<", line "<<__LINE__<<endl;
			 }
		       }
		     }
		   }
		}
	    }
	    delete inputDataList;

	  }

	  if( inputLoops != NULL ) delete inputLoops;
	  if( inputWhichLoopStrings != NULL ) delete inputWhichLoopStrings;

	  // 
	  // If any of the loops we just made were 100% dummy loops that we only made
	  // just now during this rule, then just get rid of them again:
	  // 
	  // Skip this culling out if we were supposed to insert dummy loops
	  if( ! insertEntireDummyLoop ) {
	      BlockNode *lastBlock = (BlockNode *) ( (* (output->GiveMyDataBlockList()) )[ output->GiveMyDataBlockList()->size() - 1 ] );
	      SaveFrameNode* lastSF = (SaveFrameNode *) ( (* (lastBlock->GiveMyDataList()) )[ lastBlock->GiveMyDataList()->size() - 1 ] );
	      List <ASTnode*> *lastSFLoops  = lastSF->searchForType(ASTnode::DATALOOPNODE );
	      for( int loops_i = 0 ; loops_i < lastSFLoops->size() ; ++loops_i ) {
		if( loopsToKeep.find(  (DataLoopNode*) ( (*lastSFLoops)[loops_i] )  ) ==
		      loopsToKeep.end() ) {
		  delete (*lastSFLoops)[loops_i] ;  // remove the loop from its parent.
		}
	      }
	      delete lastSFLoops;
	  }
	  
      }
      delete allMatchedFrames;
      whichLoopMap.clear();
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
	      (*errStream) << "#transformII-1# Error: The saveframe called " <<
		      allNodesInBlock->Current()->myName() <<
		      " was not mapped in the ordering." << endl;
      }
  }
  // Delete the sentinel saveframe now: assume there is exactly one:
  //
  List <ASTnode*> *sentinels = output->searchByTag( sentinelName );
  sentinels->Reset();
  delete sentinels->Current();
  delete sentinels;


  // Output unmapped data at the bottom of the file:
  if( g_generate_comments )
  {
      ostream *rememberOS = os;
      char tempfname[256];
      sprintf( tempfname, "%s_missing.str", g_outFname.c_str() );
      os = new ofstream( tempfname );
      if( os == NULL )
      {
	 cerr<< "# Unable to open temp file for writing: " << tempfname
	    << strerror(errno) << endl;
      }
      else
      {
	  (*os) << endl
		<< "## THIS SECTION CONTAINS THE TAGS THAT WERE NOT" << endl
		<< "## MENTIONED IN THE SAVE_ORDER_AND_LOOPS RULE" << endl
		<< "## AND THEREFORE WERE LOST OR INCORRECTLY ORDERED IN THE MAPPING." << endl
		<< "## [" << endl;
	  input->Unparse(0);
	  (*os) << "## ]" << endl
		<< "## END OF UNMAPPED TAG LIST" << endl;
	  os->flush();
	  os = rememberOS;
      }
  }


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
