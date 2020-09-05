//file: transform1d.cc
//by june, 7-30-99
//based on transfrom1c.cc
//This function will rename tags which are inside saveframes, those ounside saveframe will not be renamed. Usefule for keeping old tags in mmCIF file tha old name.

#ifdef BOGUS_DEF_COMMENT_OUT
/*
 * $Log: not supported by cvs2svn $
 */
#endif

#include "transforms.h"
//########################################################################################
//#######################################################################################

void save_keyed_renamed_tags_no_block( StarFileNode* AST,
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
		(*errStream) << "#transform-1d# Could not find old tag " << iterVal[2]
			<< " in input star file" << endl;


	for (oldTagMatches->Reset(); !oldTagMatches->AtEnd(); oldTagMatches->Next())
	  {
	    ASTnode *foundOldTag;
	    SaveFrameNode *astSaveFrame;

           // Get the parent save frame
       	   for (   foundOldTag = oldTagMatches->Current()
		   ;
	       	   (foundOldTag != NULL) &&
		   !(foundOldTag->isOfType( ASTnode::SAVEFRAMENODE ) )
		   ;
 	           foundOldTag = foundOldTag->myParent()
	       )
	   {
	   }

       	   if (foundOldTag == NULL)
	       continue;

	   else if( foundOldTag->isOfType( ASTnode::SAVEFRAMENODE ) )
	   {
       	  	astSaveFrame = (SaveFrameNode *)foundOldTag;
	   }
	    //Check the saveframe/block items list to see if keyed tag-value exists

	    string *asteriskStr = new string("*");
	    bool found = false;

	    // Special case: if both the _key_tag and _key_value are 
	    // asterisks, then try to apply the change regardless of what
	    // saveframe/block this is.
	    if(  tagToVal(iterVal[0]) == *asteriskStr && tagToVal(iterVal[1]) == *asteriskStr  )
		found = true;
	    else
	      {
		// Search for the key-value in the current save frame or block
		List <ASTnode *> *keyTagMatches;
		    keyTagMatches = astSaveFrame->searchByTagValue(iterVal[0], iterVal[1]);
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
		if( astSaveFrame )
		{
		    nmrSaveFrame = (SaveFrameNode *)(astSaveFrame->myParallelCopy());
		}
		List <ASTnode *> *newTagMatches =
			    nmrSaveFrame->searchByTag(iterVal[3]);
		if (newTagMatches->Length() > 0)
		  (*errStream)<< "#transform1-d# Error: on line: "
			      << tagToVal(iterVal[0]) << " "
			      << tagToVal(iterVal[1]) << " "
			      << tagToVal(iterVal[2]) << " "
			      << tagToVal(iterVal[3]) << endl
			      << "#     error was: Target tag name already exists in "
			      <<  "saveframe."  << endl;
		else
		{
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
	        }

	      } // if found

	  } // Look for next match of old tag

      } // Next set of entries in dictionary
}

