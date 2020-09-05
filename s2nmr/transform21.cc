///////////////////////////////////////////
#ifdef BOGUS_DEF_COMMENT_OUT
/*
$Log: not supported by cvs2svn $
Revision 1.4  2000/11/18 03:02:38  madings
*** empty log message ***

// Revision 1.3  2000/11/18  02:26:29  madings
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
// Revision 1.2  1999/06/29  19:04:10  zixia
// *** empty log message ***
//
// Revision 1.1  1999/06/10  17:41:05  zixia
// Add a new function transform21 by Zixia Gu.
//
*/
#endif
//////////////////////////////////////////

#include "transforms.h"

void save_create_saveframe(  StarFileNode *inStar, 
                         StarFileNode *outStar, 
                         BlockNode    *,
                         SaveFrameNode *currDicRuleSF)
{
  string                map_sf_name = string("save_create_new_saveframe" );
  List<ASTnode*>        *mapLoopMatch;
  DataLoopNameListNode  *outerMapNames;
  LoopTableNode         *outerMapLoopTbl;
  LoopTableNode         *innerMapLoopTbl;
  int                   mL_Idx; /* map loop index */
  SaveFrameNode         *newSaveFrame; 
  int                   new_tag_name_pos;
  int                   new_tag_value_pos;

  
  mapLoopMatch = currDicRuleSF->searchForTypeByTag( ASTnode::DATALOOPNODE, string( "_frame_name" ) );
  if( mapLoopMatch->size() < 0 )
  {
        (*errStream) << "#transform-21# ERROR: no loop tag called '_frame_name' in mapping file." << endl;
        delete mapLoopMatch;
        return; /* do nothing */
  }  

  outerMapLoopTbl = ((DataLoopNode*)(*mapLoopMatch)[0])->getValsPtr();
  outerMapNames =  ((DataLoopNode*)(*mapLoopMatch)[0])->getNamesPtr();
  if( outerMapNames->size() != 2 )
  {
       (*errStream) << "#transform-21# ERROR: input mapping file has " <<
           "wrong nesting level (should be one inner loop)." << endl ;
       delete mapLoopMatch;
       return; /* do nothing */
  }

   delete mapLoopMatch;

   new_tag_name_pos = -1;
   new_tag_value_pos = -1;

   for( int i = 0 ; i < (*outerMapNames)[1]->size() ; i++ )
   {
        if( (* ((*outerMapNames)[1]) )[i]->myName() == string( "_new_tag_name" ) )
            new_tag_name_pos = i;
        else if( (* ((*outerMapNames)[1]) )[i]->myName()==string( "_new_tag_value" ) )
            new_tag_value_pos = i;
   }
   if( new_tag_value_pos < 0  )
   {   (*errStream) << "#transform-21# ERROR: input mapping file is missing tag '_from_key_tag'." << endl;
       return; /* do nothing */
   }
   if( new_tag_value_pos < 0  )
   {   (*errStream) << "#transform-21# ERROR: input mapping file is missing tag '_from_key_value'." << endl;
       return; /* do nothing */
   } 

   // For each row of the map loop, make a new saveframe in outStar:
   // --------------------------------------------------------------
   for( mL_Idx = 0 ; mL_Idx < outerMapLoopTbl->size() ; mL_Idx++ )
   {  
       bool is_exist = false;
       newSaveFrame = new SaveFrameNode( (*(*outerMapLoopTbl)[mL_Idx])[0]->myValue() );

       if( (*outerMapLoopTbl)[mL_Idx]->myLoop() != NULL )
       {
           innerMapLoopTbl   = (*outerMapLoopTbl)[mL_Idx]->myLoop();  

           // For each row of the inner loop, make a new data item in the
           // saveframe:
           // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
           for( int idx = 0 ; idx < innerMapLoopTbl->size() ; idx++ )
           {
               string tagName  = (*(*innerMapLoopTbl)[idx])[new_tag_name_pos]->myValue();
               string tagValue = (*(*innerMapLoopTbl)[idx])[new_tag_value_pos]->myValue();      

	       //Check if a saveframe with key_tag, key_value already exits. If it is, do nothing.
                List<ASTnode*> *matchSF = inStar->searchForTypeByTagValue(
                        ASTnode::SAVEFRAMENODE, tagName, tagValue);
                        
	        if(matchSF->size() != 0 ){
                    (*errStream)<<"#transform-21# ERROR: a saveframe with ("<<tagName<<", "
			<<tagValue<<") already exists in the input file."<<endl;
		    is_exist = true;
		    delete matchSF;
		    break;	    
		}
		
		// Make a new data Item and append it to the new
               // saveframe:
               DataItemNode *newItem = new DataItemNode(tagName, tagValue);
               newSaveFrame->GiveMyDataList()->insert( newSaveFrame->GiveMyDataList()->end(), newItem );
	   }
       }

       //If the saveframe already exists, do nothing.
       //Continue to do the transformation on next loop row.
       if( is_exist ){
          continue;
       }
       
       // Insert the newly made saveframe at the end of the
       // output star file:
       //
       List<ASTnode*> *outputBlocks = outStar->searchForType( ASTnode::BLOCKNODE );
       BlockNode *lastOutputBlock = (BlockNode*)( (*outputBlocks)[outputBlocks->size() - 1] );
       delete outputBlocks;
       lastOutputBlock->GiveMyDataList()->insert( lastOutputBlock->GiveMyDataList()->end(), newSaveFrame );  
   }    
}
