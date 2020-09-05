///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
// Revision 1.7  2000/03/16  05:09:42  madings
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
// Revision 1.6  1999/08/04  01:39:37  madings
// Undid one of June's changes that had no squiggly braces {} around it.
// She added a condition without enclosing it in braces and I cannot
// tell where she meant it to end...
//
// Revision 1.5  1999/07/30  18:22:07  june
// june, 7-30-99
// add transform1d.cc
//
Revision 1.4  1999/04/20 21:41:43  madings
Added transforms 17 and 18 to main.cc's case statement, and
added transform17.cc and altered transform 2a and 1c so they
can handle the case where the input is not coming from a saveframe,
but is coming from outside a saveframe instead.

// Revision 1.3  1999/01/28  06:15:06  madings
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
// Revision 1.2  1998/11/11  04:08:18  madings
// Added transform6, but did so in such a way that CVS is falsely
// marking every single file as being changed, so this comment will
// probably appear in many files where it does not apply.
//
// Revision 1.4  1997/12/09  17:26:03  bsri
// Modified all the transformations so that the AST tree is searched for
// tags and save frames, and appropriate changes are made to the
// corresponding save frames in the NMR tree (by following pointers from
// the AST tree to the NMR tree: this is done by using myParallelCopy()).
//
// Revision 1.2  1997/10/01  22:38:57  bsri
// Replaced transform2.cc with new file, and added RCS Log comment
// to all the transform*.cc files.
//
*/
#endif
///////////////////////////////////////////////////////////////

#include "transforms.h"
//########################################################################################
//#######################################################################################

void save_keyed_renamed_tags( StarFileNode* AST,
		              StarFileNode* NMR,
		              BlockNode* ,
			      SaveFrameNode *currDicRuleSF)

