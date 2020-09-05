//////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
// Revision 1.7  2000/11/18  02:26:29  madings
// Fixed a problem that was common to several of the files (apparently
// all the ones edited by 'june', although it's hard to tell if this is
// her doing or not.)  The problem is that there were several places in
// the code where something like this had been done:
//     char *strPtr1, *strPtr2;
//     ...
//     /* strPtr points at some string right now */.
//     strPtr2 = new char [ strlen(strPtr1) ];  /* error: needs a +1 */
//     strcpy( strPtr2, strPtr1 );
// The problem with this, of course. is that strlen() gives the length
// of the string NOT including the null terminator, so the above code
// doesn't allocate enough space for strPtr2 to store the terminator.
// The result is that when the strcpy() is called, a zero-byte (null
// terminator) overwrites whatever was in the next byte just after strPtr2.
// Depending on what that next byte actually is, this might corrupt the
// value, or might not if it was supposed to be a zero anyway.  I suspect
// that this is what was causing some erratic segfaults we were seeing
// in this program - where seemingly irrelevant changes to the input data
// would 'fix' crashes.  (Even though the data changed had nothing to do with
// the part of the code that crashed.)  I suspect that the overwritten byte
// was part of the data the C-library heap uses internally to keep track of
// how much memory a variable on the heap is taking up (so it knows how much
// to free later when free() is called).  When 'free()' was being called, the
// wrong amount of memory was being freed, and thus the heap was a jumbled
// mess after that.)
//
// Revision 1.6  1999/07/30  18:22:07  june
// june, 7-30-99
// add transform1d.cc
//
Revision 1.5  1999/04/09 20:15:55  madings
added many, many transforms.  transform 17 still does not work yet, though.

// Revision 1.4  1999/01/28  06:15:06  madings
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
// Revision 1.3  1998/12/15  04:33:07  madings
// Addded transforms 9 though 14.  (Lots of stuff)
//
// Revision 1.2  1998/11/11  04:08:19  madings
// Added transform6, but did so in such a way that CVS is falsely
// marking every single file as being changed, so this comment will
// probably appear in many files where it does not apply.
//
// Revision 1.7  1997/12/09  17:26:53  bsri
// Modified all the transformations so that the AST tree is searched for
// tags and save frames, and appropriate changes are made to the
// corresponding save frames in the NMR tree (by following pointers from
// the AST tree to the NMR tree: this is done by using myParallelCopy()).
//
// Revision 1.5  1997/10/28  21:38:31  bsri
//  This transformation code has been changed so that it no longer iterates
//  through lists of AST nodes, instead, uses the starlib API.
//
// Revision 1.4  1997/10/10  18:34:29  madings
// Changed DataValueNode so that it now no longer has subclasses for
// single-quote-delimited strings, double-quote delimited strings,
// semicolon-delimited strings, and so on.  Now there is only one kind
// of DataValueNode, and it uses a flag to decide on the delimiter type.
// This also allows for the creation of methods to change a DataValueNode
// in place, which was not previously possible when different kinds of
// values had to be stored in different kinds of classes.
//
// Revision 1.3  1997/10/01  22:38:57  bsri
// Replaced transform2.cc with new file, and added RCS Log comment
// to all the transform*.cc files.
//
*/
#endif
///////////////////////////////////////////////////////////////

#include "transforms.h"

//########################################################################################
//########################################################################################


