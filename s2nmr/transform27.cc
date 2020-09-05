///////////////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.7  2001/06/18 20:32:53  madings
fixed bug where new loops were inserted in transform27 that didn't have
the parent ptr set right.  This was because a List<> was used where an
ASTlist<> should have been used.

Revision 1.6  2001/06/18 16:57:09  madings
Several fixes for s2nmr segfault bugs.

Revision 1.5  2000/11/18 03:02:38  madings
*** empty log message ***

// Revision 1.4  2000/11/18  02:26:29  madings
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
// Revision 1.3  1999/06/29  19:04:10  zixia
// *** empty log message ***
//
// Revision 1.2  1999/06/11  19:01:32  zixia
// *** empty log message ***
//
// Revision 1.1  1999/06/10  17:43:15  zixia
// *** empty log message ***
//
*/
#endif
//////////////////////////////////////////

#include "transforms.h"
void save_insert_loop(   StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
                         SaveFrameNode *currDicRuleSF)
{
  List<ASTnode*>        *mapLoopMatch;
  DataLoopNameListNode  *outerMapNames;
  LoopTableNode         *outerMapLoopTbl;
  LoopTableNode         *innerMapLoopTbl;
  int                   mL_Idx; /* map loop index */
  SaveFrameNode         *newSaveFrame; 
  int                   key_tag_pos = -1;
  int                   key_value_pos = -1;
  int                   column_name_pos = -1;
  
  mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, string( "_key_tag" ) );
  if( mapLoopMatch->size() < 0 )
  {
        (*errStream) << "#transform-27# ERROR: no loop tag called '_key_tag' in mapping file." << endl;
        delete mapLoopMatch;
        return; /* do nothing */
  }  

  outerMapLoopTbl = ((DataLoopNode*)(*mapLoopMatch)[0])->getValsPtr();
  outerMapNames =  ((DataLoopNode*)(*mapLoopMatch)[0])->getNamesPtr();
  if( outerMapNames->size() != 2 )
  {
       (*errStream) << "#transform-27# ERROR: input mapping file has " <<
           "wrong nesting level (should be one inner loop)." << endl ;
       delete mapLoopMatch;
       return; /* do nothing */
  }

   delete mapLoopMatch;

   
   for( int i = 0 ; i < (*outerMapNames)[0]->size() ; i++ )
   {
        if( (* ((*outerMapNames)[0]) )[i]->myName() == string( "_key_tag" ) )
            key_tag_pos = i;
	else if( (* ((*outerMapNames)[0]) )[i]->myName() == string( "_key_value" ) )
            key_value_pos = i;
   }
   if( key_tag_pos < 0  )
   {   (*errStream) << "#transform-27# ERROR: input mapping file is missing tag '_key_tag'." << endl;
       return; /* do nothing */
   }
   if( key_value_pos < 0  )
   {   (*errStream) << "#transform-27# ERROR: input mapping file is missing tag '_key_value'." << endl;
       return; /* do nothing */
   }

   for( int j = 0 ; j < (*outerMapNames)[1]->size() ; j++ )
   {
        if( (* ((*outerMapNames)[1]) )[j]->myName() == string( "_column_name" ) )
            column_name_pos = j;
   }
   if( column_name_pos < 0  )
   {   (*errStream) << "#transform-27# ERROR: input mapping file is missing tag '_column_name'." << endl;
       return; /* do nothing */
   }
   
   // For each row of the map loop, make a new loop in target saveframe:
   // --------------------------------------------------------------
   for( mL_Idx = 0 ; mL_Idx < outerMapLoopTbl->size() ; mL_Idx++ )
   {   
       List<ASTnode*>   *matchSF;
       
       matchSF = inStar->searchForTypeByTagValue(
                        ASTnode::SAVEFRAMENODE, 
                        (*(*outerMapLoopTbl)[mL_Idx])[key_tag_pos]->myValue(),
                        (*(*outerMapLoopTbl)[mL_Idx])[key_value_pos]->myValue() );
      
       if ( matchSF->size() == 0 ) {
           // JFD: need some warning here 
           (*errStream) << "#transform-27# WARNING: No matching save frames found, no insertions will be made" << endl;
       }
                  
       for(int k=0; k<matchSF->size(); k++){ 
	 ASTlist<DataNode*>  *sf_insides;
	 SaveFrameNode* sfPeer = (SaveFrameNode*)(*matchSF)[k]->myParallelCopy();
         sf_insides = sfPeer->GiveMyDataList();
       
         if( (*outerMapLoopTbl)[mL_Idx]->myLoop() != NULL )
         {
	     //Make a new empty data loop.
	     DataLoopNode          *newLoop = new DataLoopNode(string("tabulate") );
	     DataLoopNameListNode  *nameListList = newLoop->getNamesPtr();
	     LoopTableNode         *valTable = newLoop->getValsPtr();
	     LoopNameListNode      *nameList = NULL;
	     LoopRowNode           *valRow = NULL;

	     int colCount=0;
	   
	     //Populate the list of tag names for the loop.
	     nameListList->insert(nameListList->end(), new LoopNameListNode() );
	     nameList = (*nameListList)[0];
	   
             innerMapLoopTbl   = (*outerMapLoopTbl)[mL_Idx]->myLoop();  

             //Check if such a loop already exists.
	     List<ASTnode*>  *matchLoop;
	     LoopRowNode     *newRow;
	     
             matchLoop =   ((SaveFrameNode*)(*matchSF)[k])->searchForTypeByTag(
		             ASTnode::DATALOOPNODE,
			     (*(*innerMapLoopTbl)[0])[column_name_pos]->myValue());
             
	     if(matchLoop->size() != 0){
		(*errStream)<<"#transform-27# ERROR: loop already exists."<<endl;
	        delete matchLoop;
                continue;
	     }
	     
             // For each row of the inner loop, make a loop in the
             // saveframe:
             // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
             for( int idx = 0 ; idx < innerMapLoopTbl->size() ; idx++ )
             {
                 string column_name = (*(*innerMapLoopTbl)[idx])[column_name_pos]->myValue();
                 nameList->insert( nameList->end(), 
			           new DataNameNode(column_name));
		 
	     }
	   
	     newRow = new LoopRowNode(true);
	     for( int i=0; i< innerMapLoopTbl->size(); i++)
	     {
                newRow->insert(newRow->end(), new DataValueNode(".") );    
	     }
	     valTable->insert(valTable->end(), newRow);

	     sf_insides->insert(sf_insides->end(), newLoop);

	     delete matchLoop;
         }
     }
     delete matchSF; 
   }
}
