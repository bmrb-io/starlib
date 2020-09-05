/////////////////////////////////////////////////////////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
//  $Log: not supported by cvs2svn $
// Revision 1.3  2000/03/20  09:45:49  madings
// Weekend fixes to s2nmr, to respond to some of Jurgen's comments:
// Note, the old s2nmr program is under the name "/bmrb/bin/s2nmr-old", just
// in case you discover a major bug and want to get the old version back.
//
// 1 - The max number allowed for a saveframe's order sequence used to be
// 999.  This was too small.  It has been increased to 9999999, and documented
// acccordingly in specs.html
//
// 2 - Documented that only one middle initial is recognized when
// parsing names from strings.
//
// 3 - Numerous places in the code that created new values did so without
// the proper delimiter type  (For example, it might put a multi-line
// value into a quoted string ("...") instead of a semicolon string.)
// Jurgen noticed this on the citation title, but when I looked for it
// in the code, I found that particular mistake was common throughout the
// newer rules that came after transform # 17.  This has been fixed.
//
// 4 - rule 19, change data_tag_name, did not work as expected:
// I looked at the code and this was simply a case of bad documentation.
// What this rule is meant to do is look for the _BMRB_accession_number
// tag somewhere in the file and use its value as the name of the data_
// tag.  That's all it does.  If a _BMRB_accession_number tag is not there,
// it fails to do anything.  The documentation has been changed to properly
// describe this.
//
// 5 - "rule 22 is unclear to me" - I have re-written explanation of this
// rule, and fixed a bug in the code that refused to operate on multiple
// target saveframes.  (If the key/value pair designated more than one
// output saveframe, it would fail.)
//
// 6 - "rule 25 change_tag_value has backward documenation".  Looked at
// the code to see what it does, and the code is really buggy.  A redo
// from scratch would be quicker than trying to fix it.  The documentation
// isn't backward - the code is.  It uses the wrong variables for things,
// but not in a consistant manner, so I can't just quickly swap things
// around.  Also, we need to define what should be the correct behaviour
// when multiple saveframes are found by either of the two key_tag/value
// searches.  If two input saveframes are found, and two output saveframes
// are found, then is the correct behaviour to copy input 1 to output 1 and
// input 2 to output 2?  Or is the correct behaviour to copy in a 'cross
// product' fashion and copy input 1 to both outputs 1 and 2, and copy
// input 2 to both outputs 1 and 2?  Issues of possible name clashes need
// to be considered too with this.  In light of this, I'm going to put off
// working on this rule untill I am sure it is worth the time, and this
// ambigious behaviour is defined better.  I have to be careful not to spend
// too much time on this program.
//
// 7 - Fixed the broken condition where output_sf_name_tag is null (.) in
// values_to_saveframes_many_to_many.
//
// 8 - Repaired a few typos here and there.
//
// CVS----------------------------------------------------------------------
//
// Revision 1.2  1999/07/13  16:12:04  zixia
// *** empty log message ***
//
// Revision 1.1  1999/06/10  17:42:09  zixia
// *** empty log message ***
//
//////////////////////////////////////////////////////////////////////////////////////
#endif
#include "transforms.h"