void save_all_tag_to_scope(StarFileNode* AST,
			   StarFileNode* NMR,
			   BlockNode* ,
			   SaveFrameNode *currDicRuleSF)
{
  
  //The function applies transformation 2 for a specified dictionary datablock
  //First get the "save_tag_to_value" saveframe in the current dictionary datablock

  
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

  
ASTlist<BlockNode*>* listOfDataBlocks =  AST->GiveMyDataBlockList();

listOfDataBlocks->Reset();
while(!listOfDataBlocks->AtEnd())
  {
      string currDatablockName    = (listOfDataBlocks->Current())->myName();

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

	  List<ASTnode *> *hits = AST->searchByTag(iterVal[1]);

	  for (hits->Reset(); !hits->AtEnd(); hits->Next())
	  {
       	     ASTnode *found = hits->Current();

       	     SaveFrameNode *astSaveFrame = (SaveFrameNode *)found;

	     SaveFrameNode *nmrSaveFrame = (SaveFrameNode *)(astSaveFrame->myParallelCopy());
	     if (nmrSaveFrame != NULL)
	     {
	        //Search for the appropriate tag in the saveframe
	        List<ASTnode *> *tagHits = astSaveFrame->searchByTag(iterVal[0]);

	        for (tagHits->Reset(); !tagHits->AtEnd(); tagHits->Next())
	        {
		    ASTnode *astTagCur = tagHits->Current();
		    ASTnode *nmrTagPeer;

		    if (astTagCur->myType() == DataNode::DATAITEMNODE)
		    {
		          DataItemNode *cur = (DataItemNode *)astTagCur;
		          string tagValue = cur->myValue();
		          nmrSaveFrame->changeName(
			    string("save_") + deSpacify(tagValue) );

		          nmrTagPeer = astTagCur->myParallelCopy();
		          if (nmrTagPeer != NULL)
			    if( iterVal[2] == string( "yes" ) )
				delete (DataItemNode *)nmrTagPeer;
			  continue;
		    }
		    if (astTagCur->myType() == DataNode::DATANAMENODE)
		    {
       		          for ( ; ((astTagCur != NULL) && 
				(astTagCur->myType() != ASTnode::DATALOOPNODE));
 	                   	astTagCur = astTagCur->myParent() )
	       			;

			  // Check - only legal situations are:
			  //   1 - All values of the tag are the same, or
			  //   2 - If values differ, they are always different - no repeats.
			  ASTlist<DataValueNode*> *loopColumn;
			  loopColumn = ((DataLoopNode*)astTagCur)->returnLoopValues(iterVal[0]);
			  DataValueNode  *curVal;

			  bool homogeneous = true;  // Are all the saveframe names the same?
			  bool repeat      = false; // Are there any repeated names?

			  for( loopColumn->Reset() ; !loopColumn->AtEnd() ; loopColumn->Next() )
			  {
			      curVal = loopColumn->Current();
			      
			      // Loop over all the values from the start up to this
			      // one, looking for duplicates:
			      for(    loopColumn->Reset() ;
				      loopColumn->Current() != curVal ;
				      loopColumn->Next()   )
			      {
				  if( curVal->myValue() == loopColumn->Current()->myValue() )
				  {
				      repeat = true;
				  }
				  else
				      homogeneous = false;
			      }
			  }
			  if( repeat && ! homogeneous )
			  {
			      (*errStream)<<"#transform-2# Error: encountered nonhomogeneous repeat:"<<endl;
			      (*errStream)<<"#             In tag: " << iterVal[0] <<" in saveframe: "<< iterVal[1] << endl;
			      return;
			  }
			  if( homogeneous )
			  {
			      // Just rename the saveframe and delete the column from the loop.
			      // Don't do anything else.
			      loopColumn->Reset();
			      string tagValue = loopColumn->Current()->myValue();
			      nmrSaveFrame->changeName(
				string("save_") + deSpacify(tagValue) );

			      nmrTagPeer = astTagCur->myParallelCopy();
			      if (nmrTagPeer != NULL)
			      {
				  // remove the column for the saveframe name.
				  if( iterVal[2] == string( "yes" ) )
				      ((DataLoopNode *)nmrTagPeer)->RemoveColumn( iterVal[0] );
			      }
			  }
			  else // split the loop apart, it's nonhomogeneous and nonrepeating.
			  {
			      loopColumn->Reset();

			      List<DataNameNode *> *attrNameList;
			      List<DataValueNode *> *attrValList;
			      ((DataLoopNode*)astTagCur)->FlattenNestedLoop(attrNameList,
								    attrValList);
			      int length = attrNameList->Length();
			      attrValList->Reset();

			      //For each value in the loop columns, change its value.
			      for ( loopColumn->Reset(); !loopColumn->AtEnd(); loopColumn->Next() )
			      {
				 DataValueNode *currVal = loopColumn->Current();
				 string tagValue = currVal->myName();

				 //construct new save frame here
				 SaveFrameListNode *l = new SaveFrameListNode(new ASTlist<DataNode*>);
				 SaveHeadingNode *h = 
				    new SaveHeadingNode(string("save_") + deSpacify(tagValue));
				 SaveFrameNode *s = new SaveFrameNode(h,l);

				 NMR->AddSaveFrameToDataBlock(currDatablockName,s);

				 attrNameList->Reset();
				 for (int i = 1; i <= length; i++)
				 {
					string currTag = attrNameList->Current()->myName();
					string currVal = attrValList->Current()->myName();
					DataValueNode::ValType currDelim = 
					    attrValList->Current()->myDelimType();
					if( currTag != iterVal[0] || iterVal[2] != string("yes") )
					{
					    DataNameNode *currDataTag = new DataNameNode(currTag);
					    DataValueNode *currDataVal = new DataValueNode(currVal,
									    currDelim);
					    DataItemNode *currDataItem = new DataItemNode(currDataTag,
								    currDataVal);
					    //insert this item into the new save frame
					    s->AddItemToSaveFrame(currDataItem);
					}
					attrNameList->Next();
					attrValList->Next();
				 }
			      }
			      nmrTagPeer = astTagCur->myParallelCopy();
			      if (nmrTagPeer != NULL)
			      {
				  delete (DataLoopNode *)nmrTagPeer;
				  if (nmrSaveFrame->GiveMyDataList()->Length() == 0)
				      delete nmrSaveFrame;
			      }
			  }
		       }
		       else
		       { //error: found a save frame within another save frame;
		       }
		 }
		 delete tagHits;
	      }
	   }
	   delete hits;
	} //End while loop -- for all dictionary entries for this transformation
     
        listOfDataBlocks->Next();
   }
}
