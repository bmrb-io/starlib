///////////////////// RCS LOG /////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
//$Log: not supported by cvs2svn $
//Revision 1.5  2000/11/18 03:02:37  madings
//*** empty log message ***
//
// Revision 1.4  1999/07/30  18:22:07  june
// june, 7-30-99
// add transform1d.cc
//
//Revision 1.3  1999/01/28 06:15:05  madings
//Some bugfixes from Eldon's attempts to use this, as well as a major
//design change:
//
//   Bug fix 1: save_loops was re-creating tags that had been removed
//      earlier in the run.  This has been fixed.
//
//   Bug fix x: after bug fix 1 above, save_loops was leaving behind
//      loops that had no contents.  This has been fixed.
//
//   Massive design change:
//      There is no longer any default order of operations at all.
//      Instead, it uses the new 'save_order_of_operations' saveframe
//      in the mapping dictionary file to decide what rules to run
//      when.  Also, to allow it to have the same rule run more than
//      once at different times from different mapping dictionary
//      saveframes, the name of the saveframe is no longer the rule
//      to run.  Instead the name of the saveframe is irrelevant, and
//      it's the new _Saveframe_category tag that tells the program
//      what rule to run for that saveframe.  This required a small,
//      but repetative change in all the transform rules so that the
//      main.cc program can tell the transforms which saveframe in the
//      mapping file is the one they are supposed to use instead of the
//      transforms trying to look for it themselves.
//
// Revision 1.2  1998/11/11  04:08:17  madings
// Added transform6, but did so in such a way that CVS is falsely
// marking every single file as being changed, so this comment will
// probably appear in many files where it does not apply.
//
// Revision 1.6  1997/12/09  17:25:49  bsri
// Modified all the transformations so that the AST tree is searched for
// tags and save frames, and appropriate changes are made to the
// corresponding save frames in the NMR tree (by following pointers from
// the AST tree to the NMR tree: this is done by using myParallelCopy()).
//
// Revision 1.5  1997/10/21  21:12:46  bsri
// This transformation code has been changed so that it no longer iterates
// through lists of AST nodes, instead, uses the starlib API.
//
// Revision 1.3  1997/10/10  18:34:28  madings
// Changed DataValueNode so that it now no longer has subclasses for
// single-quote-delimited strings, double-quote delimited strings,
// semicolon-delimited strings, and so on.  Now there is only one kind
// of DataValueNode, and it uses a flag to decide on the delimiter type.
// This also allows for the creation of methods to change a DataValueNode
// in place, which was not previously possible when different kinds of
// values had to be stored in different kinds of classes.
//
// Revision 1.2  1997/10/01  22:38:56  bsri
// Replaced transform2.cc with new file, and added RCS Log comment
// to all the transform*.cc files.
//
*/
#endif
///////////////////////////////////////////////////////////////
#include "transforms.h"

//########################################################################################
//########################################################################################
void save_tag_to_framecode(StarFileNode* AST,
			   StarFileNode* NMR,
			   BlockNode*,
			   SaveFrameNode *currDicRuleSF)
{
  
//The function applies transformation 1b  for a specified dictionary datablock
//First get the "save_tag_to_framecode" saveframe in the current dictionary datablock
  
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

  //For each datablock we will apply this transformation
  currDicValList->Reset();
  string*  iterVal = new string[length];

  while(!currDicValList->AtEnd())
  {
      //Get the values for one loop iteration
      for(int i =0; i<length ; i++)
      {
          iterVal[i] = tagToVal(currDicValList->Current()->myName());
          currDicValList->Next();
      }

      //Retrieve the appropriate saveframe containing tag to be changed
      //If the sf tag is asterisk, then do it for all saveframes:
      List<ASTnode *> *astSaveFrameHits;
      if( iterVal[1] == string("*") )
	  astSaveFrameHits = AST->searchForType( ASTnode::SAVEFRAMENODE ); 
      else
	  astSaveFrameHits = AST->searchByTag(iterVal[1]); 
	 
      for ( astSaveFrameHits->Reset(); !astSaveFrameHits->AtEnd(); astSaveFrameHits->Next() )
      {
    	    ASTnode *saveframecur = astSaveFrameHits->Current();
	    if ( saveframecur->myType() == ASTnode::SAVEFRAMENODE )
	    {
	        List<ASTnode *> *tagHits = 
		    ((SaveFrameNode *)saveframecur)->searchByTag(iterVal[0]);

      		for ( tagHits->Reset(); !tagHits->AtEnd(); tagHits->Next() )
    	        {
		    ASTnode *astTagCur = tagHits->Current();
		    ASTnode *nmrTagPeer;

		    if ( astTagCur->myType() == ASTnode::DATAITEMNODE )
		    {
		        string tagValue = ((DataItemNode *)astTagCur)->myValue();
		        tagValue = convertToFramecode(tagValue);

			nmrTagPeer = astTagCur->myParallelCopy();
			if (nmrTagPeer != NULL)
			{
			   ((DataItemNode*)nmrTagPeer)->setValue(tagValue);
			   // Skip the dollar sign for dot values and Questions:
			   if( tagValue != string(".")  &&  tagValue != string("?") )
			       ((DataItemNode*)nmrTagPeer)->setDelimType(DataValueNode::FRAMECODE);
			}
		    }
		    else
		    {
       		        for ( ; ((astTagCur != NULL) && 
				(astTagCur->myType() != ASTnode::DATALOOPNODE));
 	                   	astTagCur = astTagCur->myParent() )
	       			;

			ASTlist<DataValueNode*> *loopColumn;

			ASTnode *nmrTagPeer = astTagCur->myParallelCopy();
			if (nmrTagPeer != NULL)
			{
			   loopColumn = ((DataLoopNode*)nmrTagPeer)->returnLoopValues(iterVal[0]);

			   //For each value in the loop columns, change its value.
			   for ( loopColumn->Reset(); !loopColumn->AtEnd(); loopColumn->Next() )
			   {
			     DataValueNode *currVal = loopColumn->Current();
		             string tagValue = currVal->myName();
		             tagValue = convertToFramecode(tagValue);

			     currVal->setValue(tagValue);
			     // Skip the dollar sign for dot values and Questions:
			     if( tagValue != string(".")  &&  tagValue != string("?") )
				 currVal->setDelimType(DataValueNode::FRAMECODE);
		           }
			}
		    }
		}
	        delete tagHits;
	    }
       }
       delete astSaveFrameHits;
   }
   delete []iterVal;
}
