
///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.6  2000/11/18 03:02:39  madings
*** empty log message ***

// Revision 1.5  1999/07/30  18:22:09  june
// june, 7-30-99
// add transform1d.cc
//
Revision 1.4  1999/04/09 20:15:55  madings
added many, many transforms.  transform 17 still does not work yet, though.

// Revision 1.3  1999/01/28  06:15:07  madings
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
// Revision 1.2  1998/11/11  04:08:24  madings
// Added transform6, but did so in such a way that CVS is falsely
// marking every single file as being changed, so this comment will
// probably appear in many files where it does not apply.
//
// Revision 1.5  1997/12/09  17:27:34  bsri
// Modified all the transformations so that the AST tree is searched for
// tags and save frames, and appropriate changes are made to the
// corresponding save frames in the NMR tree (by following pointers from
// the AST tree to the NMR tree: this is done by using myParallelCopy()).
//
// Revision 1.4  1997/10/21  21:29:17  bsri
//   This transformation code has been changed so that it no longer iterates
//   through lists of AST nodes, instead, uses the starlib API.
//
// Revision 1.2  1997/10/01  22:38:58  bsri
// Replaced transform2.cc with new file, and added RCS Log comment
// to all the transform*.cc files.
//
*/
#endif
///////////////////////////////////////////////////////////////

#include "transforms.h"
#include <iostream>
//#####################################################################################
//#####################################################################################