void save_copy_tag_into_saveframe(StarFileNode* AST,
		       StarFileNode* NMR,
		       BlockNode*    ,
		       SaveFrameNode *currDicRuleSF)
{
   List<ASTnode*>        *mapLoopMatch;
   DataLoopNameListNode  *mapNames;
   LoopTableNode         *mapLoopTbl;

   int  tag_to_move_pos =-1;
   int	key_tag_pos = -1;  
   int  key_value_pos = -1;

   // Find loop from the mapping file:
   // --------------------------------
   mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, string( "_tag_to_move" ) );
   if( mapLoopMatch->size() < 0 )
   {
       (*errStream) << "#transform-22# ERROR: no loop tag called '_tag_to_move' in mapping file." << endl;
       delete mapLoopMatch;
       return; /* do nothing */
   }
   mapLoopTbl = ((DataLoopNode*)(*mapLoopMatch)[0])->getValsPtr();
   mapNames =  ((DataLoopNode*)(*mapLoopMatch)[0])->getNamesPtr();

   delete mapLoopMatch;

   for( int i = 0 ; i < (*mapNames)[0]->size() ; i++ )
    {
        if(    (* ((*mapNames)[0]) )[i]->myName()==string( "_tag_to_move" ) )
            tag_to_move_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_key_tag" ) )
            key_tag_pos = i;
        else if( (* ((*mapNames)[0]) )[i]->myName()==string( "_key_value" ) )
            key_value_pos = i;
   }
   if( tag_to_move_pos < 0  )
   {   (*errStream) << "#transform-22# ERROR: input mapping file is missing tag '_tag_to_move'." << endl;
       return; /* do nothing */
   }
   if( key_tag_pos < 0  )
   {   (*errStream) << "#transform-22# ERROR: input mapping file is missing tag '_key_tag'." << endl;
       return; /* do nothing */
   }
   if( key_value_pos < 0  )
   {   (*errStream) << "#transform-22# ERROR: input mapping file is missing tag '_key_value'." << endl;
       return; /* do nothing */
   }

   // For each row of the loop, insert the tag into the specified saveframe.
   // -----------------------------------------------
   for(int mL_Idx = 0 ; mL_Idx < mapLoopTbl->size() ; mL_Idx++ )
   {
       List<ASTnode*> *matchSF = AST->searchForTypeByTagValue(
                        ASTnode::SAVEFRAMENODE, 
                        (*(*mapLoopTbl)[mL_Idx])[key_tag_pos]->myValue(),
                        (*(*mapLoopTbl)[mL_Idx])[key_value_pos]->myValue() );

       for( int sfIdx = 0 ; sfIdx < matchSF->size() ; sfIdx++ )
       {
	   SaveFrameNode *peerSF = (SaveFrameNode*) (*matchSF)[sfIdx]->myParallelCopy();
	   if( peerSF == NULL )
	   {
	       (*errStream) << "#transform-22# Error: internal error, no peer at line "
		<< __LINE__ << ".  Fatal error, quitting rule." << endl;
		delete matchSF;
		return;
	   }

	   //Check whether this tag is already in this saveframe. If yes, do nothing
	   List<ASTnode*> *tagInSF = peerSF->searchByTag( (*(*mapLoopTbl)[mL_Idx])[tag_to_move_pos]->myValue()  );
	   
	   if(tagInSF->size() != 0 ){
	       continue;
	   }
	   
	   
	   //Find the free tag.
	   List<ASTnode*> *matchTag =  AST->searchByTag(
			  (*(*mapLoopTbl)[mL_Idx])[tag_to_move_pos]->myValue() );
		 
	   //For each tag found in the file, do the copying.
	   for(int i=0; i<matchTag->size(); i++){
	     
	       ASTnode * par = ((DataItemNode*)((*matchTag)[i]))->myParent();

	     while(par != NULL && !par->isOfType(ASTnode::DATALOOPNODE) )
		     par = par->myParent();
		  
	     if( par !=NULL && par->isOfType(ASTnode::DATALOOPNODE)){//If the tag is in a loop.
		  //Make a copy of this DateLoopNode.
		  DataLoopNode * newLoop = new DataLoopNode(*(DataLoopNode*)par);
	    

		  peerSF->GiveMyDataList()->insert( peerSF->GiveMyDataList()->end(), newLoop );
	     }else{//If the tag is a free tag.
		 //Make a new data Item and append it to the specified saveframe
		 DataItemNode *newItem = new DataItemNode( ((DataItemNode*)((*matchTag)[i]))->myName(),
							   ((DataItemNode*)((*matchTag)[i]))->myValue(),
							   ((DataItemNode*)((*matchTag)[i]))->myDelimType());
		  
		 peerSF->GiveMyDataList()->insert(peerSF->GiveMyDataList()->end(), newItem);
		  
	     }
	   }
	   delete matchTag;
       }
       delete matchSF; 
   }
   delete mapNames;
   delete mapLoopTbl;
    
}