{

    //The function applies transformation 1c  for a specified dictionary datablock
    //First get the "save_keyed_renamed_tags" saveframe in the current dictionary datablock

    DataNode* currDicSaveFrame = currDicRuleSF;

    if(!currDicSaveFrame)
	return;     //Simply return if the desire saveframe is not found

    //Get the loop in the data dictionary
    string currDicLoopName = string("_key_tag");
    DataNode* currDicLoop = 
	((SaveFrameNode*)currDicSaveFrame)->ReturnSaveFrameDataNode(currDicLoopName);

    //Flatten the nested loop
    List<DataNameNode*>* currDicNameList;
    List<DataValueNode*>* currDicValList;
    ((DataLoopNode*) currDicLoop)->FlattenNestedLoop(currDicNameList,currDicValList);
    int length = currDicNameList->Length();

    currDicNameList->Reset();
    currDicValList->Reset();

    string*  iterVal = new string[length];

    //First search for the correct point to begin in dictionary

    while( !currDicValList->AtEnd() )
      {
	//Get the values of the next iteration
	//Note that:
	//iterVal[0] is the _key_tag
	//iterVal[1] is the _key_value
	//iterVal[2] is the _old_tag_name
	//iterVal[3] is the _new_tag_name

	for(int i =0; i<length ; i++)
	  {
	    iterVal[i] = currDicValList->Current()->myName();
	    currDicValList->Next();
	  }
#ifdef DEBUG
	  cerr << "DEBUG: working on dictionary entry:\n\t" <<
	      iterVal[0] << "\t" << iterVal[1] << "\t" <<
	      iterVal[2] << "\t" << iterVal[3] << endl;
#endif

	// Special case: skip if this is an identity transform:
	// ----------------------------------------------------
	if( tagToVal(iterVal[2]) == tagToVal(iterVal[3]) )
	  continue;
		    
	//Now apply transformations to each saveframe in generic star as appropriate

	// Search for the old tag name in the AST tree
	List <ASTnode *> *oldTagMatches = AST->searchByTag(iterVal[2]);

	if (oldTagMatches->Length() == 0)
		(*errStream) << "#transform-1c# Could not find old tag " << iterVal[2]
			<< " in input star file" << endl;


	for (oldTagMatches->Reset(); !oldTagMatches->AtEnd(); oldTagMatches->Next())
	  {
	    ASTnode *foundOldTag;
	    SaveFrameNode *astSaveFrame;
	    BlockNode *astBlock;

           // Get the parent save frame
       	   for (   foundOldTag = oldTagMatches->Current()
		   ;
	       	   (foundOldTag != NULL) &&
		   !(foundOldTag->isOfType( ASTnode::SAVEFRAMENODE ) ) &&
		   !(foundOldTag->isOfType( ASTnode::BLOCKNODE ) )
		   ;
 	           foundOldTag = foundOldTag->myParent()
	       )
	   {
	   }

       	   if (foundOldTag == NULL)
	   {
	       cerr << "Error: programming mistake at transform1.cc::"
		   << __LINE__ << endl;
	   }
	   else if( foundOldTag->isOfType( ASTnode::SAVEFRAMENODE ) )
	   {
       	  	astSaveFrame = (SaveFrameNode *)foundOldTag;
		astBlock = NULL;
	   }
	   else if( foundOldTag->isOfType( ASTnode::BLOCKNODE ) )
	   {
       	  	astSaveFrame = NULL;
       	  	astBlock = (BlockNode *)foundOldTag;
	   }
	   
	    //Check the saveframe/block items list to see if keyed tag-value exists

	    bool found = false;

	    // Special case: if both the _key_tag and _key_value are 
	    // asterisks, then try to apply the change regardless of what
	    // saveframe/block this is.
	    if(  tagToVal(iterVal[0]) == string("*") && tagToVal(iterVal[1]) == string("*")  )
	      {
		found = true;
	      }
	    else
	      {
		// Search for the key-value in the current save frame or block
		List <ASTnode *> *keyTagMatches;
	        if( astSaveFrame )
		  {
		    keyTagMatches = astSaveFrame->searchByTagValue(iterVal[0], iterVal[1]);
		  }
		else
		    {
		      keyTagMatches = astBlock->searchByTagValue(iterVal[0], iterVal[1]);
		    }
		if (keyTagMatches->Length() > 0)
		{
		  found = true;
		}

		delete keyTagMatches;
	      }

	    if(found)
	      {
		//First check if the new_tag_name already exists in the new_saveframe
		//if it does, flag an error

		SaveFrameNode *nmrSaveFrame;
		BlockNode *nmrBlock;
		if( astSaveFrame )
		{
		    nmrSaveFrame = (SaveFrameNode *)(astSaveFrame->myParallelCopy());
		    nmrBlock = NULL;
		}
		else
		{
		    nmrSaveFrame = NULL;
		    nmrBlock = (BlockNode *)(astBlock->myParallelCopy());
		}
		List <ASTnode *> *newTagMatches =
		        (nmrSaveFrame) ?
			    nmrSaveFrame->searchByTag(iterVal[3]) :
			    nmrBlock->searchByTag(iterVal[3]) ;

		//  ========== COMMENT OUT BLOCK ====================
		//  This check has been commented out so that it does
		//  not happen anymore.  Now that tags are case-insensitive,
		//  this was causing errors when trying to rename a tag in
		//  case only (i.e. rename '_foo' to '_Foo' died because it
		//  said it was already there.)
		//  if (newTagMatches->Length() > 0)
		//    (*errStream)<< "#transform1-c# Error: on line: "
		//  	      << tagToVal(iterVal[0]) << " "
		//  	      << tagToVal(iterVal[1]) << " "
		//  	      << tagToVal(iterVal[2]) << " "
		//  	      << tagToVal(iterVal[3]) << endl
		//  	      << "#     error was: Target tag name already exists in "
		//  	      << ( nmrSaveFrame ? "saveframe." : "block." ) << endl;
		//  else
		//  {
		
		    ASTnode *nmrTagPeer = oldTagMatches->Current()->myParallelCopy();

		  //At this point we know that the new tag does not exist in new_saveframe
		  //When item is found, rename the old tag name with new tagname
		  if((oldTagMatches->Current())->isOfType(DataNode::DATAITEMNODE))
		  {

		    // Rename the data item
		    ((DataItemNode*)nmrTagPeer)->setName(iterVal[3]);
	          }

		  else
		  {
		    // Loop Tag Name
		    ((DataNameNode *)nmrTagPeer)->setName(iterVal[3]);
	          }
		
	        // Not in an else-block anymore so the end-squiggle is commented out:
		// }

	      } // if found

	  } // Look for next match of old tag

      } // Next set of entries in dictionary
}

