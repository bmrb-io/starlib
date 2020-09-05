//////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
// Revision 1.6  1999/07/30  18:22:07  june
// june, 7-30-99
// add transform1d.cc
//
Revision 1.5  1999/04/20 21:41:43  madings
Added transforms 17 and 18 to main.cc's case statement, and
added transform17.cc and altered transform 2a and 1c so they
can handle the case where the input is not coming from a saveframe,
but is coming from outside a saveframe instead.

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
// Revision 1.3  1998/12/15  04:33:08  madings
// Addded transforms 9 though 14.  (Lots of stuff)
//
// Revision 1.2  1998/11/11  04:08:20  madings
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


void save_tag_to_frame_free( StarFileNode* AST,
			     StarFileNode* NMR,
			     BlockNode*,
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

		// If the value does not start with an underscore, then it is not
                // a tag, it is the direct value itself.
		if( iterVal[0].c_str()[0] != '_' )
		{
		    nmrSaveFrame->changeName( string(iterVal[0].c_str()) );
		}
		else // value is a tag to set it to:
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
			      string saveName;
			      saveName = string("save_") + deSpacify( cur->myValue() );
			      nmrSaveFrame->changeName( saveName );

			      nmrTagPeer = astTagCur->myParallelCopy();
			      if (nmrTagPeer != NULL)
			      {
				if( iterVal[2] == string( "yes" ) )
				{
				    delete (DataItemNode *)nmrTagPeer;
				}
				continue;
			      }
			}
			else
			{
			    (*errStream) <<
				"#transform-2a# Error: " <<
				"Encountered a rule for a looped tag '" <<
				iterVal[0] <<
				"' in saveframe '" <<
				iterVal[1] <<
				"'" <<
				endl <<
				"                      This rule only operates on non-looped tags." <<
				endl;
			}
		    }
		    delete tagHits;
		}
	      }
	   }
	   delete hits;
	} //End while loop -- for all dictionary entries for this transformation
     
        listOfDataBlocks->Next();
   }
}