void save_keyed_tag_to_value(StarFileNode* AST,
		       StarFileNode* NMR,
		       BlockNode*,
		       SaveFrameNode *currDicRuleSF)
{
  //The function applies transformation 3 c  for a specified dictionary datablock
  //First get the "save_keyed_tag_to_value" saveframe in the current dictionary datablock
  
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

  //Now apply transformations to each saveframe in generic star as appropriate

  currDicNameList->Reset();
  currDicValList->Reset();
	    
  string*  iterVal = new string[length];

  // Move through the dictionary until this transformation is done
  while(!currDicValList->AtEnd())
  {
    DataValueNode* rememberedCurDicPos;

    //Get the values of the next iteration
    //iterVal[0] is the _old_tag_name
    //iterVal[1] is the _key_tag
    //iterVal[2] is the _key_value
    //iterVal[3] is the _new_tag_name
    //iterVal[4] is the _tag_name_become_value

    

    // We'll want to rewind to this point in the mapping dictionary for each saveframe:
    rememberedCurDicPos =  currDicValList->Current();
    for(i =0; i<length && !currDicValList->AtEnd(); i++)
    {
	iterVal[i] = currDicValList->Current()->myName();
	currDicValList->Next();
    }


    // Find the source saveframes that match in the AST tree:

    List <ASTnode *> *saveFrameMatches = AST->searchForTypeByTagValue(
	    ASTnode::SAVEFRAMENODE, iterVal[1], iterVal[2]);

    for (saveFrameMatches->Reset(); !saveFrameMatches->AtEnd(); saveFrameMatches->Next())
    {
        // Rewind dictionary until it gets to the point where the current piece of the
        // dictionary starts.  This is ugly.  It is a fix to a bug where this code
        // didn't work when multiple saveframes were encountered (it would exhaust
        // the dictionary list and not re-use it on the second encountered saveframe):
        // I consider this to be a somewhat stopgap fix.  Ideally, this rule should be
        // re-written to make a temp sublist of the mapping dictionary for this saveframe
        // and iterate on it instead of on the big list.
        currDicValList->Reset();
        while( currDicValList->Current() != NULL && rememberedCurDicPos !=  currDicValList->Current() )
	    currDicValList->Next();
        if( currDicValList == NULL )
        {
	    currDicValList->Reset();
	    (*errStream)<<"#transform-3c# Error: This error should never happen." <<
	        "Internal error on line " << __LINE__ << endl;
 	    return;  // ugly - orphans some memory, but this is a 'should never happen'
 	             // error anyway, so....
        }
        for(i =0; i<length && !currDicValList->AtEnd(); i++)
        {
	    iterVal[i] = currDicValList->Current()->myName();
	    currDicValList->Next();
        }
	// Check if we got all 5 elements
	if (i < length)
	{
	    (*errStream)<<"#transform-3c# Error: In mapping dictionary: " <<
		currDicSaveFrame->myName() << ": The number of values in the loop " <<
		"is not an integral multiple of " << length << "." << endl;
	  return;
	}


        SaveFrameNode *astSaveFrame = (SaveFrameNode*) saveFrameMatches->Current();

        SaveFrameNode *nmrSaveFrame = (SaveFrameNode *)(astSaveFrame->myParallelCopy());

    	// Apply all the dictionary transformation to this nmr saveframe
  	bool firstItn = true;
	string loopFirstTag = tagToVal(iterVal[3]);
	string keyTag = tagToVal(iterVal[1]);
	string keyVal = tagToVal(iterVal[2]);
		
	//Initialize the NMR tag and values lists here. The purpose is to collect
	//all loop tags and values in these lists and then construct the loop
	//at the end.

	List<string>* NMRtagList = new List<string>;
	List<DataValueNode*>* NMRvalList = new List<DataValueNode*>;

	bool done = false;

	while(!done)  //While construction of loop is not over
	  {
	    if(firstItn) 
	    {
	      NMRtagList->AddToEnd(tagToVal(iterVal[3]));
	    }

	    if(string("yes") == tagToVal(iterVal[4]))
	    {
	      NMRvalList->AddToEnd(new DataValueNode(removeFirstChar(tagToVal(iterVal[0])),
							DataValueNode::NON));
	    }

	    else
	      {
		//Search for the appropriate tag in the saveframe
		List <ASTnode *> *itemList = nmrSaveFrame->searchByTag(iterVal[0]);
		for (itemList->Reset(); !itemList->AtEnd() 
			&& (!itemList->Current()->isOfType(DataNode::DATAITEMNODE))
				; itemList->Next())
		    ;

		if (! itemList->AtEnd())
		  {

		    //When tag is found add it's value to the loop 
		    NMRvalList->AddToEnd(
			    new DataValueNode(
				    ((DataItemNode*)itemList->Current())->myValue(),
				    ((DataItemNode*)itemList->Current())->myDelimType()));

		    //Delete this item from the NMR saveframelist 
		    itemList->Current()->removeMe();
		  }
		else
		  {
		      //found a loop.
		      (*errStream)<<"#transform-3c# Error: In mapping dictionary: " <<
		              currDicSaveFrame->myName() << ": This rule is only capable " <<
			      "of converting free tags to looped values.  It cannot convert " <<
			      "values that are already looped.  (Aborting rule.)" << endl;
		      return;
		  }
		delete itemList;

	      }
		    
	    if(!currDicValList->AtEnd())
	      {
		//Read in the next iteration from the dictionary
		for(i =0; i<length ; i++)
		  {
		    iterVal[i] = currDicValList->Current()->myName();
		    currDicValList->Next();
		  }
		// Check if we got all 5 elements
		if (i < length)
		{
		    (*errStream)<<"#transform-3c# Error: In mapping dictionary: " <<
		    currDicSaveFrame->myName() << ": The number of values in the loop " <<
		    "is not an integral multiple of " << length << "." << endl;
		    return;
		}
		if (keyTag != tagToVal(iterVal[1]) || keyVal != tagToVal(iterVal[2]))
		{
		  done = true;

		  // Go Back up one row, to undo the last read so things are
		  // set right for the next chunk of rows that comes along - madings fix.
		  for( int iii = 0 ; iii < length ; iii++ )
		      currDicValList->Prev();
		}
		else
		  if(tagToVal(iterVal[3]) == loopFirstTag)
		    firstItn = false;
	      }
	    else
	      done = true;

	  } //done with collecting data for loop


	//And now finally, construct the loop
	DataLoopNode * currNMRLoop = new DataLoopNode( string("tabulate") );
		
	//Now actually construct the loop from the NMRtagList and NMRvalList
	NMRtagList->Reset();
	while(!NMRtagList->AtEnd())
	  {
	    currNMRLoop->AddDataName(NMRtagList->Current());
	    NMRtagList->Next();
	  }
		  
	NMRvalList->Reset();
	while(!NMRvalList->AtEnd())
	  {
	    currNMRLoop->AddDataValue(NMRvalList->Current()->myName(),
	    NMRvalList->Current()->myDelimType());
	    NMRvalList->Next(); 
	  }

	delete NMRtagList;
	delete NMRvalList;

	//Add this loop to the NMR tree (in the current saveframe)
	nmrSaveFrame->AddItemToSaveFrame(currNMRLoop);

   } 	//endfor
   delete saveFrameMatches;
  } //endwhile
  delete []iterVal;
}

